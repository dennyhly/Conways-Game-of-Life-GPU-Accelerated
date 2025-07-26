#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <iomanip>
#include "raylib.h"
#include "simulation.hpp"

using namespace std;

struct BenchmarkResults {
    double avgUpdateTime;
    double minUpdateTime;
    double maxUpdateTime;
    int generationsTested;
    string implementationType;
};

class PerformanceMonitor {
private:
    vector<double> updateTimes;
    chrono::high_resolution_clock::time_point lastUpdateStart;
    bool isTiming;
    
public:
    PerformanceMonitor() : isTiming(false) {}
    void StartTiming() {
        lastUpdateStart = chrono::high_resolution_clock::now();
        isTiming = true;
    }
    void EndTiming() {
        if (isTiming) {
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - lastUpdateStart);
            updateTimes.push_back(duration.count());
            isTiming = false;
        }
    }
    BenchmarkResults GetResults(const string& type) {
        BenchmarkResults results;
        results.implementationType = type;
        results.generationsTested = updateTimes.size();
        
        if (updateTimes.empty()) {
            results.avgUpdateTime = results.minUpdateTime = results.maxUpdateTime = 0.0;
            return results;
        }
        
        double sum = 0.0;
        double minTime = updateTimes[0];
        double maxTime = updateTimes[0];
        
        for (double time : updateTimes) {
            sum += time;
            minTime = min(minTime, time);
            maxTime = max(maxTime, time);
        }
        
        results.avgUpdateTime = sum / updateTimes.size();
        results.minUpdateTime = minTime;
        results.maxUpdateTime = maxTime;
        
        return results;
    }
    void Reset() {
        updateTimes.clear();
        isTiming = false;
    }
    void SaveToFile(const string& filename, const BenchmarkResults& results) {
        ofstream file(filename, ios::app);
        if (file.is_open()) {
            file << fixed << setprecision(2);
            file << "=== " << results.implementationType << " Performance Results ===" << endl;
            file << "Generations tested: " << results.generationsTested << endl;
            file << "Average update time: " << results.avgUpdateTime << " microseconds" << endl;
            file << "Minimum update time: " << results.minUpdateTime << " microseconds" << endl;
            file << "Maximum update time: " << results.maxUpdateTime << " microseconds" << endl;
            file << "Average FPS equivalent: " << (1000000.0 / results.avgUpdateTime) << endl;
            file << "Timestamp: " << chrono::duration_cast<chrono::seconds>(
                chrono::system_clock::now().time_since_epoch()).count() << endl;
            file << "----------------------------------------" << endl;
            file.close();
        }
    }
};

