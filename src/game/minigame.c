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


#include "luigi_header.h"

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
            break;
        case LUIGI:
            gSPDisplayList(gDisplayListHead++, luigi_sprite_dl);
            break;
        case YOSHI:
            break;
        case WARIO:
            break;
    }
}

void mtx_chara(void) {
    gSPObjMatrix(gDisplayListHead++, &luigi_mtx);
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

void populate_entities(void) {
    // int rd = random_u16() % WARIO;
    int rd = LUIGI;
    int i;
    for (i = 0; i < ENT_SIZE; i++) {
        switch(rd) {
            case MARIO:
            break;
            case LUIGI:
            entities[i] = *((uObjSprite*)segmented_to_virtual(&luigi_obj));
            textures[i] = *((uObjTxtr*)segmented_to_virtual(&luigi_tex));
            break;
            case YOSHI:
            break;
            case WARIO:
            break;
        }
    }
}

u32 random_range(int e)  {
    return random_u16() % e;
}

void randomize_positions(void) {
    int i;
    for (i = 0; i < ENT_SIZE; i++) {
        entities[i].s.objX = random_range(320) << 2;
        entities[i].s.objY = random_range(240) << 2;
    }
}

int mini_mode = MODE_SLEUTH;

void disp_chara(int x) {

    gSPObjLoadTxtr(gDisplayListHead++, &textures[x]);
    
    gSPObjSprite(gDisplayListHead++, &entities[x]);
}

void render_minigame(void) {
    int i;
    gSPLoadUcode(gDisplayListHead++, gspS2DEX2_fifoTextStart, gspS2DEX2_fifoDataStart);

    // init
    gSPDisplayList(gDisplayListHead++, s2d_init_dl);

    // cover the end cake lol
    gDPSetFillColor(gDisplayListHead++, 0xFFFFFFFF);
    gDPFillRectangle(gDisplayListHead++, 0, 0, 320, 240);

    populate_entities();
    randomize_positions();

    if (mini_mode == MODE_PICKING) {
        scroll_pick_chara(cte);

        // if (gPlayer1Controller->buttonPressed & L_TRIG)
        if (gGlobalTimer % 2 == 0)
        cte++;
        if (cte > WARIO) cte = MARIO;
    }
    if (mini_mode == MODE_SLEUTH) {
        mtx_chara();
        place_chara(cte);
        gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
        gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2);
        gSPObjRenderMode(gDisplayListHead++, G_OBJRM_XLU | G_OBJRM_BILERP);
        for (i = 0; i < ENT_SIZE; i++) {
            disp_chara(i);
        }
    }


    gSPLoadUcode(gDisplayListHead++, gspF3DEX2_fifoTextStart, gspF3DEX2_fifoDataStart);
}
