#include "tools.hpp"
#include "prim_exception.hpp"
#include <QApplication>
#include <QMouseEvent>
#include <nameof.hpp>
#include <sstream>

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

Distance Tools::getRelevantDistance(float value, DistanceUnit currentDistanceUnit)
{
    static auto checkBounds = [](DistanceUnit unit) -> bool {
        return ((int)unit + 1 < enum_size<DistanceUnit>::value && (int)unit > 0);
    };

    Distance result(value, currentDistanceUnit);

    if(!checkBounds(currentDistanceUnit) || value == 0.0f) return result;

    if(result.first > 1000.0f)
    {
        do
        {
            result.first *= 0.001;
            result.second = (DistanceUnit)((int)result.second + 1);
        }
        while(result.first > 1000.0f && checkBounds(result.second));
        return result;
    }
    else if(result.first < 1.0f)
    {
        do
        {
            result.first *= 1000.0f;
            result.second = (DistanceUnit)((int)result.second - 1);
        }
        while(result.first < 1.0f && checkBounds(result.second));
        return result;
    }
    else
    {
        return result;
    }
}

Distance Tools::getRelevantDistance(Distance distance)
{
    return getRelevantDistance(distance.first, distance.second);
}

std::vector<std::string> Tools::splitString(const std::string &str, char separator)
{
    std::stringstream ss(str);
    std::string segment;
    std::vector<std::string> result;

    while(std::getline(ss, segment, separator))
    {
        result.push_back(segment);
    }

    return std::move(result);
}

DistanceUnit Tools::stringToDistanceUnit(const std::string& str)
{
    for(int i = 0; i < enum_size<DistanceUnit>::value; ++i)
    {
        if(str == NAMEOF((DistanceUnit)i))
            return (DistanceUnit)i;
    }
    return DistanceUnit::nm;
}
