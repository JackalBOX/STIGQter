/*
 * STIGQter - STIG fun with Qt
 *
 * Copyright © 2018–2019 Jon Hood, http://www.hoodsecurity.com/
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

#ifndef COMMON_H
#define COMMON_H

#include <QByteArrayList>
#include <QFile>
#include <QNetworkReply>

#define VERSION "0.1.1"

bool DownloadFile(const QUrl &u, QFile *f);
QString DownloadPage(const QUrl &u);
QString Excelify(const QString &s);
int GetCCINumber(QString cci);
QMap<QString, QByteArray> GetFilesFromZip(const QString &fileName, QString fileNameFilter = "");
QString PrintTrueFalse(bool tf);
QString CleanXML(QString s, bool isXml = false);

#endif // COMMON_H
