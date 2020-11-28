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

#ifdef _DEBUG

#define IS_CHARGING true
#define STATUS 17
#define CONNECTED true

#endif


BatteryInfo::BatteryInfo()
{

#ifdef Q_OS_LINUX
    QString absolutePath = "/sys/class/power_supply/";

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
    status->close();
    capacity->close();
    delete status;
    delete capacity;
#endif
}



int BatteryInfo::getCapacity() const
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
    return this->status && this->status->isOpen();
#endif
}

bool BatteryInfo::isCharging() const
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

    if (this->status && this->status->isOpen()) {
        QTextStream in(status);
        in.seek(0);
        QString status;
        in >> status;
        return status == "Charging";
    }

    return false;
#endif
}
