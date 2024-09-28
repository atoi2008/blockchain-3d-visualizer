# Minimum version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(Blockchain3DVisualizer)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Find GLFW
find_package(glfw3 3.3 REQUIRED)

# Find GLM
find_package(glm REQUIRED)

# Add include directories
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})

# Add source files
set(SOURCES
    src/main.cpp
    src/vulkan/vulkan_renderer.cpp
    src/mining/mining.cpp
    src/utils/utils.cpp
)

# Add header files
set(HEADERS
    include/vulkan_renderer.h
    include/mining.h
    include/utils.h
)

# Add shaders
set(SHADERS
    shaders/vertex_shader.spv
    shaders/fragment_shader.spv
)

# Create executable
add_executable(blockchain-3d-visualizer ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(blockchain-3d-visualizer Vulkan::Vulkan glfw glm)

# Copy shaders to build directory
file(COPY ${SHADERS} DESTINATION ${CMAKE_BINARY_DIR}/shaders)

# Set C++ flags
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(blockchain-3d-visualizer PRIVATE -Wall -Wextra -pedantic)
endif()
