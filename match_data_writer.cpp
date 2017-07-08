#include "match_data_writer.h"
#include "assert_wc.h"
#include "default_sql.h"

#include <QDate>
#include <QFile>
#include <QtDebug>
#include <QFileInfo>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

ClientServerWriter::~ClientServerWriter()
{

}

void ClientServerWriter::init(const QStringList &mode_settings)
{

}

void ClientServerWriter::process(QString &data)
{

}

//----------------------

MysqlWriter::~MysqlWriter()
{
    _db->close();
}

void MysqlWriter::init(const QStringList &mode_settings)
{
    qInfo() << "Using mysql server => 192.168.1.13";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("192.168.1.13");
    db.setDatabaseName("d2crawl");
    db.setUserName("root");
    db.setPassword("power282");

    ASSERT_WITH_CODE(db.open(), "Can't connect to DB", qTerminate());

    if ( !db.tables().contains("match") || !db.tables().contains("match_players") )
    {
        db.transaction();
        QSqlQuery query;

        bool query_res = query.exec(match_default_mysql);
        if(!query_res)
        {
            qDebug() << query.lastError();
        }

        query_res = query.exec(match_players_default_mysql);
        if(!query_res)
        {
            qDebug() << query.lastError();
        }
        db.commit();
    }

    _db = &db;
}

void MysqlWriter::process(QString &data)
{
    ASSERT_WITH_CODE(_query->exec(data), qDebug() << _query->lastError());
}

//----------------------

SqliteWriter::~SqliteWriter()
{
    _db->close();
}

void SqliteWriter::init(const QStringList &mode_settings)
{
    qInfo() << "Using sqlite file => d2crawl.sqlite";
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("d2crawl.sqlite");

    ASSERT_WITH_CODE(db.open(), "Can't connect to DB", qTerminate());

    _query = new QSqlQuery();
    if ( !db.tables().contains("match") || !db.tables().contains("match_players") )
    {
        db.transaction();

        bool query_res = _query->exec(match_default_sqlite);
        if(!query_res)
        {
            qDebug() << _query->lastError();
        }

        query_res = _query->exec(match_players_default_sqlite);

        if(!query_res)
        {
            qDebug() << _query->lastError();
        }
        db.commit();
    }

    _db = &db;
}

void SqliteWriter::process(QString &data)
{
    ASSERT_WITH_CODE(_query->exec(data), qDebug() << _query->lastError());
}

//----------------------

FileWriter::~FileWriter()
{
    _file->close();
    delete _file;
}

void FileWriter::init(const QStringList &mode_settings)
{
    QString file_name = QFileInfo( QCoreApplication::applicationFilePath() ).fileName() + "_" + QDate::currentDate().toString("dd_MM_yy") + "_" + QTime::currentTime().toString("hh_mm_ss") + ".sql";
    qInfo() << "Using file => " << file_name;
    _file = new QFile(file_name);
    ASSERT_WITH_CODE(_file->open(QIODevice::WriteOnly | QIODevice::Append), "file %s open failed", file_name.toStdString().c_str(), qTerminate());
}

void FileWriter::process(QString &data)
{
    QTextStream(_file) << data << ";" << endl;
}
