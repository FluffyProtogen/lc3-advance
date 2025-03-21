#include <stdint.h>

#include "game.h"
#include "gba.h"
#include "obj_viewer.h"
#include "text_editor.h"
#include "text_editor_menu.h"
#include "title_screen.h"

int main(void) {
    title_screen_init();

    for (;;) {
        wait_for_vblank();
        prev_keys = cur_keys;
        cur_keys = (~REG_KEYINPUT) & 0x3FF;
        switch (game_state) {
            case GS_TITLE_SCREEN:
                title_screen_update();
                break;
            case GS_TEXT_EDITOR:
                text_editor_update();
                break;
            case GS_TEXT_EDITOR_MENU:
                text_editor_menu_update();
                break;
            case GS_OBJ_VIEWER:
                obj_viewer_update();
                break;
        }
    }
}
