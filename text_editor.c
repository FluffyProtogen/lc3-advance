#include <stdint.h>
#include <string.h>

#include "assets/font.h"
#include "assets/text_editor_sprites.h"
#include "game.h"
#include "gba.h"
#include "text_editor.h"

typedef enum {
    UPPERCASE,
    LOWERCASE,
    SYMBOL,
} KeyboardMode;

typedef enum {
    KEYBOARD,
    TEXT,
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

TextEditorState te_state;

const char *KEY_TEXT[3][3] = {
    {
        "ABCDEFGHIJ 123",
        "KLMNOPQRST 456",
        "UVWXYZ    0789",
    },
    {
        "abcdefghij 123",
        "klmnopqrst 456",
        "uvwxyz    0789",
    },
    {
        "!\"#$%&'()*+,-.",
        "/:;<=>?@[\\]^_`",
        "{|}~\n\x1F          ",
    },
};

void draw_text(void) {
    for (int i = 0; i < 14; i++) {
        int line = i + te_state.top_visible_line;
        if (line >= 999)
            break;
        char *text = te_state.text[line];
        int j = 0;
        for (; text[j]; j++)
            se_mem[30][i * 32 + j + 3] = font_index(text[j]);
        for (; j < 27; j++)
            se_mem[30][i * 32 + j + 3] = font_index(0);

        int new_line = line + 1;
        if (new_line >= 100) {
            se_mem[30][i * 32] = line_num_font_index(new_line / 100);
            se_mem[30][i * 32 + 1] = line_num_font_index((new_line % 100) / 10);
            se_mem[30][i * 32 + 2] = line_num_font_index(new_line % 10);
        } else if (new_line >= 10) {
            se_mem[30][i * 32] = font_index(0);
            se_mem[30][i * 32 + 1] = line_num_font_index(new_line / 10);
            se_mem[30][i * 32 + 2] = line_num_font_index(new_line % 10);
        } else {
            se_mem[30][i * 32] = font_index(0);
            se_mem[30][i * 32 + 1] = font_index(0);
            se_mem[30][i * 32 + 2] = line_num_font_index(new_line);
        }
    }
}

void draw_keyboard(void) {
    int row_start = 15;

    for (int t = 0; t < 3; t++) {
        for (int i = 0; KEY_TEXT[te_state.keyboard_mode][t][i]; i++)
            se_mem[30][(row_start + (t * 2)) * 32 + i * 2 + 1] = font_index(KEY_TEXT[te_state.keyboard_mode][t][i]);
    }

    uint16_t id =
        (te_state.keyboard_mode == UPPERCASE ? LOWER_ID : (te_state.keyboard_mode == LOWERCASE ? SYMBOL_ID : UPPER_ID));
    obj_set_attr(&obj_buffer[0], LOWER_SPRITE_SHAPE, ATTR1_SIZE_32X32, LOWER_PALETTE_ID | id);
    obj_set_pos(&obj_buffer[0], 117, 142);
}

void text_editor_init(void) {
    memset(&te_state, 0, sizeof(TextEditorState));
    te_state.text_editor_mode = KEYBOARD;
    te_state.keyboard_mode = UPPERCASE;
    wait_for_vblank();  // memset takes a while, so wait a frame after it finishes to prevent flickering
    game_state = GS_TEXT_EDITOR;
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32X32;
    oam_init(obj_buffer, 128);

    memcpy(pal_bg_mem, font_palette, FONT_PALETTE_SIZE);
    memcpy(tile_mem[0], font_tiles, FONT_TILES_SIZE);
    memcpy(se_mem[30], font, FONT_MAP_SIZE);

    pal_bg_mem[2] = 0x2D6B;
    memcpy(tile_mem[4], text_editor_sprites, TEXT_EDITOR_SPRITES_SIZE);
    memcpy(pal_obj_mem, text_editor_sprites_palette, TEXT_EDITOR_SPRITES_PALETTE_SIZE);

    REG_BG0VOFS = 0;
    REG_BG0HOFS = 0;

    obj_set_attr(&obj_buffer[0], LOWER_SPRITE_SHAPE, ATTR1_SIZE_32X32, LOWER_PALETTE_ID | LOWER_ID);
    obj_set_pos(&obj_buffer[0], 117, 142);

    obj_set_attr(&obj_buffer[1], KEYBOARDCURSOR_SPRITE_SHAPE, ATTR1_SIZE_32X32,
                 KEYBOARDCURSOR_PALETTE_ID | KEYBOARDCURSOR_ID);

    obj_set_attr(&obj_buffer[2], LOWER_SPRITE_SHAPE, ATTR1_SIZE_32X32, LOWER_PALETTE_ID | TEXTCURSOR_ID);
    obj_set_pos(&obj_buffer[2], 8 * 3, 0);

    draw_text();
    draw_keyboard();
    obj_set_pos(&obj_buffer[1], 2, 120);
}

void handle_type(void) {
    if (te_state.keyboard_x == 6 && te_state.keyboard_y == 2) {
        te_state.keyboard_mode = (te_state.keyboard_mode + 1) % 3;
        draw_keyboard();
        return;
    }

    int cur_line = te_state.top_visible_line + te_state.cursor_y;
    if (strlen(te_state.text[cur_line]) == 27)
        return;
    for (int i = 25; i >= te_state.cursor_x; i--)
        te_state.text[cur_line][i + 1] = te_state.text[cur_line][i];

    char c = KEY_TEXT[te_state.keyboard_mode][te_state.keyboard_y][te_state.keyboard_x];
    c = c == '\x1F' ? ' ' : c;
    te_state.text[cur_line][te_state.cursor_x++] = c;
    draw_text();
}

void text_editor_update(void) {
    if (te_state.text_editor_mode == KEYBOARD) {
        if (KEY_PRESSED(KEY_RIGHT) && te_state.keyboard_x < 13) {
            if (te_state.keyboard_mode == SYMBOL) {
                if (te_state.keyboard_y == 2 && te_state.keyboard_x == 5)
                    te_state.keyboard_x = 6;
                else if (te_state.keyboard_y == 2) {
                    if (te_state.keyboard_x < 6)
                        te_state.keyboard_x++;
                } else
                    te_state.keyboard_x++;
            } else if (te_state.keyboard_x == 6 && te_state.keyboard_y == 2)
                te_state.keyboard_x = 10;
            else if (te_state.keyboard_x == 9 && te_state.keyboard_y < 2)
                te_state.keyboard_x = 11;
            else
                te_state.keyboard_x++;
        }

        if (KEY_PRESSED(KEY_LEFT) && te_state.keyboard_x > 0) {
            if (te_state.keyboard_mode == SYMBOL) {
                if (te_state.keyboard_y == 2 && te_state.keyboard_x == 6)
                    te_state.keyboard_x = 5;
                else
                    te_state.keyboard_x--;
            } else {
                if (te_state.keyboard_x == 10 && te_state.keyboard_y == 2)
                    te_state.keyboard_x = 6;
                else if (te_state.keyboard_x == 11 && te_state.keyboard_y < 2)
                    te_state.keyboard_x -= 2;
                else
                    te_state.keyboard_x--;
            }
        }

        if (KEY_PRESSED(KEY_UP) && te_state.keyboard_y > 0) {
            if (te_state.keyboard_x == 10 && te_state.keyboard_y == 2) {
                te_state.keyboard_x = 9;
                te_state.keyboard_y = 1;
            } else
                te_state.keyboard_y--;
        }

        if (KEY_PRESSED(KEY_DOWN) && te_state.keyboard_y < 2) {
            if (te_state.keyboard_mode == SYMBOL) {
                if (te_state.keyboard_y == 1 && te_state.keyboard_x > 5) {
                    te_state.keyboard_x = 6;
                    te_state.keyboard_y = 2;
                } else
                    te_state.keyboard_y++;
            } else {
                if (te_state.keyboard_x > 6 && te_state.keyboard_x < 9 && te_state.keyboard_y == 1) {
                    te_state.keyboard_x = 6;
                    te_state.keyboard_y = 2;
                } else if (te_state.keyboard_x > 6 && te_state.keyboard_x == 9 && te_state.keyboard_y == 1) {
                    te_state.keyboard_x = 10;
                    te_state.keyboard_y = 2;
                } else
                    te_state.keyboard_y++;
            }
        }

        if (KEY_PRESSED(KEY_A))
            handle_type();
    } else {
#define CUR_LINE te_state.top_visible_line + te_state.cursor_y
        if (KEY_PRESSED(KEY_RIGHT) && te_state.cursor_x < (int)strlen(te_state.text[CUR_LINE]))
            te_state.cursor_x++;
        if (KEY_PRESSED(KEY_LEFT) && te_state.cursor_x > 0)
            te_state.cursor_x--;
        if (KEY_PRESSED(KEY_UP)) {
            if (te_state.cursor_y == 0) {
                if (te_state.top_visible_line > 0) {
                    te_state.top_visible_line--;
                    draw_text();
                }
            } else
                te_state.cursor_y--;
            int len = strlen(te_state.text[CUR_LINE]);
            if (len < te_state.cursor_x)
                te_state.cursor_x = len;
        }
        if (KEY_PRESSED(KEY_DOWN)) {
            if (te_state.cursor_y == 13) {
                if (te_state.top_visible_line < 999 - 13) {
                    te_state.top_visible_line++;
                    draw_text();
                }
            } else
                te_state.cursor_y++;
            int len = strlen(te_state.text[CUR_LINE]);
            if (len < te_state.cursor_x)
                te_state.cursor_x = len;
        }
    }

    if (KEY_PRESSED(KEY_B) && te_state.cursor_x > 0) {
        int cur_line = te_state.top_visible_line + te_state.cursor_y;
        for (int i = te_state.cursor_x - 1; i < 27; i++)
            te_state.text[cur_line][i] = te_state.text[cur_line][i + 1];
        te_state.cursor_x--;
        draw_text();
    }

    if (KEY_PRESSED(KEY_R))
        te_state.text_editor_mode = (te_state.text_editor_mode + 1) % 2;

    if (te_state.keyboard_x == 6 && te_state.keyboard_y == 2)
        obj_set_pos(&obj_buffer[1], 110, 152);
    else
        obj_set_pos(&obj_buffer[1], 2 + 16 * te_state.keyboard_x, 120 + 16 * te_state.keyboard_y);

    obj_set_pos(&obj_buffer[2], te_state.cursor_x == 27 ? 239 : (24 + 8 * te_state.cursor_x), 8 * te_state.cursor_y);
    oam_copy(oam_mem, obj_buffer, 3);
}
