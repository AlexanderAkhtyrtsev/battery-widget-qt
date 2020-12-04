#include "battery.h"
#include <iostream>
using std::cout;

Battery::Battery(QWidget *parent)
    : QWidget(parent)
{
    options = nullptr;

    savedStatus = 0;
    percentTime = 0;
    configChanged = false;
    ACStatus = false;
    settings = new QSettings("AlexanderAkhtyrtsev", "Battery Widget");

    resize( settings->value("wsize", QSize(45, 90)).toSize());

    m_opacity = settings->value("opacity", 100).toInt();

    setWindowFlags(Qt::Window|Qt::FramelessWindowHint|Qt::Tool);
    setWindowFlag(Qt::WindowStaysOnTopHint, settings->value("ontop", false).toBool());
    setAttribute(Qt::WA_TranslucentBackground);

    resource = new Resource;
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
    delete resource;
    delete settings;
    delete batteryInfo;
    delete timer;
}

int Battery::getOpacity() const
{
    return m_opacity;
}

QPoint Battery::getValidPosition(const QPoint point)
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
    const auto currentCapacity = this->batteryInfo->getCapacity();
    const int delta = savedStatus - currentCapacity;

    // hide widget when battery disconnected
    if (batteryInfo->isConnected()) {
        if (isHidden()) {
            show();
        }

        repaint();
    }
    else if (!isHidden()) {
        hide();
        std::cout << "widget was hidden cause battery is disconnected.\n";
    }

    // Check if AC status changed
    if (batteryInfo->isCharging() != ACStatus) {
        elapsedTimer.restart();
        ACStatus = batteryInfo->isCharging();
        savedStatus = 0;
        percentTime = 0;
    }


    if (batteryInfo->isDischarging()) {

        // Capacity undefined
        if (!savedStatus) {
            savedStatus = currentCapacity;
        }

        // Capacity changed
        else if (savedStatus != currentCapacity)  {
            percentTime = elapsedTimer.elapsed() / 1000.0 / delta;
            savedStatus = currentCapacity;
        }

    } else if (batteryInfo->isCharging()) {
        if (!savedStatus){
            savedStatus = currentCapacity;
        } else {
            if (savedStatus != currentCapacity) {
                percentTime = elapsedTimer.elapsed() / 1000.0 / (currentCapacity - savedStatus);
                savedStatus = currentCapacity;
            }
        }
    } else {
        // if status is unknown or battery is full
        savedStatus = 0;
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
    const auto capacity = batteryInfo->getCapacity();
    QBrush brush( capacity < 21 ? Qt::red : capacity < 40 ? Qt::yellow : Qt::green );
    QRect r = rect();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setOpacity((qreal)(m_opacity)/100);

    // drawing empty battery
    painter.setBrush( QBrush(resource->getEmptyBattery()->scaled(r.width(), r.height())) );
    painter.setPen(Qt::NoPen);
    painter.drawRect(r);

    // Drawing status
    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    painter.setBrush(brush);
    r.adjust(0, r.height() * (100 - capacity)  / 100, 0, 0);
    painter.drawRect(r);


    // Charging icon
    if ( batteryInfo->isCharging() ) {
        painter.save();
        r = rect();
        r = QRect(r.width()*0.25, r.height() * 0.25, r.width() * 0.5, r.height()*0.5);
        painter.setOpacity(painter.opacity() * 0.5);
        painter.drawPixmap(r, *resource->getChargingIcon());
        painter.restore();
    }

    QFont fnt = painter.font();
    fnt.setPixelSize(rect().height() * 0.15);
    QFontMetrics fmt(fnt);

    QString percentage = QString::number(capacity) + "%";

    auto getTimeStr = [](const QString &title, const QTime &time){
        return QString("\n" + title + ": " + (time.hour() ? time.toString("h") + "h " : "") + time.toString("m") + "m");
    };

    QString onBatteryStr = "",
            timeleft     = "",
            timeUntilful = "";

    if (this->batteryInfo->isDischarging()) {
        QTime time(0,0,0); time = time.addSecs(elapsedTimer.elapsed() / 1000);
        onBatteryStr = getTimeStr("On battery", time);

        if (percentTime) {
            int sec = percentTime * capacity;
            QTime time(0,0,0); time = time.addSecs(sec);
            timeleft = getTimeStr("Time left", time);
        }

    } else if (batteryInfo->isCharging() && percentTime) {
        QTime time(0,0,0); time = time.addSecs(percentTime * (100 - capacity));
        timeUntilful = getTimeStr("Until Full", time);
    }

    this->setToolTip(QString(percentage + timeUntilful + onBatteryStr + timeleft).replace(QRegExp("[\\s]0[\\s][hms]"), ""));
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

    // fix position, dont let move over desktop
    this->move(this->getValidPosition(pos()));
    configChanged = true;
}


void Battery::mousePressEvent(QMouseEvent *event)
{
    clickCoords = event->pos();
}

void Battery::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos( event->globalX() - clickCoords.x(), event->globalY() - clickCoords.y());
    this->move( this->getValidPosition(pos) );
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


Resource::Resource()
{
    chargingIcon = new QPixmap(":/charging.png");
    emptyBattery = new QPixmap(":/empty-battery.png");
}

Resource::~Resource()
{
    delete chargingIcon;
    delete emptyBattery;
}

QPixmap *Resource::getChargingIcon() const
{
    return chargingIcon;
}

QPixmap *Resource::getEmptyBattery() const
{
    return emptyBattery;
}
