#include "renderer.hpp"
#include "../include/mandelbrot_ispc.h"
#include "../include/mandelbrot_parallel.h"
#include "thread_pool.hpp"
#include <complex>
#include <cstdint>

#include <iostream>

int inline mandelbrot(double x, double y, int max_iterations);

extern void ispc::mandelbrot_ispc(const int32_t width,
                                  const int32_t height,
                                  const int32_t maxIter,
                                  const double xmin,
                                  const double xmax,
                                  const double ymin,
                                  const double ymax,
                                  uint8_t *image_data);

extern void ispc::mandelbrot_parallel(const int32_t y,
                                      const int32_t width,
                                      const int32_t height,
                                      const int32_t maxIter,
                                      const double xmin,
                                      const double xmax,
                                      const double ymin,
                                      const double ymax,
                                      uint8_t *image_data);

Renderer::Renderer(const Settings &settings)
{
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

void Renderer::render_ispc(Image &image)
{
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
        m_pool->enqueue([=, &image_data]()
        {
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

    m_pool->wait();
}

void Renderer::render_parallel_ispc(Image &image)
{
    const uint32_t image_width = image.image_width;
    const uint32_t image_height = image.image_height;
    auto &image_data = image.image_data;

    image_data.resize((image_width * image_height) * 3);

    for (uint32_t y = 0; y < image_height; ++y)
    {
        m_pool->enqueue([=, &image_data]()
        { 
            ispc::mandelbrot_parallel(y,
                                    image_width,
                                    image_height,
                                    m_max_iter,
                                    m_x_min,
                                    m_x_max,
                                    m_y_min,
                                    m_y_max,
                                    image_data.data()); 
        });
    }

    m_pool->wait();
}
