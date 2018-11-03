#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHash>

class DatabaseHelper {
public:
    DatabaseHelper();
    DatabaseHelper(bool flag);

    void updateAccessCode(QString accessCode);
    void updateAccessCodeStatus(bool codeStatus);
    void updateCheckboxValue(int checkBoxValue);
    void updateJsonWebToken(QString jsonWebToken);

    void updateLocation(const QString &latitude, const QString &longitude, const QString &timestamp);

    QHash<QString, QString> getSettings();

private:
    void createDatabase(bool dropTablesFlag);

    QSqlDatabase database;
    QSqlQuery sqlQuery;
    QString queryString;
};

#endif // DATABASEHELPER_H
