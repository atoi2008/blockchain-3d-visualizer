
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
```

## Features

- **CPU Mining**: Utilizes the RandomX algorithm for mining, executed on the CPU for efficiency and accessibility.
- **Vulkan Visualization**: Renders real-time 3D environments reflecting mining data.
- **Cross-Platform**: Designed to run on multiple operating systems (Windows, Linux, macOS) with appropriate CPU support.
- **Procedural Graphics**: Visualizes data such as hash rate, nonce attempts, and block progress using dynamic 3D models and particle effects.

## Dependencies

- **Vulkan SDK**: (Required for 3D rendering)
- **GLFW**: (For windowing and input handling)
- **GLM**: (For 3D math operations)

## Getting Started

### Prerequisites

- **CMake**: Install CMake for building the project from [CMake Download](https://cmake.org/download/).
  
- **Vulkan SDK**: Install Vulkan SDK from [Vulkan SDK Website](https://vulkan.lunarg.com/sdk/home).
  
- **GLFW**: Install GLFW for window creation and input handling.
  - On Linux (via apt): 
    ```bash
    sudo apt-get install libglfw3-dev
    ```
  - On macOS (via Homebrew): 
    ```bash
    brew install glfw
    ```

- **GLM**: A header-only math library.

## Building the Project

To build and run the project, follow these steps:

### Clone the repository

```bash
git clone https://github.com/your-username/blockchain-3d-visualizer.git
cd blockchain-3d-visualizer
```

### Create a build directory

```bash
mkdir build && cd build
```

### Configure the build system using CMake

```bash
cmake ..
```

### Build the project

```bash
make
```

### Run the executable

```bash
./blockchain-3d-visualizer
```

## File Overview

- **main.cpp**: The main entry point, handling initialization of both the CPU mining engine and the Vulkan renderer.
- **mining/mining.cpp**: Manages CPU mining processes using the RandomX algorithm.
- **vulkan/vulkan_renderer.cpp**: Sets up Vulkan and renders the real-time 3D environment.
- **shaders/vertex_shader.vert** & **fragment_shader.frag**: GLSL shaders for 3D rendering.

## Contributing

Feel free to contribute to this project by:

- Reporting bugs or issues.
- Suggesting features or improvements.
- Submitting pull requests for code contributions.

### Contribution Workflow

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Make your changes.
4. Commit and push your changes (`git push origin feature-branch`).
5. Submit a pull request.

## Donate to the Project

If you appreciate the work and would like to support the development of this project, please consider making a donation.

### Cryptocurrency Donations

- **Bitcoin Address**: `3826jB3kQiq3mKRni3MkR6hRXEKBqRqCLR`
- **Ethereum Address**: `0xf53FD060Ae80623507689501598fE53A05E078A9`

Your support will help improve and expand the project, making it better for everyone in the community.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
