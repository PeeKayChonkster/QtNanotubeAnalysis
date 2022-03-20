#include "tools.hpp"

std::string nano::floatToString(float f, uint8_t precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << f;
    return std::move(ss.str());
}

void nano::print(std::string line)
{

}

void nano::printLine(std::string line)
{

}
