#include "BackEnd.h"
#include <QtNetwork>


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

// Called when POST /location request is responded to
void BackEnd::onLocationReply(QNetworkReply *reply){
    if (reply->error() == QNetworkReply::NoError) {
        // Network is up and request succeeded

        if (connectionStatusFlag == false) {
            connectionStatusFlag = true;
            emit networkConnectionStatusChanged();

            // network was down, but it's back up now.
            // TODO: remove all locations from database and call
            // setLocationData on all of them here
        }
    } else {
        // Network is down or requst denied for some reason.
        // TODO: Check actual reason for error (network, bad request etc)
        // We can retry if its just a server error

        qDebug() << "Failure"  << reply->errorString();
        // emit signal about error
        connectionStatusFlag = false;
        emit networkConnectionStatusChanged();

        // save failed location to database (assumed network down)
        // TODO: Find way to access posted location data, pass through onLocationReply?
        // databaseHelper.updateLocation(latitude, longitude, timestamp);
    }
}

// Calls the API with provided location
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

        QJsonObject innerJson = QJsonObject();
        innerJson["latitude"] = latitude;
        innerJson["longitude"] = longitude;
        innerJson["timestamp"] = timestamp;

        json = QJsonObject();
        json["device"] = innerJson;

        QJsonDocument jsonDoc(json);
        QByteArray jsonData= jsonDoc.toJson();

        qInfo() << "JSON OBJECT: " << json;

        // Set API url
        QUrl serviceUrl = QUrl("http://192.168.1.136:3002/location");

        // Set headers
        QNetworkRequest request(serviceUrl);
        request.setRawHeader("Authorization", "Bearer: TOKEN_HERE");
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(jsonData.size()));

        network_manager = new QNetworkAccessManager(this);
        QObject::connect(network_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(onLocationReply(QNetworkReply *)));

        // Send as POST (regardless if we know connection is established)
        network_manager->post(request, jsonData);

        // Let everyone know we did it
        sendLocationDataFlag = true;
        emit locationDataSent();

    }
}
