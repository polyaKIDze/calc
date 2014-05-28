#include <QApplication>
#include "calculate.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CalculatePlease *calculate = new CalculatePlease;
    calculate->show();
    return app.exec();
}
