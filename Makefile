CXX = g++
ISPC = ispc

SRC = src
INCLUDE = include
BUILD_DIR = build

CXXFLAGS = -std=c++17 -O3 -Wall -fPIC -I$(INCLUDE)
ISPCFLAGS = --target=avx2-i32x8 -O3 -I$(INCLUDE)

SRCS = main.cpp $(SRC)/renderer.cpp $(SRC)/thread_pool.cpp $(SRC)/image_writer.cpp $(SRC)arg_parser.cpp
ISPC_SRCS = $(SRC)/mandelbrot.ispc $(SRC)/mandelbrot_parallel.ispc
API_SRC = $(SRC)/mandelbrot_api.cpp

ISPC_OBJS = $(BUILD_DIR)/mandelbrot.o $(BUILD_DIR)/mandelbrot_parallel.o

CPP_OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/renderer.o $(BUILD_DIR)/thread_pool.o \
           $(BUILD_DIR)/image_writer.o $(BUILD_DIR)/arg_parser.o

OBJS = $(ISPC_OBJS) $(CPP_OBJS)

TARGET = $(BUILD_DIR)/mandelbrot
SHARED_TARGET = $(BUILD_DIR)/libmandelbrot.so

all: $(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# --- ISPC Compilation Rules (built first) ---
$(BUILD_DIR)/mandelbrot.o: $(SRC)/mandelbrot.ispc | $(BUILD_DIR)
	$(ISPC) $(ISPCFLAGS) $< -o $@ -h $(INCLUDE)/mandelbrot_ispc.h

$(BUILD_DIR)/mandelbrot_parallel.o: $(SRC)/mandelbrot_parallel.ispc | $(BUILD_DIR)
	$(ISPC) $(ISPCFLAGS) $< -o $@ -h $(INCLUDE)/mandelbrot_parallel.h

# --- C++ Compilation Rules (depend on ISPC headers) ---
$(BUILD_DIR)/main.o: main.cpp $(INCLUDE)/mandelbrot_ispc.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/renderer.o: $(SRC)/renderer.cpp $(INCLUDE)/mandelbrot_ispc.h \
                         $(INCLUDE)/mandelbrot_parallel.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/thread_pool.o: $(SRC)/thread_pool.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/image_writer.o: $(SRC)/image_writer.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/arg_parser.o: $(SRC)/arg_parser.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Python Shared Library ---
$(SHARED_TARGET): $(ISPC_OBJS) $(API_SRC) $(CPP_OBJS) | $(BUILD_DIR)
	$(CXX) -shared $(CXXFLAGS) -o $@ $^

# Convenience target for shared library
lib: $(SHARED_TARGET)

# Clean
clean:
	rm -rf $(BUILD_DIR) $(INCLUDE)/mandelbrot_parallel.h $(INCLUDE)/mandelbrot_ispc.h

.PHONY: all lib clean