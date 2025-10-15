# Mandelbrot Set Renderer

High-performance Mandelbrot set renderer with SIMD vectorization (ISPC), multi-threading, and Python benchmarking.

## Features

- Multiple rendering modes: Serial, ISPC, Parallel, Parallel+ISPC
- BMP output
- Python benchmarking suite with visualization
- Configurable resolution and iteration count

## Prerequisites

- C++ compiler with C++17 support (GCC 7+)
- [Intel ISPC Compiler](https://ispc.github.io/)
- Python 3.7+ with matplotlib (for benchmarking)

## Building

```bash
# Build C++ executable
make all

# Build Python shared library
make lib

# Clean build
make clean
```

## Usage

### C++ Tool

```bash
# Basic usage
./build/mandelbrot --width 1920 --height 1080 --file-name output.bmp

# Show help
./build/mandelbrot --help
```

**Available modes**: `serial`, `ispc`, `parallel`, `parallel-ispc`

### Python Benchmarking

```bash
# Run benchmark suite
python3 benchmark.py
```

This will:
- Test all rendering implementations
- Generate performance comparison plots
- Save results to `benchmark_images/`
- Display speedup analysis

**Customize benchmark** by editing `benchmark.py`:
```python
config = BenchmarkConfig(
    max_iterations=1000,
    sizes=[500, 1000, 2000, 4000],
    output_dir=Path("my_results")
)
```

## Command-Line Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--iter-count` | int | 1000 | Maximum iterations |
| `--x-min` | double | -2.0 | Min x coordinate |
| `--x-max` | double | 1.0 | Max x coordinate |
| `--y-min` | double | -1.5 | Min y coordinate |
| `--y-max` | double | 1.5 | Max y coordinate |
| `--width` | int | 800 | Image width |
| `--height` | int | 600 | Image height |
| `--file-name` | string | output.bmp | Output file |
| `--mode` | string | parallel-ispc | Render mode |

## Examples

```bash
# High-res 4K image
./build/mandelbrot --width 3840 --height 2160 --iter-count 2000 --file-name 4k.bmp

# Quick preview
./build/mandelbrot --width 800 --height 600 --iter-count 100 --mode serial

# Zoom into region
./build/mandelbrot --x-min -0.8 --x-max -0.4 --y-min -0.2 --y-max 0.2 --width 2000 --height 2000
```