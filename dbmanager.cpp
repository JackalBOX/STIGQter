/*
 * STIGQter - STIG fun with Qt
 *
 * Copyright © 2018 Jon Hood, http://www.hoodsecurity.com/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dbmanager.h"

#include <cstdlib>
#include <QCoreApplication>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QThread>

DbManager::DbManager() : DbManager(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId()))) { }

DbManager::DbManager(const QString& connectionName) : DbManager(QCoreApplication::applicationDirPath() + "/STIGQter.db", connectionName) { }

DbManager::DbManager(const QString& path, const QString& connectionName)
{
    bool initialize = false;

    //check if database file exists or create it
    if (!QFile::exists(path))
        initialize = true;

    //open SQLite Database
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isValid())
    {
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(path);
    }

    if (!db.open())
        qDebug() << "Error: Unable to open SQLite database.";

    if (initialize)
        UpdateDatabaseFromVersion(0);
}

void DbManager::AddControl(QString control, QString title)
{
    if (control.length() < 4)
    {
        qDebug() << "Received bad control.";
        return;
    }
    QString family(control.left(2));
    control = control.right(control.length()-3);
    QString enhancement("");
    if (control.contains('('))
    {
        int tmpIndex = control.indexOf('(');
        enhancement = control.right(control.length() - tmpIndex - 1);
        enhancement = enhancement.left(enhancement.length() - 1);
        control = control.left(control.indexOf('('));
    }

    Family f = GetFamily(family);

    if (f.id >= 0)
    {
        QSqlDatabase db;
        if (this->CheckDatabase(db))
        {
            QSqlQuery q(db);
            q.prepare("INSERT INTO Control (FamilyId, number, enhancement, title) VALUES(:FamilyId, :number, :enhancement, :title)");
            q.bindValue(":FamilyId", f.id);
            q.bindValue(":number", control.toInt());
            q.bindValue(":enhancement", enhancement.isEmpty() ? QVariant(QVariant::Int) : enhancement.toInt());
            q.bindValue(":title", title);
            q.exec();
            db.commit();
        }
    }
}

void DbManager::AddFamily(QString acronym, QString description)
{
    QSqlDatabase db;
    if (this->CheckDatabase(db))
    {
        QSqlQuery q(db);
        q.prepare("INSERT INTO Family (Acronym, Description) VALUES(:acronym, :description)");
        q.bindValue(":acronym", acronym);
        q.bindValue(":description", Sanitize(description));
        q.exec();
        db.commit();
    }
}

void DbManager::DeleteCCIs()
{
    QSqlDatabase db;
    if (this->CheckDatabase(db))
    {
        QSqlQuery q(db);
        q.prepare("DELETE FROM Family");
        q.exec();
        q.prepare("DELETE FROM Control");
        q.exec();
        db.commit();
    }
}

Family DbManager::GetFamily(int id)
{
    QSqlDatabase db;
    Family ret;
    ret.id = -1;
    if (this->CheckDatabase(db))
    {
        QSqlQuery q(db);
        q.prepare("SELECT id, acronym, description FROM Family WHERE id = :id");
        q.bindValue(":id", id);
        q.exec();
        if (q.next())
        {
            ret.id = q.value(0).toInt();
            ret.acronym = q.value(1).toString();
            ret.description = q.value(2).toString();
        }
    }
    return ret;
}

Family DbManager::GetFamily(QString acronym)
{
    QSqlDatabase db;
    Family ret;
    ret.id = -1;
    if (this->CheckDatabase(db))
    {
        QSqlQuery q(db);
        q.prepare("SELECT id, acronym, description FROM Family WHERE acronym = :acronym");
        q.bindValue(":acronym", acronym);
        q.exec();
        if (q.next())
        {
            ret.id = q.value(0).toInt();
            ret.acronym = q.value(1).toString();
            ret.description = q.value(2).toString();
        }
    }
    return ret;
}

QList<Family> DbManager::GetFamilies()
{
    QSqlDatabase db;
    QList<Family> ret;
    if (this->CheckDatabase(db))
    {
        QSqlQuery q(db);
        q.prepare("SELECT id, acronym, description FROM Family");
        q.exec();
        while (q.next())
        {
            Family f;
            f.id = q.value(0).toInt();
            f.acronym = q.value(1).toString();
            f.description = q.value(2).toString();
            ret.append(f);
        }
    }
    return ret;
}

QString DbManager::Sanitize(QString s)
{
    s = s.replace("\r\n", "\n");
    s = s.replace("\n", " ");
    return s;
}

bool DbManager::CheckDatabase(QSqlDatabase &db)
{
    db = QSqlDatabase::database(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())));
    if (!db.isOpen())
        db.open();
    if (!db.isOpen())
        return false;
    return db.isValid();
}

bool DbManager::UpdateDatabaseFromVersion(int version)
{
    QSqlDatabase db;
    if (this->CheckDatabase(db))
    {
        if (version <= 0)
        {
            //New database; initial setups
            QSqlQuery q(db);
            q.prepare("CREATE TABLE `Family` ( "
                        "`id`	INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "`Acronym`	TEXT UNIQUE, "
                        "`Description`	TEXT UNIQUE"
                        ")");
            q.exec();
            q.prepare("CREATE TABLE `Control` ( "
                        "`id`	INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "`FamilyId`	INTEGER NOT NULL, "
                        "`number`	INTEGER NOT NULL, "
                        "`enhancement`	INTEGER, "
                        "`title`	INTEGER, "
                        "FOREIGN KEY(`FamilyID`) REFERENCES `Family`(`id`) "
                        ")");
            q.exec();

            //write changes from update
            db.commit();
        }
    }

    return EXIT_SUCCESS;
}
