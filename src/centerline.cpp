#include "centerline.hpp"
#include <functional>
#include <tuple>

#include <fstream>

namespace {
std::vector<Point> get_closest(Point p, int n, std::vector<Point> cones) {
    std::vector<Point> closest_cones;
    while (n > 0) {
        double min_distance = 10000;
        int min_index = 0;
        for (int i = 0; i < cones.size(); ++i) {
            if (min_distance > p.distance(cones.at(i))) {
                min_distance = p.distance(cones.at(i));
                min_index = i;
            }
        }
        closest_cones.push_back(cones.at(min_index));
        cones.erase(cones.begin() + min_index);
        --n;
    }
    return closest_cones;
}

typedef std::tuple<int, int, int, int> Combination;
std::vector<Combination> nc4_combinations(int n) {
    // https://www.geeksforgeeks.org/print-all-possible-combinations-of-r-elements-in-a-given-array-of-size-n/
    std::vector<int> indexes(n);
    for (int i = 0; i < indexes.size(); ++i) {
        indexes.at(i) = i;
    }

    std::function<std::vector<Combination>(const std::vector<int>&, std::vector<int>, int, int, int)> get_combinations;
    get_combinations = [&get_combinations](const std::vector<int>& indexes, std::vector<int> data, int start, int end, int index) {
        std::vector<Combination> combinations;
        int r = 4;
        if (index == 4) {
            Combination combination = std::make_tuple(data[0], data[1], data[2], data[3]);
            return std::vector<std::tuple<int, int, int, int> >{combination};
        }

        for (int i = start; i <= end - 1 && end - i >= r - index; i++) {
            data[index] = indexes[i];
            for (Combination& c : get_combinations(indexes, data, i + 1, end, index + 1)) {
                combinations.push_back(c);
            }
        }
        return combinations;
    };

    return get_combinations(indexes, std::vector<int>(4), 0, n, 0);
}

bool contains_shape(std::vector<Shape>& shapes, const Shape& shape) {
    bool contains = false;
    for (auto& s : shapes) {
        if (s == shape) contains = true;
    }
    return contains;
}
}

std::vector<Point> make_centerline(std::vector<Point> cones, std::vector<Point> voronoi_points) {
    std::vector<Point> centerline_points;
    std::vector<Shape> shapes;
    for (auto& p : voronoi_points) {
        std::vector<Point> candidates = get_closest(p, 15, cones);
        std::vector<Combination> combinations = nc4_combinations(candidates.size());
        bool center_point_found = false;
        for (auto& combination : combinations) {
            Point A = candidates.at(std::get<0>(combination));
            Point B = candidates.at(std::get<1>(combination));
            Point C = candidates.at(std::get<2>(combination));
            Point D = candidates.at(std::get<3>(combination));
            Shape shape{A, B, C, D};
            if (shape.looks_like_rectangle() && !contains_shape(shapes, shape)) {
                shapes.push_back(shape);
                centerline_points.push_back(shape.get_center());
                center_point_found = true;
            }
        }
        // if (!center_point_found) centerline_points.push_back(p);
    }

    std::ofstream shape_file;
    shape_file.open("csv/shape.csv", std::ios::trunc);
    for (int i = 0; i < shapes.size(); ++i) {
        shape_file << shapes.at(i).get_A().x << ", " << shapes.at(i).get_A().y << "\n";
        shape_file << shapes.at(i).get_B().x << ", " << shapes.at(i).get_B().y << "\n";
        shape_file << shapes.at(i).get_C().x << ", " << shapes.at(i).get_C().y << "\n";
        shape_file << shapes.at(i).get_D().x << ", " << shapes.at(i).get_D().y << "\n";
        shape_file << shapes.at(i).get_A().x << ", " << shapes.at(i).get_A().y << "\n";
    }
    shape_file.close();
    return centerline_points;
}
