#ifndef UPDATE_MANAGER_H
#define UPDATE_MANAGER_H

#define VERSION 0.01

#include <QThread>

class QNetworkReply;
class QNetworkAccessManager;

class UpdateManager : public QObject
{
    Q_OBJECT

public:
    UpdateManager();
    virtual ~UpdateManager() override;

signals:
    void checkVersion();

private slots:
    void onTimerEvent();
    void onCheckVersion();
    void onDataReady();
    void _installUpdate();

private:
    void _performUpdate();

    QNetworkReply *_reply;
    QNetworkAccessManager *_net_manager;
};

#endif // UPDATE_MANAGER_H
