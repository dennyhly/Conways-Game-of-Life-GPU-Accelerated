# Conway's Game of Life (C++ with GPU Acceleration)

## Project Overview

This project is a C++ implementation of **Conway's Game of Life**, accelerated using the GPU, and visualized through a 2560×1440p window using **Raylib**. The goal of this project is the highlight the performance benefits available for certain algorithms when rendered on the GPU vs rendered on the CPU.

Conway's Game of Life is a  fairly simple 2D, zero-player, grid-based simulation that models cell life and death according to four simple rules. These rules produce complex and dynamic behaviors — often mimicking the growth patterns of microbial colonies.

<img src="./resources/Conway - R.PNG" alt="Screenshot_random" width="1080"/>

## Game Rules

Each cell interacts with its **8 neighboring cells** and follows these rules every generation:

1. **Underpopulation**: Any live cell with fewer than two live neighbors dies.
2. **Survival**: Any live cell with two or three live neighbors lives on.
3. **Overpopulation**: Any live cell with more than three live neighbors dies.
4. **Reproduction**: Any dead cell with exactly three live neighbors becomes a live cell.


## Features

- Full **serial CPU implementation**
- **GPU acceleration** using OpenGL fragment shaders
- Real-time rendering using **Raylib**
- Support for interactive input (click to toggle cells, random initialization)
- **Benchmark mode** to compare CPU vs GPU performance

## Implementation Details

- **Main Loop**: Handles input, updates simulation, and renders using Raylib.
- **Simulation Class**: Coordinates the state of the game, updates grid based on rules.
- **Grid Class**: Manages the 2D vector of cells, and provides methods for manipulation.
- **GPU Acceleration**: Utilizes fragment shaders to offload cell-update logic onto the GPU. Results are mapped back to the CPU for accurate cell state tracking.
- **Benchmarking**: PerformanceMonitor class and `BenchmarkResults` struct collect and report performance data over 1000 generations.

## How to Run

- Once cloned into VSCode press **F5** to run the MakeFile and launch the executable.
- **R** to initialize and reset the grid with a random pattern.
- **C** to clear the grid.
- **Spacebar** or **Enter** to pause the game.
- **F** to increase the rendering FPS.
- **S** to decrease the rendering FPS.
- **Left Click** while the game is paused to switch any cell's status.
- **B** to run Benchmark Mode.
- **Esc** to exit out of the game.

## Results
Below is a snippet from some of my benchmark results. These results show that on average there is a 10x decrease in Average Update Times (AUT) and Minimum Update Time (MUT) from switching the game logic and calculations from rendering on the GPU vs on the CPU. The Maximum Update Time stays the same due to the initializing the game on the CPU for both implementations, but can be optimized by performing the initializing on the GPU for the GPU implementation of the game logic.

CPU Implementation
| Metric                 | Value        |
| ---------------------- | ------------ |
| Generations Tested     | 1000         |
| Average Update Time    | 17,759.03 μs |
| Minimum Update Time    | 16,039.00 μs |
| Maximum Update Time    | 22,690.00 μs |
| Average FPS Equivalent | 56.31        |
| Timestamp              | `1749620007` |

GPU Implementation
| Metric                 | Value        |
| ---------------------- | ------------ |
| Generations Tested     | 1000         |
| Average Update Time    | 1,778.04 μs  |
| Minimum Update Time    | 1,487.00 μs  |
| Maximum Update Time    | 22,366.00 μs |
| Average FPS Equivalent | 562.42       |
| Timestamp              | `1749620030` |


### Prerequisites

- **Raylib** and its dependencies installed
- CUDA support installed (if experimenting with CUDA)
- Proper compiler setup (MinGW for GCC or MSVC for CUDA)
