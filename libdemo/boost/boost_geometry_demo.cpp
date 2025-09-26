#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <iomanip>
#include <iostream>

namespace bg = boost::geometry;

int main() {
    // 定义球面点类型(经度,纬度)
    typedef bg::model::point<double, 2, bg::cs::spherical_equatorial<bg::degree>> SphericalPoint;

    // 两个城市的位置
    SphericalPoint new_york(-74.0060, 40.7128);  // 纽约
    SphericalPoint london(-0.1278, 51.5074);     // 伦敦

    // 地球半径(公里)
    double const earth_radius = 6371.0;

    // 计算球面距离
    double distance = bg::distance(new_york, london) * earth_radius;

    std::cout << std::setprecision(10) << "Distance between New York and London: " << distance << " km" << std::endl;

    return 0;
}
