#pragma once

typedef enum {
    GS_TITLE_SCREEN,
    GS_TEXT_EDITOR,
    GS_TEXT_EDITOR_MENU,
    GS_OBJ_VIEWER,
} GameState;

extern GameState game_state;
