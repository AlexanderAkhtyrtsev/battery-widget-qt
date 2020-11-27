#include "batteryinfo.h"
#include <iostream>
#include <fstream>

#ifdef _DEBUG

#define IS_CHARGING true
#define STATUS 77
#define CONNECTED true

#endif


BatteryInfo::BatteryInfo()
{
#ifdef Q_OS_LINUX
    resourcePath = std::ifstream("/sys/class/power_supply/BAT0/capacity") ?
                "/sys/class/power_supply/BAT0/capacity" : "/sys/class/power_supply/BAT1/capacity";
#endif
}




int BatteryInfo::getStatus()
{
#ifdef _DEBUG
    return STATUS;
#endif


#ifdef Q_OS_WIN
    SYSTEM_POWER_STATUS ps;
    GetSystemPowerStatus(&ps);
    return ps.BatteryLifePercent;
#endif

#ifdef Q_OS_LINUX
    std::ifstream ifs(resourcePath.toStdString());
    int percent;

    if (!ifs.is_open()) {
        std::cerr << "No battery found :(\n";
        return -1;
    }

    ifs >> percent;
    ifs.close();
    return percent;
#endif
}

bool BatteryInfo::isConnected() const
{

#ifdef _DEBUG
return CONNECTED;
#endif

#ifdef Q_OS_WIN
    SYSTEM_POWER_STATUS ps;
    GetSystemPowerStatus(&ps);
    return ps.BatteryFlag < 128;
#endif

#ifdef Q_OS_LINUX
    std::ifstream in(resourcePath.toStdString());
    if (in.is_open()) {
        in.close();
        return true;
    }

    return false;
#endif
}

bool BatteryInfo::isCharging()
{

#ifdef _DEBUG
return IS_CHARGING;
#endif

#ifdef Q_OS_WIN
    SYSTEM_POWER_STATUS ps;
    GetSystemPowerStatus(&ps);
    m_ac = ps.ACLineStatus;
    return ps.BatteryFlag & 8;
#endif

#ifdef Q_OS_LINUX
    std::ifstream in(resourcePath.toStdString());
    if (in.is_open()) {
        std::string status;
        in >> status;
        in.close();
        return (status == "Charging");
    }

    return false;
#endif
}
