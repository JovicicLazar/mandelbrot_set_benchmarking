#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

#include "include/renderer.hpp"
#include "include/types.hpp"
#include "include/arg_parser.hpp"

int main(int argc, char *argv[]) {
    Settings settings;
    Image image;
    
    if (!ArgParser::parse_args(argc, argv, settings, image)) {
        return 1;
    }
    
    try {
        Renderer renderer(settings);
        renderer.render_parallel_ispc(image);
        image.save_image();
        
        std::cout << "Image saved to: " << image.file_name << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}