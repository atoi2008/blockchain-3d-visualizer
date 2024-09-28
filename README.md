# Blockchain 3D Visualizer

A C++ project that integrates **CPU mining** using the **RandomX** algorithm with **Vulkan** for real-time 3D visualization of the mining process. This project aims to create a procedural 3D environment that dynamically visualizes data from mining activities without relying on GPU-based computations for the mining algorithm.

## Project Structure

```bash
blockchain-3d-visualizer/
├── CMakeLists.txt            # CMake build configuration
├── README.md                 # Project documentation
├── src/                      # C++ source files
│   ├── main.cpp              # Main entry point
│   ├── mining/               # Mining-related code (CPU implementation)
│   ├── vulkan/               # Vulkan 3D visualization engine
│   └── utils/                # Helper functions and utilities
├── include/                  # Header files
├── shaders/                  # Vulkan shaders (vertex and fragment)
├── assets/                   # (Optional) Assets such as textures/models
└── docs/                     # Documentation (project guides, API docs)
