#pragma once
#include "grid.hpp"

class Simulation
{
    public:
        Simulation(int width, int height, int cellSize) : grid(width, height, cellSize), tempGrid(width, height, cellSize), on(false) {};
        bool IsRunning() {return on;}
        int CountLiveNeighbors(int row, int column);
        void ClearGrid();
        void CreateRandomState();
        void Draw();
        void SetCellStatus(int row, int column, int status);
        void Start() {on = true;}
        void Stop() {on = false;}
        void FlipCell(int row, int column);
        void Update(int& generation);          // Original CPU update
        void UpdateGPU(int& generation);       // New GPU update method
    private:
        bool on;
        Grid grid;
        Grid tempGrid;
};