#include <ultra64.h>
#include <PR/gs2dex.h>

#include "sm64.h"
#include "level_update.h"
#include "game_init.h"
#include "level_update.h"
#include "main.h"
#include "engine/math_util.h"
#include "area.h"
#include "sound_init.h"
#include "mario.h"
#include "debug_course.h"

#include "mario_header.h"
#include "luigi_header.h"
#include "yoshi_header.h"
#include "wario_header.h"

#include "poster.h"
#include "mario_poster.h"
#include "luigi_poster.h"
#include "yoshi_poster.h"
#include "wario_poster.h"

enum charValues {
    MARIO,
    LUIGI,
    YOSHI,
    WARIO,
};

Gfx s2d_init_dl[] = {
    gsDPPipeSync(),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetTextureConvert(G_TC_FILT),
    gsDPSetAlphaCompare(G_AC_THRESHOLD),
    gsDPSetBlendColor(0, 0, 0, 0x01),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPEndDisplayList(),
};


// render chara and posters
void scroll_pick_chara(s32 charNum) {
    switch (charNum) {
        case MARIO:
            gSPDisplayList(gDisplayListHead++, mario_poster_bg_dl);
            break;
        case LUIGI:
            gSPDisplayList(gDisplayListHead++, luigi_poster_bg_dl);
            break;
        case YOSHI:
            gSPDisplayList(gDisplayListHead++, yoshi_poster_bg_dl);
            break;
        case WARIO:
            gSPDisplayList(gDisplayListHead++, wario_poster_bg_dl);
            break;
    }
    gSPDisplayList(gDisplayListHead++, poster_bg_dl);
}

enum char_place_modes {
    MODE_UNIFORM,
    MODE_SCATTERED,
    MODE_SCROLLUNIFORM,
    MODE_SCROLLSCATTER,
};


void place_chara(s32 charNum) {
    switch (charNum) {
        case MARIO:
            gSPDisplayList(gDisplayListHead++, mario_sprite_dl);
            break;
        case LUIGI:
            gSPDisplayList(gDisplayListHead++, luigi_sprite_dl);
            break;
        case YOSHI:
            gSPDisplayList(gDisplayListHead++, yoshi_sprite_dl);
            break;
        case WARIO:
            gSPDisplayList(gDisplayListHead++, wario_sprite_dl);
            break;
    }
}

void mtx_chara(s32 charNum) {
    switch (charNum) {
        case MARIO:
            gSPObjMatrix(gDisplayListHead++, &mario_mtx);
            break;
        case LUIGI:
            gSPObjMatrix(gDisplayListHead++, &luigi_mtx);
            break;
        case YOSHI:
            gSPObjMatrix(gDisplayListHead++, &yoshi_mtx);
            break;
        case WARIO:
            gSPObjMatrix(gDisplayListHead++, &wario_mtx);
            break;
    }
}


uObjBg poster_bg = {
    0, 256<<2, 0<<2, 320<<2,  /* imageX, imageW, frameX, frameW */
    0, 191<<2, 0<<2, 240<<2,  /* imageY, imageH, frameY, frameH */
    (u64 *)&poster_tex_0,                /* imagePtr                       */
    G_BGLT_LOADBLOCK,     /* imageLoad */                      
    G_IM_FMT_RGBA,        /* imageFmt                       */
    G_IM_SIZ_16b,         /* imageSiz                       */
    0,                /* imagePal                       */
    0,             /* imageFlip                      */
    qu510(0.8),       /* scale W (s5.10) */
    qu510(0.8),       /* scale H (s5.10) */
    0,
};

Gfx poster_bg_dl[] = {
    gsDPPipeSync(),

    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2),
    gsSPObjRenderMode(G_OBJRM_XLU | G_OBJRM_BILERP),
    gsSPBgRect1Cyc(&poster_bg),
    gsSPEndDisplayList(),
};// 256 191

int cte = LUIGI;

enum modeSelect {
    MODE_PICKING,
    MODE_SLEUTH,
};

#define ENT_SIZE 100

uObjSprite entities[ENT_SIZE];
uObjTxtr textures[ENT_SIZE];
char entTracker[ENT_SIZE];

char sleuthed_char[4] = {0, 0, 0, 0};

uObjSprite *rngTimeSprite[4] = {
    &mario_obj,
    &luigi_obj,
    &yoshi_obj,
    &wario_obj,
};
uObjTxtr *rngTimeTex[4] = {
    &mario_tex,
    &luigi_tex,
    &yoshi_tex,
    &wario_tex,
};

