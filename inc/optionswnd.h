#pragma once

#include <QtWidgets>
class Battery;

class OptionsWnd : public QWidget
{
    Q_OBJECT
    QVBoxLayout *vbl;
    QCheckBox *cb_stayOnTop;
    QSlider *sl_opacity;
    QLabel *lbl_1;
    Battery *battery;
public:
    explicit OptionsWnd(Battery *parent = nullptr);
    ~OptionsWnd();
};

