#include "batteryinfo.h"
#include <iostream>
#include <fstream>



BatteryInfo::BatteryInfo()
{
#ifdef Q_OS_LINUX
    resourcePath = std::ifstream("/sys/class/power_supply/BAT0/capacity") ? "/sys/class/power_supply/BAT0/capacity" : "/sys/class/power_supply/BAT1/capacity";
#endif
}




int BatteryInfo::getStatus()
{

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

#ifdef _DEBUG
    std::ifstream in("in.txt");
    if (!in) qDebug() << "oops";
    int status; in >> status; in.close();
    return status;
#endif
}

bool BatteryInfo::isConnected() const
{
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
