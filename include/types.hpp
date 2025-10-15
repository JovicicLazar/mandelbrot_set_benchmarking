#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "image_writer.hpp"

constexpr uint32_t IMAGE_WIDTH = 3000;
constexpr uint32_t IMAGE_HEIGHT = 3000;
constexpr uint32_t MAX_ITER = 100;
constexpr double X_MAX = 1.0;
constexpr double X_MIN = -2.0;
constexpr double Y_MAX = 2.0;
constexpr double Y_MIN = -2.0;

struct Settings {
    uint32_t mat_iter = MAX_ITER;
    double x_max = X_MAX;
    double x_min = X_MIN;
    double y_max = Y_MAX;
    double y_min = Y_MIN;
};

struct Image {
    uint32_t image_width = IMAGE_WIDTH;
    uint32_t image_height = IMAGE_HEIGHT;
    std::string file_name = "image.bmp";
    std::vector<uint8_t> image_data;

    void save_image() {
        ImageWriter::save_image(file_name, image_width, image_height, image_data);
    }
};

#endif