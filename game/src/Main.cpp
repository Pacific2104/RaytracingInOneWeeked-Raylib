#include "Renderer.h"
#include "CustomCamera.h"
#include <iostream>
#include <cstdio>

#define MT 1

int main(void) 
{
    uint32_t screenWidth = 1280;
    uint32_t screenHeight = 720;
    uint32_t pixelsPerFrame = 100;
    uint32_t samples = 100;
    uint32_t depth = 25;

    uint32_t currentPixel_X = 0;
    uint32_t currentPixel_Y = 0;
    uint32_t pass = 1;

    double renderTime = 0.0f;
    char buffer[256];  // Make sure it's big enough

    InitWindow(screenWidth, screenHeight, "raytracing in one weekend");

    CustomCamera camera;

    Renderer renderer(samples, depth, camera);
    renderer.UpdateRenderPass(pass);

    snprintf(buffer, sizeof(buffer), "Sample: %d\nDepth: %d\nPass: %d", samples, depth, pass);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(GetColor(0x0f0f0fff));
#if MT
        renderer.RenderMT();
        renderTime = GetFrameTime();
        snprintf(buffer, sizeof(buffer), "Sample: %d\nDepth: %d\nPass: %d\nrendered in: %.2f seconds", samples, depth, pass, renderTime);
        DrawText(buffer, 10, 10, 10, RED);
        renderer.UpdateRenderPass(++pass);
#else
        for (uint32_t i = 0; i < pixelsPerFrame; ++i) {
            renderer.Render(currentPixel_X, currentPixel_Y);
            if (++currentPixel_X >= screenWidth) {
                currentPixel_X = 0;
                if (++currentPixel_Y >= screenHeight) {
                    snprintf(buffer, sizeof(buffer), "Sample: %d\nDepth: %d\nPass: %d\nrendered in: %.2f seconds",samples, depth, pass, renderTime);
                    renderTime = 0.0f;
                    currentPixel_Y = 0;
                    renderer.UpdateRenderPass(++pass);
                }
            }
        }
        renderTime += GetFrameTime();
        DrawText(buffer, 10, 10, 10, RED);
        DrawFPS(GetScreenWidth() - 100, 15);
#endif
        EndDrawing();
        if (IsKeyPressed(KEY_A)) {
            renderer.ExportRender("Render.png");
        }
    }
}