#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <string>
#include <sstream>
#include <iomanip>

namespace nano
{

std::string floatToString(float f, uint8_t precision);
void print(std::string line);
void printLine(std::string line);

}   // namespace nano

#endif // TOOLS_HPP
