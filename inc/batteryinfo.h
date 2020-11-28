#pragma once
#include <QApplication>
#include <QFile>

class BatteryInfo
{
public:

    bool isConnected() const;
    bool isCharging() const;
    int getCapacity() const;
    BatteryInfo();
    ~BatteryInfo();

#ifdef Q_OS_LINUX
private:
    QFile *capacity;
    QFile *status;

#endif
};

