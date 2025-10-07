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

export void mandelbrot_parallel(const uniform int y,
                            const uniform int width,
                            const uniform int height,
                            const uniform int maxIter,
                            const uniform double xmin,
                            const uniform double xmax,
                            const uniform double ymin,
                            const uniform double ymax,
                            uniform uint8 image_data[])
{
    foreach (x = 0 ... width)
    {
        double real = xmin + (xmax - xmin) * (double)x / (double)width;
        double imag = ymin + (ymax - ymin) * (double)y / (double)height;
        
        int iteration = mandelbrot(real, imag, maxIter);
        
        int color = (255 * iteration) / maxIter;
        
        int index = (y * width + x) * 3;
        
        image_data[index + 0] = color;  // R
        image_data[index + 1] = color;  // G
        image_data[index + 2] = color;  // B
    }
}