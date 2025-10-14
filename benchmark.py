import ctypes
import matplotlib.pyplot as plt
import os

SHARED_LIBRARY_PATH = "./build/libmandelbrot.so"

lib = ctypes.CDLL(SHARED_LIBRARY_PATH)

lib.create_renderer.restype = ctypes.c_void_p
lib.create_renderer.argtypes = [
    ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double
]

lib.render.restype = ctypes.c_double
lib.render.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_char_p]

lib.render_ispc.restype = ctypes.c_double
lib.render_ispc.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_char_p]

lib.render_parallel.restype = ctypes.c_double
lib.render_parallel.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_char_p]

lib.render_parallel_ispc.restype = ctypes.c_double
lib.render_parallel_ispc.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_char_p]

lib.destroy_renderer.argtypes = [ctypes.c_void_p]

renderer = lib.create_renderer(25, -2.0, 1.0, -1.5, 1.5)

sizes = [500, 1000, 2000, 4000]
tests = {
    'render': lib.render,
    'render_ispc': lib.render_ispc,
    'render_parallel': lib.render_parallel,
    'render_parallel_ispc': lib.render_parallel_ispc,
}

results = {}

os.makedirs("benchmark_images", exist_ok=True)

for test_name, test_func in tests.items():
    print(f"\nRunning {test_name}...")
    tmp_times = []
    
    for size in sizes:
        file_path = f"./benchmark_images/output_{test_name}_{size}.bmp".encode("utf-8")
        elapsed = test_func(renderer, size, size, file_path)
        tmp_times.append(elapsed)
        print(f"  {size}x{size} done in {elapsed:.2f}s")
    
    results[test_name] = tmp_times

lib.destroy_renderer(renderer)

plt.figure(figsize=(12, 7))

for test_name, times in results.items():
    plt.plot(sizes, times, marker="o", linestyle="-", label=test_name)

plt.title("Mandelbrot Render Time vs Image Size")
plt.xlabel("Image Size (Width = Height)")
plt.ylabel("Render Time (seconds)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("./benchmark_images/benchmark_results.png")
plt.show()

# Print summary
print("\n=== Summary ===")
for test_name, times in results.items():
    total_time = sum(times)
    print(f"{test_name}: {total_time:.2f}s total")