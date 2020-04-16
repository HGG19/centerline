#include "centerline.hpp"
#include "delaunay.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<Point> read_cones();

int main() {
    std::vector<Point> cones = read_cones();
    Delaunay delaunay(cones);
    std::vector<Point> voronoi_points = delaunay.run();
    std::vector<Point> centerline_points = make_centerline(cones, voronoi_points);

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
}

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


