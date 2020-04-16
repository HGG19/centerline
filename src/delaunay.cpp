#include "delaunay.hpp"
#include <algorithm>
#include <math.h>
#include <Eigen/Dense>

bool Edge::operator==(const Edge& rhs) {
    if (A == rhs.A && B == rhs.B || A == rhs.B && B == rhs.A) return true;
    else return false;
}

bool Triangle::operator==(const Triangle& rhs) {
    if (A == rhs.A) {
        if (B == rhs.B && C == rhs.C) return true;
        else if (B == rhs.C && C == rhs.B) return true;
    } else if (A == rhs.B) {
        if (B == rhs.A && C == rhs.C) return true;
        else if (B == rhs.C && C == rhs.A) return true;
    } else if (A == rhs.C) {
        if (B == rhs.A && C == rhs.B) return true;
        else if (B == rhs.B && C == rhs.A) return true;
    }
    return false;
}

Delaunay::Delaunay(std::vector<Point> cones)
: _super_size(10000.0)
, _super_A(Point{0, _super_size})
, _super_B(Point{-_super_size, -_super_size})
, _super_C(Point{_super_size, -_super_size})
, _super_triangle(Triangle{_super_A, _super_B, _super_C})
, _cones(cones)
{}

double Delaunay::_len(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

Point Delaunay::_find_triangle_center(const Triangle& triangle) {
    double center_x = (triangle.A.x + triangle.B.x + triangle.C.x) / 3;
    double center_y = (triangle.A.y + triangle.B.y + triangle.C.y) / 3;
    return Point{center_x, center_y};
}

void Delaunay::_sort_counter_clockwise(Triangle& triangle) {
    double center_x = (triangle.A.x + triangle.B.x + triangle.C.x) / 3;
    double center_y = (triangle.A.y + triangle.B.y + triangle.C.y) / 3;
    auto angle_between_x_axis = [&center_x, &center_y](double x, double y) {
        double theta = atan2((y - center_y) ,  (x - center_x));
        if (theta < 0) theta = 2 * M_PI + theta;
        return theta;
    };
    double A_angle = angle_between_x_axis(triangle.A.x, triangle.A.y);
    double B_angle = angle_between_x_axis(triangle.B.x, triangle.B.y);
    double C_angle = angle_between_x_axis(triangle.C.x, triangle.C.y);
    if (A_angle > B_angle) std::swap(triangle.A, triangle.B);
    if (A_angle > C_angle) std::swap(triangle.A, triangle.C);
    if (B_angle > C_angle) std::swap(triangle.B, triangle.C);
}

bool Delaunay::_in_triangle_circumcircle(Triangle& triangle, const Point& point) {
    _sort_counter_clockwise(triangle);
    Eigen::Matrix<double, 3, 3> points;
    points << triangle.A.x - point.x, triangle.A.y - point.y, pow(triangle.A.x - point.x, 2) +  pow(triangle.A.y - point.y, 2),
              triangle.B.x - point.x, triangle.B.y - point.y, pow(triangle.B.x - point.x, 2) +  pow(triangle.B.y - point.y, 2),
              triangle.C.x - point.x, triangle.C.y - point.y, pow(triangle.C.x - point.x, 2) +  pow(triangle.C.y - point.y, 2);
    if (points.determinant() > 0) return true;
    else return false;
}

bool Delaunay::_check_intersection(Point A, Point B, Point C, Point D) {  // todo: not reliable
    if (std::max(A.x, B.x) < std::min(C.x, D.x)) {
        return false; // No shared interval
    }
    if (fabs(A.x - B.x) <= 100 * std::numeric_limits<double>::epsilon() || fabs(C.x - D.x) <= 100 * std::numeric_limits<double>::epsilon()) {
        return false; // Not implented check, just return false
    }
    double a1 = (A.y - B.y) / (A.x - B.x);
    double a2 = (C.y - D.y) / (C.x - D.x);
    if (fabs(a1 - a2) <= 100 * std::numeric_limits<double>::epsilon()) {
        return false; // Parallel lines
    }
    double b1 = B.y - a1 * B.x;
    double b2 = D.y - a2 * D.x;
    double x_intersection = (b2 - b1) / (a1 - a2);
    if (x_intersection < std::max(std::min(A.x, B.x), std::min(C.x, D.x)) || x_intersection > std::min(std::max(A.x, B.x), std::max(C.x, D.x))) {
        return false;
    }
    if (x_intersection < std::max(std::min(A.x, B.x), std::min(C.x, D.x)) || x_intersection > std::min(std::max(A.x, B.x), std::max(C.x, D.x))) {
        return false;
    }
    return true;
}

void Delaunay::_bowyer_watson() {
    _triangulation.clear();
    _triangulation.push_back(_super_triangle);
    
    for (const auto& point : _cones) {
        std::vector<Triangle> bad_triangles;
        std::vector<Edge> bad_edges;
        std::vector<int> bad_edges_counter;
        for (auto& triangle : _triangulation) {
            if (_in_triangle_circumcircle(triangle, point)) {
                Edge AB {triangle.A, triangle.B};
                Edge BC {triangle.B, triangle.C};
                Edge CA {triangle.C, triangle.A};

                auto AB_iter = std::find(bad_edges.begin(), bad_edges.end(), AB);
                if (AB_iter != bad_edges.cend()) {
                    size_t AB_index = AB_iter - bad_edges.begin();
                    bad_edges_counter.at(AB_index) += 1;
                } else {
                    bad_edges.push_back(AB);
                    bad_edges_counter.push_back(1);
                }
                auto BC_iter = std::find(bad_edges.begin(), bad_edges.end(), BC);
                if (BC_iter != bad_edges.cend()) {
                    size_t BC_index = BC_iter - bad_edges.begin();
                    bad_edges_counter.at(BC_index) += 1;
                } else {
                    bad_edges.push_back(BC);
                    bad_edges_counter.push_back(1);
                }
                auto CA_iter = std::find(bad_edges.begin(), bad_edges.end(), CA);
                if (CA_iter != bad_edges.cend()) {
                    size_t CA_index = CA_iter - bad_edges.begin();
                    bad_edges_counter.at(CA_index) += 1;
                } else {
                    bad_edges.push_back(CA);
                    bad_edges_counter.push_back(1);
                }
                bad_triangles.push_back(triangle);
            }
        }
        std::vector<Edge> polygon;
        for (const auto& triangle : bad_triangles) {
            Edge AB {triangle.A, triangle.B};
            Edge BC {triangle.B, triangle.C};
            Edge CA {triangle.C, triangle.A};
            size_t AB_index = std::find(bad_edges.begin(), bad_edges.end(), AB) - bad_edges.begin();
            size_t BC_index = std::find(bad_edges.begin(), bad_edges.end(), BC) - bad_edges.begin();
            size_t CA_index = std::find(bad_edges.begin(), bad_edges.end(), CA) - bad_edges.begin();
            if (bad_edges_counter.at(AB_index) == 1) polygon.emplace_back(std::move(AB));
            if (bad_edges_counter.at(BC_index) == 1) polygon.emplace_back(std::move(BC));
            if (bad_edges_counter.at(CA_index) == 1) polygon.emplace_back(std::move(CA));
        }
        _triangulation.erase(std::remove_if(_triangulation.begin(), _triangulation.end(),
            [&bad_triangles](Triangle& t) {
                for (auto& bad_triangle : bad_triangles) {
                    if (t == bad_triangle) return true;
                }
                return false;
        }), _triangulation.end());
        for (auto& edge : polygon) {
            _triangulation.emplace_back(Triangle{edge.A, edge.B, point});
        }
    }
    _triangulation.erase(std::remove_if(_triangulation.begin(), _triangulation.end(),
        [this](Triangle& t) {
            if (t.A == _super_A || t.A == _super_B || t.A == _super_C) return true;
            if (t.B == _super_A || t.B == _super_B || t.B == _super_C) return true;
            if (t.C == _super_A || t.C == _super_B || t.C == _super_C) return true;
            return false;
    }), _triangulation.end());
}

void Delaunay::_find_voronoi() {
    auto closest_cone_distance = [this](Point p) {
        double dist = _super_size;
        for (auto& cone : _cones) {
            if (dist > pow(cone.x - p.x, 2) + pow(cone.y - p.y, 2)) {
                dist = pow(cone.x - p.x, 2) + pow(cone.y - p.y, 2);
            }
        }
        return dist;
    };

    std::vector<Point> unsorted_voronoi_points;
    std::vector<Triangle> unsorted_delaunay_triangles;
    for (auto& triangle : _triangulation) {
        Point candidate =_find_triangle_center(triangle);
        if (closest_cone_distance(candidate) > 0.7 && closest_cone_distance(candidate) < 25.0) {
            unsorted_voronoi_points.emplace_back(_find_triangle_center(triangle));
        }
        unsorted_delaunay_triangles.emplace_back(Triangle{triangle.A, triangle.B, triangle.C});
    }

    std::vector<Point> sorted_voronoi_points;
    sorted_voronoi_points.push_back(Point{0.1, 0.1});
    std::vector<Triangle> sorted_delaunay_triangles;
    while (unsorted_voronoi_points.size() > 0) {
        Point candidate{_super_size, _super_size};
        int candidate_index = 0;
        for (int i = 0; i < unsorted_voronoi_points.size(); ++i) {
            if (sorted_voronoi_points.back().distance(unsorted_voronoi_points.at(i)) < sorted_voronoi_points.back().distance(candidate)) {
                candidate = unsorted_voronoi_points.at(i);
                candidate_index = i;
            }
        }
        sorted_voronoi_points.emplace_back(Point{candidate});
        sorted_delaunay_triangles.push_back(unsorted_delaunay_triangles.at(candidate_index));
        unsorted_voronoi_points.erase(unsorted_voronoi_points.begin() + candidate_index);
        unsorted_delaunay_triangles.erase(unsorted_delaunay_triangles.begin() + candidate_index);
    }

    for (int i = 0; i < sorted_voronoi_points.size(); ++i) {
        bool intersect = false;
        for (int j = 0; j < i - 4; ++j) { // todo: Fix so j < i - 2 works
            Point A{sorted_voronoi_points.at(j)};
            Point B{sorted_voronoi_points.at(j + 1)};
            Point C{_voronoi_diagram.back()};
            Point D{sorted_voronoi_points.at(i)};
            if (!intersect) intersect = _check_intersection(A, B, C, D);
        }
        if (!intersect) {
            Point candidate = sorted_voronoi_points.at(i);
            _voronoi_diagram.emplace_back(candidate);
        } 
    }
}

std::vector<Point> Delaunay::run() {
    _bowyer_watson();
    _find_voronoi();
    return _voronoi_diagram;
}
