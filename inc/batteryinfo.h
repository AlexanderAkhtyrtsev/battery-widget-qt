#pragma once
#include <QApplication>
#include <QFile>

class BatteryInfo
{
public:

    bool isConnected() const;
    bool isCharging() const;
    bool isDischarging() const;
    int getCapacity() const;

    BatteryInfo();
    ~BatteryInfo();

#ifdef Q_OS_LINUX
    const QString readStatus() const;

private:
    QFile *capacity;
    QFile *status;
#endif
};