index_of_sleuth = 0;
void populate_entities(s32 charNum) {
    // int rd = LUIGI;
    int i;
    for (i = 0; i < ENT_SIZE; i++) {
        int rd = random_u16() % (WARIO + 1);
        switch(rd) {
            case MARIO:
                if (sleuthed_char[MARIO] == 0) {
                    entities[i] = *((uObjSprite*)segmented_to_virtual(&mario_obj));
                    textures[i] = *((uObjTxtr*)segmented_to_virtual(&mario_tex));
                    entTracker[i] = MARIO;
                    if (charNum == MARIO) {
                        sleuthed_char[MARIO] = 1;
                        index_of_sleuth = i;
                    }
                }
                else {
                    int x = random_u16() % (WARIO + 1);
                    if (x == MARIO) x++;
                    entities[i] = *((uObjSprite*)segmented_to_virtual(rngTimeSprite[x]));
                    textures[i] = *((uObjTxtr*)segmented_to_virtual(rngTimeTex[x]));
                    entTracker[i] = x;
                }
            break;
            case LUIGI:
                if (sleuthed_char[LUIGI] == 0) {
                    entities[i] = *((uObjSprite*)segmented_to_virtual(&luigi_obj));
                    textures[i] = *((uObjTxtr*)segmented_to_virtual(&luigi_tex));
                    entTracker[i] = LUIGI;
                    if (charNum == LUIGI) {
                        sleuthed_char[LUIGI] = 1;
                        index_of_sleuth = i;
                    }
                }
                else {
                    int x = random_u16() % (WARIO + 1);
                    if (x == LUIGI) x++;
                    entities[i] = *((uObjSprite*)segmented_to_virtual(rngTimeSprite[x]));
                    textures[i] = *((uObjTxtr*)segmented_to_virtual(rngTimeTex[x]));
                    entTracker[i] = x;
                }
            break;
            case YOSHI:
                if (sleuthed_char[YOSHI] == 0) {
                    entities[i] = *((uObjSprite*)segmented_to_virtual(&yoshi_obj));
                    textures[i] = *((uObjTxtr*)segmented_to_virtual(&yoshi_tex));
                    entTracker[i] = YOSHI;
                    if (charNum == YOSHI) {
                        sleuthed_char[YOSHI] = 1;
                        index_of_sleuth = i;
                    }
                }
                else {
                    int x = random_u16() % (WARIO + 1);
                    if (x == YOSHI) x--;
                    entities[i] = *((uObjSprite*)segmented_to_virtual(rngTimeSprite[x]));
                    textures[i] = *((uObjTxtr*)segmented_to_virtual(rngTimeTex[x]));
                    entTracker[i] = x;
                }
            break;
            case WARIO:
                if (sleuthed_char[WARIO] == 0) {
                    entities[i] = *((uObjSprite*)segmented_to_virtual(&wario_obj));
                    textures[i] = *((uObjTxtr*)segmented_to_virtual(&wario_tex));
                    entTracker[i] = WARIO;
                    if (charNum == WARIO) {
                        sleuthed_char[WARIO] = 1;
                        index_of_sleuth = i;
                    }
                }
                else {
                    int x = random_u16() % (WARIO + 1);
                    if (x == WARIO) x--;
                    entities[i] = *((uObjSprite*)segmented_to_virtual(rngTimeSprite[x]));
                    textures[i] = *((uObjTxtr*)segmented_to_virtual(rngTimeTex[x]));
                    entTracker[i] = x;
                }
            break;
        }
    }
}

u32 random_range(int e)  {
    int x = random_u16();
    if (x == 0) x ++;
    return (osGetTime() / x) % (e + 1);
}

#define abs(x) ((x) < 0 ? -(x) : (x))

void randomize_positions(void) {
    int i;
    entities[index_of_sleuth].s.objX = random_range(320) << 2;
    entities[index_of_sleuth].s.objY = random_range(240) << 2;
    for (i = 0; i < ENT_SIZE; i++) {
        if (i != index_of_sleuth) {
            int x = random_range(320) << 2;
            int y = random_range(240) << 2;

            int x2 = entities[index_of_sleuth].s.objX;
            int y2 = entities[index_of_sleuth].s.objX;

            if (abs(y2 - y) < 30) y += 50;
            if (abs(x2 - x) < 30) x += 50;
            entities[i].s.objX = x;
            entities[i].s.objY = y;
        }
    }
}

