#include "BackEnd.h"

BackEnd::BackEnd(QObject *parent) : QObject(parent) {
    //sending 'true' to the constructor will drop all tables
    //and recreate them, all data will be lost
    databaseHelper = DatabaseHelper(true);

    //get serverKey and serverStatus from the database
    QHash<QString, QString> hashmap = databaseHelper.getServerKey();

    accessCode = hashmap["server_key"];
    accessCodeStatusFlag = hashmap["key_status"].toInt();

    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost("http:://www.google.ca", 80);

    if (socket->waitForConnected(5000)) {
        qDebug() << "Connected";
    }
    else {
        qDebug() << "Not connected";
    }
}

//getter methods
QString BackEnd::serverAccessCode() {
    return accessCode;
}

QString BackEnd::locationData() {
    return "";
}

bool BackEnd::serverAccessCodeStatus() {
    return accessCodeStatusFlag;
}

bool BackEnd::locationDataSentStatus() {
    return sendLocationDataFlag;
}

bool BackEnd::networkConnectionStatus() {
    return connectionStatusFlag;
}

bool BackEnd::sendLocationDataStatus() {
    return sendLocationDataFlag;
}


//setter methods
void BackEnd::setServerAccessCode(const QString &newCode) {
    if(newCode != "") {
        accessCode = newCode;

        //perform a server test with the new string
        //TODO::
        accessCodeStatusFlag = true;

        databaseHelper.insertServerKey(accessCode, accessCodeStatusFlag);

        emit serverAccessCodeChanged();
    }
}

void BackEnd::setLocationData(const QString &locationInformation) {
    if(locationInformation.length() > 1) {
        coordinates = locationInformation.left(locationInformation.indexOf("|"));
        timestamp = locationInformation.mid(locationInformation.indexOf("|") + 1);

        latitude = coordinates.left(coordinates.indexOf(" "));
        longitude = coordinates.mid(coordinates.indexOf(" ") + 1);

        qInfo() << "[timestamp]: " + timestamp + "\n";
        qInfo() << "[coordinates]: " + coordinates + "\n";
        qInfo() << "[latitude]: " + latitude + "\n";
        qInfo() << "[longitude]: " + longitude + "\n";

        json = QJsonObject();
        json["serverString"] = accessCode;
        json["latitude"] = latitude;
        json["longitude"] = longitude;
        json["timestamp"] = timestamp;

        qInfo() << "JSON OBJECT: " << json;
        //Test to see if theres an internet connection and the server is available
        //TODO::

        if(storeLocationDataFlag) {
            if(accessCodeStatusFlag && connectionStatusFlag) {
                //there is an internet connection
                //server string is valid
                //send ALL stored database entries, then delete the entries
                sendLocationDataFlag = true;

                emit locationDataSent();
            }
            else {
                //no internet connection
                //server string is invalid
                databaseHelper.insertLocation(latitude, longitude, timestamp);

                sendLocationDataFlag = false;

                emit locationDataSent();
            }
        }
    }
}
