#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

static int width  = 1000;
static int height = 1000;
static int fontsize = 30;

typedef enum {
    START,
    SEASONING,
    COALS,
} Screen;

static char *dialog[] = {
    "Welcome Ladies and Gentlemen to the MERMAID game show!!! This is another line that I need to fill to test some things. This is a third line, I wonder what will happen?",
    "TWO",
    "THREE",
};

int dialog_counter = 0;

float magnitude(float x) {
    if (x < 0) return -1.0f;
    if (x > 0) return 1.0f;
    return 0.0f;
}

void DrawTextBox(char *dialog[], int *dialog_counter, Font font) {
    int box_width = width;
    int box_height = 200;
    const char *contin = "Continue";
    float contin_width = MeasureTextEx(font, contin, fontsize, 0).x;

    DrawRectangle(0, height - 200, box_width, 200, BLACK);
    GuiLabel((Rectangle) { .x = 0, .y = height - 280, .width = box_width, .height = box_height }, dialog[*dialog_counter]);

    DrawTextEx(font, contin, (Vector2){ width - contin_width, height - 60 + 10 }, fontsize, 0, RAYWHITE);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        (*dialog_counter)++;
    }
}

void DrawTextureTiled(Texture2D texture) {
    for (int i = 0; i < width; i += texture.width) {
        for (int j = 0; j < height; j += texture.height) {
            DrawTexture(texture, i, j, WHITE);
        }
    }
}

double sq(double x) {
    return x * x;
}

double coals_score_modifier(double t) {
    return 0.001 * (-sq(t - 3) + 1000);
}

int main(void) {
    InitWindow(width, height, "Test");
    SetTargetFPS(60);
    Font font = LoadFontEx("./resources/Courier Prime.ttf", 96, 0, 0);
    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
    GuiSetFont(font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontsize);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xf5f5f5ff);
    GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_WORD);
    GuiSetStyle(DEFAULT, TEXT_LINE_SPACING, fontsize);

    int speed = 10;
    int x = 0;
    int y = 0;

    Screen which = SEASONING;

    double score = 0;
    bool coals_timer_start = false;
    int coals_timer = 0;

    Image seasoning = LoadImage("./resources/seasoning.png");
    Texture2D seasoning_texture = LoadTextureFromImage(seasoning);
    UnloadImage(seasoning);

    Image fish = LoadImage("./resources/fish.png");
    ImageRotateCCW(&fish);
    Texture2D fish_texture = LoadTextureFromImage(fish);
    UnloadImage(fish);

    Image coals = LoadImage("./resources/coals.png");
    Texture2D coals_texture = LoadTextureFromImage(coals);
    UnloadImage(coals);
    
    while (!WindowShouldClose()) {
        switch (which) {
        case START: {
            
        } break;
        case SEASONING: {
            if (IsKeyDown(KEY_LEFT)) {
                if (x > 0) x -= speed;
                score += .1;
            }
            
            if (IsKeyDown(KEY_RIGHT)) {
                if (x < width) x += speed;
                score += .1;
            }
            
            if (IsKeyDown(KEY_UP)) {
                if (y > 0) y -= speed;
                score += .1;
            }
            
            if (IsKeyDown(KEY_DOWN)) {
                if (y < height) y += speed;
                score += .1;
            }
        } break;
        case COALS: {
            if (IsKeyDown(KEY_LEFT)) {
                if (x > 0) x -= speed;
            }
            
            if (IsKeyDown(KEY_RIGHT)) {
                if (x < width) x += speed;
            }
            
            if (IsKeyDown(KEY_UP)) {
                if (y > 0) y -= speed;
            }
            
            if (IsKeyDown(KEY_DOWN)) {
                if (y < height) y += speed;
            }

            if (x > width/2 - coals_texture.width/2 && x < width/2 + coals_texture.width/2) {
                if (!coals_timer_start) coals_timer_start = true;
                score += coals_score_modifier(coals_timer);
            } else {
                if (coals_timer_start) {
                    coals_timer_start = false;
                    coals_timer = 0;
                }
            }

            if (coals_timer_start) {
                coals_timer++;
            }
        } break;
        }

        BeginDrawing();
        switch (which) {
        case START: {
            ClearBackground(RAYWHITE);
        } break;
        case SEASONING: {
            DrawTextureTiled(seasoning_texture);
            DrawTextEx(font, "YOU", (Vector2){x, y-20}, fontsize, 0, RAYWHITE);
            DrawTexture(fish_texture, x, y, WHITE);
            DrawTextEx(font, TextFormat("Score: %f", score), (Vector2){ 0, 0 }, fontsize, 0, RAYWHITE);
        } break;
        case COALS: {
            ClearBackground(RAYWHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, 0, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, coals_texture.height, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, coals_texture.height*2, WHITE);
            DrawTextEx(font, "YOU", (Vector2){x, y-20}, fontsize, 0, GREEN);
            DrawTexture(fish_texture, x, y, WHITE);
            DrawTextEx(font, TextFormat("Score: %f", score), (Vector2){ 0, 0 }, fontsize, 0, BLACK);
        } break;
        }

        DrawTextBox(dialog, &dialog_counter, font);
        EndDrawing();

        

    }

    CloseWindow();
}
