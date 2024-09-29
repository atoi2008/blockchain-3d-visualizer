cmake_minimum_required(VERSION 3.10)
project(Blockchain3DVisualizer)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure out-of-source builds
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Set the vcpkg toolchain file if VCPKG_ROOT is defined
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
else()
    message(WARNING "VCPKG_ROOT environment variable is not set. Make sure to set it for vcpkg integration.")
endif()

# Find Vulkan and GLFW
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Include directories
include_directories(${Vulkan_INCLUDE_DIRS})

# Add the executable and source files
add_executable(Blockchain3DVisualizer 
    src/main.cpp 
    src/vulkan/VulkanRenderer.cpp 
    src/mining/RandomXMiner.cpp
)

# Link Vulkan and GLFW libraries
target_link_libraries(Blockchain3DVisualizer Vulkan::Vulkan glfw)

# Platform-specific configurations
if(WIN32)
    # Windows-specific settings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(UNIX)
    # Unix-specific settings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
