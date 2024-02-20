#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main(void) {
    InitWindow(400, 400, "Test");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
}
