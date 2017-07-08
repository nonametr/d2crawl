#include "match_crawler.h"
#include "assert_wc.h"

#include <iostream>
#include <chrono>
#include <map>
#include <set>

#include <curl/curl.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QThread>
#include <QFile>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDate>
using namespace std;

string MatchCrawler::_data;

MatchCrawler::MatchCrawler(int flags, QStringList &&mode_settings, long match_seq_num_offset, string &&api_key) : _flags(flags), _api_key(api_key)
{
    if(flags & (1 << CLO_USE_FILE))
    {
        _data_writer = new FileWriter();
    }
    if(flags & (1 << CLO_USE_MYSQL))
    {
        _data_writer = new MysqlWriter();
    }
    if(flags & (1 << CLO_USE_SQLITE))
    {
        _data_writer = new SqliteWriter();
    }
    if(flags & (1 << CLO_USE_CLIENT_SERVER))
    {
        _data_writer = new ClientServerWriter();
    }

    _data_writer->init(mode_settings);

    _match_seq_file = new QFile(LAST_MATCH_SEQ_NUM_FILENAME);
    ASSERT_WITH_CODE(_match_seq_file->open(QIODevice::ReadWrite), "file %s open failed", LAST_MATCH_SEQ_NUM_FILENAME, qTerminate());

    if(match_seq_num_offset)
    {
        _last_match_seq_num = match_seq_num_offset;
    }
    else if(_match_seq_file->isOpen())
    {
        _last_match_seq_num = _match_seq_file->readAll().toLongLong();
    }
}

MatchCrawler::~MatchCrawler()
{
    _match_seq_file->close();
    delete _match_seq_file;
}

size_t MatchCrawler::_onData(char* buf, size_t size, size_t nmemb, void* up)
{
    for (uint c = 0; c < size*nmemb; c++)
    {
        _data.push_back(buf[c]);
    }
    return size * nmemb;
}

QString asString(QJsonValue val)
{
    switch (val.type()) {
        case QJsonValue::Null:
            return "null";
        case QJsonValue::Bool:
            return val.toBool() ? "1" : "0";
        case QJsonValue::Array:
        {
            QJsonDocument doc;
            doc.setArray(val.toArray());

            return QString(doc.toJson());
        }
        case QJsonValue::Object:
        {
            QJsonDocument doc;
            doc.setObject(val.toObject());

            return QString(doc.toJson());
        }
        case QJsonValue::Undefined:
            return "undefined";
        default:
            return val.toVariant().toString();
    }
}

RetCode MatchCrawler::_process(QString data)
{
    QJsonDocument resp_jdoc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject resp_jobj = resp_jdoc.object()["result"].toObject();

    if(resp_jobj["status"].toInt() == false)
    {
        qWarning() << "Got bad responce from server: \n " << data;
        if(data.contains("Too Many Requests"))
        {
            return RetCode::RC_TRY_AGAIN_LATER;
        }
        else
        {
            return RetCode::RC_FAIL;
        }
    }
    else
    {
        map<QString, QString> sql_batches;
        _processData(resp_jobj["matches"].toArray(), sql_batches);

        QSqlQuery query;
        for(auto it_batch : sql_batches)
        {
            QString keys = it_batch.first;
            QString values = it_batch.second;

            values.chop(1);

            QString table_name = keys.contains("player_slot") ? "match_players" : "match";
            QString insert_query = _flags & (1 << CLO_USE_SQLITE) ? "INSERT OR IGNORE" : "INSERT IGNORE";
            QString query_str = QString("%1 INTO `%2`(%3) VALUES%4").arg(insert_query).arg(table_name).arg(keys).arg(values);

            _data_writer->process(query_str);
        }
    }

    QJsonArray matches_jarr = resp_jobj["matches"].toArray();
    _last_match_seq_num = matches_jarr[matches_jarr.size() - 1].toObject()["match_seq_num"].toVariant().toLongLong();

    _match_seq_file->resize(0);
    QTextStream(_match_seq_file) << QString::number(_last_match_seq_num);

    return RetCode::RC_OK;
}

