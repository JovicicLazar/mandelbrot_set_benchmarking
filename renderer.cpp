#include "renderer.hpp"
#include "mandelbrot_ispc.h"
#include "mandelbrot_paralel.h"
#include "thread_pool.hpp"
#include <complex>
#include <cstdint>

#include <iostream>

int inline mandelbrot(double x, double y, int max_iterations);

void Renderer::render(const Settings &settings, Image &image)
{
    const uint32_t image_width = settings.image_width;
    const uint32_t image_height = settings.image_height;
    const uint32_t max_iter = settings.mat_iter;
    const double x_min = settings.x_min;
    const double x_max = settings.x_max;
    const double y_min = settings.y_min;
    const double y_max = settings.y_max;
    auto &image_data = image.image_data;

    image_data.resize((image_width * image_height) * 3);

    for (uint32_t y = 0; y < image_height; ++y)
    {
        for (uint32_t x = 0; x < image_width; ++x)
        {
            double real = x_min + (x_max - x_min) * x / image_width;
            double imag = y_min + (y_max - y_min) * y / image_height;

            int index = (y * image_width + x) * 3;

            int iteration = mandelbrot(real, imag, max_iter);

            uint8_t color = static_cast<uint8_t>(255.0 * iteration / max_iter);

            uint8_t r = color;
            uint8_t g = color;
            uint8_t b = color;

            image_data[index + 0] = r;
            image_data[index + 1] = g;
            image_data[index + 2] = b;
        }
    }
}

void Renderer::render_ispc(const Settings &settings, Image &image) {
    const uint32_t image_width = settings.image_width;
    const uint32_t image_height = settings.image_height;
    const uint32_t max_iter = settings.mat_iter;
    const double x_min = settings.x_min;
    const double x_max = settings.x_max;
    const double y_min = settings.y_min;
    const double y_max = settings.y_max;
    auto &image_data = image.image_data;

    image_data.resize((image_width * image_height) * 3);

    ispc::mandelbrot_ispc(image_width, image_height, max_iter, x_min, x_max, y_min, y_max, image_data.data());
}

int inline mandelbrot(double x, double y, int max_iterations)
{
    std::complex<double> c(x, y);
    std::complex<double> z = 0;
    int iterations = 0;

    while (iterations < max_iterations && abs(z) <= 2.0)
    {
        z = (z * z) + c;
        ++iterations;
    }

    return iterations;
}


void Renderer::render_parallel(const Settings &settings, Image &image)
{
    const uint32_t image_width = settings.image_width;
    const uint32_t image_height = settings.image_height;
    const uint32_t max_iter = settings.mat_iter;
    const double x_min = settings.x_min;
    const double x_max = settings.x_max;
    const double y_min = settings.y_min;
    const double y_max = settings.y_max;
    auto &image_data = image.image_data;

    image_data.resize((image_width * image_height) * 3);

    ThreadPool pool(std::thread::hardware_concurrency());

    for (uint32_t y = 0; y < image_height; ++y)
    {
        pool.enqueue([=, &image_data]() {
            for (uint32_t x = 0; x < image_width; ++x)
            {
                double real = x_min + (x_max - x_min) * x / image_width;
                double imag = y_min + (y_max - y_min) * y / image_height;

                int index = (y * image_width + x) * 3;

                int iteration = mandelbrot(real, imag, max_iter);

                uint8_t color = static_cast<uint8_t>(255.0 * iteration / max_iter);

                image_data[index + 0] = color;
                image_data[index + 1] = color;
                image_data[index + 2] = color;
            }
        });
    }
}


void Renderer::render_paralel_ispc(const Settings &settings, Image &image) {
    const uint32_t image_width = settings.image_width;
    const uint32_t image_height = settings.image_height;
    const uint32_t max_iter = settings.mat_iter;
    const double x_min = settings.x_min;
    const double x_max = settings.x_max;
    const double y_min = settings.y_min;
    const double y_max = settings.y_max;
    auto &image_data = image.image_data;

    image_data.resize((image_width * image_height) * 3);

    ThreadPool pool(std::thread::hardware_concurrency());


    for(uint32_t y = 0; y < image_height; ++y) {
        pool.enqueue([=, &image_data]() {
            ispc::mandelbrot_parallel(y, image_width, image_height, max_iter, x_min, x_max, y_min, y_max, image_data.data());
        });
    }
}
