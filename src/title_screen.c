#include <string.h>

#include "../assets/title_screen_background.h"
#include "../assets/title_screen_sprites.h"
#include "file_chooser.h"
#include "game.h"
#include "gba.h"

typedef struct {
    int animation_steps;
    float speed;
    float f_pos;
    int steps;
    float scaling;
} TitleScreenState;

TitleScreenState ts_state;

void title_screen_update(void) {
    int title_x = 68;
    if (ts_state.steps < 6) {
        ts_state.speed += 0.2;
        ts_state.f_pos += ts_state.speed;
        int title_y = ts_state.f_pos;
        if (title_y > 72) {
            ts_state.steps++;
            ts_state.scaling *= 0.7;
            ts_state.speed = -6.0 * ts_state.scaling;
        }
        obj_set_pos(&obj_buffer[0], title_x, title_y);
        obj_set_pos(&obj_buffer[1], title_x + 64, title_y);
    }

    REG_BG0VOFS = (frame_count) / 3;
    REG_BG0HOFS = (frame_count) / 3;

    if (ts_state.animation_steps > 150) {
        int state = ts_state.animation_steps % 60 < 30 ? ATTR0_HIDE : ATTR0_WIDE;
        obj_set_attr(&obj_buffer[2], state, ATTR1_SIZE_64X32, PRESS_START_PALETTE_ID | PRESS_START_ID);
        obj_set_pos(&obj_buffer[2], 98, 95);
    }

    if (KEY_PRESSED(KEY_START)) {
        file_chooser_init();
        return;
    }

    oam_copy(oam_mem, obj_buffer, 3);
    ts_state.animation_steps++;
}

void title_screen_init(void) {
    ts_state = (TitleScreenState){.scaling = 1.0};
    wait_for_vblank();
    update_fn = title_screen_update;
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32X32;
    oam_init(obj_buffer, 128);

    memcpy(tile_mem[4], title_screen_sprites, TITLE_SCREEN_SPRITES_SIZE);
    memcpy(pal_obj_mem, title_screen_sprites_palette, TITLE_SCREEN_SPRITES_PALETTE_SIZE);
    obj_set_attr(&obj_buffer[0], ATTR0_WIDE, ATTR1_SIZE_64X32, LC3ADVANCESPLIT1_PALETTE_ID | LC3ADVANCESPLIT1_ID);
    obj_set_attr(&obj_buffer[1], ATTR0_WIDE, ATTR1_SIZE_64X32, LC3ADVANCESPLIT2_PALETTE_ID | LC3ADVANCESPLIT2_ID);

    memcpy(pal_bg_mem, title_screen_background_palette, TITLE_SCREEN_BACKGROUND_PALETTE_SIZE);
    memcpy(tile_mem[0], title_screen_background_tiles, TITLE_SCREEN_BACKGROUND_TILES_SIZE);
    memcpy(se_mem[30], title_screen_background, TITLE_SCREEN_BACKGROUND_MAP_SIZE);

    pal_obj_mem[1] = 0x7FFF;
    pal_obj_mem[2] = 0x7FFF;
    pal_obj_mem[3] = 0x7FFF;
}
