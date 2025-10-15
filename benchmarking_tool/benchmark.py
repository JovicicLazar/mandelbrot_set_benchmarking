import ctypes
import matplotlib.pyplot as plt
from pathlib import Path
from dataclasses import dataclass
from typing import Dict, List, Callable
from enum import Enum

class RenderMethod(Enum):
    """Enum for different rendering methods"""
    SERIAL = "render"
    ISPC = "render_ispc"
    PARALLEL = "render_parallel"
    PARALLEL_ISPC = "render_parallel_ispc"


@dataclass
class BenchmarkConfig:
    """Configuration for benchmark runs"""
    max_iterations: int = 25
    x_min: float = -2.0
    x_max: float = 1.0
    y_min: float = -1.5
    y_max: float = 1.5
    sizes: List[int] = None
    output_dir: Path = Path("benchmark_images")
    
    def __post_init__(self):
        if self.sizes is None:
            self.sizes = [500, 1000, 2000, 4000]
        self.output_dir.mkdir(exist_ok=True)


class MandelbrotLibrary:
    """Wrapper for the C++ shared library"""
    
    def __init__(self, lib_path: str = "../build/libmandelbrot.so"):
        self.lib = ctypes.CDLL(lib_path)
        self._setup_function_signatures()
        self._renderer = None
    
    def _setup_function_signatures(self):
        """Setup ctypes function signatures"""
        self.lib.create_renderer.restype = ctypes.c_void_p
        self.lib.create_renderer.argtypes = [
            ctypes.c_int, ctypes.c_double, ctypes.c_double, 
            ctypes.c_double, ctypes.c_double
        ]
        
        for method in RenderMethod:
            func = getattr(self.lib, method.value)
            func.restype = ctypes.c_double
            func.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_char_p]
        
        self.lib.destroy_renderer.argtypes = [ctypes.c_void_p]
    
    def create_renderer(self, config: BenchmarkConfig):
        """Create a new renderer instance"""
        self._renderer = self.lib.create_renderer(
            config.max_iterations,
            config.x_min, config.x_max,
            config.y_min, config.y_max
        )
        return self
    
    def get_render_function(self, method: RenderMethod) -> Callable:
        """Get render function by method"""
        return getattr(self.lib, method.value)
    
    def render(self, method: RenderMethod, width: int, height: int, filepath: str) -> float:
        """Render using specified method and return elapsed time"""
        func = self.get_render_function(method)
        return func(self._renderer, width, height, filepath.encode('utf-8'))
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self._renderer:
            self.lib.destroy_renderer(self._renderer)


class BenchmarkRunner:
    """Handles benchmark execution and result collection"""
    
    def __init__(self, library: MandelbrotLibrary, config: BenchmarkConfig):
        self.library = library
        self.config = config
        self.results: Dict[str, List[float]] = {}
    
    def run(self):
        """Run all benchmarks"""
        for method in RenderMethod:
            print(f"\n{'='*60}")
            print(f"Running {method.name} ({method.value})")
            print('='*60)
            
            times = self._benchmark_method(method)
            self.results[method.name] = times
        
        return self.results
    
    def _benchmark_method(self, method: RenderMethod) -> List[float]:
        """Benchmark a single render method across all sizes"""
        times = []
        
        for size in self.config.sizes:
            filepath = self.config.output_dir / f"output_{method.value}_{size}.bmp"
            
            elapsed = self.library.render(method, size, size, str(filepath))
            times.append(elapsed)
            
            print(f"  ✓ {size:5d}×{size:<5d} completed in {elapsed:6.2f}s")
        
        return times
    
    def print_summary(self):
        """Print benchmark summary"""
        print("\n" + "="*60)
        print("BENCHMARK SUMMARY")
        print("="*60)
        
        for method_name, times in self.results.items():
            total = sum(times)
            avg = total / len(times)
            print(f"{method_name:20s}: {total:7.2f}s total | {avg:6.2f}s avg")


class BenchmarkVisualizer:
    """Handles visualization of benchmark results"""
    
    def __init__(self, config: BenchmarkConfig):
        self.config = config
    
    def plot(self, results: Dict[str, List[float]]):
        """Create and save benchmark plots"""
        plt.figure(figsize=(14, 8))
        
        for method_name, times in results.items():
            plt.plot(
                self.config.sizes, 
                times, 
                marker='o', 
                linestyle='-', 
                linewidth=2,
                markersize=8,
                label=method_name
            )
        
        plt.title("Mandelbrot Set Render Time vs Image Size", fontsize=16, fontweight='bold')
        plt.xlabel("Image Size (Width = Height)", fontsize=12)
        plt.ylabel("Render Time (seconds)", fontsize=12)
        plt.legend(fontsize=10, loc='upper left')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        output_path = self.config.output_dir / "benchmark_results.png"
        plt.savefig(output_path, dpi=300)
        print(f"Plot saved to: {output_path}")
        plt.show()
    
    def plot_speedup(self, results: Dict[str, List[float]]):
        """Plot speedup relative to serial implementation"""
        if "SERIAL" not in results:
            return
        
        baseline = results["SERIAL"]
        
        plt.figure(figsize=(14, 8))
        
        for method_name, times in results.items():
            if method_name == "SERIAL":
                continue
            
            speedups = [baseline[i] / times[i] for i in range(len(times))]
            plt.plot(
                self.config.sizes,
                speedups,
                marker='s',
                linestyle='-',
                linewidth=2,
                markersize=8,
                label=method_name
            )
        
        plt.axhline(y=1.0, color='r', linestyle='--', alpha=0.5, label='Baseline (Serial)')
        plt.title("Speedup Relative to Serial Implementation", fontsize=16, fontweight='bold')
        plt.xlabel("Image Size (Width = Height)", fontsize=12)
        plt.ylabel("Speedup Factor", fontsize=12)
        plt.legend(fontsize=10, loc='best')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        output_path = self.config.output_dir / "speedup_results.png"
        plt.savefig(output_path, dpi=300)
        print(f"Speedup plot saved to: {output_path}")
        plt.show()


def main():
    """Main execution function"""
    
    config = BenchmarkConfig(
        max_iterations=25,
        sizes=[500, 1000, 2000, 4000],
        output_dir=Path("benchmark_images")
    )
    
    with MandelbrotLibrary().create_renderer(config) as mandelbrot:
        runner = BenchmarkRunner(mandelbrot, config)
        results = runner.run()
        runner.print_summary()
    
    visualizer = BenchmarkVisualizer(config)
    visualizer.plot(results)
    visualizer.plot_speedup(results)


if __name__ == "__main__":
    main()