#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QDebug>
#include <QHash>
#include <QJsonObject>
#include <QtNetwork>

#include "Databasehelper.h"

class BackEnd : public QObject {
    Q_OBJECT
        Q_PROPERTY(QString serverAccessCode READ serverAccessCode WRITE setServerAccessCode NOTIFY serverAccessCodeChanged)
        Q_PROPERTY(QString locationData READ locationData WRITE setLocationData NOTIFY locationDataSent)

        Q_PROPERTY(bool serverAccessCodeStatus READ serverAccessCodeStatus)
        Q_PROPERTY(bool locationDataSentStatus READ locationDataSentStatus)
        Q_PROPERTY(bool sendLocationDataStatus READ sendLocationDataStatus)
        Q_PROPERTY(bool networkConnectionStatus READ networkConnectionStatus)

    //TODO::
    //get internet working and server communication
public:
    explicit BackEnd(QObject *parent = nullptr);

    //getter methods
    QString serverAccessCode();
    QString locationData();

    bool serverAccessCodeStatus();
    bool locationDataSentStatus();
    bool networkConnectionStatus();
    bool sendLocationDataStatus();

    //setter methods
    void setServerAccessCode(const QString &newCode);
    void setLocationData(const QString &locationInformation);

signals:
    void serverAccessCodeChanged();
    void locationDataSent();

private:
    DatabaseHelper databaseHelper;
    QJsonObject json;

    QString accessCode;
    QString timestamp;
    QString coordinates;
    QString longitude;
    QString latitude;

    bool accessCodeStatusFlag;
    bool sendLocationDataFlag;
    bool storeLocationDataFlag;
    bool connectionStatusFlag = false;
};
#endif // BACKEND_H
