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

#include "nvidia.h"
#include <X11/Xlib.h>
#include "NVCtrl/NVCtrlLib.h"

nvidia::nvidia(QObject *parent) : QObject(parent)
{

}
bool nvidia::setupXNVCtrl()
{
    // Open the x-server connection and check if the extension exists
    dpy = XOpenDisplay(nullptr);
    Bool ret;
    int *event_basep = nullptr;
    int *error_basep = nullptr;

    ret = XNVCTRLQueryExtension(dpy,
                                event_basep,
                                error_basep);
    qDebug() << ret;

    queryGPUCount();
    queryGPUNames();
    queryGPUUIDs();
    queryGPUFeatures();
    return ret;
}
void nvidia::queryGPUCount()
{
    Bool ret;
    ret = XNVCTRLQueryTargetCount(dpy, NV_CTRL_TARGET_TYPE_GPU, &gpuCount);
    if (!ret) {
        qDebug() << "Failed to query amount of GPUs";
    }
    // Create an appropriate number of GPU objects
    for (int i=0; i<gpuCount; i++) {
        GPU gpu;
        GPUList.append(gpu);
    }
    qDebug() << GPUList.size() << "gpus";
    //setupNVML(0);
}
void nvidia::queryGPUNames()
{
    Bool ret;
    for (int i=0; i<gpuCount; i++) {
        ret = XNVCTRLQueryTargetStringAttribute(dpy,
                                                NV_CTRL_TARGET_TYPE_GPU,
                                                i,
                                                0,
                                                NV_CTRL_STRING_PRODUCT_NAME,
                                                &GPUList[i].name);
        if (!ret) {
            qDebug() << "Failed to query GPU Name";
        }
        qDebug() << GPUList[i].name;
    }
}
void nvidia::queryGPUUIDs()
{
    Bool ret;
    for (int i=0; i<gpuCount; i++) {
        ret = XNVCTRLQueryTargetStringAttribute(dpy,
                                                NV_CTRL_TARGET_TYPE_GPU,
                                                i,
                                                0,
                                                NV_CTRL_STRING_GPU_UUID,
                                                &GPUList[i].uuid);
        if (!ret) {
            qDebug() << "Failed to query GPU UUID";
        }
        qDebug() << GPUList[i].uuid;
    }
}
void nvidia::queryGPUFeatures()
{
    // Query static features related to the GPU such as maximum offsets here
    Bool ret;
    NVCTRLAttributeValidValuesRec values;
    for (int i=0; i<gpuCount; i++) {
        // Query if voltage offset is writable/readable
        ret = XNVCTRLQueryValidTargetAttributeValues(dpy,
                                                     NV_CTRL_TARGET_TYPE_GPU,
                                                     i,
                                                     0,
                                                     NV_CTRL_GPU_OVER_VOLTAGE_OFFSET,
                                                     &values);
        if ((values.permissions & ATTRIBUTE_TYPE_WRITE) == ATTRIBUTE_TYPE_WRITE) {
            GPUList[i].overVoltAvailable = true;
            GPUList[i].voltageReadable = true;
            // If the feature is writable save the offset range
            GPUList[i].minVoltageOffset = values.u.range.min;
            GPUList[i].maxVoltageOffset = values.u.range.max;
            //qDebug() << values.u.range.min << values.u.range.max << "offset range";
        } else {
            // Check if it's readable
            if ((values.permissions & ATTRIBUTE_TYPE_READ) == ATTRIBUTE_TYPE_READ) {
                GPUList[i].voltageReadable = true;
            }
        }

        // Query if core clock offset is writable
        ret = XNVCTRLQueryValidTargetAttributeValues(dpy,
                                                     NV_CTRL_TARGET_TYPE_GPU,
                                                     i,
                                                     3,
                                                     NV_CTRL_GPU_NVCLOCK_OFFSET,
                                                     &values);
        if ((values.permissions & ATTRIBUTE_TYPE_WRITE) == ATTRIBUTE_TYPE_WRITE) {
            GPUList[i].overClockAvailable = true;
            GPUList[i].coreClkReadable = true;
            GPUList[i].minCoreClkOffset = values.u.range.min;
            GPUList[i].maxCoreClkOffset = values.u.range.max;
            //qDebug() << values.u.range.min << values.u.range.max << "offset range";
        } else {
            if ((values.permissions & ATTRIBUTE_TYPE_READ) == ATTRIBUTE_TYPE_READ) {
                GPUList[i].coreClkReadable = true;
            }
        }

        // Query if memory clock offset is writable
        ret = XNVCTRLQueryValidTargetAttributeValues(dpy,
                                                     NV_CTRL_TARGET_TYPE_GPU,
                                                     i,
                                                     3,
                                                     NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET,
                                                     &values);
        if ((values.permissions & ATTRIBUTE_TYPE_WRITE) == ATTRIBUTE_TYPE_WRITE) {
            GPUList[i].memOverClockAvailable = true;
            GPUList[i].memClkReadable = true;
            GPUList[i].minMemClkOffset = values.u.range.min;
            GPUList[i].maxMemClkOffset = values.u.range.max;
            qDebug() << values.u.range.min << values.u.range.max << "offset range";
        } else {
            if ((values.permissions & ATTRIBUTE_TYPE_READ) == ATTRIBUTE_TYPE_READ) {
                GPUList[i].memClkReadable = true;
            }
        }
        // Query if fan control mode is writable
        ret = XNVCTRLQueryValidTargetAttributeValues(dpy,
                                                     NV_CTRL_TARGET_TYPE_GPU,
                                                     i,
                                                     0,
                                                     NV_CTRL_GPU_COOLER_MANUAL_CONTROL,
                                                     &values);
        if ((values.permissions & ATTRIBUTE_TYPE_WRITE) == ATTRIBUTE_TYPE_WRITE) {
            GPUList[i].manualFanCtrlAvailable = true;
            qDebug() << "fan control available";
            // Query fan control mode
            int retval;
            ret = XNVCTRLQueryTargetAttribute(dpy,
                                              NV_CTRL_TARGET_TYPE_GPU,
                                              i,
                                              0,
                                              NV_CTRL_GPU_COOLER_MANUAL_CONTROL,
                                              &retval);
            if ((retval & NV_CTRL_GPU_COOLER_MANUAL_CONTROL_TRUE) == NV_CTRL_GPU_COOLER_MANUAL_CONTROL_TRUE) {
                qDebug() << "fanctl on";
                GPUList[i].fanControlMode = 1;
            } else {
                GPUList[i].fanControlMode = 0;
                qDebug() << "fanctl off";
            }
        }
        // Query amount of VRAM
        ret = XNVCTRLQueryTargetAttribute(dpy,
                                          NV_CTRL_TARGET_TYPE_GPU,
                                          i,
                                          0,
                                          NV_CTRL_TOTAL_DEDICATED_GPU_MEMORY,
                                          &GPUList[i].totalVRAM);
        qDebug() << GPUList[i].totalVRAM << "vram";
    }
}
void nvidia::queryGPUVoltage(int GPUIndex)
{
    Bool ret;
    ret = XNVCTRLQueryTargetAttribute(dpy,
                                      NV_CTRL_TARGET_TYPE_GPU,
                                      GPUIndex,
                                      0,
                                      NV_CTRL_GPU_CURRENT_CORE_VOLTAGE,
                                      &GPUList[GPUIndex].voltage);
    if (!ret) {
        qDebug() << "Couldn't query voltage for GPU";
    }
    qDebug() << GPUList[GPUIndex].voltage;
}
void nvidia::queryGPUTemp(int GPUIndex)
{
    qDebug() << GPUList.size();
    Bool ret;
    ret = XNVCTRLQueryTargetAttribute(dpy,
                                      NV_CTRL_TARGET_TYPE_THERMAL_SENSOR,
                                      GPUIndex,
                                      0,
                                      NV_CTRL_THERMAL_SENSOR_READING,
                                      &GPUList[GPUIndex].temp);
    if (!ret) {
        qDebug() << "failed to query GPU temperature";
    }
    qDebug() << GPUList[GPUIndex].temp;
}
void nvidia::queryGPUFrequencies(int GPUIndex)
{
    Bool ret;
    int packedInt = 0;
    int mem = 0;
    int core = 0;
    ret = XNVCTRLQueryTargetAttribute(dpy,
                                      NV_CTRL_TARGET_TYPE_GPU,
                                      GPUIndex,
                                      0,
                                      NV_CTRL_GPU_CURRENT_CLOCK_FREQS,
                                      &packedInt);
    // The function returns a 32-bit packed integer, GPU Clock is the upper 16 and Memory Clock lower 16
    mem = (packedInt) & 0xFFFF;
    core = (packedInt & (0xFFFF << (32 - 16))) >> (32 - 16);
    qDebug() << GPUList[GPUIndex].coreFreq << "freq" << core << mem;
    GPUList[GPUIndex].coreFreq = core;
    GPUList[GPUIndex].memFreq = mem;
}
void nvidia::queryGPUFanSpeed(int GPUIndex)
{
    Bool ret;
    ret = XNVCTRLQueryTargetAttribute(dpy,
                                      NV_CTRL_TARGET_TYPE_COOLER,
                                      GPUIndex,
                                      0,
                                      NV_CTRL_THERMAL_COOLER_CURRENT_LEVEL,
                                      &GPUList[GPUIndex].fanSpeed);

}
void nvidia::queryGPUUsedVRAM(int GPUIndex)
{
    Bool ret = XNVCTRLQueryTargetAttribute(dpy,
                                           NV_CTRL_TARGET_TYPE_GPU,
                                           GPUIndex,
                                           0,
                                           NV_CTRL_USED_DEDICATED_GPU_MEMORY,
                                           &GPUList[GPUIndex].usedVRAM);
    if (!ret) {
        qDebug() << "failed to query used VRAM";
    }
    qDebug() << GPUList[GPUIndex].usedVRAM << "usedvram";
}
void nvidia::queryGPUFreqOffset(int GPUIndex)
{
    Bool ret = XNVCTRLQueryTargetAttribute(dpy,
                                           NV_CTRL_TARGET_TYPE_GPU,
                                           GPUIndex,
                                           3,
                                           NV_CTRL_GPU_NVCLOCK_OFFSET,
                                           &GPUList[GPUIndex].coreClkOffset);
}
void nvidia::queryGPUMemClkOffset(int GPUIndex)
{
    Bool ret = XNVCTRLQueryTargetAttribute(dpy,
                                           NV_CTRL_TARGET_TYPE_GPU,
                                           GPUIndex,
                                           3,
                                           NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET,
                                           &GPUList[GPUIndex].memClkOffset);
}
void nvidia::queryGPUVoltageOffset(int GPUIndex)
{
    Bool ret = XNVCTRLQueryTargetAttribute(dpy,
                                           NV_CTRL_TARGET_TYPE_GPU,
                                           GPUIndex,
                                           0,
                                           NV_CTRL_GPU_OVER_VOLTAGE_OFFSET,
                                           &GPUList[GPUIndex].voltageOffset);
    qDebug() << GPUList[GPUIndex].voltageOffset << "offset";
}

