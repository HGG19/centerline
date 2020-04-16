#include "shape.hpp"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include "delaunay.hpp"


std::vector<Point> read_cones() {
    std::vector<Point> cones;
    std::fstream cones_file;
    cones_file.open("csv/cones.csv", std::ios::in);
    double x = 0;
    double y = 0;
    std::string val;
    std::string line;

    int i = 0;
    while (std::getline(cones_file, line)) {
        std::stringstream ss(line);
        while(ss >> val) {
            if (i == 0) {
                x = std::stod(val);
                ++i;
            } else if (i == 1) {
                y = std::stod(val);
                --i;
            }
            if (ss.peek() == ',') ss.ignore();
        }
        cones.emplace_back(Point{x, y});
    }
    return cones;
}

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


int main() {
    std::vector<Point> cones = read_cones();
    Delaunay delaunay(cones);
    std::vector<Point> voronoi_points = delaunay.run();

    std::vector<Point> centerline_points;
    std::vector<Shape> shapes;
    for (auto& p : voronoi_points) {
        std::vector<Point> candidates = get_closest(p, 15, cones);
        std::vector<std::tuple<int, int, int, int> > combinations = nc4_combinations(candidates.size()); 
        for (auto& combination : combinations) {
            Shape shape{candidates.at(std::get<0>(combination)), candidates.at(std::get<1>(combination)), candidates.at(std::get<2>(combination)), candidates.at(std::get<3>(combination))};
            if (shape.looks_like_rectangle()) {
                bool already_found = false;
                for (auto& s : shapes) {
                    if (s == shape) already_found = true;
                }
                if (!already_found) shapes.push_back(shape);
            }
        }
    }

    for (auto& shape : shapes) {
        centerline_points.push_back(shape.get_center());
    }

    std::ofstream centerline_file;
    centerline_file.open("csv/centerline.csv", std::ios::trunc);
    for (int i = 0; i < centerline_points.size(); ++i) {
        centerline_file << centerline_points.at(i).x << ", " << centerline_points.at(i).y << "\n";
    }
    centerline_file.close();

    std::ofstream voronoi_file;
    voronoi_file.open("csv/voronoi.csv", std::ios::trunc);
    for (int i = 0; i < voronoi_points.size(); ++i) {
        voronoi_file << voronoi_points.at(i).x << ", " << voronoi_points.at(i).y << "\n";
    }
    voronoi_file.close();

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
}
