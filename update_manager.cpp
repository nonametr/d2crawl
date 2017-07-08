#include "update_manager.h"
#include "assert_wc.h"


#include <QTimer>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

UpdateManager::UpdateManager()
{
    _net_manager = new QNetworkAccessManager(this);

    connect(this, SIGNAL(checkVersion()), SLOT(onCheckVersion()));

    QTimer::singleShot(500, this, SLOT(onTimerEvent()));
}

UpdateManager::~UpdateManager()
{
    delete _net_manager;
}

void UpdateManager::onTimerEvent()
{
    emit checkVersion();
}

void UpdateManager::onCheckVersion()
{
    QNetworkRequest request= QNetworkRequest(QUrl("http://www.magicmess.online/d2c/version.php"));
    _reply = _net_manager->get(request);

    ASSERT_WITH_CODE(_reply->error() == QNetworkReply::NoError, "Error requesting version info from master server", qDebug() << _reply->errorString(); this->deleteLater());

    connect(_reply, SIGNAL(finished()), this, SLOT(onDataReady()));
}

void UpdateManager::onDataReady()
{
    QByteArray data = _reply->readAll();

    QJsonDocument resp_jdoc = QJsonDocument::fromJson(QString(data).toUtf8());
    QJsonObject resp_jobj = resp_jdoc.object();

    if(resp_jobj["core"].toDouble() != VERSION)
    {
        qDebug() << "New version available!";
        _performUpdate();
        return;
    }

    QTimer::singleShot(1000, this, SLOT(onTimerEvent()));
}

void UpdateManager::_performUpdate()
{
    QNetworkRequest request= QNetworkRequest(QUrl("http://www.magicmess.online/d2c/update.exe"));
    _reply = _net_manager->get(request);

    ASSERT_WITH_CODE(_reply->error() == QNetworkReply::NoError, "Error downloading update", qDebug() << _reply->errorString(); this->deleteLater());

    connect(_reply, SIGNAL(finished()), this, SLOT(_installUpdate()));
}

void UpdateManager::_installUpdate()
{
    QByteArray data = _reply->readAll();

    qDebug() << "Installing update...";

    QTimer::singleShot(1000, this, SLOT(onTimerEvent()));
}
