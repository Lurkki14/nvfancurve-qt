/*This file is part of TuxClocker.

Copyright (c) 2019 Jussi Kuokkanen

TuxClocker is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TuxClocker is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TuxClocker.  If not, see <https://www.gnu.org/licenses/>.*/

#include "newprofile.h"
#include "ui_newprofile.h"
#include "src/gui/mainwindow.h"

newProfile::newProfile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newProfile)
{
    ui->setupUi(this);
    listProfiles();

    //connect(ui->profileList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(editEntryName(QListWidgetItem*)));

    SignalItemDelegate *delegate = new SignalItemDelegate(ui->profileList);

    connect(delegate, SIGNAL(editFinished()), SLOT(saveChange()));
    ui->profileList->setItemDelegate(delegate);
}

newProfile::~newProfile()
{
    delete ui;
}

void newProfile::on_profileNameEdit_textChanged(const QString &arg1)
{
    newProfileName = arg1;
}
void newProfile::saveChange()
{
    //qDebug() << "edit finished";
    //QListWidgetItem *item = ui->profileList->item(latestIndex);
    newProfileList.clear();
    for (int i=0; i<ui->profileList->count(); i++) {
        newProfileList.append(ui->profileList->item(i)->text());
        //qDebug() << newProfileList[i];
        ui->profileList->item(i)->setFlags(Qt::ItemIsEnabled);
    }
}
void newProfile::on_saveButton_clicked()
{
    QSettings settings("tuxclocker");
    // Add the new ones
    for (int i=0; i<newProfileList.size(); i++) {
        settings.setValue(newProfileList[i] + "/isProfile", true);
    }
    for (int i=0; i<removedList.size(); i++) {
        settings.beginGroup(removedList[i]);
        settings.remove("");
        settings.endGroup();
    }
    close();
}

void newProfile::listProfiles()
{
    QSettings settings("tuxclocker");
    QString isProfile = "/isProfile";
    QStringList groups = settings.childGroups();
    //qDebug() << "testi";
    for (int i=0; i<groups.length(); i++) {
        // Make a query $profile/isProfile
        QString group = groups[i];
        QString query = group.append(isProfile);
        //qDebug() << query;
        if (settings.value(query).toBool()) {
            //qDebug() << groups[i];
            ui->profileList->addItem(groups[i]);
            origProfileList.append(groups[i]);
            newProfileList.append(groups[i]);
        }
    }
    // Make the items editable
    for (int i=0; i<ui->profileList->count(); i++) {
        ui->profileList->item(i)->setFlags(Qt::ItemIsEnabled);
    }
}

/*void newProfile::editEntryName(QListWidgetItem *item)
{
    qDebug() << "item dblclicked";
    ui->profileList->editItem(item);
    latestIndex = ui->profileList->currentRow();
}*/

void newProfile::on_cancelButton_clicked()
{
    close();
}
void newProfile::on_addButton_pressed()
{
    ui->profileList->addItem("");
    int itemCount = ui->profileList->count()-1;
    latestIndex = itemCount;
    ui->profileList->item(itemCount)->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
    ui->profileList->editItem(ui->profileList->item(itemCount));
}

void newProfile::on_removeButton_pressed()
{
    if (ui->profileList->count() > 1) {
        int index = ui->profileList->currentRow();
        QListWidgetItem *item = ui->profileList->item(index);
        removedList.append(item->text());
        newProfileList.removeAt(index);
        ui->profileList->takeItem(index);
        delete item;
    }
}
