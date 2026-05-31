#pragma once
#include <cmath>
#include <iomanip>


void is_out_of_bounds_message(int index);

void failed_allocation_message(std::bad_alloc& e);

double calculateDistance(double lat1, double lon1, double lat2, double lon2);
