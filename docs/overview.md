
---
# Project Overview

## Architecture

The project is structured into two major components:

1. **Mining Engine** (CPU):
   - Executes a RandomX-based mining algorithm on the CPU, collecting real-time data such as hash rate, nonce attempts, and block progress.
   - This data is made available to the rendering engine for visualization purposes.

2. **Visualization Engine** (Vulkan):
   - The Vulkan renderer visualizes mining data by creating a procedural 3D environment.
   - Mining metrics like hash rate, block progress, and nonce attempts control various aspects of the scene (e.g., particle effects, object sizes, color changes).

## Threading Model

The mining and rendering processes run concurrently, with mining data being shared across threads. Thread safety is ensured by:
- Using **mutexes** or **atomic variables** to control access to shared data.
- The rendering thread reads mining data at regular intervals to update the visualization.

## Future Enhancements

- **Improved Graphics**: Procedural terrain generation based on mining difficulty.
- **Networked Mining**: Real-time data from multiple miners visualized in a shared 3D space.
- **Customizable Visuals**: Allow users to change rendering styles based on mining data (e.g., different color schemes for high hash rates).
