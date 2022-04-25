#include "tools.hpp"
#include "prim_exception.hpp"
#include <QApplication>
#include <QMouseEvent>
#include <nameof.hpp>
#include <sstream>
#include <initializer_list>

std::string Tools::floatToString(float f, uint8_t precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << f;
    return std::move(ss.str());
}

void Tools::print(std::string line, QColor color)
{
    if(!initialized) throw PRIM_EXCEPTION("Tools weren't initialized!");
    mw->console->print(line.c_str(), color);
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

float Tools::getAverage(const std::vector<float> &values)
{
    if(values.size() == 0) return 0.0f;
    if(values.size() == 1) return values[0];
    float summ = 0.0f;
    for(int i = 0; i < values.size(); ++i)
    {
        summ += values[i];
    }
    return summ / values.size();
}

float Tools::getAverageDifference(const std::vector<float>& values)
{
    if(values.size() < 2) return *values.begin();
    float summ = 0.0f;
    for(int i = 0; i < values.size(); ++i)
    {
        for(int j = i + 1; j < values.size(); ++j)
        {
            summ += std::abs(values[i] - values[j]);
        }
    }
    return summ / (values.size() * (values.size() - 1) + 0.5f);
}
