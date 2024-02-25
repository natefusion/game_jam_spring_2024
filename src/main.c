#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

static int width  = 1000;
static int height = 1000;
static int fontsize = 30;
static double last_time = 0;
static int speed = 10;

typedef enum {
    SEASONING,
    COALS,
    TITLE,
    START,
    CREDITS,
    REDFISH_WINS,
    YOUFISH_WINS,
    BLUFISH_WINS
} Screen;

static Screen screen = TITLE;
static bool timer_running = false;

typedef enum {
    MERMAID = 0,
    ONEFISH = 1,
    REDFISH = 2,
    BLUFISH = 3,
    YOUFISH = 4,
    SEASON = 5,
    GRILL = 6,
} Character;

typedef struct {
    int x;
    int y;
    float tug_of_war_score;
    float seasoning_score;
    float coals_score;
    int coals_timer;
    bool coals_timer_start;
    Character name;
} Fish;

Fish make_fish(Character name) {
    return (Fish) {
        .x = GetRandomValue(speed, width - 1 - speed),
        .y = GetRandomValue(speed, height - 1 - speed),
        .tug_of_war_score = 0,
        .seasoning_score = 0,
        .coals_score = 0,
        .coals_timer = 0,
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
    
    bool has_ended = GetTime() - last_time >= 20;
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

static bool play_seasoning = false;
void start_seasoning_minigame(void) { screen = SEASONING; play_seasoning = true; start_minigame_timer(); }

static bool play_firedance = false;
void start_coals_minigame(void) { screen = COALS; play_firedance = true; start_minigame_timer(); }

static bool draw_hook = false;
void launch_hook(void) {
    draw_hook = true;
}

static bool draw_fish_and_hook = false;
void take_fish(void) {
    draw_fish_and_hook = true;
    draw_hook = false;
}

static int bite = 0;
static bool play_chomp = false;
void take_bite(void) {
    bite++;
    play_chomp = true;
}

static bool decide_winner = false;
void goto_winner(void) {
    decide_winner = true;
}

void goto_credits(void) {
    screen = CREDITS;
}

typedef struct {
    Character speaker;
    char *text;
    void (*trigger_action)();
} Character_Dialog;

#define DIALOG_LINES 36

static Character_Dialog dialog[DIALOG_LINES] = {
    {ONEFISH, "Ladies and gentlefish ...", NULL},
    {ONEFISH, "Welcome to HOOKED ON YOU", NULL},
    {ONEFISH, "I'm your host, ONEFISH, and this is the only game show where YOU can have the chance to be hooked by our beautiful ...", NULL},
    {ONEFISH, "MERMAID!!!", NULL},
    {MERMAID, "Thank you so much ONEFISH. I'm flattered to have the opportunity to catch the fish of my dreams.", NULL},
    {ONEFISH, "MERMAID, you won't be disappointed. We have three wonderful contestants here today.", NULL},
    {ONEFISH, "Now, without furthur ado, let's welcome our first contestant:", NULL},
    {ONEFISH, "REDFISH!!!", enter_redfish},
    {REDFISH, "blub blub", NULL},
    {MERMAID, "Oh my, your red color makes my mouth water!", NULL},
    {ONEFISH, "Now, let's welcome our second contestant:", NULL},
    {ONEFISH, "BLUFISH!!!", enter_blufish},
    {BLUFISH, "blub, blub MERMAID blub blub ", NULL},
    {MERMAID, "Oh my, you sure know how to flatter, BLUFISH!", NULL},
    {ONEFISH, "And finally, our third contestant:", NULL},
    {ONEFISH, "Erm, sorry, what was your name again?", NULL},
    {ONEFISH, "Oh, yes of course, how could I forget. Welcome:", NULL},
    {ONEFISH, "YOUFISH!!!", enter_youfish},
    {YOUFISH, "...", NULL},
    {MERMAID, "There's no need to be nervous YOUFISH. That orange color of your just might get me hooked!", NULL},
    {ONEFISH, "Now, to become hooked by this wonderful  MERMAID here, you all must endure THREE TRIALS!", NULL},
    {ONEFISH, "No, wait, excuse me. TWO TRIALS!"},
    {ONEFISH, "Due to health and safety reasons from a local FISH rights group, we will not be playing the first trial"},
    {ONEFISH, "The fish to collect the most points wins!", enter_scores},
    {ONEFISH, "The second, ehem, first trial is a test of TASTE. Show MERMAID what enhances your best qualities!. Collect the most SEASONING to win!", start_seasoning_minigame},
    {ONEFISH, "Now, for the second and final trial to show  MERMAID that you can handle the HEAT! Stay on the HOT COALS for as long as you can to win!", start_coals_minigame},
    {ONEFISH, "Wow, what a wonderful competition that was, and my, was it a close game!", NULL},
    {ONEFISH, "Without furthur ado, the winner is ...", goto_winner},
    {ONEFISH, "MERMAID, are you satisfied with your catch?", NULL},
    {MERMAID, "I am absolutely HOOKED!!", launch_hook},
    {MERMAID, "Get over here!", take_fish},
    {MERMAID, "It's dinner time and I'm hungry!", take_bite},
    {MERMAID, "OM", take_bite},
    {MERMAID, "NOM", take_bite},
    {MERMAID, "NOM", NULL},
    {MERMAID, "OOH, that hit the spot!", goto_credits},
};
static int dialog_counter = 0;

#define YOUFISH_DIALOG_LINES 2
static Character_Dialog youfish_route[] = {
    {ONEFISH, "YOUFISH!!!", NULL},
    {YOUFISH, "...", NULL},
};
static int youfish_dialog_counter = 0;

#define REDFISH_DIALOG_LINES 2
static Character_Dialog redfish_route[] = {
    {ONEFISH, "REDFISH!!!", NULL},
    {REDFISH, "blub, blu blu blubbbb. HOOKED bloop blu blub!", NULL},
};
static int redfish_dialog_counter = 0;

#define BLUFISH_DIALOG_LINES 2
static Character_Dialog blufish_route[] = {
    {ONEFISH, "BLUFISH!!!", NULL},
    {REDFISH, "Blub!, blu blo blubb. blubb HOOKED blu blub blubb blub!", NULL},
};
static int blufish_dialog_counter = 0;

float max(float x, float y) {
    if (x > y) return x;
    return y;
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
    const char *contin = "Left click to continue";
    float contin_width = MeasureTextEx(font, contin, fontsize, 0).x;

    DrawRectangle(0, height - box_height, box_width, box_height, BLACK);
    GuiLabel((Rectangle) { .x = 0, .y = height - box_height -100, .width = box_width, .height = box_height}, text);

    DrawTextEx(font, contin, (Vector2){ width - contin_width, height - 60 + 10 }, fontsize, 0, RAYWHITE);

    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void DrawTextureTiled(Texture2D foreground, Texture2D background, bool exclude_me[20][20]) {
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            if (!exclude_me[i][j]) {
                DrawTexture(foreground, i*foreground.width, j*foreground.height, WHITE);
            } else {
                DrawTextureRec(background, (Rectangle){i*foreground.width, j*foreground.height, foreground.width, foreground.height}, (Vector2) {i*foreground.width, j*foreground.height}, WHITE) ;
            }
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
    InitAudioDevice();
    SetTargetFPS(60);
    Font font = LoadFontEx("./resources/Courier Prime.ttf", 96, 0, 0);
    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
    GuiSetFont(font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontsize);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xf5f5f5ff);
    GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_WORD);
    GuiSetStyle(DEFAULT, TEXT_LINE_SPACING, fontsize);

    Fish redfish = make_fish(REDFISH);
    Fish blufish = make_fish(BLUFISH);
    Fish youfish = make_fish(YOUFISH);

    Character winning_fish = REDFISH;

    Image seasoning = LoadImage("./resources/seasoning.png");
    Texture2D seasoning_texture = LoadTextureFromImage(seasoning);
    UnloadImage(seasoning);
    bool seasoning_collected[20][20];
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            seasoning_collected[i][j] = false;
        }
    }

    Image onefish_pic = LoadImage("./resources/onefish.png");
    Image youfish_pic = LoadImage("./resources/youfish.png");
    Image redfish_pic = LoadImage("./resources/redfish.png");
    Image blufish_pic = LoadImage("./resources/blufish.png");
    Image mermaid_pic = LoadImage("./resources/mermaid.png");
    Image season_pic = LoadImage("./resources/season.png");
    Image grill_pic = LoadImage("./resources/grill.png");
    Image coals = LoadImage("./resources/coals.png");
    Image showroom = LoadImage("./resources/background0.png");
    Image seafloor = LoadImage("./resources/seafloor.png");
    Image hook = LoadImage("./resources/hook.png");

    Texture2D coals_texture = LoadTextureFromImage(coals);
    Texture2D showroom_texture = LoadTextureFromImage(showroom);
    Texture2D seafloor_texture = LoadTextureFromImage(seafloor);
    Texture2D hook_texture = LoadTextureFromImage(hook);
    
    Texture2D character_pngs[] = {
        [MERMAID] = LoadTextureFromImage(mermaid_pic),
        [ONEFISH] = LoadTextureFromImage(onefish_pic),
        [REDFISH] = LoadTextureFromImage(redfish_pic),
        [BLUFISH] = LoadTextureFromImage(blufish_pic),
        [YOUFISH] = LoadTextureFromImage(youfish_pic),
        [SEASON] = LoadTextureFromImage(season_pic),
        [GRILL] = LoadTextureFromImage(grill_pic),
    };

    UnloadImage(onefish_pic);
    UnloadImage(redfish_pic);
    UnloadImage(blufish_pic);
    UnloadImage(youfish_pic);
    UnloadImage(mermaid_pic);
    UnloadImage(season_pic);
    UnloadImage(grill_pic);
    UnloadImage(coals);
    UnloadImage(showroom);
    UnloadImage(seafloor);
    UnloadImage(hook);

    Music chomp = LoadMusicStream("./resources/chomp.mp3");
    chomp.looping = false;

    Music firedance = LoadMusicStream("./resources/firedance.mp3");
    firedance.looping = false;

    Music seasoning_mp3 = LoadMusicStream("./resources/seasoning.mp3");
    seasoning_mp3.looping = false;
    
    while (!WindowShouldClose()) {
        switch (screen) {
        case TITLE: {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                screen = START;
            }
        } break;
        case START: {
            if (decide_winner) {
                float rs = total_score(redfish);
                float bs = total_score(blufish);
                float ys = total_score(youfish);
                if (rs > bs && rs > ys) {
                    screen = REDFISH_WINS;
                    winning_fish = REDFISH;
                } else if (bs > rs && bs > ys) {
                    screen = BLUFISH_WINS;
                    winning_fish = BLUFISH;
                } else if (ys > rs && ys > bs) {
                    screen = YOUFISH_WINS;
                    winning_fish = YOUFISH;
                }

                decide_winner = false;
            }

            UpdateMusicStream(chomp);
            
            if (play_chomp) {
                PlayMusicStream(chomp);
                play_chomp = false;
            }
        } break;
        case SEASONING: {
            if (timer_has_ended()) {
                screen = START;
                Image redfish_pic = LoadImageFromTexture(character_pngs[REDFISH]);
                Image blufish_pic = LoadImageFromTexture(character_pngs[BLUFISH]);
                Image youfish_pic = LoadImageFromTexture(character_pngs[YOUFISH]);
                Image season_pic = LoadImageFromTexture(character_pngs[SEASON]);
                ImageDraw(&redfish_pic, season_pic, (Rectangle){0, 0, season_pic.width, season_pic.height}, (Rectangle){0, 0, season_pic.width, season_pic.height}, WHITE);
                ImageDraw(&blufish_pic, season_pic, (Rectangle){0, 0, season_pic.width, season_pic.height}, (Rectangle){0, 0, season_pic.width, season_pic.height}, WHITE);
                ImageDraw(&youfish_pic, season_pic, (Rectangle){0, 0, season_pic.width, season_pic.height}, (Rectangle){0, 0, season_pic.width, season_pic.height}, WHITE);
                UpdateTexture(character_pngs[REDFISH], redfish_pic.data);
                UpdateTexture(character_pngs[BLUFISH], blufish_pic.data);
                UpdateTexture(character_pngs[YOUFISH], youfish_pic.data);

                StopMusicStream(seasoning_mp3);
            }

            UpdateMusicStream(seasoning_mp3);

            if (play_seasoning) {
                PlayMusicStream(seasoning_mp3);
                play_seasoning = false;
            }


            redfish.x += speed * GetRandomValue(-1, 1);
            redfish.y += speed * GetRandomValue(-1, 1);
            if (redfish.x < 0) redfish.x = speed;
            if (redfish.x > width) redfish.x = width - speed;

            blufish.x += speed * GetRandomValue(-1, 1);
            blufish.y += speed * GetRandomValue(-1, 1);
            if (blufish.x < 0) blufish.x = speed;
            if (blufish.x > width) blufish.x = speed;

            if (IsKeyDown(KEY_LEFT)  && youfish.x > 0)      youfish.x -= speed;
            if (IsKeyDown(KEY_RIGHT) && youfish.x < width)  youfish.x += speed;
            if (IsKeyDown(KEY_UP)    && youfish.y > 0)      youfish.y -= speed;
            if (IsKeyDown(KEY_DOWN)  && youfish.y < height) youfish.y += speed;

            int fish_w = character_pngs[YOUFISH].width;
            int fish_h = character_pngs[YOUFISH].height;
            int yf_ssx = (youfish.x+fish_w/2) / seasoning_texture.width;
            int yf_ssy = (youfish.y+fish_h/2) / seasoning_texture.height;
            int rf_ssx = (redfish.x+fish_w/2) / seasoning_texture.width;
            int rf_ssy = (redfish.y+fish_h/2) / seasoning_texture.height;
            int bf_ssx = (blufish.x+fish_w/2) / seasoning_texture.width;
            int bf_ssy = (blufish.y+fish_h/2) / seasoning_texture.height;
            
            if (!seasoning_collected[yf_ssx][yf_ssy]) youfish.seasoning_score += 1;
            if (!seasoning_collected[rf_ssx][rf_ssy]) redfish.seasoning_score += 1;
            if (!seasoning_collected[bf_ssx][bf_ssy]) blufish.seasoning_score += 1;

            seasoning_collected[yf_ssx][yf_ssy] = true;
            seasoning_collected[rf_ssx][rf_ssy] = true;
            seasoning_collected[bf_ssx][bf_ssy] = true;

        } break;
        case COALS: {
            if (timer_has_ended()) {
                screen = START;

                StopMusicStream(firedance);

                Image redfish_pic = LoadImageFromTexture(character_pngs[REDFISH]);
                Image blufish_pic = LoadImageFromTexture(character_pngs[BLUFISH]);
                Image youfish_pic = LoadImageFromTexture(character_pngs[YOUFISH]);
                Image grill_pic = LoadImageFromTexture(character_pngs[GRILL]);
                ImageDraw(&redfish_pic, grill_pic, (Rectangle){0, 0, grill_pic.width, grill_pic.height}, (Rectangle){0, 0, grill_pic.width, grill_pic.height}, WHITE);
                ImageDraw(&blufish_pic, grill_pic, (Rectangle){0, 0, grill_pic.width, grill_pic.height}, (Rectangle){0, 0, grill_pic.width, grill_pic.height}, WHITE);
                ImageDraw(&youfish_pic, grill_pic, (Rectangle){0, 0, grill_pic.width, grill_pic.height}, (Rectangle){0, 0, grill_pic.width, grill_pic.height}, WHITE);
                UpdateTexture(character_pngs[REDFISH], redfish_pic.data);
                UpdateTexture(character_pngs[BLUFISH], blufish_pic.data);
                UpdateTexture(character_pngs[YOUFISH], youfish_pic.data);
            }

            UpdateMusicStream(firedance);

            if (play_firedance) {
                PlayMusicStream(firedance);
                play_firedance = false;
            }
            
            redfish.x += speed * GetRandomValue(-1, 1);
            if (redfish.x < 0) redfish.x = speed;
            if (redfish.x > width) redfish.x = width - speed;

            blufish.x += speed * GetRandomValue(-1, 1);
            if (blufish.x < 0) blufish.x = speed;
            if (blufish.x > width) blufish.x = width - speed;

            if (IsKeyDown(KEY_LEFT)  && youfish.x > 0)      youfish.x -= speed;
            if (IsKeyDown(KEY_RIGHT) && youfish.x < width)  youfish.x += speed;
            if (IsKeyDown(KEY_UP)    && youfish.y > 0)      youfish.y -= speed;
            if (IsKeyDown(KEY_DOWN)  && youfish.y < height) youfish.y += speed;

            if (youfish.x > width/2 - coals_texture.width/2 && youfish.x < width/2 + coals_texture.width/2) {
                if (!youfish.coals_timer_start) youfish.coals_timer_start = true;
                youfish.coals_score += coals_score_modifier(youfish.coals_timer);
                if (youfish.coals_score < 0)
                    youfish.coals_score = 0;
            } else {
                if (youfish.coals_timer_start) {
                    youfish.coals_timer_start = false;
                    youfish.coals_timer = 0;
                }
            }

            if (redfish.x > width/2 - coals_texture.width/2 && redfish.x < width/2 + coals_texture.width/2) {
                if (!redfish.coals_timer_start) redfish.coals_timer_start = true;
                redfish.coals_score += coals_score_modifier(redfish.coals_timer);
                if (redfish.coals_score < 0)
                    redfish.coals_score = 0;
            } else {
                if (redfish.coals_timer_start) {
                    redfish.coals_timer_start = false;
                    redfish.coals_timer = 0;
                }
            }

            if (blufish.x > width/2 - coals_texture.width/2 && blufish.x < width/2 + coals_texture.width/2) {
                if (!blufish.coals_timer_start) blufish.coals_timer_start = true;
                blufish.coals_score += coals_score_modifier(blufish.coals_timer);
                if (blufish.coals_score < 0)
                    blufish.coals_score = 0;
            } else {
                if (blufish.coals_timer_start) {
                    blufish.coals_timer_start = false;
                    blufish.coals_timer = 0;
                }
            }

            if (youfish.coals_timer_start) youfish.coals_timer++;
            if (redfish.coals_timer_start) redfish.coals_timer++;
            if (blufish.coals_timer_start) blufish.coals_timer++;
        } break;
        case CREDITS: {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                exit(0);
            }
        } break;
        default:
            break;
        }

        BeginDrawing();

        switch (screen) {
        case TITLE: {
            char *text = "Left click to play";
            char *title = "Hooked On You!";
            int title_fontsize = fontsize + fontsize;
            Vector2 text_size = MeasureTextEx(font, text, fontsize, 0);
            Vector2 title_size = MeasureTextEx(font, title, title_fontsize, 0);
            
            DrawTexture(seafloor_texture, 0, 0, WHITE);
            DrawTextEx(font, title, (Vector2){ width/2.0 - title_size.x/2.0f, height/2.0 - title_fontsize * 3.0}, title_fontsize, 0, WHITE);
            DrawTextEx(font, text, (Vector2){ width/2.0 - text_size.x/2.0f, height/2.0 - fontsize/2.0}, fontsize, 0, WHITE);
        } break;
        case START: {
            DrawTexture(showroom_texture, 0, 0, WHITE);

            if (!draw_fish_and_hook) {
                if (draw_redfish) { DrawTexture(character_pngs[REDFISH], 549, 403, WHITE); }
                if (draw_blufish) { DrawTexture(character_pngs[BLUFISH], 689, 403, WHITE); }
                if (draw_youfish) { DrawTexture(character_pngs[YOUFISH], 847, 403, WHITE); }
                if (draw_scores)  { DrawScores(font, total_score(redfish), total_score(blufish), total_score(youfish)); }
                            
                if (draw_hook) {
                    switch (winning_fish) {
                    case REDFISH: {
                        DrawTexture(hook_texture, 125-300, 305, WHITE);
                    } break;
                    case BLUFISH: {
                        DrawTexture(hook_texture, 125-150, 305, WHITE);;
                    } break;
                    case YOUFISH: {
                        DrawTexture(hook_texture, 125, 305, WHITE);
                    } break;
                    default:
                        break;
                    }
                }
            } else {
                switch (bite) {
                case 0: {
                    DrawTexture(hook_texture, -630, 250, WHITE);
                    DrawTexture(character_pngs[winning_fish], 80, 365, WHITE);
                } break;
                case 1: {
                    int w = character_pngs[winning_fish].width;
                    int h = character_pngs[winning_fish].height;
                    DrawTextureRec(character_pngs[winning_fish], (Rectangle){.x=0,.y=30, .width=w, .height=h-30},(Vector2){80, 365}, WHITE);    
                } break;
                case 2: {
                    int w = character_pngs[winning_fish].width;
                    int h = character_pngs[winning_fish].height;
                    
                    DrawTextureRec(character_pngs[winning_fish], (Rectangle){.x=0,.y=60, .width=w, .height=h-60},(Vector2){80, 365}, WHITE);  
                } break;
                case 3:
                    break;
                }


                // my god is this shitty 
                switch (winning_fish) {
                case REDFISH: {
                    DrawTexture(character_pngs[BLUFISH], 689, 403, WHITE);
                    DrawTexture(character_pngs[YOUFISH], 847, 403, WHITE);
                } break;
                case BLUFISH: {
                    DrawTexture(character_pngs[REDFISH], 549, 403, WHITE);
                    DrawTexture(character_pngs[YOUFISH], 847, 403, WHITE);
                } break;
                case YOUFISH: {
                    DrawTexture(character_pngs[REDFISH], 549, 403, WHITE);
                    DrawTexture(character_pngs[BLUFISH], 689, 403, WHITE);
                } break;
                default:
                    break;
                }
            }

            DrawHeadShot(character_pngs[dialog[dialog_counter].speaker]);
            DrawPlayerName(font, characters_tostring(dialog[dialog_counter].speaker));
            if (DrawTextBox(dialog[dialog_counter].text, font)) {
                if (dialog_counter < DIALOG_LINES) {
                    if (dialog[dialog_counter].trigger_action) {
                        dialog[dialog_counter].trigger_action();
                    }
                }
                
                if (dialog_counter < DIALOG_LINES - 1) {
                    dialog_counter++;
                    StopMusicStream(chomp);
                }
            }

        } break;
        case SEASONING: {
            DrawTextureTiled(seasoning_texture, seafloor_texture, seasoning_collected);
            DrawTexture(character_pngs[YOUFISH], youfish.x, youfish.y, WHITE);
            DrawTexture(character_pngs[REDFISH], redfish.x, redfish.y, WHITE);
            DrawTexture(character_pngs[BLUFISH], blufish.x, blufish.y, WHITE);
            if (draw_scores) { DrawScores(font, total_score(redfish), total_score(blufish), total_score(youfish)); }
        } break;
        case COALS: {
            DrawTexture(seafloor_texture, 0, 0, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, 0, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, coals_texture.height, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, coals_texture.height*2, WHITE);
            DrawTexture(coals_texture, width/2 - coals_texture.width/2, coals_texture.height*3, WHITE);
            DrawTexture(character_pngs[YOUFISH], youfish.x, youfish.y, WHITE);
            DrawTexture(character_pngs[REDFISH], redfish.x, redfish.y, WHITE);
            DrawTexture(character_pngs[BLUFISH], blufish.x, blufish.y, WHITE);
            if (draw_scores) { DrawScores(font, total_score(redfish), total_score(blufish), total_score(youfish)); }
        } break;
        case YOUFISH_WINS: {
            if (DrawTextBox(youfish_route[youfish_dialog_counter].text, font)) {
                if (youfish_dialog_counter < YOUFISH_DIALOG_LINES - 1) {                    
                    youfish_dialog_counter++;
                } else {
                    screen = START;
                }
            }
        } break;
        case REDFISH_WINS: {
            if (DrawTextBox(redfish_route[redfish_dialog_counter].text, font)) {
            if (redfish_dialog_counter < REDFISH_DIALOG_LINES - 1) {                    
                redfish_dialog_counter++;
            } else {
                screen = START;
            }
        }
        } break;
        case BLUFISH_WINS: {
            if (DrawTextBox(blufish_route[blufish_dialog_counter].text, font)) {
                if (blufish_dialog_counter < BLUFISH_DIALOG_LINES - 1) {                    
                blufish_dialog_counter++;
            } else {
                screen = START;
            }
        }
        } break;
        case CREDITS: {
            int i = 0;
            DrawTexture(seafloor_texture, 0, 0, WHITE);
            DrawTextEx(font, "Credits:", (Vector2){5, 5+fontsize*i++}, fontsize, 0, WHITE);
            DrawTextEx(font, "Programming:", (Vector2){5, 5+fontsize*i++}, fontsize, 0, WHITE);
            DrawTextEx(font, "    Nathan Piel", (Vector2){5, 5+fontsize*i++}, fontsize, 0, WHITE);
            DrawTextEx(font, "Story:", (Vector2){5, 5+fontsize*i++}, fontsize, 0, WHITE);
            DrawTextEx(font, "    Nathan Piel", (Vector2){5, 5+fontsize*i++}, fontsize, 0, WHITE);
            DrawTextEx(font, "Art:", (Vector2){5, 5+fontsize*i++}, fontsize, 0, WHITE);
            DrawTextEx(font, "    Random images from google", (Vector2){5, 5+fontsize*i++}, fontsize, 0, WHITE);
            DrawTextEx(font, "    Edited together in GIMP by Nathan Piel", (Vector2){5, 5+fontsize*i++}, fontsize, 0, WHITE);
            DrawTextEx(font, "Music:", (Vector2){5, 5+fontsize*i++}, fontsize, 0, WHITE);
            DrawTextEx(font, "    firedance.mp3: Band standtune from high school", (Vector2){5, 5+fontsize*i++}, fontsize, 0, WHITE);
            DrawTextEx(font, "    seasoning.mp3: Original song by Nathan Piel", (Vector2){5, 5+fontsize*i++}, fontsize, 0, WHITE);

            char *text = "Left click to exit";
            Vector2 text_size = MeasureTextEx(font, text, fontsize, 0);
            DrawTextEx(font, text, (Vector2){ width/2.0 - text_size.x/2.0f, height/2.0 - fontsize/2.0}, fontsize, 0, WHITE);
        } break;
        }

        EndDrawing();
    }

    CloseWindow();
}
