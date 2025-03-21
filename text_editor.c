#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "assets/font.h"
#include "assets/text_editor_sprites.h"
#include "game.h"
#include "gba.h"
#include "text_editor.h"
#include "text_editor_menu.h"

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

void text_editor_init(bool clear) {
    if (clear) {
        memset(&te_state, 0, sizeof(TextEditorState));

        for (int j = 0; j < 999; j++)
            for (int i = 0; i < 27; i++)
                te_state.text[j][i] = FLASH_MEM[j * 27 + i];
    }

    te_state.text_editor_mode = TE_KEYBOARD;
    te_state.keyboard_mode = UPPERCASE;
    wait_for_vblank();
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

    obj_set_attr(&obj_buffer[3], LOWER_SPRITE_SHAPE, ATTR1_SIZE_32X32, LOWER_PALETTE_ID | KEYBOARDENABLED_ID);
    obj_set_pos(&obj_buffer[3], 8 * 29, 0);

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

    char c = KEY_TEXT[te_state.keyboard_mode][te_state.keyboard_y][te_state.keyboard_x];
    if (c != '\n') {
        for (int i = 25; i >= te_state.cursor_x; i--)
            te_state.text[cur_line][i + 1] = te_state.text[cur_line][i];
        c = c == '\x1F' ? ' ' : c;
        te_state.text[cur_line][te_state.cursor_x++] = c;
        draw_text();
        return;
    }

    if (cur_line == 999)
        return;

    if (te_state.text[998][0] != 0)
        return;

    for (int i = 999 - 2; i >= cur_line; i--)
        memcpy(te_state.text[i + 1], te_state.text[i], sizeof(*te_state.text));
    memset(te_state.text[cur_line + 1], 0, sizeof(*te_state.text));
    memcpy(te_state.text[cur_line + 1], &te_state.text[cur_line][te_state.cursor_x],
           sizeof(*te_state.text) - te_state.cursor_x);
    memset(&te_state.text[cur_line][te_state.cursor_x], 0, sizeof(*te_state.text) - te_state.cursor_x);
    te_state.cursor_x = 0;
    if (te_state.cursor_y == 13)
        te_state.top_visible_line++;
    else
        te_state.cursor_y++;
    draw_text();
}

bool key(int key) {
    static int last_frame = 0;
    if (KEY_PRESSED(key)) {
        last_frame = frame_count;
        return true;
    }
    if ((cur_keys & KEY_L) && (cur_keys & key) && frame_count - last_frame > 6) {
        last_frame = frame_count;
        return true;
    }
    return false;
}

void handle_delete(void) {
    int cur_line = te_state.top_visible_line + te_state.cursor_y;
    if (te_state.cursor_x > 0) {
        for (int i = te_state.cursor_x - 1; i < 27; i++)
            te_state.text[cur_line][i] = te_state.text[cur_line][i + 1];
        te_state.cursor_x--;
        draw_text();
        return;
    }

    if (cur_line == 0)
        return;

    int prev_len = strlen(te_state.text[cur_line - 1]);
    int cur_len = strlen(te_state.text[cur_line]);
    if (prev_len + cur_len > 27)
        return;

    for (int i = 0; i < cur_len; i++)
        te_state.text[cur_line - 1][prev_len + i] = te_state.text[cur_line][i];

    for (int i = cur_line; i < 999 - 1; i++)
        memcpy(te_state.text[i], te_state.text[i + 1], sizeof(*te_state.text));
    if (te_state.cursor_y == 0)
        te_state.top_visible_line--;
    else
        te_state.cursor_y--;
    te_state.cursor_x = prev_len;
    draw_text();
}

void text_editor_update(void) {
    if (te_state.text_editor_mode == TE_KEYBOARD) {
        if (key(KEY_RIGHT) && te_state.keyboard_x < 13) {
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

        if (key(KEY_LEFT) && te_state.keyboard_x > 0) {
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

        if (key(KEY_UP) && te_state.keyboard_y > 0) {
            if (te_state.keyboard_x == 10 && te_state.keyboard_y == 2) {
                te_state.keyboard_x = 9;
                te_state.keyboard_y = 1;
            } else
                te_state.keyboard_y--;
        }

        if (key(KEY_DOWN) && te_state.keyboard_y < 2) {
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

    } else {
#define CUR_LINE te_state.top_visible_line + te_state.cursor_y
        if (key(KEY_RIGHT) && te_state.cursor_x < (int)strlen(te_state.text[CUR_LINE]))
            te_state.cursor_x++;
        if (key(KEY_LEFT) && te_state.cursor_x > 0)
            te_state.cursor_x--;
        if (key(KEY_UP)) {
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
        if (key(KEY_DOWN)) {
            if (te_state.cursor_y == 13) {
                if (te_state.top_visible_line < 999 - 14) {
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

    if (key(KEY_A))
        handle_type();

    if (key(KEY_B))
        handle_delete();

    if (KEY_PRESSED(KEY_R)) {
        te_state.text_editor_mode = (te_state.text_editor_mode + 1) % 2;
        int id = (te_state.text_editor_mode == TE_KEYBOARD ? KEYBOARDENABLED_ID : CURSORENABLED_ID);
        obj_set_attr(&obj_buffer[3], LOWER_SPRITE_SHAPE, ATTR1_SIZE_32X32, LOWER_PALETTE_ID | id);
        obj_set_pos(&obj_buffer[3], 8 * 29, 0);
    }

    if (KEY_PRESSED(KEY_START)) {
        text_editor_menu_init();
        return;
    }

    if (te_state.keyboard_x == 6 && te_state.keyboard_y == 2)
        obj_set_pos(&obj_buffer[1], 110, 152);
    else
        obj_set_pos(&obj_buffer[1], 2 + 16 * te_state.keyboard_x, 120 + 16 * te_state.keyboard_y);

    obj_set_pos(&obj_buffer[2], te_state.cursor_x == 27 ? 239 : (24 + 8 * te_state.cursor_x), 8 * te_state.cursor_y);
    oam_copy(oam_mem, obj_buffer, 4);
}
