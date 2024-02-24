#include "raylib.h"

/* #define RAYGUI_IMPLEMENTATION */
/* #include "raygui.h" */

static int width  = 1000;
static int height = 1000;
static int fontsize = 50;
static int timer = 0;
static bool animate = true;
static Font font = LoadFontEx("/home/nathan/.fonts/c/CourierPrime_Regular.ttf", 96, 0, 0);

static char *dialog[] = {
    "ONE",
    "TWO",
    "THREE",
};

int dialog_counter = 0;

float magnitude(float x) {
    if (x < 0) return -1.0f;
    if (x > 0) return 1.0f;
    return 0.0f;
}

bool DrawTextBox(char const *text, Font font) {
    DrawRectangle(20, height - 200, width - 40, 180, BLACK);
    DrawTextEx(font, text, (Vector2){ 20, height - 200 }, 30, 0, RAYWHITE);
    const char *contin = "Continue";
    float contin_width = MeasureTextEx(font, contin, 30, 0).x;
    
    if (animate) {
        DrawRectangle(20 + timer*3, height - 200, width - 40 - timer*3, 30, BLACK);
    } else {
        DrawTextEx(font, contin, (Vector2){ width - contin_width - 20, height - 60 + 10 }, 30, 0, RAYWHITE);
    }
    
    if (timer * 3 >= MeasureTextEx(font, text, 30, 0).x) {
        timer = 0;
        animate = false;
    }
    
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

int main(void) {
    InitWindow(width, height, "Test");
    SetTargetFPS(60);

    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (DrawTextBox(dialog[dialog_counter], font)) {
            if (dialog_counter < 3) {
                dialog_counter++;
                timer = 0;
                animate = true;
            }

        }
        EndDrawing();

        timer+=1;
    }

    CloseWindow();
}
