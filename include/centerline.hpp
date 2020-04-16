#ifndef __CENTERLINE__HPP__
#define __CENTERLINE__HPP__

#include "shape.hpp"
#include <vector>

std::vector<Point> make_centerline(std::vector<Point> cones, std::vector<Point> voronoi_points);

#endif // __CENTERLINE__HPP__ 
