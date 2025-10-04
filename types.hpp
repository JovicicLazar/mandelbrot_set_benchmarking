#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

constexpr uint32_t IMAGE_WIDTH = 10000;
constexpr uint32_t IMAGE_HEIGHT = 10000;
constexpr uint32_t MAX_ITER = 25;
constexpr double X_MAX = 1.0;
constexpr double X_MIN = -2.0;
constexpr double Y_MAX = 2.0;
constexpr double Y_MIN = -2.0;

struct Settings {
    uint32_t image_width = IMAGE_WIDTH;
    uint32_t image_height = IMAGE_HEIGHT;
    uint32_t mat_iter = MAX_ITER;
    double x_max = X_MAX;
    double x_min = X_MIN;
    double y_max = Y_MAX;
    double y_min = Y_MIN;
};

struct Image {
    std::string file_name;
    std::vector<uint8_t> image_data;
};

#endif