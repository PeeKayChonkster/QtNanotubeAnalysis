#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <string>
#include <iomanip>
#include <QColor>
#include <utility>
#include "mainwindow.h"

enum class DistanceUnit { pm, nm, mkm, mm, m, km };
typedef std::pair<float, DistanceUnit> Distance;


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
    static Distance getRelevantDistance(float value, DistanceUnit currentDistanceUnit = DistanceUnit::nm);
    static Distance getRelevantDistance(Distance distance);
    static std::vector<std::string> splitString(const std::string& str, char separator);
    static DistanceUnit stringToDistanceUnit(const std::string& str);
};


template<class T> struct enum_size;
template<> struct enum_size<DistanceUnit> { static const size_t value = 6; };

struct Colors
{
    inline static const QColor red = QColor(255, 0, 0);
    inline static const QColor blue = QColor(0, 0, 255);
    inline static const QColor lime = QColor(0, 255, 0);
    inline static const QColor white = QColor(255, 255, 255);
    inline static const QColor black = QColor(0, 0, 0);
    inline static const QColor cyan = QColor(0,255,255);
    inline static const QColor magenta = QColor(255,0,255);
    inline static const QColor silver = QColor(192,192,192);
    inline static const QColor gray = QColor(128,128,128);
    inline static const QColor maroon = QColor(128,0,0);
    inline static const QColor olive = QColor(128,128,0);
    inline static const QColor purple = QColor(128,0,128);
    inline static const QColor green = QColor(0,128,0);
    inline static const QColor teal = QColor(0,128,128);
    inline static const QColor navy = QColor(0,0,128);
};

#endif // TOOLS_HPP
