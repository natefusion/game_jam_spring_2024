#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

static int width  = 1000;
static int height = 1000;
static int fontsize = 30;
static double last_time = 0;

typedef enum {
    TUG_OF_WAR,
    SEASONING,
    COALS,
    START,
} Screen;

static Screen screen = START;
static bool timer_running = false;

typedef enum {
    MERMAID = 0,
    ONEFISH = 1,
    REDFISH = 2,
    BLUFISH = 3,
    YOUFISH = 4
} Character;

typedef struct {
    int x;
    int y;
    float tug_of_war_score;
    float seasoning_score;
    float coals_score;
    Character name;
} Fish;

Fish make_fish(Character name) {
    return (Fish) {
        .x = 0,
        .y = 0,
        .tug_of_war_score = 0,
        .seasoning_score = 0,
        .coals_score = 0,
        .name = name
    };
}

float total_score(Fish fish) {
    return fish.tug_of_war_score + fish.seasoning_score + fish.coals_score;
}

char * characters_tostring(Character x) {
    switch (x) {
    case MERMAID: return "MERMAID";
    case ONEFISH: return "ONEFISH";
    case REDFISH: return "REDFISH";
    case BLUFISH: return "BLUFISH";
    case YOUFISH: return "YOUFISH";
    default: return "";
    }
}

void start_minigame_timer(void) {
    last_time = GetTime();
    timer_running = true;
}

bool timer_has_ended(void) {
    if (!timer_running) {
        return true;
    }
    
    bool has_ended = GetTime() - last_time >= 5;
    if (has_ended) {
        timer_running = false;
    }
    
    return has_ended;
}

static bool draw_redfish = false;
static bool draw_blufish = false;
static bool draw_youfish = false;
static bool draw_scores = false;

void enter_redfish(void) { draw_redfish = true; }
void enter_blufish(void) { draw_blufish = true; }
void enter_youfish(void) { draw_youfish = true; }
void enter_scores(void) { draw_scores = true; }

void start_tug_of_war_minigame(void) { screen = TUG_OF_WAR; start_minigame_timer(); }
void start_seasoning_minigame(void) { screen = SEASONING; start_minigame_timer(); }
void start_coals_minigame(void) { screen = COALS; start_minigame_timer(); }

typedef struct {
    Character speaker;
    char *text;
    void (*trigger_action)(void);
} Character_Dialog;

#define DIALOG_LINES 31

