#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define str(x) #x

static int width  = 1000;
static int height = 1000;
static int fontsize = 30;

static char player_name[10] = "YOUFISH";

typedef enum {
    START,
    SEASONING,
    COALS,
} Screen;

typedef enum {
    MERMAID = 0,
    ONEFISH = 1,
    REDFISH = 2,
    BLUEFISH = 3,
    PLAYERFISH = 4
} Character;

char * characters_tostring(Character x) {
    switch (x) {
    case MERMAID: return "MERMAID";
    case ONEFISH: return "ONEFISH";
    case REDFISH: return "REDFISH";
    case BLUEFISH: return "BLUEFISH";
    case PLAYERFISH: return player_name;
    default: return "";
    }
}

typedef struct {
    Character speaker;
    char *text;
} Character_Dialog;

static Character_Dialog dialog[] = {
    {ONEFISH, "Ladies and gentlefish ..."},
    {ONEFISH, "Welcome to HOOKED ON YOU"},
    {ONEFISH, "I'm your host, ONEFISH, and this is the only game show where YOU can have the chance to be hooked by our beautiful ..."},
    {ONEFISH, "MAGGIE MERMAID!!!"},
};

static int dialog_counter = 0;

float magnitude(float x) {
    if (x < 0) return -1.0f;
    if (x > 0) return 1.0f;
    return 0.0f;
}

void DrawPlayerName(Font font, char *text) {
    DrawRectangle(0, height - 250 - fontsize, 5 + MeasureTextEx(font, text, fontsize, 0).x, fontsize, BLACK);
    DrawTextEx(font, text, (Vector2){ 0, height - 250 - fontsize}, fontsize, 0, WHITE);
}

void DrawHeadShot(Texture2D texture) {
    int w = 100;
    int h = 100;

    DrawRectangle(width - w - 20, height - h - 250 - 20, w + 20, h + 20 - 10, WHITE);
    DrawRectangle(width - w - 10, height - h - 250 - 10, w, h, BLACK);
    DrawTexture(texture, width - w, height - h - 250, WHITE);
    DrawRectangle(width - w - 20, height - h - 160, w + 20, 10, WHITE);
}

bool DrawTextBox(Character_Dialog dialog[], int dialog_counter, Font font) {
    int box_width = width;
    int box_height = 250;
    const char *contin = "Continue";
    float contin_width = MeasureTextEx(font, contin, fontsize, 0).x;

    DrawRectangle(0, height - box_height, box_width, box_height, BLACK);
    GuiLabel((Rectangle) { .x = 0, .y = height - box_height -100, .width = box_width, .height = box_height}, dialog[dialog_counter].text);

    DrawTextEx(font, contin, (Vector2){ width - contin_width, height - 60 + 10 }, fontsize, 0, RAYWHITE);

    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
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

    Screen which = START;

    double score = 0;
    bool coals_timer_start = false;
    int coals_timer = 0;

    Image seasoning = LoadImage("./resources/seasoning.png");
    Texture2D seasoning_texture = LoadTextureFromImage(seasoning);
    UnloadImage(seasoning);

    Image onefish = LoadImage("./resources/onefish.png");
    Image playerfish = LoadImage("./resources/playerfish.png");
    Image redfish = LoadImage("./resources/redfish.png");
    Image bluefish = LoadImage("./resources/bluefish.png");
    Image mermaid = LoadImage("./resources/mermaid.png");

    Texture2D character_pngs[] = {
        [MERMAID] = LoadTextureFromImage(mermaid),
        [ONEFISH] = LoadTextureFromImage(onefish),
        [REDFISH] = LoadTextureFromImage(redfish),
        [BLUEFISH] = LoadTextureFromImage(bluefish),
        [PLAYERFISH] = LoadTextureFromImage(playerfish),
    };
    
    UnloadImage(mermaid);
    UnloadImage(onefish);
    UnloadImage(redfish);
    UnloadImage(bluefish);
    UnloadImage(playerfish);
    
    Image coals = LoadImage("./resources/coals.png");
    Texture2D coals_texture = LoadTextureFromImage(coals);
    UnloadImage(coals);

    Image seafloor = LoadImage("./resources/background1.png");
    Texture2D seafloor_texture = LoadTextureFromImage(seafloor);
    UnloadImage(seafloor);
    
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
            DrawTexture(seafloor_texture, 0, 0, WHITE);
        } break;
        case SEASONING: {
            DrawTextureTiled(seasoning_texture);
            DrawTextEx(font, "YOU", (Vector2){x, y-20}, fontsize, 0, RAYWHITE);
            DrawTexture(character_pngs[PLAYERFISH], x, y, WHITE);
            DrawTextEx(font, TextFormat("Score: %f", score), (Vector2){ 0, 0 }, fontsize, 0, RAYWHITE);
        } break;
        case COALS: {
            ClearBackground(RAYWHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, 0, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, coals_texture.height, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, coals_texture.height*2, WHITE);
            DrawTextEx(font, "YOU", (Vector2){x, y-20}, fontsize, 0, GREEN);
            DrawTexture(character_pngs[PLAYERFISH], x, y, WHITE);
            DrawTextEx(font, TextFormat("Score: %f", score), (Vector2){ 0, 0 }, fontsize, 0, BLACK);
        } break;
        }
        
        DrawHeadShot(character_pngs[dialog[dialog_counter].speaker]);
        DrawPlayerName(font, characters_tostring(dialog[dialog_counter].speaker));
        if (DrawTextBox(dialog, dialog_counter, font))
            dialog_counter++;
        EndDrawing();

        

    }

    CloseWindow();
}
