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

#include "assetview.h"
#include "dbmanager.h"
#include "stig.h"
#include "stigcheck.h"
#include "ui_assetview.h"

#include <QInputDialog>
#include <QMessageBox>

AssetView::AssetView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssetView),
    _justification()
{
    ui->setupUi(this);
}

AssetView::AssetView(const Asset &a, QWidget *parent) : AssetView(parent)
{
    _a = a;
    Display();
}

AssetView::~AssetView()
{
    delete ui;
}

void AssetView::Display()
{
    SelectSTIGs();
    ShowChecks();
}

void AssetView::SelectSTIGs()
{
    DbManager db;
    ui->lstSTIGs->clear();
    QList<STIG> stigs = _a.STIGs();
    foreach (const STIG s, db.GetSTIGs())
    {
        QListWidgetItem *i = new QListWidgetItem(PrintSTIG(s));
        ui->lstSTIGs->addItem(i);
        i->setData(Qt::UserRole, QVariant::fromValue<STIG>(s));
        i->setSelected(stigs.contains(s));
    }
}

void AssetView::ShowChecks()
{
    ui->lstChecks->clear();
    int total = 0;
    int open = 0;
    int closed = 0;
    foreach(const CKLCheck c, _a.CKLChecks())
    {
        total++;
        switch (c.status)
        {
        case Status::NotAFinding:
            closed++;
            break;
        case Status::Open:
            open++;
            break;
        default:
            break;
        }
        QListWidgetItem *i = new QListWidgetItem(PrintCKLCheck(c));
        ui->lstChecks->addItem(i);
        i->setData(Qt::UserRole, QVariant::fromValue<CKLCheck>(c));
    }
    ui->lblTotalChecks->setText(QString::number(total));
    ui->lblOpen->setText(QString::number(open));
    ui->lblNotAFinding->setText(QString::number(closed));
    ui->lstChecks->sortItems();
}

void AssetView::UpdateCKLCheck(const CKLCheck &cc)
{
    ui->txtComments->blockSignals(true);
    ui->txtFindingDetails->blockSignals(true);
    ui->cboBoxStatus->blockSignals(true);
    ui->cboBoxSeverity->blockSignals(true);
    ui->cboBoxStatus->setCurrentText(GetStatus(cc.status));
    ui->txtComments->clear();
    ui->txtComments->insertPlainText(cc.comments);
    ui->txtFindingDetails->clear();
    ui->txtFindingDetails->insertPlainText(cc.findingDetails);
    _justification = cc.severityJustification;

    UpdateSTIGCheck(cc.STIGCheck());
    if (cc.severityOverride != Severity::none)
        ui->cboBoxSeverity->setCurrentText(GetSeverity(cc.severityOverride));

    ui->txtComments->blockSignals(false);
    ui->txtFindingDetails->blockSignals(false);
    ui->cboBoxStatus->blockSignals(false);
    ui->cboBoxSeverity->blockSignals(false);
}

void AssetView::UpdateSTIGCheck(const STIGCheck &sc)
{
    ui->lblCheckRule->setText(sc.rule);
    ui->lblCheckTitle->setText(sc.title);
    ui->cboBoxSeverity->setCurrentText(GetSeverity(sc.severity));
    ui->cbDocumentable->setChecked(sc.documentable);
    ui->lblDiscussion->setText(sc.vulnDiscussion);
    ui->lblFalsePositives->setText(sc.falsePositives);
    ui->lblFalseNegatives->setText(sc.falseNegatives);
    ui->lblFix->setText(sc.fix);
    ui->lblCheck->setText(sc.check);
}

void AssetView::UpdateCKL()
{
    QList<QListWidgetItem*> selectedItems = ui->lstChecks->selectedItems();
    if (selectedItems.count() > 0)
    {
        CKLCheck cc = selectedItems.first()->data(Qt::UserRole).value<CKLCheck>();
        cc.comments = ui->txtComments->toPlainText();
        cc.findingDetails = ui->txtFindingDetails->toPlainText();
        Severity tmpSeverity = GetSeverity(ui->cboBoxSeverity->currentText());
        cc.severityOverride = (tmpSeverity == cc.STIGCheck().severity) ? cc.severityOverride = Severity::none : tmpSeverity;
        cc.status = GetStatus(ui->cboBoxStatus->currentText());
        cc.severityJustification = _justification;
        DbManager db;
        db.UpdateCKLCheck(cc);
    }
}

void AssetView::UpdateCKLSeverity(const QString &val)
{
    QList<QListWidgetItem*> selectedItems = ui->lstChecks->selectedItems();
    if (selectedItems.count() > 0)
    {
        CKLCheck cc = selectedItems.first()->data(Qt::UserRole).value<CKLCheck>();
        STIGCheck sc = cc.STIGCheck();
        Severity tmpSeverity = GetSeverity(val);
        if (sc.severity != tmpSeverity)
        {
            if (tmpSeverity == Severity::none)
            {
                QMessageBox::warning(nullptr, "Removed Severity Override", "Severity override is removed; findings cannot be downgraded to CAT IV.");
                _justification = "";
                ui->cboBoxSeverity->blockSignals(true);
                ui->cboBoxSeverity->setCurrentText(GetSeverity(sc.severity));
                ui->cboBoxSeverity->blockSignals(false);
            }
            else
            {
                bool ok(false);
                QString justification = QInputDialog::getMultiLineText(this, tr("Severity Override Justification"),
                                        tr("Justification:"), _justification, &ok);
                if (ok)
                {
                    _justification = justification;
                }
                else
                {
                    ui->cboBoxSeverity->blockSignals(true);
                    ui->cboBoxSeverity->setCurrentText(GetSeverity(sc.severity));
                    ui->cboBoxSeverity->blockSignals(false);
                    return;
                }
            }
        }
        UpdateCKL();
    }
}

void AssetView::CheckSelected(QListWidgetItem *current, QListWidgetItem *previous [[maybe_unused]])
{
    if (current)
    {
        CKLCheck cc = current->data(Qt::UserRole).value<CKLCheck>();
        DbManager db;
        UpdateCKLCheck(db.GetCKLCheck(cc));
    }
}
