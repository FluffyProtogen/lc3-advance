#include "file_chooser.h"
#include <stdint.h>
#include <string.h>

#include "../assets/font.h"
#include "../assets/text_editor_sprites.h"
#include "game.h"
#include "gba.h"
#include "text_editor.h"

typedef struct {
    int cursor_pos;
} FileChooserState;

FileChooserState fc_state;

void file_chooser_update(void) {
    if (KEY_PRESSED(KEY_UP) && fc_state.cursor_pos > 0)
        fc_state.cursor_pos--;

    if (KEY_PRESSED(KEY_DOWN) && fc_state.cursor_pos < 1)
        fc_state.cursor_pos++;

    if (KEY_PRESSED(KEY_A)) {
        FLASH_MEM[0x5555] = 0xAA;
        FLASH_MEM[0x2AAA] = 0x55;
        FLASH_MEM[0x5555] = 0xB0;              // enable bank switch
        FLASH_MEM[0x0] = fc_state.cursor_pos;  // sets save file

        text_editor_init(true);
        return;
    }

    if (fc_state.cursor_pos == 0)
        obj_set_pos(&obj_buffer[0], 2 + 8 * 11, 8 * 9);
    else
        obj_set_pos(&obj_buffer[0], 2 + 8 * 11, 8 * 11);

    oam_copy(oam_mem, obj_buffer, 1);
}

void file_chooser_init(void) {
    wait_for_vblank();
    fc_state = (FileChooserState){0};
    update_fn = file_chooser_update;
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32X32;
    oam_init(obj_buffer, 128);

    memcpy(pal_bg_mem, font_palette, FONT_PALETTE_SIZE);
    memcpy(tile_mem[0], font_tiles, FONT_TILES_SIZE);
    memcpy(se_mem[30], font, FONT_MAP_SIZE);

    memcpy(tile_mem[4], text_editor_sprites, TEXT_EDITOR_SPRITES_SIZE);
    memcpy(pal_obj_mem, text_editor_sprites_palette, TEXT_EDITOR_SPRITES_PALETTE_SIZE);

    obj_set_attr(&obj_buffer[0], KEYBOARDCURSOR_SPRITE_SHAPE, ATTR1_SIZE_32X32,
                 KEYBOARDCURSOR_PALETTE_ID | KEYBOARDCURSOR_ID);
    obj_set_pos(&obj_buffer[0], 2 + 8 * 11, 8 * 9);

    pal_bg_mem[2] = 0x2D6B;
    REG_BG0VOFS = 0;
    REG_BG0HOFS = 0;

    char *text[] = {"CHOOSE FILE", "File 1", "File 2"};
    for (int i = 0; i < (int)(sizeof(text) / sizeof(*text)); i++) {
        int offset = (30 - strlen(text[i])) / 2;
        for (int j = 0; text[i][j]; j++)
            se_mem[30][32 * (7 + i * 2) + offset + j] = font_index(text[i][j]);
    }
}
