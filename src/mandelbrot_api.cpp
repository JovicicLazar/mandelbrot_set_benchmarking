#include "renderer.hpp"
#include "types.hpp"
#include <iostream>
#include <chrono>

extern "C"
{
    Renderer *create_renderer(int max_iter, double xmin, double xmax, double ymin, double ymax)
    {
        Settings s;
        s.mat_iter = max_iter;
        s.x_min = xmin;
        s.x_max = xmax;
        s.y_min = ymin;
        s.y_max = ymax;
        std::cout << "Renderer creaeted" << std::endl;
        return new Renderer(s);
    }

    double render(Renderer *renderer, int width, int height, const char *filename)
    {
        Image image;
        image.image_width = width;
        image.image_height = height;
        image.file_name = filename;

        auto start_time = std::chrono::high_resolution_clock::now();
        renderer->render(image);
        auto end_time = std::chrono::high_resolution_clock::now();

        image.save_image();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000000.0;
    }

    double render_ispc(Renderer *renderer, int width, int height, const char *filename)
    {
        Image image;
        image.image_width = width;
        image.image_height = height;
        image.file_name = filename;

        auto start_time = std::chrono::high_resolution_clock::now();
        renderer->render_ispc(image);
        auto end_time = std::chrono::high_resolution_clock::now();

        image.save_image();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000000.0;
    }

    double render_parallel(Renderer *renderer, int width, int height, const char *filename)
    {
        Image image;
        image.image_width = width;
        image.image_height = height;
        image.file_name = filename;

        auto start_time = std::chrono::high_resolution_clock::now();
        renderer->render_parallel(image);
        auto end_time = std::chrono::high_resolution_clock::now();

        image.save_image();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000000.0;
    }

    double render_parallel_ispc(Renderer *renderer, int width, int height, const char *filename)
    {
        Image image;
        image.image_width = width;
        image.image_height = height;
        image.file_name = filename;

        auto start_time = std::chrono::high_resolution_clock::now();
        renderer->render_parallel_ispc(image);
        auto end_time = std::chrono::high_resolution_clock::now();

        image.save_image();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000000.0;
    }

    void destroy_renderer(Renderer *renderer)
    {
        delete renderer;
    }
}
