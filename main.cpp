#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

#include "renderer.hpp"
#include "types.hpp"

int main() {

    Image image;
    image.file_name = "new_image.ppm";
    Settings settings;

    Renderer::render(settings, image);

    std::ofstream ofs(image.file_name, std::ios::binary);
    ofs << "P6\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";
    ofs.write(reinterpret_cast<char*>(image.image_data.data()), image.image_data.size());
    ofs.close();

    std::cout << "Wrote mandelbrot.ppm\n";

    return 0;
}
