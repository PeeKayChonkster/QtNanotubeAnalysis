#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <string>
#include <iomanip>
#include <QColor>
#include "mainwindow.h"

namespace tools
{
static MainWindow* mw = nullptr;
static bool initialized = false;

void init(MainWindow* mainWindow);
std::string floatToString(float f, uint8_t precision);
void print(std::string line, QColor colorj = QColorConstants::White);

}   // namespace tools

#endif // TOOLS_HPP
