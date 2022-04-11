#ifndef NANOTUBE_HPP
#define NANOTUBE_HPP
#include <vector>
#include <cinttypes>
#include <QPoint>
#include <cmath>

namespace nano
{

template<class T>
struct Point
{
    T x;
    T y;

    Point() = default;
    Point(T x, T y): x(x), y(y) {};
    Point(const QPoint& other) : x(other.x()), y(other.y()) {};

    Point operator+(const Point& rhs) const { return Point(x + rhs.x, y + rhs.y); }
    Point operator-(const Point& rhs) const { return Point(x - rhs.x, y - rhs.y); }
    Point operator+(const T number) const { return Point(x + number, y + number); }
    Point operator-(const T number) const { return *this + (-number); }
    Point operator+=(const T number) { x += number; y += number; return *this; }
    Point operator-=(const T number) { return *this += -number; }
    bool operator==(const QPoint& rhs) const { return x == rhs.x() && y == rhs.y(); }

    operator QPoint() const { return QPoint(static_cast<int>(x), static_cast<int>(y)); }
    operator Point<uint16_t>() { return Point<uint16_t>(x, y); }

    inline float length() const { return std::sqrt(x * x + y * y); }
};

typedef Point<uint16_t> ImgPoint;

struct Element
{
    std::vector<ImgPoint> points;

    void addPoints(std::vector<ImgPoint>&& newPoints);

    Element() = default;
    Element(std::vector<ImgPoint>&& points);
};

} // namespace nano

#endif // NANOTUBE_HPP
