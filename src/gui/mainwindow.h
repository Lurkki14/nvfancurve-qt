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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settings/editprofile.h"
#include <QProcess>
#include <QList>
#include <QByteArray>
#include "src/base/nvidia/nvidia.h"
//#include <NVCtrl/NVCtrl.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString currentProfile;
    QString nvFanQ = "/bin/sh -c \"nvidia-smi --query-gpu=fan.speed --format=csv | egrep -o '[0-9]{1,4}'\"";
    QString nvVoltQ = "nvidia-settings -q GPUCurrentCoreVoltage -t";
    QString nvVoltOfsQ = "nvidia-settings -q GPUOverVoltageOffset -t";
    QString nvVoltOfsLimQ = "/bin/sh -c \"nvidia-settings -a GPUOverVoltageOffset=99999999 | egrep -o '[0-9]{1,9}'\"";
    QString nvCoreClkOfsQ = "nvidia-settings -q GPUGraphicsClockOffset[3] -t";
    QString nvCurMaxClkQ = "/bin/sh -c \"nvidia-smi --query-supported-clocks=gr --format=csv | egrep -o '[0-9]{2,9}'\"";
    QString nvMaxPowerLimQ = "/bin/sh -c \"nvidia-smi --query-gpu=power.max_limit --format=csv | egrep -o '[0-9]{1,7}'\"";
    QString nvMinPowerLimQ = "/bin/sh -c \"nvidia-smi --query-gpu=power.min_limit --format=csv | egrep -o '[0-9]{1,7}'\"";
    QString nvCurPowerLimQ = "/bin/sh -c \"nvidia-smi --query-gpu=power.limit --format=csv | egrep -o '[0-9]{1,7}'\"";
    QString nvClockLimQ = "/bin/sh -c \"nvidia-settings -a GPUGraphicsClockOffset[3]=999999 | egrep -o '[-0-9]{2,9}'\"";
    QString nvMemClkLimQ = "/bin/sh -c \"nvidia-settings -a GPUMemoryTransferRateOffset[3]=999999 | egrep -o '[-0-9]{2,9}'\"";
    QString nvCurMaxMemClkQ = "/bin/sh -c \"nvidia-smi --query-supported-clocks=mem --format=csv | egrep -o '[0-9]{2,9}'\"";
    QString nvCurMemClkOfsQ = "nvidia-settings -q GPUMemoryTransferRateOffset[3] -t";
    QString nvTempQ = "nvidia-smi --query-gpu=temperature.gpu --format=csv,noheader,nounits";

    QString nvCoreClkSet = "nvidia-settings -a GPUGraphicsClockOffset[3]=";
    QString nvMemClkSet = "nvidia-settings -a GPUMemoryTransferRateOffset[3]=";
    QString nvPowerLimSet = "nvidia-smi -pl ";
    QString nvFanSpeedSet = "nvidia-settings -a GPUTargetFanSpeed=";
    QString nvVoltageSet = "nvidia-settings -a GPUOverVoltageOffset=";

    QString nvFanCtlStateSet = "nvidia-settings -a GPUFanControlState=";

    QString nvFanCtlStateQ = "nvidia-settings -q GPUFanControlState -t";

    QString grepStringToInt = " | egrep -o '[0-9]{0,100}'\"";

    QString queryForNvidiaProp = "/bin/sh -c \"lspci -vnn | grep -c 'Kernel driver in use: nvidia'\"";
    QString queryGPUName = "nvidia-smi --query-gpu=gpu_name --format=csv,noheader";
    QString nvGPUCountQ = "nvidia-smi --query-gpu=count --format=csv,noheader";
    QString nvUUIDQ = "nvidia-smi --query-gpu=uuid --format=csv,noheader";

    QString errorText = "Failed to apply these settings: ";

    QString gpuDriver;
    QVector <int> xCurvePoints, yCurvePoints;
    int currentGPUIndex = 0;

    int voltInt = 0;
    int voltOfsInt = 0;
    int coreFreqOfsInt = 0;

    int maxPowerLimInt = 0;
    int minPowerLimInt = 0;
    int curPowerLimInt = 0;

    int minCoreClkOfsInt = 0;
    int maxCoreClkOfsInt = 0;
    int curMaxClkInt = 0;
    int minMemClkOfsInt = 0;
    int maxMemClkOfsInt = 0;
    int minVoltOfsInt = 0;
    int maxVoltOfsInt = 0;
    int curMaxMemClkInt = 0;
    int memClkOfsInt = 0;
    int fanSpeed = 0;
    int temp = 0;
    int targetFanSpeed = 0;
    int fanControlMode = 0;

    int defCoreClk = 0;
    int defMemClk = 0;
    int defVolt = 0;

    int latestClkOfs = 0;
    int latestPowerLim = 0;
    int latestMemClkOfs = 0;
    int latestVoltOfs = 0;

    bool isRoot = false;
public slots:

