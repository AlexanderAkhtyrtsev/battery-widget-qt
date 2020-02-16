#include "battery.h"

OptionsWnd::OptionsWnd(Battery *parent) : QWidget(parent)
{
    setWindowFlags(Qt::Tool);
    setWindowTitle("Options");

    battery = parent;
    cb_stayOnTop = new QCheckBox("Always on top");
    cb_stayOnTop->setChecked(battery->settings->value("ontop", false).toBool());
    QObject::connect(cb_stayOnTop, SIGNAL(clicked(bool)), battery, SLOT(pinOnTop(bool)));

    lbl_1 = new QLabel("Opacity");

    sl_opacity = new QSlider(Qt::Horizontal);
    sl_opacity->setMinimum(15);
    sl_opacity->setMaximum(100);
    sl_opacity->setValue(battery->getOpacity());
    QObject::connect(sl_opacity, SIGNAL(valueChanged(int)), battery, SLOT(setOpacity(int)));

    vbl = new QVBoxLayout;
    vbl->addWidget(cb_stayOnTop);
    vbl->addWidget(lbl_1);
    vbl->addWidget(sl_opacity);
    setLayout(vbl);
}
