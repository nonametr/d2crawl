#ifndef MATCH_DATA_WRITER_H
#define MATCH_DATA_WRITER_H

class QStringList;
class QString;
class QFile;
class QSqlDatabase;
class QSqlQuery;

class IMatchDataWriter
{
public:
    virtual ~IMatchDataWriter() = default;

    virtual void init(const QStringList &mode_settings) = 0;
    virtual void process(QString &data) = 0;
};

class ClientServerWriter : public IMatchDataWriter
{
public:
    virtual ~ClientServerWriter() override;

    virtual void init(const QStringList &mode_settings) override;
    virtual void process(QString &data) override;
};

class MysqlWriter : public IMatchDataWriter
{
public:
    virtual ~MysqlWriter() override;

    virtual void init(const QStringList &mode_settings) override;
    virtual void process(QString &data) override;

private:
    QSqlQuery *_query;
    QSqlDatabase *_db;
};

class SqliteWriter : public IMatchDataWriter
{
public:
    virtual ~SqliteWriter() override;

    virtual void init(const QStringList &mode_settings) override;
    virtual void process(QString &data) override;

private:
    QSqlQuery *_query;
    QSqlDatabase *_db;
};

class FileWriter : public IMatchDataWriter
{
public:
    virtual ~FileWriter() override;

    virtual void init(const QStringList &mode_settings) override;
    virtual void process(QString &data) override;

private:
    QFile *_file;
};

#endif // MATCH_DATA_WRITER_H
