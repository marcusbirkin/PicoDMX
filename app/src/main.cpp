#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationVersion(VER_PRODUCTVERSION_STR);
    QApplication::setOrganizationName(VER_COMPANYNAME_STR);
    MainWindow w;
    w.show();
    return a.exec();
}
