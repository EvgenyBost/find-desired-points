#ifndef LINE_H
#define LINE_H
#include <math.h>

class Line {
public:
    struct Point {
        double x;
        double y;
        Point(double _x = 0, double _y = 0) : x(_x), y(_y) {}

        double distanceTo(const Point& other) const {
            return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
        }
    };

    //Line(double x1, double y1, double x2, double y2);
    Line(Point p1, Point p2);

    bool mayIntersect(const Line& other) const;
    bool intersect(const Line& other, Point& outPoint) const;

    void printInfo() const;

private:
    Point p1, p2;
    double A, B, C; //A*x + B*y + C = 0

    void calculateCoefficients();
};

#endif // LINE_H
