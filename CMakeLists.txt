cmake_minimum_required(VERSION 3.10)

# Set project name
project(Blockchain3DVisualizer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the build type (default is Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Check if vcpkg is used
if(EXISTS "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# Add the source directory for RandomX
add_subdirectory(libs/randomx)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Find GLFW and GLM using the recommended CMake targets
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/libs/randomx/src)  # Include RandomX headers

# Add the executable
add_executable(blockchain-3d-visualizer
    src/main.cpp
    src/mining/RandomXMiner.cpp
    src/vulkan/VulkanRenderer.cpp
)

# Link libraries
target_link_libraries(blockchain-3d-visualizer
    PRIVATE
    Vulkan::Vulkan  # Vulkan built-in CMake target
    glfw  # GLFW CMake target
    randomx  # Link with RandomX
)

# Message to verify configuration
message(STATUS "CMake configuration complete for ${CMAKE_SYSTEM_NAME} with C++ standard ${CMAKE_CXX_STANDARD}")
