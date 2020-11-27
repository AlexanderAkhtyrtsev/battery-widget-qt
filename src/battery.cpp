#include "battery.h"
#include <iostream>
using std::cout;

Battery::Battery(QWidget *parent)
    : QWidget(parent)
{
    options = 0;
    m_ac2 = -1;
    saved_status = -1;
    time_per1 = -1;
    time_ch1 = -1;
    configChanged = false;
    settings = new QSettings("AlexanderAkhtyrtsev", "Battery Widget");
    resize( settings->value("wsize", QSize(45, 90)).toSize());
    charging_icon = new QPixmap(":/charging.png");
    empty_battery = new QPixmap(":/empty-battery.png");
    m_opacity = settings->value("opacity", 100).toInt();

    setWindowFlags(Qt::Window|Qt::FramelessWindowHint|Qt::Tool);
    setWindowFlag(Qt::WindowStaysOnTopHint, settings->value("ontop", false).toBool());
    setAttribute(Qt::WA_TranslucentBackground);

    batteryInfo = new BatteryInfo;

    timer = new QTimer(this);
    timer->setInterval(2000);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(check()));
    timer->start();
    move(settings->value("wpos", QPoint(QApplication::desktop()->width() - width() - 10, 40)).toPoint());

}

Battery::~Battery()
{
    delete options;
    delete charging_icon;
    delete empty_battery;
    delete settings;
    delete batteryInfo;
}

int Battery::getOpacity() const
{
    return m_opacity;
}

QPoint Battery::getValidPosition(QPoint point)
{
    QPoint valid(point);
    QDesktopWidget *desktop = QApplication::desktop();
    int w = desktop->width() - width(),
        h = desktop->height() - height();

    valid.setX(qMax(0, qMin(valid.x(), w)));
    valid.setY(qMax(0, qMin(valid.y(), h)));
    return valid;
}

void Battery::check()
{
    // hide widget when battery disconnected
    if (batteryInfo->isConnected()) {
        if (isHidden()) {
            show();
        }
        repaint();
    }
    else if (!isHidden()) {
      //  hide();
    }

    if (!m_ac) {
        if (m_ac != m_ac2) { // when ac disconnected
            m_ac2 = m_ac;
            elapsedTimer.restart();
        }

        if (saved_status == -1) {
            elapsedTimer.restart();
            saved_status = batteryInfo->getStatus();
        }
        else if (saved_status != batteryInfo->getStatus())  {
            time_per1 = elapsedTimer.elapsed() / 1000.0 / (saved_status - batteryInfo->getStatus());
            saved_status = -1;
        }

    } else {
        if (m_ac != m_ac2) { // when AC connected
            m_ac2 = m_ac;
            saved_status = -1;
        }
        if (batteryInfo->isCharging()) {
            if (saved_status == -1){
                elapsedTimer.restart();
                saved_status = batteryInfo->getStatus();
            } else {
                if (saved_status != batteryInfo->getStatus()) {
                    time_ch1 = elapsedTimer.elapsed() / 1000.0 / (batteryInfo->getStatus() - saved_status);
                    saved_status = -1;
                }
            }
        } else if(time_ch1 != -1) {
            time_ch1 = -1;
        }
    }


    if (configChanged) {
        settings->setValue("opacity", m_opacity);
        settings->setValue("ontop",  static_cast<bool>(this->windowFlags() & Qt::WindowStaysOnTopHint));
        settings->setValue("wpos", this->pos());
        settings->setValue("wsize", this->size());
        configChanged = false;
    }

}

void Battery::setOpacity(int opacity)
{
    m_opacity = opacity;
    configChanged = true;
    this->repaint();
}

void Battery::pinOnTop(bool b)
{
    setWindowFlag(Qt::WindowStaysOnTopHint, b);
    configChanged = true;
}

void Battery::paintEvent(QPaintEvent *)
{
    float s = batteryInfo->getStatus();
    QBrush brush( s < 21 ? Qt::red : s < 40 ? Qt::yellow : Qt::green );
    QRect r = rect();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setOpacity((qreal)(m_opacity)/100);

    // drawing empty battery
    painter.setBrush( QBrush(empty_battery->scaled(r.width(), r.height())) );
    painter.setPen(Qt::NoPen);
    painter.drawRect(r);

    // Drawing status
    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    painter.setBrush(brush);
    r.adjust(0, r.height()*(1 - s / 100), 0, 0);
    painter.drawRect(r);


    // Charging icon
    if ( batteryInfo->isCharging() ) {
        painter.save();
        r = rect();
        r = QRect(r.width()*0.25, r.height() * 0.25, r.width() * 0.5, r.height()*0.5);
        painter.setOpacity(painter.opacity() * 0.5);
        painter.drawPixmap(r, *charging_icon);
        painter.restore();
    }

    QFont fnt = painter.font();
    fnt.setPixelSize(rect().height() * 0.15);
    QFontMetrics fmt(fnt);

    QString percentage = QString::number(s) + "%";
    //TODO: timeleft
    QString onBatteryStr = "", timeleft = "", timeUntilful = "";
    if (!m_ac) {
        QTime t1(0,0,0); t1 = t1.addSecs(elapsedTimer.elapsed() / 1000);
        onBatteryStr = "\nOn battery: " + t1.toString("h") + " h " + t1.toString("m") + " m " + t1.toString("s") + " s";

        if (time_per1 != -1) {
            int sec = time_per1 * s;
            QTime t2(0,0,0); t2 = t2.addSecs(sec);
            timeleft = "\nTime left: " + t2.toString("h") + " h " + t2.toString("m") + " m " + t2.toString("s") + " s";
        }

    } else if (batteryInfo->isCharging() && time_ch1 != -1) {
        QTime t1(0,0,0); t1 = t1.addSecs(time_ch1 * (100-s));
        timeUntilful = "\nUntil Full: " + t1.toString("h") + " h " + t1.toString("m") + " m " + t1.toString("s") + " s";
    }

    this->setToolTip(QString(percentage+timeUntilful+onBatteryStr+timeleft).replace(QRegExp("[\\s]0[\\s][hms]"), ""));
    int fw = fmt.horizontalAdvance(percentage);
    painter.setFont(fnt);
    painter.setPen(QPen(Qt::black));
    painter.drawText(QRect(rect().width() / 2 - fw/2, rect().height()/2 - fmt.height()/2, fw, fmt.height()), percentage);

    painter.end();
}

void Battery::wheelEvent(QWheelEvent *pe)
{
    int d = pe->angleDelta().y() > 0 ? 10 : -10;
    this->resize( qMax(35, qMin(width()+d/2, 300)), qMax(70, qMin(height()+d, 600)) );

    this->move(this->getValidPosition(pos()));
    configChanged = true;
}


void Battery::mousePressEvent(QMouseEvent *event)
{
    m_mc_x = event->x();
    m_mc_y = event->y();
}

void Battery::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos( event->globalX() - m_mc_x, event->globalY() - m_mc_y);
    move( this->getValidPosition(pos) );
    configChanged = true;
}

void Battery::keyPressEvent(QKeyEvent *pe)
{
    switch(pe->key()) {
        case Qt::Key_Escape:
            timer->stop();
            QApplication::exit(0);
    }
}

void Battery::mouseDoubleClickEvent(QMouseEvent *)
{
    if (!options) {
        options = new OptionsWnd(this);
    }

    options->show();
    options->move(this->pos());
}
