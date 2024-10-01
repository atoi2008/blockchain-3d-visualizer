cmake_minimum_required(VERSION 3.10)

# Set project name
project(Blockchain3DVisualizer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Check if vcpkg toolchain is available and set the toolchain file
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# Platform-specific configurations
if(MSVC)
    message(STATUS "Configuring for MSVC (Windows)")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)  # Suppress MSVC warnings
else()
    message(STATUS "Configuring for Linux/WSL")
endif()

# Add the RandomX library
add_subdirectory(libs/randomx)  # RandomX has its own CMakeLists.txt

# Set the path for RandomX include directory
set(RandomX_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/randomx/src")


# Add include directories for RandomX
target_include_directories(randomx PUBLIC
    "${RandomX_INCLUDE_DIR}"
)
# Find Vulkan
find_package(Vulkan REQUIRED)

# Find GLFW and GLM using vcpkg
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

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
    glm::glm  # glm CMake target
    randomx  # Link with RandomX from add_subdirectory
)

# Message to verify configuration
message(STATUS "CMake configuration complete for ${CMAKE_SYSTEM_NAME} with C++ standard ${CMAKE_CXX_STANDARD}")
