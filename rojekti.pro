#-------------------------------------------------
#
# Project created by QtCreator 2018-11-02T23:09:52
#
#-------------------------------------------------

# This file is part of TuxClocker.

# Copyright (c) 2019 Jussi Kuokkanen

# TuxClocker is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# TuxClocker is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with TuxClocker.  If not, see <https://www.gnu.org/licenses/>.

QT       += core
QT       += gui
QT       += widgets
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 5)

TARGET = tuxclocker
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
CONFIG (release, debug|release) {
           DEFINES += QT_NO_DEBUG_OUTPUT
       }

INCLUDEPATH += src

include(src/gui/gui.pri)
include(src/base/base.pri)

INCLUDEPATH += "/usr/lib"
INCLUDEPATH += $$(INCLUDEPATH)

LIBS += -lXext -lXNVCtrl -lX11 -lnvidia-ml

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src/resources.qrc
