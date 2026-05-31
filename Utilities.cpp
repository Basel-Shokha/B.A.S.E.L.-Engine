#include "Utilities.h"
#include <iostream>
#include <cmath>


using std::cerr;

void is_out_of_bounds_message(int index) {
    std::cerr << index << " is out of bounds" << std::endl;
    exit(1);
}

void failed_allocation_message(std::bad_alloc& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
}


double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double EARTH_RADIUS = 6371000.0;
    const double PI = 3.14159265358979323846;

    double lat1_radian = lat1 * PI / 180.0;
    double lat2_radian = lat2 * PI / 180.0;
    double lat_diff = (lat2 - lat1) * PI / 180.0;
    double lon_diff = (lon2 - lon1) * PI / 180.0;

    double a = std::sin(lat_diff / 2) * std::sin(lat_diff / 2) +
               std::cos(lat1_radian) * std::cos(lat2_radian) *
               std::sin(lon_diff / 2) * std::sin(lon_diff / 2);

    return 2 * EARTH_RADIUS * std::atan2(std::sqrt(a), std::sqrt(1 - a));
}

