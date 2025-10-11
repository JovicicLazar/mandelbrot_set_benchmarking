static inline int mandelbrot(double re, double im, int max_iterations)
{
    double z_re = re;
    double z_im = im;
    int iterations = 0;
    
    while (iterations < max_iterations && (z_re * z_re + z_im * z_im) <= 4.0)
    {
        double new_re = (z_re * z_re) - (z_im * z_im);
        double new_im = 2.0 * z_re * z_im;
        
        unmasked {
            z_re = new_re + re;
            z_im = new_im + im;
        }
        ++iterations;
    }
    
    return iterations;
}

export void mandelbrot_parallel_optimized(
    uniform int y,
    uniform int width,
    uniform int height,
    uniform int maxIter,
    uniform double xmin,
    uniform double xmax,
    uniform double ymin,
    uniform double ymax,
    uniform uint8 rPlane[],
    uniform uint8 gPlane[],
    uniform uint8 bPlane[])
{
    double x_range = xmax - xmin;
    double y_range = ymax - ymin;
    double inv_width = 1.0 / (double)width;
    double inv_height = 1.0 / (double)height;

    double imag = ymin + y_range * y * inv_height;

    foreach (x = 0 ... width)
    {
        double real = xmin + x_range * x * inv_width;

        int iteration = mandelbrot(real, imag, maxIter);
        uint8 color = (255 * iteration) / maxIter;

        rPlane[x] = color;
        gPlane[x] = color;
        bPlane[x] = color;
    }
}
