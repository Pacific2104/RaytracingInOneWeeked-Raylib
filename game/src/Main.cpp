#include "raylib.h"
#include "Renderer.h"

int main(void) 
{
    int screenWidth = 720;
    int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "raytracing in one weekend");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    
    Renderer renderer;

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (IsWindowResized()) {
            renderer.OnResize();
        }
        BeginDrawing();
        ClearBackground(GetColor(0x0f0f0fff));
        renderer.Render();

        DrawFPS(GetScreenWidth() - 100, 15);
        EndDrawing();
    }
}