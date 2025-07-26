#pragma once
#include <vector>
#include <raylib.h>

class Grid
{
    public:
        Grid(int width, int height, int sizeOfCell) : rows(height/sizeOfCell), columns(width/sizeOfCell), cellSize(sizeOfCell), cells(rows, std::vector<int>(columns, 0)) {};
        bool inBounds(int row, int column);
        int GetColumns() {return columns;}
        int GetRows() {return rows;}
        int GetStatus(int row, int column);
        void Clear();
        void Draw();
        void FillRandom();
        void FlipCell(int row, int column);
        void SetStatus(int row, int column, int status);

        void InitGPUCompute();
        void UpdateWithGPU();
        void CleanupGPU();
        void UploadGridToTexture(RenderTexture2D& target);  // New helper method
        void DownloadTextureToTempGrid(RenderTexture2D& source);
        void ApplyGPUResults();
        int GetTempStatus(int row, int column) const;
        void SetTempStatus(int row, int column, int value);
        void DebugInputData();
        
    private:
        int cellSize;
        int columns;
        int rows;
        std::vector<std::vector<int>> cells;
        std::vector<std::vector<int>> tempCells; // Add this for GPU results

        RenderTexture2D computeTexture1;
        RenderTexture2D computeTexture2;
        Shader computeShader;
        int gridSizeLocation;
        bool gpuInitialized = false;
        bool firstGPUUpdate = true;
};