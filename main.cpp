#include "mainwindow.h"

#include <QApplication>
#include "prim_exception.hpp"

int main(int argc, char *argv[])
{
    try
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        int result = a.exec();
        return result;
    }
    catch (prim::Exception& e)
    {
        qDebug() << e.what().c_str();
    }
}
