#include "BackEnd.h"

BackEnd::BackEnd(QObject *parent) : QObject(parent) {
    //sending 'true' to the constructor will drop all tables
    //and recreate them, all data will be lost
    databaseHelper = DatabaseHelper(false);

    //get accessCode, accessCodeStatus and checkbox value from the database
    QHash<QString, QString> hashmap = databaseHelper.getSettings();

    accessCode = hashmap["access_code"];
    accessCodeStatusFlag = hashmap["code_status"].toInt();
    checkboxValue = hashmap["checkbox_value"].toInt();
    jsonWebToken = hashmap["json_webtoken"];

    qInfo() << "[access_code]: " + accessCode + "\n";
    qInfo() << "[code_status]: " + QString::number(accessCodeStatusFlag)+ "\n";
    qInfo() << "[checkbox_value]: " + QString::number(checkboxValue) + "\n";
    qInfo() << "[json_webtoken]: " + jsonWebToken + "\n";

    //connect to google.ca to test network connection
    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost("www.google.ca", 80);

    if(socket->waitForConnected(5000)) {
        connectionStatusFlag = true;
    }
    else {
        connectionStatusFlag = false;
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

int BackEnd::checkboxStatus() {
    return checkboxValue;
}

//setter methods
void BackEnd::setServerAccessCode(const QString &newCode) {
    if(newCode != "") {
        accessCode = newCode;

        //perform a server test with the new string
        //TODO::
        accessCodeStatusFlag = true;

        databaseHelper.updateAccessCode(accessCode);
        databaseHelper.updateAccessCodeStatus(accessCodeStatusFlag);
        databaseHelper.updateJsonWebToken("JSON WEBTOKEN NEW UPDATED VALUE");

        emit serverAccessCodeChanged();
    }
}

void BackEnd::setCheckboxStatus(const int &newValue) {
    checkboxValue = newValue;
    databaseHelper.updateCheckboxValue(checkboxValue);
}

void BackEnd::setLocationData(const QString &locationInformation) {
    if(locationInformation.length() > 1) {
        coordinates = locationInformation.left(locationInformation.indexOf("|"));
        timestamp = locationInformation.mid(locationInformation.indexOf("|") + 1);

        latitude = coordinates.left(coordinates.indexOf(" "));
        longitude = coordinates.mid(coordinates.indexOf(" ") + 1);

        QDateTime datetime = QDateTime::fromString(timestamp);
        timestamp = datetime.toString("yyyy-MM-dd hh:mm:ss");

        qInfo() << "[timestamp]: " + timestamp + "\n";
        qInfo() << "[coordinates]: " + coordinates + "\n";
        qInfo() << "[latitude]: " + latitude + "\n";
        qInfo() << "[longitude]: " + longitude + "\n";

        //TODO::
        //need to nest these, check GitLab
        json = QJsonObject();
        json["serverString"] = accessCode;
        json["latitude"] = latitude;
        json["longitude"] = longitude;
        json["timestamp"] = timestamp;

        qInfo() << "JSON OBJECT: " << json;
        //Test to see if theres an internet connection and the server is available
        //TODO::
        QTcpSocket *socket = new QTcpSocket();
        socket->connectToHost("www.google.ca", 80);

        if(socket->waitForConnected(5000)) {
            connectionStatusFlag = true;
            emit networkConnectionStatusChanged();
        }
        else {
            connectionStatusFlag = false;
            emit networkConnectionStatusChanged();
        }

        if(accessCodeStatusFlag && connectionStatusFlag) {
            //there is an internet connection
            //server string is valid
            //send ALL stored database entries, then delete the entries
            sendLocationDataFlag = true;

            emit locationDataSent();
        }
        else if(accessCodeStatusFlag) {
            //no internet connection
            //server string is invalid
            databaseHelper.updateLocation(latitude, longitude, timestamp);

            sendLocationDataFlag = false;

            emit locationDataSent();
        }
        else {
            qInfo() << "No location data stored or sent";
        }
    }
}
