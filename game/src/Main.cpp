#include "raylib.h"
#include "Renderer.h"

int main(void) 
{
    int screenWidth = 512;
    int screenHeight = 512;

    //uint32_t pixelCount = screenWidth * screenHeight;
    uint32_t currentPixel_X = 0;
    uint32_t currentPixel_Y = 0;
    uint32_t pass = 1;

    InitWindow(screenWidth, screenHeight, "raytracing in one weekend");

    Renderer renderer;
    renderer.UpdateRenderPass(pass);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(GetColor(0x0f0f0fff));
        renderer.Render(currentPixel_X, currentPixel_Y);
        DrawFPS(GetScreenWidth() - 100, 15);
        EndDrawing();
        if (IsKeyPressed(KEY_A)) {
            renderer.ExportRender("Render.png");
        }
        currentPixel_X++;
        if (currentPixel_X >= screenHeight) {
            currentPixel_X = 0;
            currentPixel_Y++;
            if (currentPixel_Y >= screenWidth) {
                currentPixel_Y = 0;
                pass++;
                renderer.UpdateRenderPass(pass);
            }
        }        
    }
}