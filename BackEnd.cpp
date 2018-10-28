#include "BackEnd.h"

BackEnd::BackEnd(QObject *parent) : QObject(parent) {
    //sending 'true' to the constructor will drop all tables
    //and recreate them, all data will be lost
    databaseHelper = DatabaseHelper(false);

    //get serverKey and serverStatus from the database
    QHash<QString, QString> hashmap = databaseHelper.getServerKey();

    serverString = hashmap["server_key"];
    stringStatus = hashmap["key_status"].toInt();

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
QString BackEnd::connectionString() {
    return serverString;
}

QString BackEnd::gpsLocation() {
    return "Timestamp: " + timestamp + "\n" + "Coordinates: " + coordinates;
}

int BackEnd::connectionStringStatus() {
    return stringStatus;
}

bool BackEnd::sendGpsDataStatus() {
    //will return the status of the sent GPS data
    return sendFlag;
}

//setter methods
void BackEnd::setConnectionString(const QString &newString) {
    if(serverString != "") {
        serverString = newString;

        //perform a server test with the new string
        //TODO::
        stringStatus = 1;

        databaseHelper.insertServerKey(serverString, stringStatus);

        emit connectionStringChanged();
    }
}

void BackEnd::setGpsLocation(const QString &gpsInformation) {
    if(gpsInformation.length() > 1) {
        coordinates = gpsInformation.left(gpsInformation.indexOf("|"));
        timestamp = gpsInformation.mid(gpsInformation.indexOf("|") + 1);

        latitude = coordinates.left(coordinates.indexOf(" "));
        longitude = coordinates.mid(coordinates.indexOf(" ") + 1);
        qInfo() << "[timestamp]: " + timestamp + "\n";
        qInfo() << "[coordinates]: " + coordinates + "\n";
        qInfo() << "[latitude]: " + latitude + "\n";
        qInfo() << "[longitude]: " + longitude + "\n";
        json = QJsonObject();

        json["serverString"] = serverString;
        json["latitude"] = latitude;
        json["longitude"] = longitude;
        json["timestamp"] = timestamp;

        qInfo() << "JSON OBJECT: " << json;
        //Test to see if theres an internet connection and the server is available
        //TODO::
        bool connectionFlag = false;

        if(stringStatus == 1 && connectionFlag) {
            //there is an internet connection
            //server string is valid
            //send ALL stored database entries, then delete the entries


            sendFlag = true;

            emit gpsLocationDataSent();
        }
        else {
            //no internet connection
            //server string is invalid
            databaseHelper.insertLocation(latitude, longitude, timestamp);
            sendFlag = false;

            emit gpsLocationDataSent();
        }
    }
}