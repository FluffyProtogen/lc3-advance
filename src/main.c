#include <stdint.h>

#include "game.h"
#include "gba.h"
#include "title_screen.h"

int main(void) {
    title_screen_init();

    for (;;) {
        wait_for_vblank();
        prev_keys = cur_keys;
        cur_keys = (~REG_KEYINPUT) & 0x3FF;
        update_fn();
    }
}