int main() {
    SetTraceLogLevel(LOG_WARNING);
    const int SCREEN_WIDTH = 2560;
    const int SCREEN_HEIGHT = 1440;
    const int CELL_SIZE = 10;
    int fps = 165;
    int generation = 1;
    bool useGPU = false;
    
    PerformanceMonitor perfMonitor;
    bool benchmarkMode = false;
    int benchmarkGenerations = 0;
    const int BENCHMARK_TARGET = 1000;
    double lastUpdateTime = 0.0;
    double avgUpdateTime = 0.0;
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Conway's Game of Life");
    SetTargetFPS(fps);
    Simulation simulation{SCREEN_WIDTH, SCREEN_HEIGHT, CELL_SIZE};

    while (!WindowShouldClose()) {
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePosition = GetMousePosition();
            int row = mousePosition.y / CELL_SIZE;
            int column = mousePosition.x / CELL_SIZE;
            simulation.FlipCell(row, column);
        }
        
        if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if (simulation.IsRunning()) {
                simulation.Stop();
                SetWindowTitle("Game of Life has stopped.");
            }
            else {
                simulation.Start();
                SetWindowTitle("Game of Life is running ...");
            }
        }
        else if(IsKeyPressed(KEY_F)) {
            fps += 2;
            SetTargetFPS(fps);
        }
        else if(IsKeyPressed(KEY_S)) {
            if(fps > 5) {
                fps -= 2;
                SetTargetFPS(fps);
            }
        }
        else if(IsKeyPressed(KEY_R)) {
            generation = 1;
            simulation.CreateRandomState();
            perfMonitor.Reset();
        }
        else if(IsKeyPressed(KEY_C)) {
            generation = 1;
            simulation.ClearGrid();
            perfMonitor.Reset();
        }
        else if(IsKeyPressed(KEY_B)) {
            benchmarkMode = true;
            benchmarkGenerations = 0;
            perfMonitor.Reset();
            simulation.CreateRandomState();
            simulation.Start();
            SetWindowTitle(TextFormat("Benchmarking... %d/%d", benchmarkGenerations, BENCHMARK_TARGET));
            cout << "Starting benchmark mode - " << BENCHMARK_TARGET << " generations..." << endl;
        }
        else if(IsKeyPressed(KEY_G)) {
            useGPU = !useGPU;
            //cout << "Switched to " << (useGPU ? "GPU" : "CPU") << " mode" << endl;
        }
        
        if (simulation.IsRunning()) {//update game if running
            perfMonitor.StartTiming();
            if(useGPU) {simulation.UpdateGPU(generation);} 
            else {simulation.Update(generation);}
            perfMonitor.EndTiming();
            
            BenchmarkResults currentResults = perfMonitor.GetResults("Display");
            if (currentResults.generationsTested > 0) {//update averagetimes
                avgUpdateTime = currentResults.avgUpdateTime;
            }
            
            if (benchmarkMode) {//handling benchmark mode
                benchmarkGenerations++;
                SetWindowTitle(TextFormat("Benchmarking... %d/%d", benchmarkGenerations, BENCHMARK_TARGET));
                
                if (benchmarkGenerations >= BENCHMARK_TARGET) {
                    benchmarkMode = false;
                    BenchmarkResults results;

                    if (useGPU) {results = perfMonitor.GetResults("GPU Implementation");}
                    else {results = perfMonitor.GetResults("CPU Implementation");}
                    
                    perfMonitor.SaveToFile("benchmark_results.txt", results);
                    
                    cout << "\n=== Benchmark Complete ===" << endl;
                    cout << "Implementation: " << results.implementationType << endl;
                    cout << "Generations tested: " << results.generationsTested << endl;
                    cout << "Average update time: " << fixed << setprecision(2) << results.avgUpdateTime << " microseconds" << endl;
                    cout << "Min update time: " << results.minUpdateTime << " microseconds" << endl;
                    cout << "Max update time: " << results.maxUpdateTime << " microseconds" << endl;
                    cout << "Average equivalent FPS: " << (1000000.0 / results.avgUpdateTime) << endl;
                    cout << "Results saved to benchmark_results.txt" << endl;
                    SetWindowTitle("Benchmark Complete - Check console for results");
                }
            }
        }

        BeginDrawing(); //render frames
        ClearBackground(Color{73, 94, 53, 255});
        simulation.Draw();
        DrawText(TextFormat("%d FPS", fps), 2460, 10, 20, WHITE);
        DrawText(TextFormat("Generation: %d", generation), 10, 10, 20, WHITE);
        if (avgUpdateTime > 0) {
            DrawText(TextFormat("Update: %.1f microsecs", avgUpdateTime), 10, 35, 20, YELLOW);
            DrawText(TextFormat("Equiv FPS: %.1f", 1000000.0 / avgUpdateTime), 10, 60, 20, YELLOW);
        }
        if (benchmarkMode) {
            DrawRectangle(10, SCREEN_HEIGHT - 60, 300, 50, Color{0, 0, 0, 150});
            DrawText(TextFormat("Benchmarking: %d/%d", benchmarkGenerations, BENCHMARK_TARGET), 15, SCREEN_HEIGHT - 50, 20, GREEN);
            
            float progress = (float)benchmarkGenerations / BENCHMARK_TARGET;
            DrawRectangle(15, SCREEN_HEIGHT - 25, 290, 10, WHITE);
            DrawRectangle(15, SCREEN_HEIGHT - 25, (int)(290 * progress), 10, GREEN);
        }
        if (!benchmarkMode && generation < 5) {
            DrawText("Press B to start benchmark", 10, SCREEN_HEIGHT - 30, 20, WHITE);
        }
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}