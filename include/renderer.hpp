#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "types.hpp"
#include "thread_pool.hpp"
#include <memory>

class Renderer
{
public:
    Renderer(const Settings& settings);

    void render(Image &image);

    void render_ispc(Image &image);

    void render_parallel(Image &image);

    void render_parallel_ispc(Image& image);

private:
    std::shared_ptr<ThreadPool> m_pool;
    uint32_t m_max_iter;
    double m_x_min;
    double m_x_max;
    double m_y_min;
    double m_y_max;
};

#endif