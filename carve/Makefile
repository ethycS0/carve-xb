
# Compiler and flags
CXX = g++
NVCC = nvcc
CXXFLAGS = -std=c++17 -Wall -g -I/opt/cuda/include -Iinclude
NVCCFLAGS = -std=c++17 -Xcompiler "-Wall -g" -G -I/opt/cuda/include

# Directories
SRC_DIR = .
FILET_DIR = ./filetypes
OBJ_DIR = ./obj

# Source files and objects
CPP_SRCS = $(SRC_DIR)/xfs.cpp $(SRC_DIR)/main.cpp  

CPP_OBJS = $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
CU_OBJS = $(CU_SRCS:$(SRC_DIR)/%.cu=$(OBJ_DIR)/%.o)

# Default output executable
EXEC = main

# Default target: build with carve.cpp
all: $(EXEC)

# Build with CUDA carve.cu
cuda: $(CUDA_EXEC)

# Linking rules
$(EXEC): $(CPP_OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(CPP_OBJS)

$(CUDA_EXEC): $(CPP_OBJS) $(CU_OBJS)
	$(NVCC) $(NVCCFLAGS) -o $(CUDA_EXEC) $(CPP_OBJS) $(CU_OBJS) -lcudart

# Object file generation rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR) $(OBJ_DIR)/$(FILET_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cu
	@mkdir -p $(OBJ_DIR)
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(EXEC) $(CUDA_EXEC)
	rm -rf output

# Install placeholder
install: $(EXEC)
	# Add installation steps here if necessary (e.g., copy to /usr/local/bin)

parse:
	$(CXX) $(CXXFLAGS) parser.cpp -o parser

.PHONY: all clean install cuda

