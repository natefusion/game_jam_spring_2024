#include "raylib.h"

/* #define RAYGUI_IMPLEMENTATION */
/* #include "raygui.h" */

static int width  = 1000;
static int height = 1000;
static int fontsize = 50;
static int timer = 0;
static bool animate = true;

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

void bouncy(void) {
    int     text_alpha            = 0xff;
    bool    text_alpha_going_down = false;
    Vector2 mouse_offset          = { 0, 0 }; // so that the mouse doesn't lock itself to the top left corner of the text when dragged
    bool    dragging_text         = false;
    Vector2 text_velocity         = { 1, 1 };
    
    Font font = LoadFontEx("/home/nathan/.fonts/c/CourierPrime_Regular.ttf", 96, 0, 0);
    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    
    char   *text          = "DRAG ME";
    Vector2 text_size     = MeasureTextEx(font, text, fontsize, 0);
    Vector2 text_position = { GetRandomValue(0, width-text_size.x), GetRandomValue(0, height-text_size.y) };
    
    while (!WindowShouldClose()) {
        int mx = GetMouseX();
        int my = GetMouseY();
        
        float tpx_left  = text_position.x;
        float tpx_right = text_position.x + text_size.x;
        float tpy_top   = text_position.y;
        float tpy_bot   = text_position.y + text_size.y;
        
        bool tpx_lo = tpx_left <= 0;
        bool tpx_hi = tpx_right >= width;
        bool tpy_lo = tpy_top <= 0;
        bool tpy_hi = tpy_bot >= height;

        bool mouse_in_text = mx >= tpx_left && mx <= tpx_right &&
                             my >= tpy_top  && my <= tpy_bot;
        
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && (dragging_text || mouse_in_text)) {
            if (!dragging_text) {
                dragging_text = true;
                text_alpha = 0xff;
                mouse_offset = (Vector2){ .x = mx - tpx_left, .y = my - tpy_top };
            } else {
                if (tpx_lo) {
                    text_position.x = 0;
                } else if (tpx_hi) {
                    text_position.x = width - text_size.x;
                } else {
                    text_position.x = mx - mouse_offset.x;                    
                }

                if (tpy_lo) {
                    text_position.y = 0;
                } else if (tpy_hi) {
                    text_position.y = height - text_size.y;
                } else {
                    text_position.y = my - mouse_offset.y;
                }

                // nudge the text in the direction of your mouse when the mouse button is released
                Vector2 delta = GetMouseDelta();
                text_velocity.x = magnitude(delta.x);
                text_velocity.y = magnitude(delta.y);
            }
        } else {
            if (dragging_text)
                dragging_text = 0;
            
            if (tpx_lo || tpx_hi)
                text_velocity.x *= -1;

            if (tpy_lo || tpy_hi)
                text_velocity.y *= -1;

            text_position.x += text_velocity.x;
            text_position.y += text_velocity.y;

            if (!text_alpha_going_down && text_alpha <= 0xff)
                text_alpha += 4;
            else if (text_alpha_going_down && text_alpha >= 0)
                text_alpha -= 4;

            if (text_alpha <= 0) {
                text_alpha_going_down = false;
                text_alpha = 0;
            }

            if (text_alpha >= 0xff) {
                text_alpha_going_down = true;
                text_alpha = 0xff;
            }
        }

        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        DrawTextEx(font, text, text_position, fontsize, 0, (Color){ .r=0, .g=0, .b=0, .a=text_alpha });
        
        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();    
}

bool DrawTextBox(char const *text, Font font) {
    DrawRectangle(20, height - 200, width - 40, 180, BLACK);
    DrawTextEx(font, text, (Vector2){ 20, height - 200 }, 30, 0, RAYWHITE);
    
    if (animate) {
        DrawRectangle(20 + timer*3, height - 200, width - 40 - timer*3, 30, BLACK);
    } else {
        DrawTextEx(font, "Continue", (Vector2){ 20, height - 200 + 30 }, 30, 0, RAYWHITE);
    }
    
    if (timer >= MeasureTextEx(font, text, 30, 0).x) {
        timer = 0;
        animate = false;
    }
    
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

int main(void) {
    InitWindow(width, height, "Test");
    SetTargetFPS(60);

    Font font = LoadFontEx("./resources/Courier Prime.ttf", 96, 0, 0);
    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (DrawTextBox(dialog[dialog_counter], font)) {
            if (dialog_counter < 4) {
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
