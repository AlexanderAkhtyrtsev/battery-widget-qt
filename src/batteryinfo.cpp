#include "batteryinfo.h"

#ifdef Q_OS_LINUX
#include <QFileInfo>
#include <QDir>

#include <iostream>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#endif



BatteryInfo::BatteryInfo()
{

#ifdef Q_OS_LINUX
    QString absolutePath = "/sys/class/power_supply/";
#ifdef _DEBUG
    absolutePath = "/home/seven/";
#endif
    capacity = nullptr;
    status   = nullptr;


    // Find path of battery in proc filesystem.

    QDirIterator it(absolutePath);

    // Iterate through the directory using the QDirIterator
    while (it.hasNext()) {
        QString filename = it.next();
        QFileInfo file(filename);

        if (!file.isDir()) { // Check if it's a dir
            continue;
        }

        if (file.fileName().contains(QString("BAT"), Qt::CaseInsensitive)) {
            absolutePath += file.fileName();

            capacity = new QFile(absolutePath +  "/capacity");
            status   = new QFile(absolutePath +  "/status");

            capacity->open(QIODevice::ReadOnly);
            status->open(QIODevice::ReadOnly);

            std::cout << "reading data from " << absolutePath.toStdString() << std::endl;
            break;
        }
    }

    if (!status || !capacity) {
        std::cerr << "no battery connected :(";
    }
#endif
}


BatteryInfo::~BatteryInfo()
{
#ifdef Q_OS_LINUX
    if(status) status->close();
    if (capacity)capacity->close();
    delete status;
    delete capacity;
#endif
}



int BatteryInfo::getCapacity() const
{
#ifdef Q_OS_WIN
    SYSTEM_POWER_STATUS ps;
    GetSystemPowerStatus(&ps);
    return ps.BatteryLifePercent;
#endif

#ifdef Q_OS_LINUX

    if (!this->capacity || !this->capacity->isOpen()) {
        std::cerr << "No battery found :(\n";
        return -1;
    }

    QTextStream in(capacity);
    in.seek(0);
    int percent;
    in >> percent;
    return percent;
#endif
}

#ifdef Q_OS_LINUX
const QString BatteryInfo::readStatus() const
{
    QString currentStatus = "";

    if (this->status && this->status->isOpen()) {
        QTextStream in(this->status);
        in.seek(0);
        in >> currentStatus;
    }

    return currentStatus;
}

#endif
bool BatteryInfo::isConnected() const
{
#ifdef Q_OS_WIN
    SYSTEM_POWER_STATUS ps;
    GetSystemPowerStatus(&ps);
    return ps.BatteryFlag < 128;
#endif

#ifdef Q_OS_LINUX
    return this->status && this->status->isOpen();
#endif
}

bool BatteryInfo::isCharging() const
{
#ifdef Q_OS_WIN
    SYSTEM_POWER_STATUS ps;
    GetSystemPowerStatus(&ps);
    return ps.BatteryFlag & 8;
#endif

#ifdef Q_OS_LINUX
    return this->readStatus() == "Charging";
#endif
}

bool BatteryInfo::isDischarging() const
{
#ifdef Q_OS_WIN
    SYSTEM_POWER_STATUS ps;
    GetSystemPowerStatus(&ps);
    return (ps.BatteryFlag != 255 && ps.BatteryFlag != 128 && !(ps.BatteryFlag & 8));
#endif

#ifdef Q_OS_LINUX
    return this->readStatus() == "Discharging";
#endif
}