private slots:

    void on_actionEdit_current_profile_triggered(bool checked);

    void on_profileComboBox_activated(const QString &arg1);
    void on_frequencySlider_valueChanged(int value);
    void on_frequencySpinBox_valueChanged(int arg1);

    void on_newProfile_clicked();

    void on_powerLimSlider_valueChanged(int value);

    void on_powerLimSpinBox_valueChanged(int arg1);

    void on_newProfile_closed();
    void on_memClkSlider_valueChanged(int value);

    void on_memClkSpinBox_valueChanged(int arg1);

    void on_voltageSlider_valueChanged(int value);

    void on_voltageSpinBox_valueChanged(int arg1);

    void fanSpeedUpdater();
    void applyGPUSettings();
    void on_fanSlider_valueChanged(int value);

    void on_fanSpinBox_valueChanged(int arg1);

    void on_applyButton_clicked();

    void getGPUDriver();
    void generateFanPoint();
    void checkForRoot();
    void tempUpdater();
    void resetChanges();
    void resetTimer();

    void on_editFanCurveButton_pressed();

    void on_editProfile_closed();
    void applyFanMode();
    void enableFanUpdater();
    void setupMonitorTab();
    void updateMonitor();
    void saveProfileSettings();
    void loadProfileSettings();
    void checkForProfiles();
    void on_fanModeComboBox_currentIndexChanged(int index);
    void tabHandler(int index);
    void setupGraphMonitorTab();
    void plotHovered(QMouseEvent *event);
    void clearPlots();
    void clearExtremeValues();
    void on_actionManage_profiles_triggered();

    void on_GPUComboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    bool noProfiles = true;
    QStringList UUIDList;
    QString latestUUID;
    nvidia *nv;

    QTimer *resettimer = new QTimer(this);
    QTimer *fanUpdateTimer = new QTimer(this);
    QTimer *statusLabelResetTimer = new QTimer(this);
    QTimer *fanUpdaterDisablerTimer = new QTimer(this);
    QTimer *monitorUpdater = new QTimer(this);
    QTimer *plotHoverUpdater = new QTimer(this);

    QTreeWidgetItem *gputemp = new QTreeWidgetItem;
    QTreeWidgetItem *powerdraw = new QTreeWidgetItem;
    QTreeWidgetItem *voltage = new QTreeWidgetItem;
    QTreeWidgetItem *coreclock = new QTreeWidgetItem;
    QTreeWidgetItem *memclock = new QTreeWidgetItem;
    QTreeWidgetItem *coreutil = new QTreeWidgetItem;
    QTreeWidgetItem *memutil = new QTreeWidgetItem;
    QTreeWidgetItem *fanspeed = new QTreeWidgetItem;
    QTreeWidgetItem *memusage = new QTreeWidgetItem;
    QTreeWidgetItem *curmaxclk = new QTreeWidgetItem;
    QTreeWidgetItem *curmaxmemclk = new QTreeWidgetItem;
    QTreeWidgetItem *curpowerlim = new QTreeWidgetItem;

    // Widgets for the graph monitor
    QWidget *plotWidget = new QWidget;
    QScrollArea *plotScrollArea = new QScrollArea;
    QVBoxLayout *lo = new QVBoxLayout;

    QVBoxLayout *plotLayout = new QVBoxLayout;

    QCustomPlot *tempPlot = new QCustomPlot(this);
    QCustomPlot *powerDrawPlot = new QCustomPlot(this);
    QCustomPlot *coreClkPlot = new QCustomPlot(this);
    QCustomPlot *memClkPlot = new QCustomPlot(this);
    QCustomPlot *coreUtilPlot = new QCustomPlot(this);
    QCustomPlot *memUtilPlot = new QCustomPlot(this);
    QCustomPlot *voltagePlot = new QCustomPlot(this);
    QCustomPlot *fanSpeedPlot = new QCustomPlot(this);

    //QVector <double> qv_time;

    struct plotCmds
    {
        QVector <double> vector;
        double valueq;
        double maxval;
        double minval;
        QCustomPlot *plot;
        QVBoxLayout *layout;
        QWidget *widget;
        QCPTextElement *mintext;
        QCPTextElement *maxtext;
        QCPTextElement *curtext;
        QCPItemTracer *tracer;
        QCPItemText *valText;
    };
    struct datavector {
        QVector <double> vector;
    };

    struct GPUData {
        QVector <datavector> data;
        QVector <double> qv_time;
    };
    QVector <GPUData> GPU;
    int counter = 0;
    // The maximum size of plot data vectors (range +1)
    int  plotVectorSize = 181;

    plotCmds powerdrawplot;
    plotCmds tempplot;
    plotCmds coreclkplot;
    plotCmds memclkplot;
    plotCmds coreutilplot;
    plotCmds memutilplot;
    plotCmds voltageplot;
    plotCmds fanspeedplot;
    QVector <plotCmds> plotCmdsList;

};

#endif // MAINWINDOW_H