static Character_Dialog dialog[DIALOG_LINES] = {
    {ONEFISH, "Ladies and gentlefish ...", NULL},
    {ONEFISH, "Welcome to HOOKED ON YOU", NULL},
    {ONEFISH, "I'm your host, ONEFISH, and this is the only game show where YOU can have the chance to be hooked by our beautiful ...", NULL},
    {ONEFISH, "MAGGIE MERMAID!!!", NULL},
    {MERMAID, "Thank you so much ONEFISH. I'm flattered to have the opportunity to catch the fish of my dreams.", NULL},
    {ONEFISH, "MAGGIE MERMAID, you won't be disappointed. We have three wonderful contestants here today.", NULL},
    {ONEFISH, "Now, without furthur ado, let's welcome our first contestant:", NULL},
    {ONEFISH, "REDFISH!!!", enter_redfish},
    {REDFISH, "blub blub", NULL},
    {MERMAID, "Oh my, your red color makes my mouth water!", NULL},
    {ONEFISH, "Now, let's welcome our second contestant:", NULL},
    {ONEFISH, "BLUFISH!!!", enter_blufish},
    {BLUFISH, "blub, blub MAGGIE MERMAID blub blub ", NULL},
    {MERMAID, "Oh my, you sure know how to flatter, BLUFISH!", NULL},
    {ONEFISH, "And finally, our third contestant:", NULL},
    {ONEFISH, "Erm, sorry, what was your name again?", NULL},
    {ONEFISH, "Oh, yes of course, how could I forget. Welcome:", NULL},
    {ONEFISH, "YOUFISH!!!", enter_youfish},
    {YOUFISH, "...", NULL},
    {MERMAID, "There's no need to be nervous YOUFISH. That orange color of your just might get me hooked!", NULL},
    {ONEFISH, "Now, to become hooked by this wonderful MAGGIE MERMAID here, you all must endure three trials!", NULL},
    {ONEFISH, "The fish to collect the most points wins!", enter_scores},
    {ONEFISH, "The first trial is a test of STRENGTH. Show MAGGIE MERMAID your muscles in a game of TUG OF WAR", start_tug_of_war_minigame},
    {ONEFISH, "The second trial is a test of TASTE. Show MAGGIE MERMAID what enhances your best qualities!. Collect the most SEASONING to win!", start_seasoning_minigame},
    {ONEFISH, "Now, for the third and final test show MAGGIE MERMAID that you can handle the HEAT! Stay on the HOT COALS for as long as you can to win!", start_coals_minigame},
    {ONEFISH, "Wow, what a wonderful competition that was, and my, was it a close game!", NULL},
    {ONEFISH, "Without furthur ado, the winner is ...", NULL},
    {REDFISH, "Wow, I'm so excited. Being hooked has always been my dream!", NULL},
    {BLUFISH, "This is unbelievable. I can't wait to finally be hooked!", NULL},
    {YOUFISH, "...", NULL},
    {ONEFISH, "MAGGIE MERMAID, are you satisfied with your catch?", NULL},
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

void DrawScores(Font font, float redfish_score, float blufish_score, float youfish_score) {
    DrawTextEx(font, TextFormat("REDFISH SCORE: %.0f", redfish_score), (Vector2){0, 0}, fontsize, 0, WHITE);
    DrawTextEx(font, TextFormat("BLUFISH SCORE: %.0f", blufish_score), (Vector2){0, fontsize}, fontsize, 0, WHITE);
    DrawTextEx(font, TextFormat("YOUFISH SCORE: %.0f", youfish_score), (Vector2){0, fontsize*2}, fontsize, 0, WHITE);
}

void DrawHeadShot(Texture2D texture) {
    int w = 100;
    int h = 100;

    DrawRectangle(width - w - 20, height - h - 250 - 20, w + 20, h + 20 - 10, WHITE);
    DrawRectangle(width - w - 10, height - h - 250 - 10, w, h, BLACK);
    DrawTexture(texture, width - w, height - h - 250, WHITE);
    DrawRectangle(width - w - 20, height - h - 160, w + 20, 10, WHITE);
}

bool DrawTextBox(char *text, Font font) {
    int box_width = width;
    int box_height = 250;
    const char *contin = "Continue";
    float contin_width = MeasureTextEx(font, contin, fontsize, 0).x;

    DrawRectangle(0, height - box_height, box_width, box_height, BLACK);
    GuiLabel((Rectangle) { .x = 0, .y = height - box_height -100, .width = box_width, .height = box_height}, text);

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

    bool coals_timer_start = false;
    int coals_timer = 0;

    Fish redfish = make_fish(REDFISH);
    Fish blufish = make_fish(BLUFISH);
    Fish youfish = make_fish(YOUFISH);

    Image seasoning = LoadImage("./resources/seasoning.png");
    Texture2D seasoning_texture = LoadTextureFromImage(seasoning);
    UnloadImage(seasoning);

    Image onefish_pic = LoadImage("./resources/onefish.png");
    Image youfish_pic = LoadImage("./resources/youfish.png");
    Image redfish_pic = LoadImage("./resources/redfish.png");
    Image blufish_pic = LoadImage("./resources/blufish.png");
    Image mermaid_pic = LoadImage("./resources/mermaid.png");

    Texture2D character_pngs[] = {
        [MERMAID] = LoadTextureFromImage(mermaid_pic),
        [ONEFISH] = LoadTextureFromImage(onefish_pic),
        [REDFISH] = LoadTextureFromImage(redfish_pic),
        [BLUFISH] = LoadTextureFromImage(blufish_pic),
        [YOUFISH] = LoadTextureFromImage(youfish_pic),
    };
    
    UnloadImage(mermaid_pic);
    UnloadImage(onefish_pic);
    UnloadImage(redfish_pic);
    UnloadImage(blufish_pic);
    UnloadImage(youfish_pic);
    
    Image coals = LoadImage("./resources/coals.png");
    Texture2D coals_texture = LoadTextureFromImage(coals);
    UnloadImage(coals);

    Image showroom = LoadImage("./resources/background0.png");
    Texture2D showroom_texture = LoadTextureFromImage(showroom);
    UnloadImage(showroom);

    Image seafloor = LoadImage("./resources/seafloor.png");
    Texture2D seafloor_texture = LoadTextureFromImage(seafloor);
    UnloadImage(seafloor);
    
    while (!WindowShouldClose()) {
        switch (screen) {
        case START: {
            // nothing here :)
        } break;
        case TUG_OF_WAR: {
            if (timer_has_ended()) {
                screen = START;
            }
        } break;
        case SEASONING: {
            if (timer_has_ended()) {
                screen = START;
            }
            if (IsKeyDown(KEY_LEFT)) {
                if (x > 0) x -= speed;
                youfish.seasoning_score += .1;
            }
            
            if (IsKeyDown(KEY_RIGHT)) {
                if (x < width) x += speed;
                youfish.seasoning_score += .1;
            }
            
            if (IsKeyDown(KEY_UP)) {
                if (y > 0) y -= speed;
                youfish.seasoning_score += .1;
            }
            
            if (IsKeyDown(KEY_DOWN)) {
                if (y < height) y += speed;
                youfish.seasoning_score += .1;
            }
        } break;
        case COALS: {
            if (timer_has_ended()) {
                screen = START;
            }
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
                youfish.coals_score += coals_score_modifier(coals_timer);
                if (youfish.coals_score < 0)
                    youfish.coals_score = 0;
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

        switch (screen) {
        case START: {
            DrawTexture(showroom_texture, 0, 0, WHITE);

            if (draw_redfish) { DrawTexture(character_pngs[REDFISH], 549, 403, WHITE); }
            if (draw_blufish) { DrawTexture(character_pngs[BLUFISH], 689, 403, WHITE); }
            if (draw_youfish) { DrawTexture(character_pngs[YOUFISH], 847, 403, WHITE); }
            if (draw_scores)  { DrawScores(font, total_score(redfish), total_score(blufish), total_score(youfish)); }

            DrawHeadShot(character_pngs[dialog[dialog_counter].speaker]);
            DrawPlayerName(font, characters_tostring(dialog[dialog_counter].speaker));
            if (DrawTextBox(dialog[dialog_counter].text, font)) {
                if (dialog_counter < DIALOG_LINES - 1) {
                    if (dialog[dialog_counter].trigger_action) {
                        dialog[dialog_counter].trigger_action();
                    }
                    dialog_counter++;
                }
            }
        } break;
        case TUG_OF_WAR: {
            DrawTexture(seafloor_texture, 0, 0, WHITE);
            if (draw_scores) { DrawScores(font, total_score(redfish), total_score(blufish), total_score(youfish)); }
        } break;
        case SEASONING: {
            DrawTextureTiled(seasoning_texture);
            DrawTextEx(font, "YOU", (Vector2){x, y-20}, fontsize, 0, RAYWHITE);
            DrawTexture(character_pngs[YOUFISH], x, y, WHITE);
            if (draw_scores) { DrawScores(font, total_score(redfish), total_score(blufish), total_score(youfish)); }
        } break;
        case COALS: {
            DrawTexture(seafloor_texture, 0, 0, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, 0, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, coals_texture.height, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, coals_texture.height*2, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, coals_texture.height*3, WHITE);
            DrawTextEx(font, "YOU", (Vector2){x, y-20}, fontsize, 0, GREEN);
            DrawTexture(character_pngs[YOUFISH], x, y, WHITE);
            if (draw_scores) { DrawScores(font, total_score(redfish), total_score(blufish), total_score(youfish)); }
        } break;
        }

        EndDrawing();
    }

    CloseWindow();
}
