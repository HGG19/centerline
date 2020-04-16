#include "shape.hpp"
#include <algorithm>
#include <math.h>
#include <tuple>
#include <iostream>

bool Point::operator==(const Point& rhs) const {
    double radius = 0.5;
    if (fabs(x - rhs.x) <= 100 * std::numeric_limits<double>::epsilon() + radius) {
        if (fabs(y - rhs.y) <= 100 * std::numeric_limits<double>::epsilon() + radius) {
            return true;
        }
    }
    return false;
}

double Point::distance(const Point& rhs) {
    return sqrt(pow(x - rhs.x, 2) + pow(y - rhs.y, 2));
}

Shape::Shape(Point A, Point B, Point C, Point D)
: _A(A)
, _B(B)
, _C(C)
, _D(D)
{
    sort();
}

const Point& Shape::get_A() const {
    return _A;
}

const Point& Shape::get_B() const {
    return _B;
}

const Point& Shape::get_C() const {
    return _C;
}
const Point& Shape::get_D() const {
    return _D;
}
bool Shape::looks_like_rectangle() {
    if (_A == _B || _A == _C || _A == _D || _B == _C || _B == _D || _C == _D) return 0;
    double max_distance = 8.0;
    if (_A.distance(_B) >= max_distance || _A.distance(_D) >= max_distance || _B.distance(_C) >= max_distance || _C.distance(_D) >= max_distance) return 0;
    double A_angle = acos(((_A.x - _B.x) * (_A.x - _D.x) + (_A.y - _B.y) * (_A.y - _D.y)) / (_A.distance(_B) * _A.distance(_D)));
    double B_angle = acos(((_B.x - _A.x) * (_B.x - _C.x) + (_B.y - _A.y) * (_B.y - _C.y)) / (_B.distance(_A) * _B.distance(_C)));
    double C_angle = acos(((_C.x - _B.x) * (_C.x - _D.x) + (_C.y - _B.y) * (_C.y - _D.y)) / (_C.distance(_B) * _C.distance(_D)));
    double D_angle = acos(((_D.x - _C.x) * (_D.x - _A.x) + (_D.y - _C.y) * (_D.y - _A.y)) / (_D.distance(_C) * _D.distance(_A)));
    double range = 0.35;
    if (A_angle > (M_PI / 2) + range || A_angle < (M_PI / 2) - range) return 0;
    if (B_angle > (M_PI / 2) + range || B_angle < (M_PI / 2) - range) return 0;
    if (C_angle > (M_PI / 2) + range || C_angle < (M_PI / 2) - range) return 0;
    if (D_angle > (M_PI / 2) + range || D_angle < (M_PI / 2) - range) return 0;
    return 1;
}

bool Shape::operator==(const Shape& rhs) {
    if (_A == rhs.get_A() && _B == rhs.get_B() && _C == rhs.get_C() && _D == rhs.get_D()) return true;
    else return false;
}

double Shape::area() {
    //return 0.5 * std::hypot(_A.x - _C.x, _A.y - _C.y) * std::hypot(_D.x - _B.x, _D.y - _B.y);
    return _A.distance(_B) * _A.distance(_D);
}

void Shape::sort() {
    while (!(_A.x <= _B.x && _A.x <= _C.x && _A.x <= _D.x && _A.y <= _B.y && _A.y <= _C.y && _A.y <= _D.y) && !(_B.y <= _C.y && _B.y <= _D.y) && !(_C.y <= _D.y)) {
        std::swap(_A, _D);
        std::swap(_D, _C);
        std::swap(_C, _B);
    }
}

Point Shape::get_center() const {
    double x_center = (_A.x + _B.x + _C.x + _D.x) / 4;
    double y_center = (_A.y + _B.y + _C.y + _D.y) / 4;
    return Point{x_center, y_center};
}


