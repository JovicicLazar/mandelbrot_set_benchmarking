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
    Renderer renderer(settings);
    renderer.render_parallel_ispc(image);
    image.save_image();
    std::cout << "Wrote mandelbrot.ppm\n";
    return 0;
}
