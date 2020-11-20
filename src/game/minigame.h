#ifndef MINIGAME_H
#define MINIGAME_H

// #define JABO

#define JABO_MOMENT 16
#define JABO_MOMENT_HITBOX -10

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
    MODE_SINESCATTER,
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