#pragma once

#include <stdbool.h>

typedef enum {
    UPPERCASE,
    LOWERCASE,
    SYMBOL,
} KeyboardMode;

typedef enum {
    TE_KEYBOARD,
    TE_TEXT,
} TextEditorMode;

typedef struct {
    char text[999][28];
    int top_visible_line;
    int keyboard_x;
    int keyboard_y;
    int cursor_x;
    int cursor_y;
    KeyboardMode keyboard_mode;
    TextEditorMode text_editor_mode;
} TextEditorState;

extern TextEditorState te_state;

void text_editor_init(bool clear);

bool l_key(int key);
