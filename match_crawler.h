#ifndef D2CRAWLER_H
#define D2CRAWLER_H

#include "match_data_writer.h"

#include <string>
#include <map>

#include <QThread>

#define MAX_REQ_ATTEMPTS 300
#define LAST_MATCH_SEQ_NUM_FILENAME "last_match_seq_num"

class QFile;
class QJsonArray;
class QString;
class QSqlDatabase;
class QStringList;

enum RetCode { RC_OK, RC_TRY_AGAIN_LATER, RC_FAIL };
enum COMMAND_LINE_OPTIONS { CLO_LOG_TO_CONSOLE = 0, CLO_LOG_VERBOSE, CLO_USE_SQLITE, CLO_USE_MYSQL, CLO_USE_FILE, CLO_USE_CLIENT_SERVER };

class MatchCrawler : public QThread
{
    Q_OBJECT

public:
    MatchCrawler(int flags, QStringList &&mode_settings, long match_seq_num_offset, std::string &&api_key);
    virtual ~MatchCrawler();

    void run() override;

private:
    static size_t _onData(char* buf, size_t size, size_t nmemb, void* up);
    static std::string _data;

    void _initDB(QSqlDatabase db);
    void _processData(QJsonArray &&data_jarr, std::map<QString, QString> &sql_batches, int match_id = 0);
    RetCode _process(QString data);

    int _flags;
    long _last_match_seq_num = 0;

    IMatchDataWriter *_data_writer;
    std::string _api_key;
    QFile *_match_seq_file;
};

#endif // D2CRAWLER_H
