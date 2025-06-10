#include "raylib.h"
#include "Renderer.h"

int main(void) 
{
    int screenWidth = 1024;
    int screenHeight = 1024;

    InitWindow(screenWidth, screenHeight, "raytracing in one weekend");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    
    Renderer renderer(8);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (IsWindowResized()) {
            renderer.OnResize();
        }
        BeginDrawing();
        ClearBackground(GetColor(0x0f0f0fff));
        renderer.Render();
        //DrawFPS(GetScreenWidth() - 100, 15);
        EndDrawing();
        if (IsKeyPressed(KEY_A)) {
            renderer.ExportRender("Render.png");
        }
    }
}