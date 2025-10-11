#include "renderer.hpp"
#include "../include/mandelbrot_ispc.h"
#include "../include/mandelbrot_parallel.h"
#include "../include/mandelbrot_parallel_optimized.h"
#include "thread_pool.hpp"
#include <complex>
#include <cstdint>

#include <iostream>

int inline mandelbrot(double x, double y, int max_iterations);

Renderer::Renderer(const Settings& settings) {
    m_pool = std::make_shared<ThreadPool>(std::thread::hardware_concurrency());
    m_max_iter = settings.mat_iter;
    m_x_min = settings.x_min;
    m_x_max = settings.x_max;
    m_y_min = settings.y_min;
    m_y_max = settings.y_max;
}

void Renderer::render(Image &image)
{
    const uint32_t image_width = image.image_width;
    const uint32_t image_height = image.image_height;
    auto &image_data = image.image_data;

    image_data.resize((image_width * image_height) * 3);

    for (uint32_t y = 0; y < image_height; ++y)
    {
        for (uint32_t x = 0; x < image_width; ++x)
        {
            
            double real = m_x_min + (m_x_max - m_x_min) * x / image_width;
            double imag = m_y_min + (m_y_max - m_y_min) * y / image_height;

            int index = (y * image_width + x) * 3;

            int iteration = mandelbrot(real, imag, m_max_iter);

            uint8_t color = static_cast<uint8_t>(255.0 * iteration / m_max_iter);

            uint8_t r = color;
            uint8_t g = color;
            uint8_t b = color;

            image_data[index + 0] = r;
            image_data[index + 1] = g;
            image_data[index + 2] = b;
        }
    }
}

void Renderer::render_ispc(Image &image) {
    const uint32_t image_width = image.image_width;
    const uint32_t image_height = image.image_height;
    auto &image_data = image.image_data;

    image_data.resize((image_width * image_height) * 3);

    ispc::mandelbrot_ispc(image_width, image_height, m_max_iter, m_x_min, m_x_max, m_y_min, m_y_max, image_data.data());
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


void Renderer::render_parallel(Image &image)
{
    const uint32_t image_width = image.image_width;
    const uint32_t image_height = image.image_height;
    auto &image_data = image.image_data;

    image_data.resize((image_width * image_height) * 3);

    for (uint32_t y = 0; y < image_height; ++y)
    {
        m_pool->enqueue([=, &image_data]() {
            for (uint32_t x = 0; x < image_width; ++x)
            {
                double real = m_x_min + (m_x_max - m_x_min) * x / image_width;
                double imag = m_y_min + (m_y_max - m_y_min) * y / image_height;

                int index = (y * image_width + x) * 3;

                int iteration = mandelbrot(real, imag, m_max_iter);

                uint8_t color = static_cast<uint8_t>(255.0 * iteration / m_max_iter);

                image_data[index + 0] = color;
                image_data[index + 1] = color;
                image_data[index + 2] = color;
            }
        });
    }
}


void Renderer::render_parallel_ispc(Image &image) {
    const uint32_t image_width = image.image_width;
    const uint32_t image_height = image.image_height;
    auto &image_data = image.image_data;

    image_data.resize((image_width * image_height) * 3);

    for(uint32_t y = 0; y < image_height; ++y) {
        m_pool->enqueue([=, &image_data]() {
            ispc::mandelbrot_parallel(y, image_width, image_height, m_max_iter, m_x_min, m_x_max, m_y_min, m_y_max, image_data.data());
        });
    }
}

void Renderer::render_parallel_ispc_optimized(Image &image) {
    const uint32_t image_width = image.image_width;
    const uint32_t image_height = image.image_height;
    auto &image_data = image.image_data;
    const uint32_t size = image_height * image_width;

    std::vector<uint8_t> rPlane(size);
    std::vector<uint8_t> gPlane(size);
    std::vector<uint8_t> bPlane(size);

    image_data.resize(size * 3);

    {

        for(uint32_t y = 0; y < image_height; ++y) {
            m_pool->enqueue([=, &rPlane, &bPlane, &gPlane]() {
                int row_start = y * image_width;
                ispc::mandelbrot_parallel_optimized(y, image_width, image_height, m_max_iter, m_x_min, m_x_max, m_y_min, m_y_max, rPlane.data() + row_start, gPlane.data() + row_start, bPlane.data() + row_start);
            });
        }
    }

    for (int i = 0; i < size; i++)
    {
        image_data[i * 3 + 0] = bPlane[i];
        image_data[i * 3 + 1] = gPlane[i];
        image_data[i * 3 + 2] = rPlane[i];
    }
}
