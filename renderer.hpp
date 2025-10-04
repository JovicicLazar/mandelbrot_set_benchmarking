#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "types.hpp"

class Renderer {
    public:
        static void render(const Settings& Settings, Image& image);

        static void render_ispc(const Settings& Settings, Image& image);
};

#endif