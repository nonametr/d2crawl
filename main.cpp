#include <iostream>
#include "match_crawler.h"
#include "update_manager.h"

#include <QtCore/QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QTimer>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    UpdateManager update_manager;

    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz t} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");

    QCoreApplication::setApplicationName("Dota 2 Crawler");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("This dota2 crawler can crawl: 'matches', 'player_matches' and 'player_info'. \n"
                                     "Usage example: \n"
                                     "\t./d2crawler -cv -k 'your_steam_api_key' -m master hostname - start crawling with verbose console logging in slave mode using master server 'hostname'\n"
                                     "\t./d2crawler -k 'your_steam_api_key' -m mysql hostname port login password dbname -s 7000007 - start crawling to mysql database");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("p1", QCoreApplication::translate("main", "parameter 1."));

    parser.addOptions(
    {
        {
            "c",
            QCoreApplication::translate("main", "Force logging to console.")
        },
        {
            {"V", "verbose"},
            QCoreApplication::translate("main", "Enable verbose logging.")
        },
        {
            {"m", "mode"},
            QCoreApplication::translate("main", "Set work <mode>[file, sqlite, mysql, master]."),
            QCoreApplication::translate("main", "<mode>."),
            "master"
        },
        {
            {"i", "init"},
            QCoreApplication::translate("main", "Mode dependent initalization string"),
            QCoreApplication::translate("main", "<mode params>."),
            "magicmess.online/d2c/"
        },
        {
            {"k", "key"},
            QCoreApplication::translate("main", "Steam API <key> to use."),
            QCoreApplication::translate("main", "key"),
            "YOUR_UNIQ_VALVE_STEAM_API_KEY"
        },
        {
            {"s", "seq"},
            QCoreApplication::translate("main", "Set <match_seq_num>."),
            QCoreApplication::translate("main", "match_seq_num"),
            "0"
        }
    });

    if(!parser.parse(app.arguments()))
    {
        qCritical() << parser.errorText();
        parser.showHelp();
    }

    if(parser.isSet("h"))
    {
        parser.showHelp();
    }
    if(parser.isSet("v"))
    {
        parser.showVersion();
    }
    if(!parser.isSet("k"))
    {
        qCritical() << "Steam API key is REQUIRED parameter ";
        parser.showHelp();
    }

    qputenv("QT_LOGGING_TO_CONSOLE", QByteArray(parser.isSet("c") ? "1" : "0"));

    QMap<QString, int> mode_to_flag_map =
    {
        {"master", 1 << CLO_USE_CLIENT_SERVER},
        {"mysql", 1 << CLO_USE_MYSQL},
        {"sqlite", 1 << CLO_USE_SQLITE},
        {"file", 1 << CLO_USE_FILE}
    };

    int flags = mode_to_flag_map[parser.value("m")] +
            ((parser.isSet("c") ? 1 : 0) << CLO_LOG_TO_CONSOLE) +
            ((parser.isSet("V") ? 1 : 0) << CLO_LOG_VERBOSE);

    MatchCrawler match_crawler(flags, parser.positionalArguments(), parser.value("s").toInt(), parser.value("k").toStdString());
    match_crawler.start();

    return app.exec();
}
