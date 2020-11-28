#include "battery.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Battery w;
    w.check();
    return a.exec();
}
