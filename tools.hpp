#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <string>
#include <iomanip>
#include <QColor>
#include "mainwindow.h"

class Tools
{
private:
    inline static MainWindow* mw= nullptr;
    inline static bool initialized = false;
public:
    static void init(MainWindow* mainWindow);
    static std::string floatToString(float f, uint8_t precision);
    static void print(std::string line, QColor colorj = QColorConstants::White);
    static MainWindow* getMainWindow();
};

#endif // TOOLS_HPP
