#ifndef MINIGAME_H
#define MINIGAME_H

enum charValues {
    MARIO,
    LUIGI,
    YOSHI,
    WARIO,
};


enum char_place_modes {
    MODE_UNIFORM,
    MODE_SCATTERED,
    MODE_SCROLLUNIFORM,
    MODE_SCROLLSCATTER,
};

enum modeSelect {
    MODE_PICKING,
    MODE_SLEUTH,
    MODE_SCORING,
    MODE_PREGAMEOVER,
    MODE_GAMEOVER,
};

enum clickModes {
    MODE_NOTCLICK,
    MODE_CLICK,
};

#define SCORE_TIME 90

#endif