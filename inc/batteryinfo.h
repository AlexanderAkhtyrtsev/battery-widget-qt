#pragma once
#include <QApplication>

class BatteryInfo
{
public:

    bool isConnected() const;
    bool isCharging();
    int getStatus();
    BatteryInfo();

#ifdef Q_OS_LINUX

private:
    QString resourcePath;

#endif
};

