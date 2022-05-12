#include "mainwindow.h"

#include <QApplication>
#include "prim_exception.hpp"

int main(int argc, char *argv[])
{
    try
    {
        QApplication a(argc, argv);

        a.setWindowIcon(QIcon("./res/icons/analysis.png"));

        QFont mainFont("Arial", 10, QFont::Normal, false);
        QApplication::setFont(mainFont);

        MainWindow w;
        w.show();
        int result = a.exec();
        return result;
    }
    catch (prim::Exception& e)
    {
        qDebug() << e.what().c_str();
        return 1;
    }
    catch (QException& e)
    {
        qDebug() << e.what();
        return 1;
    }
    catch (std::exception& e)
    {
        qDebug() << e.what();
        return 1;
    }
}
