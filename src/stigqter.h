/*
 * STIGQter - STIG fun with Qt
 *
 * Copyright © 2018–2020 Jon Hood, http://www.hoodsecurity.com/
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

#ifndef STIGQTER_H
#define STIGQTER_H

#include <QMainWindow>
#include <QSettings>
#include <QShortcut>

#include "dbmanager.h"

namespace Ui {
    class STIGQter;
}

class STIGQter : public QMainWindow
{
    Q_OBJECT

public:
    explicit STIGQter(QWidget *parent = nullptr);
    ~STIGQter();
    bool isProcessingEnabled();
#ifdef USE_TESTS
    void RunTests();
#endif

private Q_SLOTS:

    void CompletedThread();

    void About();
    void AddAsset();
    void AddSTIGs();
    void CloseTab(int index);
    void DeleteCCIs();
    void DeleteEmass();
    void DeleteSTIGs();
    void DownloadSTIGs();
    void ExportCKLs();
    void ExportCMRS();
    void ExportEMASS();
    void ExportHTML();
    void FindingsReport();
    void ImportCKLs();
    void ImportEMASS();
    void Load();
    void MapUnmapped();
    void OpenCKL();
    bool Reset(bool checkOnly = false);
    void Save();
    void SaveAs();
    void SelectAsset();
    void SelectSTIG();
    void ShowMessage(const QString &title, const QString &message);
    void UpdateCCIs();

    void Initialize(int max, int val = 0);
    void Progress(int val);

private:
    Ui::STIGQter *ui;
    DbManager *db;
    QList<QThread *> threads;
    QList<QObject *> workers;
    bool _updatedAssets;
    bool _updatedCCIs;
    bool _updatedSTIGs;
    QString lastSaveLocation;
    QList<QShortcut*> _shortcuts;
    void closeEvent(QCloseEvent *event);
    void CleanThreads();
    void DisableInput();
    void DisplayAssets();
    void DisplayCCIs();
    void DisplaySTIGs();
    void EnableInput();
    void UpdateSTIGs();
    bool _isTesting;
};

#endif // STIGQTER_H
