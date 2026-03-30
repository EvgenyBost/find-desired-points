#include "line.h"
#include <iostream>
#include <QString>
#include <numeric>

//To be able to print QString using std::cout
std::ostream& operator<<(std::ostream& os, const QString& str) {
    return os << str.toStdString();
}

// Line::Line(double x1, double y1, double x2, double y2)
//     : x1(x1), y1(y1), x2(x2), y2(y2)
// {
//     calculateCoefficients();
// }

Line::Line(Point p1, Point p2)
    : p1(p1), p2(p2)
{
    calculateCoefficients();
}

void Line::calculateCoefficients() {
    double rawA = p1.y - p2.y;
    double rawB = p2.x - p1.x;
    double rawC = p1.x * p2.y - p2.x * p1.y;

    //To reduce, find NOD
    long long common = std::gcd(static_cast<long long>(rawA),
                                std::gcd(static_cast<long long>(rawB),
                                         static_cast<long long>(rawC)));

    if (common != 0) {
        A = rawA / common;
        B = rawB / common;
        C = rawC / common;
    } else {
        A = rawA; B = rawB; C = rawC;
    }

    // Normalization: A > 0
    if (A < 0 || (A == 0 && B < 0)) {
        A = -A; B = -B; C = -C;
    }
}

//Just to increase performance
bool Line::mayIntersect(const Line& other) const {
    // Find first line bounds inside working field
    double minX1 = std::min(this->p1.x, this->p2.x), maxX1 = std::max(this->p1.x, this->p2.x);
    double minY1 = std::min(this->p1.y, this->p2.y), maxY1 = std::max(this->p1.y, this->p2.y);
    // Find second line bounds inside working field
    double minX2 = std::min(other.p1.x, other.p2.x), maxX2 = std::max(other.p1.x, other.p2.x);
    double minY2 = std::min(other.p1.y, other.p2.y), maxY2 = std::max(other.p1.y, other.p2.y);

    // If rectangles not intersected - lines exactly not intersected
    return !(maxX1 < minX2 || maxX2 < minX1 || maxY1 < minY2 || maxY2 < minY1);
}

bool Line::intersect(const Line& other, Point& outPoint) const {
    //Kramer's method
    double det = this->A * other.B - other.A * this->B;

    if (std::abs(det) < 1e-9) {
        return false; // Parrallel or equal line
    }

    outPoint.x = (this->B * other.C - other.B * this->C) / det;
    outPoint.y = (other.A * this->C - this->A * other.C) / det;
    return true;
}

void Line::printInfo() const {
    auto fmtSign = [](double val) {
        QString num = QString::number(qAbs(val), 'g', 6);
        return val >= 0 ? QString("+ %1").arg(num) : QString("- %1").arg(num);
    };

    std::cout << QString("Line: (%1, %2) -> (%3, %4) | Equation: %5*X %6*Y %7 = 0\n")
                     .arg(p1.x, 0, 'g', 6)
                     .arg(p1.y, 0, 'g', 6)
                     .arg(p2.x, 0, 'g', 6)
                     .arg(p2.y, 0, 'g', 6)
                     .arg(QString::number(A, 'g', 6), fmtSign(B), fmtSign(C));
}
