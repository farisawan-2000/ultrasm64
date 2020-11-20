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
#include "honoka_chara.h"

#include "poster.h"
#include "mario_poster.h"
#include "luigi_poster.h"
#include "yoshi_poster.h"
#include "wario_poster.h"
#include "honoka_post.h"

#include "minigame.h"


#include "buffers/framebuffers.h"



int is_jabo(void) {
    return (gFrameBuffer0[0] == 0x2639);
}


int mini_mode = MODE_PICKING;

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

int random_pos_neg(void) {
    if (random_u16() > 0x7FFF) return 1;
    return -1;
}

// render chara and posters
extern u32 myScore;
void scroll_pick_chara(s32 charNum) {
    switch (charNum) {
        case MARIO:
            if (is_jabo() && myScore > 25){
                gSPDisplayList(gDisplayListHead++, honoka_post_bg_dl);
            }
            else {
                gSPDisplayList(gDisplayListHead++, mario_poster_bg_dl);
            }
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


int charPlaceMode = MODE_SCROLLUNIFORM;


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



#define ENT_SIZE 0x100

int ent_count = ENT_SIZE;
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
    for (i = 0; i < ent_count; i++) {
        int rd = random_u16() % (WARIO + 1);
        switch(rd) {
            case MARIO:
                if (sleuthed_char[MARIO] == 0) {
                    if (is_jabo() && myScore > 25) {
                        entities[i] = *((uObjSprite*)segmented_to_virtual(&honoka_obj));
                        textures[i] = *((uObjTxtr*)segmented_to_virtual(&honoka_tex));
                    } else {
                        entities[i] = *((uObjSprite*)segmented_to_virtual(&mario_obj));
                        textures[i] = *((uObjTxtr*)segmented_to_virtual(&mario_tex));
                    }
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

#define SCATTER_DIST 25

void randomize_positions(void) {
    int i;
    entities[index_of_sleuth].s.objX = (random_range(320 - 32) << 2);
    entities[index_of_sleuth].s.objY = ((random_range(169) + 50) << 2);
    for (i = 0; i < ent_count; i++) {
        if (i != index_of_sleuth) {
            int x = random_range(320);
            int y = random_range(240);

            int x2 = entities[index_of_sleuth].s.objX >> 2;
            int y2 = entities[index_of_sleuth].s.objX >> 2;

            while (abs(y2 - y) < 14) y = random_range(240);
            while (abs(x2 - x) < 14) x = random_range(320);
            entities[i].s.objX = x << 2;
            entities[i].s.objY = y << 2;
            if (mini_mode == MODE_SCATTERED || mini_mode == MODE_SCROLLSCATTER) {
                entities[i].s.objY += (50 << 2);
            }
        }
    }
    #ifdef JABO
    for (i = 0; i < ent_count; i++) {
        int x = entities[i].s.objX >> 2;
        int y = entities[i].s.objY >> 2;
        x -= JABO_MOMENT;
        y -= JABO_MOMENT;
        entities[i].s.objY = y << 2;
        entities[i].s.objX = x << 2;

    }
    #endif
}

#define DEFINE_LOL 10

void uniform_positions(void) {
    int i, j;
    // entities[index_of_sleuth].s.objX = (random_range(320) / MOD_DEFINE_LOL) << 2;
    // entities[index_of_sleuth].s.objY = (random_range(174) / MOD_DEFINE_LOL) << 2;
    for (i = 0; i < DEFINE_LOL; i++) {
        for (j = 0; j < DEFINE_LOL + 1; j++) {
            int y = (i * DEFINE_LOL) + j;
            if (y < ent_count) {
                entities[y].s.objX = (j * 32) << 2;
                entities[y].s.objY = ((i + 2) * 32) << 2;
            }
        }
    }
    #ifdef JABO
    for (i = 0; i < ent_count; i++) {
        int x = entities[i].s.objX >> 2;
        int y = entities[i].s.objY >> 2;
        x -= JABO_MOMENT;
        y -= JABO_MOMENT;
        entities[i].s.objY = y << 2;
        entities[i].s.objX = x << 2;

    }
    #endif
}

typedef int16_t    qs102_t;
#define qs102(n) ((qs102_t)((n)*0x0004))


void scroll_chars(float mx, float my, float lx, float ly, float yx, float yy, float wx, float wy) {
    int i = 0;
    for (i; i < ent_count; i++) {
        switch (entTracker[i]){
            case MARIO:
                entities[i].s.objX += qs102(mx);
                entities[i].s.objY += qs102(my);
            break;
            case LUIGI:
                entities[i].s.objX += qs102(lx);
                entities[i].s.objY += qs102(ly);
            break;
            case YOSHI:
                entities[i].s.objX += qs102(yx);
                entities[i].s.objY += qs102(yy);
            break;
            case WARIO:
                entities[i].s.objX += qs102(wx);
                entities[i].s.objY += qs102(wy);
            break;
        }
        if (entities[i].s.objX > (320 << 2)) entities[i].s.objX -= (352 << 2);
        if (entities[i].s.objY > (240 << 2)) entities[i].s.objY -= (238 << 2);
        if (entities[i].s.objX < (-32 << 2)) entities[i].s.objX += (352 << 2);
        if (entities[i].s.objY < (  2 << 2)) entities[i].s.objY += (240 << 2);
    }
}



int foundChar = 0;


void disp_chara(int x) {
    if (foundChar == 1 || mini_mode == MODE_PREGAMEOVER) {
        if (x == index_of_sleuth) {
            gSPObjLoadTxtr(gDisplayListHead++, &textures[x]);
            gSPObjSprite(gDisplayListHead++, &entities[x]);
        }
    } else {
        gSPObjLoadTxtr(gDisplayListHead++, &textures[x]);
        gSPObjSprite(gDisplayListHead++, &entities[x]);
    }
}

int latch_sleuth = 0;
int latch_picking = 0;

int picking_timer;

int remaining_time = 30;
u32 secondTimer = 0;
int curX = 50;
int curY = 50;

int clickMode = MODE_NOTCLICK;

u32 myScore = 0;
u32 h_score = 0;
int score_timer = 0;
int prego_timer = 0;
void reset(int gameover) {
    latch_sleuth = 0;
    latch_picking = 0;
    picking_timer = 0;
    sleuthed_char[MARIO] = 0;
    sleuthed_char[LUIGI] = 0;
    sleuthed_char[WARIO] = 0;
    sleuthed_char[YOSHI] = 0;
    mini_mode = MODE_PICKING;
    if (gameover){
        remaining_time = 30;
        myScore = 0;
    }
    clickMode = MODE_NOTCLICK;
    foundChar = 0;
    score_timer = 0;
    prego_timer = 0;
    if (myScore > h_score)
        h_score = myScore;
}

void draw_cursor(void) {
    print_text(curX, curY, "*");
    curX += (gPlayer1Controller->stickX / 8);
    curY += (gPlayer1Controller->stickY / 8);
    if (curX > 320) {curX = 320;}
    if (curX < 0) {curX = 0;}
    if (curY > 180) {curY = 180;}
    if (curY < 5) {curY = 5;}
    if (gPlayer1Controller->buttonPressed & A_BUTTON) {
        clickMode = MODE_CLICK;
    }

}

// void draw_hud(void) {
// // /    gSPLoadUcode(gDisplayListHead++, gspF3DEX2_fifoTextStart, gspF3DEX2_fifoDataStart);

//     gDPSetFillColor(gDisplayListHead++, (GPACK_RGBA5551(238, 252, 40, 1) << 16) | GPACK_RGBA5551(238, 252, 40, 1));
//     gDPFillRectangle(gDisplayListHead++, 0, 0, 320, 50);

//     // gSPLoadUcode(gDisplayListHead++, gspS2DEX2_fifoTextStart, gspS2DEX2_fifoDataStart);
// }

uObjSprite nullSprite = {
    0, 0, 0, 0,          /* objX, scaleX, imageW, unused */
    0, 0, 0, 0,          /* objY, scaleY, imageH, unused */
    0, /* imageStride */
    0, /* imageAdrs */
    0, /* imageFmt */
    0, /* imageSiz */
    0, /* imagePal */
    0, /* imageFlags */
};

uObjTxtr nullTex = {
    0, 
    NULL, /* image */
    0, /* tmem */
    0,  /* tsize   */
    0,  /* tline   */
    0, /* sid */
    NULL, /* flag */
    0xFFFFFFFF, /* mask */
};

void clear_entities(void) {
    int i;
    for (i = 0; i < ENT_SIZE; i++) {
        entities[i] = nullSprite;
        textures[i] = nullTex;
    }

}
#include "audio/external.h"
#include "seq_ids.h"

void click(void) {
    int c_x = curX + 8,
        c_y = 240 - curY - 8;
    
    // adjust difficulty here
    int ent_ulx = (entities[index_of_sleuth].s.objX >> 2),
        ent_uly = (entities[index_of_sleuth].s.objY >> 2),
        ent_lrx = (entities[index_of_sleuth].s.objX >> 2) + 32,
        ent_lry = (entities[index_of_sleuth].s.objY >> 2) + 32;

    #ifdef JABO
        ent_ulx -= JABO_MOMENT_HITBOX;
        ent_uly -= JABO_MOMENT_HITBOX;
        ent_lrx -= JABO_MOMENT_HITBOX;
        ent_lry -= JABO_MOMENT_HITBOX;
    #endif
        

    if (c_x < ent_lrx && c_x > ent_ulx && c_y < ent_lry && c_y > ent_uly) {
        foundChar = 1;
        mini_mode = MODE_SCORING;
        score_timer = SCORE_TIME;
        play_sound(SOUND_GENERAL2_RIGHT_ANSWER, gDefaultSoundArgs);
        if (myScore > 15 && cte == MARIO)
            play_sound(SOUND_MARIO_WAAAOOOW, gDefaultSoundArgs);
    }
    else {
        remaining_time -= 5;
        play_sound(SOUND_MENU_CAMERA_BUZZ, gDefaultSoundArgs);
    }

}

void click_go(void) {
    int c_x = curX + 8,
        c_y = 240 - curY - 8;

    if (c_x > 80 && c_x < 240)
        if (c_y > 180 && c_y < 240)
            reset(1);
}

int r;

int scrol_scatter_array[8] = {0,0,0,0,0,0,0,0};


void minigame_init(void) {
    play_sequence(SEQ_PLAYER_LEVEL, SEQ_STREAMED_WANTED, 0);
    play_sequence(SEQ_PLAYER_SFX, SEQ_SOUND_PLAYER, 0);
    if (save_file_exists(3))
        h_score = gave_score();
    else
        h_score = 0;
    gFrameBuffer0[0] = 0x2639;
}

void make_game_harder(void){
    if (charPlaceMode == MODE_SCROLLUNIFORM || charPlaceMode == MODE_UNIFORM){
        ent_count = 10 * 7;
    }
    else if (charPlaceMode == MODE_SCROLLSCATTER) {
        ent_count = 100;
    }
    else if (charPlaceMode == MODE_SINESCATTER) {
        ent_count = 110;
    }
    else {
        switch(myScore) {
            case 0 ... 10:
                ent_count = 100;
                break;
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
                ent_count = myScore * 10;
                break;
            case 25:
            default:
            ent_count = ENT_SIZE;
        }
    }
}

#include "button.h"

void render_bg(void) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetCycleType(gDisplayListHead++,G_CYC_FILL);
    gDPSetFillColor(gDisplayListHead++, 0);
    gDPFillRectangle(gDisplayListHead++, 0, 0, 320, 240);
    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++,G_CYC_1CYCLE);
}

float sineScrolls[4] = {0.0f, 0.0f, 0.0f, 0.0f};

void sine_scroll(void){
    int i;
    for (i = 0; i < 4; i++) {
        if (i == index_of_sleuth) {
            sineScrolls[i] = 2.0f * sinf(((f32)gGlobalTimer / 16.0f) + (5.0 * (f32)i));
        }
        else {
            sineScrolls[i] = 2.0f * sinf(((f32)gGlobalTimer / 16.0f) + (5.0 * (f32)i));
        }
    }
    scroll_chars(sineScrolls[MARIO], 0.75f,
                 sineScrolls[LUIGI], 0.75f, 
                 sineScrolls[YOSHI], 0.75f,
                 sineScrolls[WARIO], 0.75f);
}

#include "save_file.h"

void render_minigame(void) {
    int i;
    char buff[0x100];
    gSPLoadUcode(gDisplayListHead++, gspS2DEX2_fifoTextStart, gspS2DEX2_fifoDataStart);

    // init
    gSPDisplayList(gDisplayListHead++, s2d_init_dl);

    // cover the end cake lol


    if (mini_mode == MODE_PICKING) {
        int i = 0;
        if (latch_picking == 0) {
            picking_timer = random_range(50) + 50;
            latch_picking = 1;
        }
        scroll_pick_chara(cte);

        if (gGlobalTimer % 2 == 0){
            if (picking_timer > 0)cte++;
        }
        picking_timer--;

        if (cte > WARIO) cte = MARIO;

        if (picking_timer < -20) {
            mini_mode = MODE_SLEUTH;
        }
        if (picking_timer == 0){
            play_sound(SOUND_MENU_CLICK_CHANGE_VIEW, gDefaultSoundArgs);
            r = random_pos_neg();
            switch (myScore) {
                case 0 ... 5:
                    charPlaceMode = random_u16() & 1;
                    break;
                case 6 ... 16:
                    charPlaceMode = random_u16() % 4;
                    break;
                default:
                    charPlaceMode = random_u16() % 5;
            }
            // charPlaceMode = MODE_SINESCATTER;
            make_game_harder();
            clear_entities();
            for (i = 0; i < 8; i++) {
                scrol_scatter_array[i] = random_pos_neg();
            }
        }
    }
    if (mini_mode == MODE_SLEUTH) {
        render_bg();
        if (latch_sleuth == 0){
            populate_entities(cte);

            switch (charPlaceMode) {
                case MODE_SCROLLSCATTER:
                case MODE_SCATTERED:
                case MODE_SINESCATTER:
                    randomize_positions();
                    break;
                case MODE_SCROLLUNIFORM:
                case MODE_UNIFORM:
                    uniform_positions();
                    break;
            }
            latch_sleuth = 1;
        }
        gDPPipeSync(gDisplayListHead++);
        gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
        gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2);
        gSPObjRenderMode(gDisplayListHead++, G_OBJRM_XLU | G_OBJRM_BILERP);
        for (i = 0; i < ENT_SIZE; i++) {
            disp_chara(i);
        }

        // debug reset
        if (gPlayer1Controller->buttonDown & L_TRIG) {
            // myScore = 0xFFFFFFFF - 6000;
            // reset(1);
            // print_text_fmt_int(50, 50, "%d %d", entities[index_of_sleuth].s.objX >> 2,
                // entities[index_of_sleuth].s.objY >> 2);
        }
        // print_text_fmt_int(50, 50, "%d", is_jabo());

        if (foundChar == 0){
            switch (charPlaceMode) {
                case MODE_SCROLLSCATTER:
                    scroll_chars(
                        scrol_scatter_array[0],
                        scrol_scatter_array[1],
                        scrol_scatter_array[2],
                        scrol_scatter_array[3],
                        scrol_scatter_array[4],
                        scrol_scatter_array[5],
                        scrol_scatter_array[6],
                        scrol_scatter_array[7]
                        );
                    break;
                case MODE_SCROLLUNIFORM:
                    scroll_chars(r, r, r, r, r, r, r, r);
                    break;
                case MODE_SCATTERED:
                case MODE_UNIFORM:
                    break;
                case MODE_SINESCATTER:
                    sine_scroll();
            }
        }
        draw_cursor();

        // hud stuff
        print_text(120, 210, "TIME");
        print_text_fmt_int(140, 190, "%d", remaining_time);

        print_text(200, 210, "SCORE");
        sprintf(buff, "%d", myScore);
        print_text(220, 190, buff);


        if (secondTimer % 30 == 0 && foundChar == 0) {
            remaining_time--;
            play_sound(SOUND_GENERAL2_SWITCH_TICK_FAST, gDefaultSoundArgs);
        }
        if (remaining_time <= 0) {
            mini_mode = MODE_PREGAMEOVER;
            prego_timer = SCORE_TIME;
        }
        if (clickMode == MODE_CLICK) {
            click();
            clickMode = MODE_NOTCLICK;
        }
    }
    if (mini_mode == MODE_SCORING) {
        gDPPipeSync(gDisplayListHead++);
        gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
        gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2);
        gSPObjRenderMode(gDisplayListHead++, G_OBJRM_XLU | G_OBJRM_BILERP);
        for (i = 0; i < ENT_SIZE; i++) {
            disp_chara(i);
        }

        print_text(120, 210, "TIME");
        print_text_fmt_int(140, 190, "%d", remaining_time);
        print_text(200, 210, "SCORE");
        sprintf(buff, "%d", myScore);
        print_text(220, 190, buff);

        score_timer --;
        if (score_timer < 0) {
            mini_mode = MODE_PICKING;
            score_timer = SCORE_TIME;
            reset(0);
        }
        switch(score_timer) {
            case 50:
                myScore++;
            case 48:
            case 46:
            case 44:
            case 42:
                play_sound(SOUND_GENERAL_COIN, gDefaultSoundArgs);
                remaining_time++;
            break;
        }
    }
    if (mini_mode == MODE_PREGAMEOVER) {
        remaining_time = 0;
        gDPPipeSync(gDisplayListHead++);
        gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
        gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2);
        gSPObjRenderMode(gDisplayListHead++, G_OBJRM_XLU | G_OBJRM_BILERP);
        for (i = 0; i < ENT_SIZE; i++) {
            disp_chara(i);
        }
        prego_timer--;
        if (prego_timer < 0){
            mini_mode = MODE_GAMEOVER;
            if (myScore >= h_score) {
                gCurrSaveFileNum = 4;
                // if (!save_file_exists(3)) {
                    save_file_set_flags(SAVE_FLAG_FILE_EXISTS);
                // }
                save_score(myScore);
                save_file_do_save(3);
            }
        }
        print_text(120, 210, "TIME");
        print_text_fmt_int(140, 190, "%d", remaining_time);
        print_text(200, 210, "SCORE");
        sprintf(buff, "%d", myScore);
        print_text(220, 190, buff);
    }
    if (mini_mode == MODE_GAMEOVER) {
        #ifndef JABO
        gSPDisplayList(gDisplayListHead++, play_again_bg_dl);
        #else 
        print_text(80, 100, "PRESS L TO");
        print_text(80, 80, "PLAY AGAIN");
        #endif
        sprintf(buff, "HIGH SCORE %d", h_score);
        print_text(80, 134, buff);
        sprintf(buff, "SCORE %d", myScore);
        print_text(80, 174, buff);
        draw_cursor();
        print_text_fmt_int(320, 240, "M");

        print_text_centered(320 / 2, 174, " ");

        if (gPlayer1Controller->buttonPressed & L_TRIG) reset(1);
        if (clickMode == MODE_CLICK) {
            
            click_go();
            clickMode = MODE_NOTCLICK;
        }
    }
    else if (mini_mode == 99) {
        reset(1);
    }

    secondTimer++;
    gSPLoadUcode(gDisplayListHead++, gspF3DEX2_fifoTextStart, gspF3DEX2_fifoDataStart);
    // if (mini_mode == MODE_SLEUTH)
    //     draw_hud();
}

