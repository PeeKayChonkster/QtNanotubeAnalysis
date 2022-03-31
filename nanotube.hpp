#ifndef NANOTUBE_HPP
#define NANOTUBE_HPP
#include <vector>
#include <cinttypes>
#include <QPoint>

namespace nano
{

struct Point
{
    uint16_t x;
    uint16_t y;

    Point() = default;
    Point(uint16_t x, uint16_t y): x(x), y(y) {};

    Point operator+(const Point& rhs) const { return Point(x + rhs.x, y + rhs.y); }
    Point operator-(const Point& rhs) const { return Point(x - rhs.x, y - rhs.y); }
    bool operator==(const QPoint& rhs) const { return x == rhs.x() && y == rhs.y(); }
};

struct Nanotube
{
    std::vector<Point> points;

    void addPoints(std::vector<Point>&& newPoints);

    Nanotube() = default;
    Nanotube(std::vector<Point>&& points);
};

} // namespace nano

#endif // NANOTUBE_HPP
