#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "assets/font.h"
#include "assets/text_editor_sprites.h"
#include "game.h"
#include "gba.h"
#include "text_editor_menu.h"

void text_editor_menu_init(void) {
    wait_for_vblank();  // memset takes a while, so wait a frame after it finishes to prevent flickering
    game_state = GS_TEXT_EDITOR_MENU;
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

    obj_set_pos(&obj_buffer[1], 2, 120);
}

void text_editor_menu_update(void) {
}
