#pragma once
#include "optionswnd.h"
#include "batteryinfo.h"

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
    QTimer *timer;
    QPixmap *charging_icon;
    QPoint validPos(QPoint);
    OptionsWnd *options;
    QPixmap *empty_battery;
    QElapsedTimer elapsedTimer;
    int m_ac, m_ac2, saved_status;
    double time_per1, time_ch1;
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
    int m_mc_x, m_mc_y; // mouseclick coord
    void keyPressEvent(QKeyEvent *);
    void mouseDoubleClickEvent(QMouseEvent *event);
};
