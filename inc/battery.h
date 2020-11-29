#pragma once
#include "optionswnd.h"
#include "batteryinfo.h"

class Resource
{
public:
    Resource();
    ~Resource();
    QPixmap *getChargingIcon() const;
    QPixmap *getEmptyBattery() const;
private:
    QPixmap *chargingIcon;
    QPixmap *emptyBattery;
};


class Battery : public QWidget
{
    Q_OBJECT
public:
    Battery(QWidget *parent = nullptr);
    ~Battery();
    QSettings *settings;
    int getOpacity() const;

private:
    BatteryInfo *batteryInfo;

    int m_opacity;
    bool ACStatus;
    QTimer *timer;
    Resource *resource;


    QPoint getValidPosition(const QPoint);
    OptionsWnd *options;
    QElapsedTimer elapsedTimer;
    int savedStatus;
    double percentTime;
    bool configChanged;

public slots:
    void check();
    void setOpacity(int);
    void pinOnTop(bool);

protected:
    void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *);
    void mouseDoubleClickEvent(QMouseEvent *event);
    QPoint clickCoords; // mouseclick coords
};
