#include "tools.hpp"
#include "prim_exception.hpp"
#include <QApplication>
#include <QMouseEvent>

std::string tools::floatToString(float f, uint8_t precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << f;
    return std::move(ss.str());
}

void tools::print(std::string line, QColor color)
{
    if(!initialized) throw PRIM_EXCEPTION("Tools weren't initialized!");
    mw->console.print(line.c_str(), color);
}

void tools::init(MainWindow* mainWindow)
{
    initialized = true;
    mw = mainWindow;
}

void tools::registerEvent(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

    }
}
