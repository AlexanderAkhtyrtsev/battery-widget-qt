#pragma once
#include <fstream>
#include "optionswnd.h"


class Battery : public QWidget
{
    Q_OBJECT
    QPixmap *empty_battery;
    QElapsedTimer elapsedTimer;
    int m_ac, m_ac2, saved_status;
    double time_per1, time_ch1;
    bool configChanged;
public:
    Battery(QWidget *parent = 0);
    ~Battery();
    QSettings *settings;
    int getStatus();
    bool isConnected() const;
    bool isCharging();
    int getOpacity() const;
private:
    int m_opacity;
    QTimer *timer;
    QPixmap *charging_icon;
    QPoint validPos(QPoint);
    OptionsWnd *options;
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