void scroll_chars(int mx, int my, int lx, int ly, int yx, int yy, int wx, int wy) {
    int i = 0;
    for (i; i < ENT_SIZE; i++) {
        switch (entTracker[i]){
            case MARIO:
                entities[i].s.objX += (mx << 2);
                entities[i].s.objY += (my << 2);
            break;
            case LUIGI:
                entities[i].s.objX += (lx << 2);
                entities[i].s.objY += (ly << 2);
            break;
            case YOSHI:
                entities[i].s.objX += (yx << 2);
                entities[i].s.objY += (yy << 2);
            break;
            case WARIO:
                entities[i].s.objX += (wx << 2);
                entities[i].s.objY += (wy << 2);
            break;
        }
        if (entities[i].s.objX > (320 << 2)) entities[i].s.objX = (-32 << 2);
        if (entities[i].s.objY > (240 << 2)) entities[i].s.objY = (2 << 2);
        if (entities[i].s.objX < (-32 << 2)) entities[i].s.objX = (320 << 2);
        if (entities[i].s.objY < (2 << 2)) entities[i].s.objY = (240 << 2);
    }
}

int mini_mode = MODE_PICKING;

void disp_chara(int x) {

    gSPObjLoadTxtr(gDisplayListHead++, &textures[x]);
    
    gSPObjSprite(gDisplayListHead++, &entities[x]);
}

int latch_sleuth = 0;
int latch_picking = 0;

int picking_timer;

int remaining_time = 30;
u32 secondTimer = 0;

void reset(void) {
    latch_sleuth = 0;
    latch_picking = 0;
    picking_timer = 0;
    sleuthed_char[MARIO] = 0;
    sleuthed_char[LUIGI] = 0;
    sleuthed_char[WARIO] = 0;
    sleuthed_char[YOSHI] = 0;
    mini_mode = MODE_PICKING;
    remaining_time = 30;
}

int curX = 50;
int curY = 50;
void draw_cursor(void) {
    print_text(curX, curY, "*");
    curX += (gPlayer1Controller->stickX / 4);
    curY += (gPlayer1Controller->stickY / 4);
    if (curX > 320) {curX = 320;}
    if (curX < 0) {curX = 0;}
    if (curY > 180) {curY = 180;}
    if (curY < 0) {curY = 0;}
}

// void draw_hud(void) {
// // /    gSPLoadUcode(gDisplayListHead++, gspF3DEX2_fifoTextStart, gspF3DEX2_fifoDataStart);

//     gDPSetFillColor(gDisplayListHead++, (GPACK_RGBA5551(238, 252, 40, 1) << 16) | GPACK_RGBA5551(238, 252, 40, 1));
//     gDPFillRectangle(gDisplayListHead++, 0, 0, 320, 50);

//     // gSPLoadUcode(gDisplayListHead++, gspS2DEX2_fifoTextStart, gspS2DEX2_fifoDataStart);
// }



void render_minigame(void) {
    int i;
    gSPLoadUcode(gDisplayListHead++, gspS2DEX2_fifoTextStart, gspS2DEX2_fifoDataStart);

    // init
    gSPDisplayList(gDisplayListHead++, s2d_init_dl);

    // cover the end cake lol
    gDPSetFillColor(gDisplayListHead++, 0xFFFFFFFF);
    gDPFillRectangle(gDisplayListHead++, 0, 0, 320, 240);


    if (mini_mode == MODE_PICKING) {
        if (latch_picking == 0) {
            picking_timer = random_range(50) + 50;
            latch_picking = 1;
        }
        scroll_pick_chara(cte);

        // if (gPlayer1Controller->buttonPressed & L_TRIG)
        if (gGlobalTimer % 2 == 0){
            if (picking_timer > 0)cte++;
        }
        picking_timer--;
        if (cte > WARIO) cte = MARIO;
        if (picking_timer < -20) {
            // picking_timer = 0;
            mini_mode = MODE_SLEUTH;
        }
    }
    if (mini_mode == MODE_SLEUTH) {
        if (latch_sleuth == 0){
            populate_entities(cte);
            randomize_positions();
            latch_sleuth = 1;
        }
        mtx_chara(cte);
        place_chara(cte);
        gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
        gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2);
        gSPObjRenderMode(gDisplayListHead++, G_OBJRM_XLU | G_OBJRM_BILERP);
        for (i = 0; i < ENT_SIZE; i++) {
            disp_chara(i);
        }
        // debug reset
        if (gPlayer1Controller->buttonPressed & L_TRIG) {
            reset();
        }
        scroll_chars(1, 1, -1, -1, 1, 0, 0, -1);
        draw_cursor();
        print_text(120, 210, "TIME");
        print_text_fmt_int(140, 190, "%d", remaining_time);
        if (secondTimer % 30 == 0) {
            remaining_time--;
        }
        if (remaining_time == 0) reset();
    }

    secondTimer++;
    gSPLoadUcode(gDisplayListHead++, gspF3DEX2_fifoTextStart, gspF3DEX2_fifoDataStart);
    // if (mini_mode == MODE_SLEUTH)
    //     draw_hud();
}
