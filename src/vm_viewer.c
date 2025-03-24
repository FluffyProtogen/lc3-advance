#include <stdint.h>
#include <string.h>

#include "../assets/font.h"
#include "../assets/text_editor_sprites.h"
#include "game.h"
#include "gba.h"
#include "text_editor.h"
#include "vm.h"
#include "vm_viewer.h"

typedef struct {
    int keyboard_x;
    int keyboard_y;
    KeyboardMode keyboard_mode;
    char text[10][20];
    int text_x;
    int text_y;
    bool running;
    VirtualMachine vm;
} VmViewerState;

__attribute__((section(".ewram"))) VmViewerState vv_state;

void vm_put_char(char c) {
    if (c != '\n')
        vv_state.text[vv_state.text_y][vv_state.text_x] = c;
    if (vv_state.text_x == 19 || c == '\n') {
        if (vv_state.text_y == 9) {
            for (int i = 0; i < 9; i++)
                memcpy(&vv_state.text[i], &vv_state.text[i + 1], 20);
            memset(&vv_state.text[9], 0, 20);
            vv_state.text_x = 0;
        } else {
            vv_state.text_x = 0;
            vv_state.text_y++;
        }
    } else
        vv_state.text_x++;
}

static void draw_text(void) {
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 20; j++)
            se_mem[30][(i + 1) * 32 + j + 1] = font_index(vv_state.text[i][j]);
}

static void draw_keyboard(void) {
    int row_start = 15;

    for (int t = 0; t < 3; t++)
        for (int i = 0; KEY_TEXT[vv_state.keyboard_mode][t][i]; i++)
            se_mem[30][(row_start + (t * 2)) * 32 + i * 2 + 1] = font_index(KEY_TEXT[vv_state.keyboard_mode][t][i]);

    uint16_t id =
        (vv_state.keyboard_mode == UPPERCASE ? LOWER_ID : (vv_state.keyboard_mode == LOWERCASE ? SYMBOL_ID : UPPER_ID));
    obj_set_attr(&obj_buffer[0], LOWER_SPRITE_SHAPE, ATTR1_SIZE_32X32, LOWER_PALETTE_ID | id);
    obj_set_pos(&obj_buffer[0], 117, 142);
}

static void handle_type(void) {
    if (vv_state.keyboard_x == 6 && vv_state.keyboard_y == 2) {
        vv_state.keyboard_mode = (vv_state.keyboard_mode + 1) % 3;
        draw_keyboard();
        return;
    }
}

void vm_viewer_update(void) {
    if (vv_state.running)
        vv_state.running = vm_exec_next_instruction(&vv_state.vm);
    draw_text();
    if (l_key(KEY_RIGHT) && vv_state.keyboard_x < 13) {
        if (vv_state.keyboard_mode == SYMBOL) {
            if (vv_state.keyboard_y == 2 && vv_state.keyboard_x == 5)
                vv_state.keyboard_x = 6;
            else if (vv_state.keyboard_y == 2) {
                if (vv_state.keyboard_x < 6)
                    vv_state.keyboard_x++;
            } else
                vv_state.keyboard_x++;
        } else if (vv_state.keyboard_x == 6 && vv_state.keyboard_y == 2)
            vv_state.keyboard_x = 10;
        else if (vv_state.keyboard_x == 9 && vv_state.keyboard_y < 2)
            vv_state.keyboard_x = 11;
        else
            vv_state.keyboard_x++;
    }

    if (l_key(KEY_LEFT) && vv_state.keyboard_x > 0) {
        if (vv_state.keyboard_mode == SYMBOL) {
            if (vv_state.keyboard_y == 2 && vv_state.keyboard_x == 6)
                vv_state.keyboard_x = 5;
            else
                vv_state.keyboard_x--;
        } else {
            if (vv_state.keyboard_x == 10 && vv_state.keyboard_y == 2)
                vv_state.keyboard_x = 6;
            else if (vv_state.keyboard_x == 11 && vv_state.keyboard_y < 2)
                vv_state.keyboard_x -= 2;
            else
                vv_state.keyboard_x--;
        }
    }

    if (l_key(KEY_UP) && vv_state.keyboard_y > 0) {
        if (vv_state.keyboard_x == 10 && vv_state.keyboard_y == 2) {
            vv_state.keyboard_x = 9;
            vv_state.keyboard_y = 1;
        } else
            vv_state.keyboard_y--;
    }

    if (l_key(KEY_DOWN) && vv_state.keyboard_y < 2) {
        if (vv_state.keyboard_mode == SYMBOL) {
            if (vv_state.keyboard_y == 1 && vv_state.keyboard_x > 5) {
                vv_state.keyboard_x = 6;
                vv_state.keyboard_y = 2;
            } else
                vv_state.keyboard_y++;
        } else {
            if (vv_state.keyboard_x > 6 && vv_state.keyboard_x < 9 && vv_state.keyboard_y == 1) {
                vv_state.keyboard_x = 6;
                vv_state.keyboard_y = 2;
            } else if (vv_state.keyboard_x > 6 && vv_state.keyboard_x == 9 && vv_state.keyboard_y == 1) {
                vv_state.keyboard_x = 10;
                vv_state.keyboard_y = 2;
            } else
                vv_state.keyboard_y++;
        }
    }

    if (l_key(KEY_A))
        handle_type();

    if (vv_state.keyboard_x == 6 && vv_state.keyboard_y == 2)
        obj_set_pos(&obj_buffer[1], 110, 152);
    else
        obj_set_pos(&obj_buffer[1], 2 + 16 * vv_state.keyboard_x, 120 + 16 * vv_state.keyboard_y);

    oam_copy(oam_mem, obj_buffer, 2);
}

void vm_viewer_init(void) {
    vm_load(&vv_state.vm);
    memset(&vv_state, 0, sizeof(VmViewerState) - sizeof(VirtualMachine));
    vv_state.running = true;
    wait_for_vblank();
    update_fn = vm_viewer_update;
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32X32;
    oam_init(obj_buffer, 128);

    memcpy(pal_bg_mem, font_palette, FONT_PALETTE_SIZE);
    memcpy(tile_mem[0], font_tiles, FONT_TILES_SIZE);
    memcpy(se_mem[30], font, FONT_MAP_SIZE);

    memcpy(tile_mem[4], text_editor_sprites, TEXT_EDITOR_SPRITES_SIZE);
    memcpy(pal_obj_mem, text_editor_sprites_palette, TEXT_EDITOR_SPRITES_PALETTE_SIZE);

    obj_set_attr(&obj_buffer[0], LOWER_SPRITE_SHAPE, ATTR1_SIZE_32X32, LOWER_PALETTE_ID | LOWER_ID);
    obj_set_pos(&obj_buffer[0], 117, 142);

    obj_set_attr(&obj_buffer[1], KEYBOARDCURSOR_SPRITE_SHAPE, ATTR1_SIZE_32X32,
                 KEYBOARDCURSOR_PALETTE_ID | KEYBOARDCURSOR_ID);
    obj_set_pos(&obj_buffer[1], 2, 120);

    pal_bg_mem[2] = 0x2D6B;
    REG_BG0VOFS = 0;
    REG_BG0HOFS = 0;
    draw_keyboard();
}
