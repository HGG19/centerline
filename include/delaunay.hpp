#ifndef __DELAUNAY__HPP__
#define __DELAUNAY__HPP__

#include "shape.hpp"
#include <vector>

struct Triangle {
    Point A;
    Point B;
    Point C;
    bool operator==(const Triangle& rhs);
};

struct Edge {
    Point A;
    Point B;
    bool operator==(const Edge& rhs);
};

class Delaunay {
public:
    Delaunay(std::vector<Point> cones);
    std::vector<Point> run();
private:
    double _len(double x1, double y1, double x2, double y2);
    Point _find_triangle_center(const Triangle& triangle);
    void _sort_counter_clockwise(Triangle& triangle);
    bool _in_triangle_circumcircle(Triangle& triangle, const Point& p); // A, B and C must be sorted counterclockwise
    bool _check_intersection(Point A, Point B, Point C, Point D);
    void _bowyer_watson();
    void _find_voronoi();

    const double _super_size;
    const Point _super_A;
    const Point _super_B;
    const Point _super_C;
    const Triangle _super_triangle;
    std::vector<Triangle> _triangulation;
    std::vector<Point> _voronoi_diagram;
    std::vector<Point> _cones;
};

#endif // __DELAUNAY__HPP__
