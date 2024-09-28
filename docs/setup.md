# Setup Guide

## Prerequisites

- Ensure you have a working **C++17** environment.
- You need to have **CMake** installed to configure and build the project.

## Vulkan SDK Setup

1. Download and install the Vulkan SDK from [here](https://vulkan.lunarg.com/sdk/home).
2. Add the Vulkan SDK path to your environment variables:
   - On **Linux**:
     ```bash
     export VULKAN_SDK=<path-to-vulkan-sdk>/x86_64
     ```
   - On **Windows**:
     - Add the SDK path to the System Environment Variables.

## OpenCL Setup

- Ensure your system supports OpenCL and that the appropriate drivers are installed:
  - For **NVIDIA** GPUs, install the [CUDA toolkit](https://developer.nvidia.com/cuda-toolkit).
  - For **AMD** GPUs, install [ROCm](https://rocmdocs.amd.com/en/latest/).

## Building on Linux

1. Install dependencies:
   ```bash
   sudo apt-get install cmake libglfw3-dev