void MatchCrawler::_processData(QJsonArray &&data_jarr, map<QString, QString> &sql_batches, int match_id)
{
    static const set<QString> supported_keys =
    {
        "account_id", "player_slot", "hero_id", "item_0", "item_1", "item_2", "item_3", "item_4", "item_5",
        "backpack_0", "backpack_1", "backpack_2", "kills", "deaths", "assists", "leaver_status",
        "last_hits", "denies", "gold_per_min", "xp_per_min", "level", "hero_damage", "tower_damage",
        "hero_healing", "gold", "gold_spent", "scaled_hero_damage", "scaled_tower_damage", "scaled_hero_healing",
        "players", "radiant_win", "duration", "pre_game_duration", "start_time", "match_id", "match_seq_num",
        "tower_status_radiant", "tower_status_dire", "barracks_status_radiant", "barracks_status_dire", "cluster",
        "first_blood_time", "lobby_type", "human_players", "leagueid", "positive_votes", "negative_votes", "game_mode",
        "flags", "engine", "radiant_score", "dire_score"
    };

    for(int data_indx = 0; data_indx < data_jarr.size(); ++data_indx)
    {
        vector<QString> keys_sql;
        vector<QString> vals_sql;
        QJsonObject data_jobj = data_jarr[data_indx].toObject();

        if(match_id)
        {
            data_jobj["match_id"] = match_id;
        }
        for(QJsonObject::iterator it_data = data_jobj.begin(); it_data != data_jobj.end(); ++it_data)
        {
            QString data_key = it_data.key();
            QJsonValue data_val = it_data.value();

            ASSERT_WITH_CODE(supported_keys.find(data_key) != supported_keys.end(), "Unexpected data found in server response => '%s'", data_key.toStdString().c_str(), continue);

            if(data_val.isArray())
            {
                int subdata_match_id = 0;
                if(data_jobj.contains("match_id"))
                {
                    subdata_match_id = data_jobj["match_id"].toInt();
                }
                _processData(data_val.toArray(), sql_batches, subdata_match_id);
            }
            else
            {
                keys_sql.push_back(data_key);
                vals_sql.push_back(asString(data_val));
            }
        }

        QString key = std::accumulate(std::next(keys_sql.begin()), keys_sql.end(),
                                           "`" + *keys_sql.begin() + "`",
                                           [](QString a, QString b) {
                                               return a + ",`" + b + "`";
                                           });

        QString val = std::accumulate(std::next(vals_sql.begin()), vals_sql.end(),
                                           *vals_sql.begin(),
                                           [](QString a, QString b) {
                                               return a + "," + b;
                                           });

        sql_batches[key] += "(" + val + "),";
    }
}

void MatchCrawler::run()
{
    static const uint chunk_size = 1000;
    static const uint urlbuf_size = 1024;

    std::chrono::high_resolution_clock::time_point req_start;
    std::chrono::high_resolution_clock::time_point req_finish;
    std::chrono::high_resolution_clock::time_point process_finish;

    CURL* curl; //our curl object

    curl_global_init(CURL_GLOBAL_ALL); //pretty obvious
    curl = curl_easy_init();

    qInfo() << "crawling started...";

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &_onData);
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //tell curl to output its progress

    double adjusted_wait = 0.0;
    RetCode rc;
    char urlbuf[urlbuf_size] = {'\0'};
    long prev_match_seq_num = -1;
    while(prev_match_seq_num != _last_match_seq_num)
    {
        prev_match_seq_num = _last_match_seq_num;
        snprintf(urlbuf, urlbuf_size,
                 "http://api.steampowered.com/IDOTA2Match_570/GetMatchHistoryBySequenceNum/v1?key=%s&start_at_match_seq_num=%ld&matches_requested=%u",
                 _api_key.c_str(), _last_match_seq_num + 1l, chunk_size);

        curl_easy_setopt(curl, CURLOPT_URL, urlbuf);

        int attempt_id = 0;

        do
        {
            _data.clear();
            req_start = std::chrono::high_resolution_clock::now();
            curl_easy_perform(curl);
            req_finish = std::chrono::high_resolution_clock::now();
            rc = _process(_data.c_str());
            if(rc != RC_OK)
            {
                adjusted_wait += 0.05;
                qInfo() << "wating 2 sec...";
                QThread::sleep(2);
            }
            else
                break;

        } while(++attempt_id < MAX_REQ_ATTEMPTS);

        Q_ASSERT_X(rc == RC_OK, "processing", "something went wrong during request...");

        process_finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> exec_time = process_finish - req_start;
        std::chrono::duration<double> req_time = req_finish - req_start;
        std::chrono::duration<double> proc_time = process_finish - req_finish;

        //target request rate <1/s
        double wait_time = (1.0 - proc_time.count()) + adjusted_wait;
        if(wait_time > 0)
        {
            QThread::usleep(static_cast<ulong>(wait_time * 1000000));
        }

        qInfo().nospace() << "chunk [" << prev_match_seq_num << "," << _last_match_seq_num << "]" << " processed in " << exec_time.count() << "(" << req_time.count() << "+" << proc_time.count() << ") sec." << "[wt=" << wait_time << "]";
    }

    cin.get();

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}
