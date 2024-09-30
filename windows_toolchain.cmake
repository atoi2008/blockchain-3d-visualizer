# Target Windows
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 10)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Specify the location of the MSVC compilers
set(CMAKE_C_COMPILER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe")
set(CMAKE_CXX_COMPILER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe")

# Force CMake to skip ABI checks to avoid failures in cross-compiling
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# Set the paths to the Windows SDK and MSVC runtime if required
set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0.22621.0")

# Vulkan SDK paths
set(VULKAN_SDK_PATH "E:/VulkanSDK/1.3.290.0")
set(Vulkan_INCLUDE_DIR "${VULKAN_SDK_PATH}/Include")
set(Vulkan_LIBRARY "${VULKAN_SDK_PATH}/Lib/vulkan-1.lib")

# Include the Vulkan package
find_package(Vulkan REQUIRED)

# vcpkg paths
set(VCPKG_ROOT "E:/source/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# Include directories for GLFW and GLM from vcpkg
set(GLFW_INCLUDE_DIR "${VCPKG_ROOT}/installed/x64-windows/include")
set(GLM_INCLUDE_DIR "${VCPKG_ROOT}/installed/x64-windows/include")

# Add include directories
include_directories(
    "${GLFW_INCLUDE_DIR}"
    "${GLM_INCLUDE_DIR}"
    "${Vulkan_INCLUDE_DIR}"
)

# Link GLFW and GLM
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Message to verify toolchain usage
message(STATUS "Using Windows toolchain with MSVC, Vulkan SDK, GLFW, and GLM")
