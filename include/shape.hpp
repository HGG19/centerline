#ifndef __SHAPE__HPP__
#define __SHAPE__HPP__

struct Point {
    double x;
    double y;
    bool operator==(const Point& rhs);
    double distance(const Point& rhs);
};

class Shape {
public:
    Shape();
    Shape(Point A, Point B, Point C, Point D);
    bool looks_like_rectangle();
    bool operator==(const Shape& rhs);
    double area();
    const Point&  get_A() const;
    const Point&  get_B() const;
    const Point&  get_C() const;
    const Point&  get_D() const;
    Point get_center() const;
private:
    double distance(Point A, Point B);
    void sort();
    Point _A;    
    Point _B;
    Point _C;
    Point _D;
};

#endif // __SHAPE__HPP__
