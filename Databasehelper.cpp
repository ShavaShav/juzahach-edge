#include "Databasehelper.h"

DatabaseHelper::DatabaseHelper() {
    //default constructor
}

DatabaseHelper::DatabaseHelper(bool flag) {
    //connect to existing database or create a new one
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("data.db");
    database.open();

    sqlQuery = QSqlQuery(database);

    createDatabase(flag);
}

//create / load the database
void DatabaseHelper::createDatabase(bool dropTablesFlag) {
    //////////////////////////////////////////////////////////////
    //[WARNING] All data will be lost if dropTablesFlag == TRUE //
    //////////////////////////////////////////////////////////////
    if(dropTablesFlag) {
        queryString =
            "DROP TABLE IF EXISTS Settings;";

        if(sqlQuery.exec(queryString)) {
            qInfo() << "\n[SQL][SUCCESS][DROP][Settings]\n";
        }
        else {
            qInfo() << "\n[SQL][FAILURE][DROP][Settings]\n";
            qInfo() << sqlQuery.lastError();
        }

        sqlQuery.finish();

        queryString =
            "DROP TABLE IF EXISTS Locations;";

        if(sqlQuery.exec(queryString)) {
            qInfo() << "\n[SQL][SUCCESS][DROP][Locations]\n";
        }
        else {
            qInfo() << "\n[SQL][FAILURE][DROP][Locations]\n";
            qInfo() << sqlQuery.lastError();
        }

        sqlQuery.finish();
    }
    //////////////////////////////////////////////////////////////

    //create the database tables, will show error if they already exist (thats okay)
    queryString =
        "CREATE TABLE Settings ( "
            "access_code TEXT NOT NULL, "
            "code_status INTEGER NOT NULL, "
            "checkbox_value INTEGER NOT NULL, "
            "json_webtoken TEXT NOT NULL "
        ");";

    if(sqlQuery.exec(queryString)) {
        qInfo() << "\n[SQL][SUCCESS][CREATE][Settings]\n";

        //table is being created for the first time
        //add dummy values
        sqlQuery.finish();

        queryString =
            "INSERT INTO Settings (access_code, code_status, checkbox_value, json_webtoken) "
            "VALUES ('access_code_placeholder', 0, 0, 'json_web_token_placeholder') "
            ";";

        if(sqlQuery.exec(queryString)) {
            qInfo() << "\n[SQL][SUCCESS][INSERT][Settings]\n";
        }
        else {
            qInfo() << "\n[SQL][FAILURE][INSERT][Settings]\n";
            qInfo() << sqlQuery.lastError();
        }
    }
    else {
        qInfo() << "\n[SQL][FAILURE][CREATE][Settings]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();

    queryString =
        "CREATE TABLE Locations ( "
            "index_entry INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            "latitude TEXT NOT NULL, "
            "longitude TEXT NOT NULL, "
            "timestamp TEXT NOT NULL "
        ");";

    if(sqlQuery.exec(queryString)) {
        qInfo() << "\n[SQL][SUCCESS][CREATE][Locations]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][CREATE][Locations]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

//get the 4 stored user settings
QHash<QString, QString> DatabaseHelper::getSettings() {
    queryString =
        "SELECT * FROM Settings;";

    if(sqlQuery.exec(queryString)) {
        qInfo() << "\n[SQL][SUCCESS][SELECT][*][Settings]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][SELECT][*][Settings]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.first();

    QHash<QString, QString> hashmap;

    hashmap.insert("access_code", sqlQuery.value(0).toString());
    hashmap.insert("code_status", sqlQuery.value(1).toString());
    hashmap.insert("checkbox_value", sqlQuery.value(2).toString());
    hashmap.insert("json_webtoken", sqlQuery.value(3).toString());

    sqlQuery.finish();

    return hashmap;
}

//update / setter methods
void DatabaseHelper::updateAccessCode(const QString accessCode) {
    qInfo() << "[VALUE] access_code: " + accessCode;

    queryString =
        "UPDATE Settings SET access_code = ':code';";

    sqlQuery.prepare(queryString);
    sqlQuery.bindValue(":code", accessCode);

    if(sqlQuery.exec(queryString)) {
        qInfo() << "\n[SQL][SUCCESS][UPDATE][Settings][access_code]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][UPDATE][Settings][access_code]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

void DatabaseHelper::updateAccessCodeStatus(const bool codeStatus) {
    qInfo() << "[VALUE] code_status: " + QString::number(codeStatus);

    //for some reason, cannot prepare with the codeStatus variable....
    if(codeStatus) {
        queryString =
             "UPDATE Settings SET code_status = 1";
    }
    else if(codeStatus) {
        queryString =
             "UPDATE Settings SET code_status = 0";
    }

    if(sqlQuery.exec(queryString)) {
        qInfo() << "\n[SQL][SUCCESS][UPDATE][Settings][code_status]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][UPDATE][Settings][code_status]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

void DatabaseHelper::updateCheckboxValue(const int checkBoxValue) {
    qInfo() << "[VALUE] checkbox_value: " + QString::number(checkBoxValue);

    //for some reason, cannot prepare with the checkBoxValue variable....
    if(checkBoxValue == 0) {
        queryString =
            "UPDATE Settings SET checkbox_value = 0";
    }
    else if(checkBoxValue == 2) {
        queryString =
            "UPDATE Settings SET checkbox_value = 2";
    }

    if(sqlQuery.exec(queryString)) {
        qInfo() << "\n[SQL][SUCCESS][UPDATE][Settings][checkbox_value]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][UPDATE][Settings][checkbox_value]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

void DatabaseHelper::updateJsonWebToken(const QString jsonWebToken) {
    qInfo() << "[VALUE] json_webtoken: "+ jsonWebToken;

    queryString =
        "UPDATE Settings SET json_webtoken = ':json';";

    sqlQuery.prepare(queryString);
    sqlQuery.bindValue(":json", jsonWebToken);

    if(sqlQuery.exec(queryString)) {
        qInfo() << "\n[SQL][SUCCESS][UPDATE][Settings][json_webtoken]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][UPDATE][Settings][json_webtoken]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

void DatabaseHelper::updateLocation(const QString &latitude, const QString &longitude, const QString &timestamp) {
    //store the coordinates in the database
    queryString =
        "INSERT INTO Locations (latitude, longitude, timestamp) "
        "VALUES (:latitude, :longitude, :timestamp);";

    sqlQuery.prepare(queryString);
    sqlQuery.bindValue(":latitude", latitude);
    sqlQuery.bindValue(":longitude", longitude);
    sqlQuery.bindValue(":timestamp", timestamp);

    if(sqlQuery.exec()) {
        qInfo() << "\n[SQL][SUCCESS][INSERT][Locations]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][INSERT][Locations]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}
