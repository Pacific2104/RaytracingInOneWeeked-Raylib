#include "Renderer.h"
#include "CustomCamera.h"
#include <iostream>

#define MT 0

int main(void) 
{
    uint32_t screenWidth = 1280;
    uint32_t screenHeight = 720;
    uint32_t pixelsPerFrame = 20;
    uint32_t samples = 150;
    uint32_t depth = 25;

    uint32_t currentPixel_X = 0;
    uint32_t currentPixel_Y = 0;
    uint32_t pass = 1;

    double renderTime = 0.0f;

    InitWindow(screenWidth, screenHeight, "raytracing in one weekend");

    CustomCamera camera;

    Renderer renderer(samples, depth, camera);
    renderer.UpdateRenderPass(pass);

    std::cout <<
        "\nSTATS" <<
        "\nResolution: " << screenWidth << " X " << screenHeight <<
        "\nSamples: " << samples <<
        "\ndepth: " << depth <<
        "\nPixel Per Frame: " << pixelsPerFrame << std::endl;

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(GetColor(0x0f0f0fff));
#if MT
        renderer.RenderMT();
#else
        for (uint32_t i = 0; i < pixelsPerFrame; ++i) {
            renderer.Render(currentPixel_X, currentPixel_Y);
            if (++currentPixel_X >= screenWidth) {
                currentPixel_X = 0;
                if (++currentPixel_Y >= screenHeight) {
                    std::cout << 
                        "\nPass: " << pass <<
                        "\nrendered in: " << renderTime << " seconds" << std::endl;
                    renderTime = 0.0f;

                    currentPixel_Y = 0;
                    renderer.UpdateRenderPass(++pass);
                }
            }
        }
        renderTime += GetFrameTime();
        DrawFPS(GetScreenWidth() - 100, 15);
#endif
        EndDrawing();
        if (IsKeyPressed(KEY_A)) {
            renderer.ExportRender("Render.png");
        }
    }
}