CXX = g++
ISPC = ispc

SRC = src
INCLUDE = include

CXXFLAGS = -std=c++17 -O3 -Wall -I$(INCLUDE)
ISPCFLAGS = --target=avx2-i32x8 -O3 -I$(INCLUDE)

SRCS = main.cpp $(SRC)/renderer.cpp $(SRC)/thread_pool.cpp $(SRC)/image_writer.cpp
ISPC_SRCS = $(SRC)/mandelbrot.ispc $(SRC)/mandelbrot_parallel.ispc $(SRC)/mandelbrot_parallel_optimized.ispc

OBJS = main.o renderer.o thread_pool.o image_writer.o mandelbrot.o mandelbrot_parallel.o mandelbrot_parallel_optimized.o

TARGET = mandelbrot

all: mandelbrot.o mandelbrot_parallel.o mandelbrot_parallel_optimized.o $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

mandelbrot.o: $(SRC)/mandelbrot.ispc
	$(ISPC) $(ISPCFLAGS) $< -o $@ -h $(INCLUDE)/mandelbrot_ispc.h

mandelbrot_parallel.o: $(SRC)/mandelbrot_parallel.ispc
	$(ISPC) $(ISPCFLAGS) $< -o $@ -h $(INCLUDE)/mandelbrot_parallel.h

mandelbrot_parallel_optimized.o: $(SRC)/mandelbrot_parallel_optimized.ispc
	$(ISPC) $(ISPCFLAGS) $< -o $@ -h $(INCLUDE)/mandelbrot_parallel_optimized.h

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

renderer.o: $(SRC)/renderer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

thread_pool.o: $(SRC)/thread_pool.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

image_writer.o: $(SRC)/image_writer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(INCLUDE)/mandelbrot_parallel.h $(INCLUDE)/mandelbrot_ispc.h $(INCLUDE)/mandelbrot_parallel_optimized.h

.PHONY: all clean 
