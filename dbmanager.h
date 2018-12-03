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

#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QString>

#include "cci.h"
#include "control.h"
#include "family.h"
#include "stig.h"
#include "stigcheck.h"

class DbManager
{
public:
    DbManager();
    DbManager(const QString& connectionName);
    DbManager(const QString& path, const QString& connectionName);
    ~DbManager();
    void DelayCommit(bool delay);

    void AddCCI(int cci, QString control, QString definition);
    void AddControl(QString control, QString title);
    void AddFamily(QString acronym, QString description);
    void AddSTIG(STIG s, QList<STIGCheck*> c);

    void DeleteCCIs();
    void DeleteSTIG(int id);
    void DeleteSTIG(STIG s);

    CCI GetCCI(int cci, bool includeControl = true);
    CCI GetCCI(CCI cci, bool includeControl = true);
    QList<CCI> GetCCIs(bool includeControl = true);
    QList<STIGCheck*> GetSTIGChecksPtr(STIG stig, bool includeCCI = true);
    QList<STIG> GetSTIGs(bool includeChecks = true);
    Control GetControl(int id, bool includeFamily = true);
    Control GetControl(QString control, bool includeId = true);
    Family GetFamily(QString acronym);
    Family GetFamily(int id);
    QList<Family> GetFamilies();
    QString GetVariable(QString name);

    void UpdateVariable(QString name, QString value);

    QString Sanitize(QString s);

private:
    bool UpdateDatabaseFromVersion(int version);
    bool CheckDatabase(QSqlDatabase &db);
    bool _delayCommit;
};

#endif // DBMANAGER_H
