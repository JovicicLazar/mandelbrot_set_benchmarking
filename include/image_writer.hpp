#ifndef IMAGE_WRITER_HPP
#define IMAGE_WRITER_HPP

#include <cstdint>
#include <string>
#include <vector>

class ImageWriter
{
public:
    static void save_image(const std::string &file_name, const uint32_t image_width, const uint32_t image_height, const std::vector<uint8_t> image_data);
};

#endif