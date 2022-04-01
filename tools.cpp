#include "tools.hpp"
#include "prim_exception.hpp"
#include <QApplication>
#include <QMouseEvent>

std::string Tools::floatToString(float f, uint8_t precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << f;
    return std::move(ss.str());
}

void Tools::print(std::string line, QColor color)
{
    if(!initialized) throw PRIM_EXCEPTION("Tools weren't initialized!");
    mw->console.print(line.c_str(), color);
}

void Tools::init(MainWindow* mainWindow)
{
    initialized = true;
    mw = mainWindow;
}

MainWindow* Tools::getMainWindow()
{
    return mw;
}
