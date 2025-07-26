#include <raylib.h>
#include "grid.hpp"

bool Grid::inBounds(int row, int column) {return (row >= 0 && row < rows && column >= 0 && column < columns);}

int Grid::GetStatus(int row, int column) {
    if(inBounds(row, column)) {
        return cells[row][column];
    }
    return 0;
}

void Grid::Clear() {
    for(int row = 0; row < rows; row++) {
        for(int column = 0; column < columns; column++) {
            cells[row][column] = 0;
        }
    }
}

void Grid::Draw() {
    for(int row = 0; row < rows; row ++) {
        for(int column = 0; column < columns; column++) {
            Color color = cells[row][column] ? Color{255, 128, 164, 255} : Color{89, 115, 65, 255};
            DrawRectangle(column * cellSize, row * cellSize, cellSize - 1, cellSize - 1, color);
        }
    }
}

void Grid::FillRandom() {
    for(int row = 0; row < rows; row ++) {
        for(int column = 0; column < columns; column++) {
            int randomValue = GetRandomValue(0, 4);
            cells[row][column] = (randomValue == 4) ? 1 : 0;
        }
    }
}

void Grid::FlipCell(int row, int column) {
    if(inBounds(row, column)) {
        cells[row][column] = !cells[row][column];
    }
}

void Grid::SetStatus(int row, int column, int status) {
    if(inBounds(row, column)) {
        cells[row][column] = status;
    }
}

void Grid::InitGPUCompute() {
    if(gpuInitialized) return;
    
    tempCells.resize(rows);
    for(int i = 0; i < rows; i++) {
        tempCells[i].resize(columns, 0);
    }
        
    computeTexture1 = LoadRenderTexture(columns, rows);
    computeTexture2 = LoadRenderTexture(columns, rows);
    
    SetTextureFilter(computeTexture1.texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(computeTexture2.texture, TEXTURE_FILTER_POINT);
    
    computeShader = LoadShader(0, "conway_compute.fs");
    gridSizeLocation = GetShaderLocation(computeShader, "gridSize");

    UploadGridToTexture(computeTexture1);
    
    BeginTextureMode(computeTexture2);
    ClearBackground(BLACK);
    EndTextureMode();
    
    gpuInitialized = true;
}

void Grid::CleanupGPU() {
    if(gpuInitialized) {
        UnloadRenderTexture(computeTexture1);
        UnloadRenderTexture(computeTexture2);
        UnloadShader(computeShader);
        gpuInitialized = false;
    }
}

void Grid::UploadGridToTexture(RenderTexture2D& target) {
    std::vector<unsigned char> textureData(columns * rows * 4);
    
    for(int row = 0; row < rows; row++) {
        for(int col = 0; col < columns; col++) {
            int index = (row * columns + col) * 4;
            unsigned char value = cells[row][col] ? 255 : 0;
            textureData[index + 0] = value;  // R
            textureData[index + 1] = value;  // G  
            textureData[index + 2] = value;  // B
            textureData[index + 3] = 255;    // Alpha
        }
    }
    UpdateTexture(target.texture, textureData.data());
}

void Grid::UpdateWithGPU() {
    if(!gpuInitialized) InitGPUCompute();
    
    UploadGridToTexture(computeTexture1);
    
    Vector2 gridSizeVec = {(float)columns, (float)rows};
    SetShaderValue(computeShader, gridSizeLocation, &gridSizeVec, SHADER_UNIFORM_VEC2);
    
    BeginTextureMode(computeTexture2);
    ClearBackground(BLACK);
    BeginShaderMode(computeShader);
    DrawTexture(computeTexture1.texture, 0, 0, WHITE);
    EndShaderMode();
    EndTextureMode();
    
    Image resultImage = LoadImageFromTexture(computeTexture2.texture);
    if(resultImage.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) {
        ImageFormat(&resultImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    }
    
    Color* pixels = LoadImageColors(resultImage);
    for(int row = 0; row < rows; row++) {
        for(int col = 0; col < columns; col++) {
            int index = row * columns + col;
            cells[row][col] = (pixels[index].r > 127) ? 1 : 0;
        }
    }

    UnloadImageColors(pixels);
    UnloadImage(resultImage);
    
    RenderTexture2D temp = computeTexture1;
    computeTexture1 = computeTexture2;
    computeTexture2 = temp;
}