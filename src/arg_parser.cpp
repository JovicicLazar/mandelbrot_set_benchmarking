#include "../include/arg_parser.hpp"
#include <cstring>
#include <string>
#include <iostream>
#include <stdexcept>

void print_usage(const char* program_name);

bool ArgParser::parse_args(int argc, char *argv[], Settings &settings, Image &image)
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        
        if (i + 1 >= argc)
        {
            std::cerr << "Error: " << arg << " requires a value\n";
            return false;
        }
        
        if (arg == "--iter-count")
        {
            settings.mat_iter = std::stoi(argv[++i]);
        }
        else if (arg == "--x-max")
        {
            settings.x_max = std::stod(argv[++i]);
        }
        else if (arg == "--y-max")
        {
            settings.y_max = std::stod(argv[++i]);
        }
        else if (arg == "--x-min")
        {
            settings.x_min = std::stod(argv[++i]);
        }
        else if (arg == "--y-min")
        {
            settings.y_min = std::stod(argv[++i]); 
        }
        else if (arg == "--width")
        {
            image.image_width = std::stoi(argv[++i]);
        }
        else if (arg == "--height")
        {
            image.image_height = std::stoi(argv[++i]);
        }
        else if (arg == "--file-name")
        {
            image.file_name = argv[++i];
        }
        else if (arg == "--help" || arg == "-h")
        {
            print_usage(argv[0]);
            return false;
        }
        else
        {
            std::cerr << "Error: Unknown argument '" << arg << "'\n";
            print_usage(argv[0]);
            return false;
        }
    }
    
    if (settings.mat_iter <= 0)
    {
        std::cerr << "Error: Iteration count must be greater than 0\n";
        return false;
    }
    
    if (image.image_width <= 0 || image.image_height <= 0)
    {
        std::cerr << "Error: Width and height must be greater than 0\n";
        return false;
    }
    
    if (settings.x_min >= settings.x_max || settings.y_min >= settings.y_max)
    {
        std::cerr << "Error: Invalid coordinate ranges (min must be < max)\n";
        return false;
    }
    
    return true;
}

void print_usage(const char* program_name)
{
    std::cout << "Usage: " << program_name << " [OPTIONS]\n\n"
              << "Options:\n"
              << "  --iter-count <int>    Maximum iterations (default: 1000)\n"
              << "  --x-min <double>      Minimum x coordinate (default: -2.0)\n"
              << "  --x-max <double>      Maximum x coordinate (default: 1.0)\n"
              << "  --y-min <double>      Minimum y coordinate (default: -1.5)\n"
              << "  --y-max <double>      Maximum y coordinate (default: 1.5)\n"
              << "  --width <int>         Image width in pixels\n"
              << "  --height <int>        Image height in pixels\n"
              << "  --file-name <string>  Output file name\n"
              << "  --help, -h            Show this help message\n";
}