#include <vector>
#include <utility>
#include "simulation.hpp"

int Simulation::CountLiveNeighbors(int row, int column) {
    std::vector<std::pair<int, int>> neighborOffsets = {
        {-1, -1}, {-1, 0}, {-1, 1}, 
        { 0, -1},          { 0, 1}, 
        { 1, -1}, { 1, 0}, { 1, 1}
    };
    int neighbors = 0;

    for(const auto& offset : neighborOffsets) {
        int neighborRow = (row + offset.first + grid.GetRows()) % grid.GetRows();
        int neighborColumn = (column + offset.second + grid.GetColumns()) % grid.GetColumns();
        neighbors += grid.GetStatus(neighborRow, neighborColumn);
    }
    return neighbors;
}

void Simulation::ClearGrid() {if(!IsRunning()){grid.Clear();}}

void Simulation::CreateRandomState() {if(!IsRunning()) {grid.FillRandom();}}

void Simulation::Draw() {grid.Draw();}

void Simulation::SetCellStatus(int row, int column, int status) {grid.SetStatus(row, column, status);}

void Simulation::FlipCell(int row, int column) {if(!IsRunning()) {grid.FlipCell(row, column);}}

// Original CPU update method
void Simulation::Update(int& generation) {
    int maxRows = grid.GetRows();
    int maxCols = grid.GetColumns();
    if(IsRunning()) {
        for(int row = 0; row < maxRows; row++) {
            for(int column = 0; column < maxCols; column++) {
                int liveNeighbors = CountLiveNeighbors(row, column);
                int cellStatus = grid.GetStatus(row, column);
                
                if(cellStatus) {
                    if(liveNeighbors > 3 || liveNeighbors < 2) {tempGrid.SetStatus(row, column, 0);}
                    else {tempGrid.SetStatus(row, column, 1);}
                }
                else {
                    if(liveNeighbors == 3) {tempGrid.SetStatus(row, column, 1);}
                }
            }
        }
        ++generation;
        grid = tempGrid;
    }
}

// New GPU update method
void Simulation::UpdateGPU(int& generation) {
    if(IsRunning()) {
        grid.UpdateWithGPU();
        ++generation;
    }
}