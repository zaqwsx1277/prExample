#include "prExampleMainWindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    prExampleMainWindow w;
    w.show();

    return a.exec();
}
