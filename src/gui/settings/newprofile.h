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

#ifndef NEWPROFILE_H
#define NEWPROFILE_H

#include <QDialog>
#include <QAbstractButton>
#include <QSettings>
#include <QListWidget>
#include <QStyledItemDelegate>

namespace Ui {
class newProfile;
}

class newProfile : public QDialog
{
    Q_OBJECT

public:
    explicit newProfile(QWidget *parent = nullptr);
    ~newProfile();

signals:
    void mousePressEvent(QMouseEvent* event);

private slots:
    void on_saveButton_clicked();
    void on_profileNameEdit_textChanged(const QString &arg1);
    void on_cancelButton_clicked();
    void listProfiles();
    //void editEntryName(QListWidgetItem *item);

    void on_addButton_pressed();

    void saveChange();
    void on_removeButton_pressed();

private:
    Ui::newProfile *ui;
    QString newProfileName;
    QStyledItemDelegate *deleg = new QStyledItemDelegate(this);
    QStringList origProfileList;
    QStringList newProfileList;
    QStringList removedList;
    int latestIndex = 0;
};

// New class for editing so we can detect when the editing has finished
class SignalItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    Q_DISABLE_COPY(SignalItemDelegate)
public:
    explicit SignalItemDelegate(QObject* parent = nullptr):QStyledItemDelegate(parent){
        QObject::connect(this,&SignalItemDelegate::closeEditor,this,&SignalItemDelegate::editFinished);
    }
    void setEditorData(QWidget *editor, const QModelIndex &index) const override {
    void editStarted();
    return QStyledItemDelegate::setEditorData(editor,index);
    }
Q_SIGNALS:
    void editStarted();
    void editFinished();
};
#endif // NEWPROFILE_H
