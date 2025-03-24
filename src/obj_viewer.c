#include <stdint.h>
#include <string.h>

#include "../assets/font.h"
#include "../assets/text_editor_sprites.h"
#include "assembler/object.h"
#include "game.h"
#include "gba.h"
#include "obj_viewer.h"
#include "text_editor.h"

typedef struct {
    int top_visible_line;
} ObjViewerState;

ObjViewerState ov_state;

void draw_obj(void) {
    for (int i = 0; i < 20; i++) {
        int line = i + ov_state.top_visible_line;
        if (line >= 3000)
            break;
        char *text = obj_buf[line];
        int j = 0;
        for (; j < 4 && text[j]; j++)
            se_mem[30][i * 32 + j + 4] = font_index(text[j]);
        for (; j < 26; j++)
            se_mem[30][i * 32 + j + 4] = font_index(0);

        memset(&se_mem[30][i * 32], font_index(' '), 4);
        int new_line = line + 1;
        if (new_line >= 1000) {
            se_mem[30][i * 32] = line_num_font_index(new_line / 1000);
            se_mem[30][i * 32 + 1] = line_num_font_index((new_line % 1000) / 100);
            se_mem[30][i * 32 + 2] = line_num_font_index((new_line % 100) / 10);
            se_mem[30][i * 32 + 3] = line_num_font_index(new_line % 10);
        } else if (new_line >= 100) {
            se_mem[30][i * 32 + 1] = line_num_font_index(new_line / 100);
            se_mem[30][i * 32 + 2] = line_num_font_index((new_line % 100) / 10);
            se_mem[30][i * 32 + 3] = line_num_font_index(new_line % 10);
        } else if (new_line >= 10) {
            se_mem[30][i * 32 + 2] = line_num_font_index(new_line / 10);
            se_mem[30][i * 32 + 3] = line_num_font_index(new_line % 10);
        } else {
            se_mem[30][i * 32 + 2] = font_index(' ');
            se_mem[30][i * 32 + 3] = line_num_font_index(new_line);
        }
    }
}

bool key_pressed_or_held(int key) {
    static int last_frame = 0;
    if (KEY_PRESSED(key) || ((cur_keys & key) && frame_count - last_frame > 2)) {
        last_frame = frame_count;
        return true;
    }
    return false;
}

void obj_viewer_update(void) {
    if (key_pressed_or_held(KEY_UP) && ov_state.top_visible_line > 0) {
        ov_state.top_visible_line--;
        draw_obj();
    }

    if (key_pressed_or_held(KEY_DOWN) && ov_state.top_visible_line < 3000 - 20) {
        ov_state.top_visible_line++;
        draw_obj();
    }

    if (KEY_PRESSED(KEY_B) || KEY_PRESSED(KEY_A)) {
        text_editor_init(false);
        return;
    }
}

void obj_viewer_init(void) {
    ov_state = (ObjViewerState){0};
    update_fn = obj_viewer_update;
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32X32;
    oam_init(obj_buffer, 128);

    memcpy(pal_bg_mem, font_palette, FONT_PALETTE_SIZE);
    memcpy(tile_mem[0], font_tiles, FONT_TILES_SIZE);
    memcpy(se_mem[30], font, FONT_MAP_SIZE);

    memcpy(tile_mem[4], text_editor_sprites, TEXT_EDITOR_SPRITES_SIZE);
    memcpy(pal_obj_mem, text_editor_sprites_palette, TEXT_EDITOR_SPRITES_PALETTE_SIZE);

    pal_bg_mem[2] = 0x2D6B;
    REG_BG0VOFS = 0;
    REG_BG0HOFS = 0;
    draw_obj();
}
