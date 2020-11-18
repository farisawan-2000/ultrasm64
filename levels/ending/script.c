#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/area.h"
#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/ending/header.h"

extern const LevelScript marker[];
extern const LevelScript level_intro_entry_2[];
extern const LevelScript level_main_menu_entry_1[];
extern const LevelScript level_intro_entry_1[];
void minigame_init(void);
const LevelScript level_ending_entry[] = {
    /*0*/ INIT_LEVEL(),
    /*1*/ LOAD_YAY0(/*seg*/ 0x07, _ending_segment_7SegmentRomStart, _ending_segment_7SegmentRomEnd),
    /*4*/ ALLOC_LEVEL_POOL(),

    /*5*/ AREA(/*index*/ 1, ending_geo_000050),
    /*7*/ END_AREA(),

    /*8*/ FREE_LEVEL_POOL(),
    /*9*/ SLEEP(/*frames*/ 2),
    /*10*/ BLACKOUT(/*active*/ FALSE),
    /*11*/ LOAD_AREA(/*area*/ 1),
    CALL(0, minigame_init),
    /*12*/ TRANSITION(/*transType*/ WARP_TRANSITION_FADE_FROM_STAR, /*time*/ 20, /*color*/ 0xFF, 0xFF, 0xFF),
    /*14*/ SLEEP(/*frames*/ 2),
    CALL_LOOP(/*arg*/ 0, /*func*/ lvl_play_the_end_screen_sound),
        // CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
        // CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    // L1:
    /*17*/ SLEEP(/*frames*/ 1),
    EXIT_AND_EXECUTE(/*seg*/ 0x14, _introSegmentRomStart, _introSegmentRomEnd, level_intro_entry_1),
    // /*18*/ JUMP(level_intro_entry_1),
    // SLEEP_BEFORE_EXIT(1),
    // EXIT(),
};
const LevelScript marker[] = {
    STOP_MUSIC(/*fadeOutTime*/ 0x00BE),
    TRANSITION(/*transType*/ WARP_TRANSITION_FADE_INTO_COLOR, /*time*/ 16, /*color*/ 0xFF, 0xFF, 0xFF),
    SLEEP(/*frames*/ 16),
    CLEAR_LEVEL(),
    SLEEP(/*frames*/ 2),
    SET_REG(/*value*/ 16),
    EXIT_AND_EXECUTE(/*seg*/ 0x14, _menuSegmentRomStart, _menuSegmentRomEnd, level_main_menu_entry_1),
};