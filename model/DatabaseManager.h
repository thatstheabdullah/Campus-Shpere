// Implementation logic for Database Management

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QDebug>

class DatabaseManager
{
public:

    // This function sets up the connection
    void openDatabase()
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        // Sets the path to the same folder as the app
        db.setDatabaseName(QCoreApplication::applicationDirPath() + "/campussphere.db");

        if (db.open())
        {
            qDebug() << "Database connected successfully!";
        }
        else
        {
            qDebug() << "Error: " << db.lastError().text();
        }
    }

    // This function creates the tables if they don't exist
    void setupTables()
    {
        QSqlQuery q;

        // 1. Users Table
        q.exec("CREATE TABLE IF NOT EXISTS users("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT NOT NULL UNIQUE, "
               "password_hash TEXT NOT NULL)");

        // 2. Posts Table
        q.exec("CREATE TABLE IF NOT EXISTS posts("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "author_username TEXT NOT NULL, "
               "text TEXT NOT NULL)");

        // 3. Activity Log
        q.exec("CREATE TABLE IF NOT EXISTS activity_log("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT, "
               "action TEXT)");
    }
};

#endif