bool nvidia::assignGPUFanSpeed(int GPUIndex, int targetValue)
{
    Bool ret;
    ret = XNVCTRLSetTargetAttributeAndGetStatus(dpy,
                                                NV_CTRL_TARGET_TYPE_COOLER,
                                                GPUIndex,
                                                0,
                                                NV_CTRL_THERMAL_COOLER_LEVEL,
                                                targetValue);
    return ret;
}
bool nvidia::assignGPUFanCtlMode(int GPUIndex, int targetValue)
{
    Bool ret = XNVCTRLSetTargetAttributeAndGetStatus(dpy,
                                                     NV_CTRL_TARGET_TYPE_GPU,
                                                     GPUIndex,
                                                     0,
                                                     NV_CTRL_GPU_COOLER_MANUAL_CONTROL,
                                                     targetValue);
    return ret;
}
bool nvidia::assignGPUFreqOffset(int GPUIndex, int targetValue)
{
    Bool ret = XNVCTRLSetTargetAttributeAndGetStatus(dpy,
                                                     NV_CTRL_TARGET_TYPE_GPU,
                                                     GPUIndex,
                                                     3, // This argument is the performance mode
                                                     NV_CTRL_GPU_NVCLOCK_OFFSET,
                                                     targetValue);
    qDebug() << ret << "freqassign";
    return ret;
}
bool nvidia::assignGPUMemClockOffset(int GPUIndex, int targetValue)
{
    Bool ret = XNVCTRLSetTargetAttributeAndGetStatus(dpy,
                                                     NV_CTRL_TARGET_TYPE_GPU,
                                                     GPUIndex,
                                                     3,
                                                     NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET,
                                                     targetValue);
    qDebug() << ret << "memassign";
    return ret;
}
bool nvidia::assignGPUVoltageOffset(int GPUIndex, int targetValue)
{
    Bool ret = XNVCTRLSetTargetAttributeAndGetStatus(dpy,
                                                     NV_CTRL_TARGET_TYPE_GPU,
                                                     GPUIndex,
                                                     0,
                                                     NV_CTRL_GPU_OVER_VOLTAGE_OFFSET,
                                                     targetValue);
    qDebug() << ret;
    return ret;
}
/*bool nvidia::setup()
{
    dpy = XOpenDisplay(nullptr);
    int *temp;
    XNVCTRLQueryTargetAttribute(dpy,
                                NV_CTRL_TARGET_TYPE_THERMAL_SENSOR,
                                0,
                                0,
                                NV_CTRL_THERMAL_SENSOR_READING,
                                temp);
    qDebug() << temp;
    return true;
}*/
bool nvidia::setupNVML(int GPUIndex)
{
    nvmlDevice_t *dev = new nvmlDevice_t;
    device = dev;
    nvmlReturn_t ret = nvmlInit();
    char name[64];
    nvmlDeviceGetHandleByIndex(GPUIndex, dev);
    //nvmlDeviceGetName(dev, name, sizeof(name)/sizeof(name[0]));
    qDebug() << name << "from nvml";
    if (NVML_SUCCESS != ret) {
        return false;
    }
    //queryGPUUtils(0);
    //queryGPUPowerLimitLimits(0);
    //queryGPUPowerDraw(0);
    //qDebug() << assignGPUPowerLimit(150000);
    return true;
}
void nvidia::queryGPUUtils(int GPUIndex)
{
    nvmlUtilization_t utils;
    nvmlReturn_t ret = nvmlDeviceGetUtilizationRates(*device, &utils);
    if (NVML_SUCCESS != ret) {
        qDebug() << "failed to query GPU utilizations";
    }
    qDebug() << utils.gpu << utils.memory << "utils";
    GPUList[GPUIndex].memUtil = utils.memory;
    GPUList[GPUIndex].coreUtil = utils.gpu;
}
void nvidia::queryGPUPowerDraw(int GPUIndex)
{
    uint usg;
    nvmlReturn_t ret = nvmlDeviceGetPowerUsage(*device, &usg);
    if (NVML_SUCCESS != ret) {
        qDebug() << "failed to query power usage";
    } else {
        GPUList[GPUIndex].powerDraw = usg;
        qDebug() << usg << "pwrusg";
    }
}
void nvidia::queryGPUPowerLimitLimits(int GPUIndex)
{
    uint maxlim;
    uint minlim;
    nvmlReturn_t ret = nvmlDeviceGetPowerManagementLimitConstraints(*device, &minlim, &maxlim);
    if (NVML_SUCCESS != ret) {
        qDebug() << "failed to query power limit constraints";
    } else {
        GPUList[GPUIndex].minPowerLim = minlim;
        GPUList[GPUIndex].maxPowerLim = maxlim;
        qDebug() << minlim << maxlim << "powerlims";
    }
}
void nvidia::queryGPUPowerLimit(int GPUIndex)
{
    uint lim;
    nvmlReturn_t ret = nvmlDeviceGetPowerManagementLimit(*device, &lim);
    if (NVML_SUCCESS != ret) {
        qDebug() << "failed to query power limit";
    } else {
        GPUList[GPUIndex].powerLim = lim;

    }
}
void nvidia::queryGPUCurrentMaxClocks(int GPUIndex)
{
    uint curmax;
    // Query maximum core clock
    nvmlReturn_t ret = nvmlDeviceGetMaxClockInfo(*device, NVML_CLOCK_GRAPHICS, &curmax);
    if (NVML_SUCCESS != ret) {
        qDebug() << "Failed to query maximum core clock";
    } else {
        GPUList[GPUIndex].maxCoreClk = curmax;
    }

    // Query maximum memory clock
    ret = nvmlDeviceGetMaxClockInfo(*device, NVML_CLOCK_MEM, &curmax);
    if (NVML_SUCCESS != ret) {
        qDebug() << "Failed to query maximum memory clock";
    } else {
        GPUList[GPUIndex].maxMemClk = curmax;
        qDebug() << curmax << "current max clock";
    }
}
void nvidia::queryGPUPowerLimitAvailability(int GPUIndex)
{
    // Assign the current power limit to see if modifying it is available
    nvmlReturn_t ret = nvmlDeviceSetPowerManagementLimit(*device, GPUList[GPUIndex].powerLim);
    if (NVML_ERROR_NOT_SUPPORTED == ret) {
        GPUList[GPUIndex].powerLimitAvailable = false;
    } else {
        GPUList[GPUIndex].powerLimitAvailable = true;
    }
}
bool nvidia::assignGPUPowerLimit(uint targetValue)
{
    nvmlReturn_t ret = nvmlDeviceSetPowerManagementLimit(*device, targetValue);
    if (NVML_SUCCESS != ret) {
        return false;
    }
    return true;
}
