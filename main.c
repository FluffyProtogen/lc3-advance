#include <stdint.h>

#include "game.h"
#include "gba.h"
#include "title_screen.h"

int main(void) {
    GameState game_state = GS_TITLE_SCREEN;
    title_screen_init();

    for (;;) {
        wait_for_vblank();
        switch (game_state) {
            case GS_TITLE_SCREEN:
                title_screen_update();
                break;
        }
    }

    return 0;
}
