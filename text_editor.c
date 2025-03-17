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

typedef struct {
    char text[500][27];
    int top_visible_line;
} TextEditorState;

TextEditorState te_state;

int font_index(char c) {
    // clang-format off
    switch (c) {
        case 'I': return 1;
        case 'L': return 2;
        case 'S': return 3;
        case '0': return 4;
        case 'A': return 5;
        case 'O': return 6;
        case '9': return 7;
        case '8': return 8;
        case 'G': return 9;
        case '@': return 10;
        case '3': return 11;
        case '2': return 12;
        case '?': return 13;
        case '[': return 14;
        case 'k': return 15;
        case ']': return 16;
        case '&': return 17;
        case 't': return 18;
        case '}': return 19;
        case ')': return 20;
        case 'C': return 21;
        case '4': return 22;
        case 'f': return 23;
        case '6': return 24;
        case '$': return 25;
        case '^': return 26;
        case '1': return 27;
        case '7': return 28;
        case 'Z': return 29;
        case 'M': return 30;
        case 'N': return 31;
        case 'K': return 32;
        case 'W': return 33;
        case 'X': return 34;
        case 'Y': return 35;
        case '\\': return 36;
        case '5': return 37;
        case 'E': return 38;
        case 'F': return 39;
        case 'T': return 40;
        case '\'': return 41;
        case 'R': return 42;
        case 'B': return 43;
        case 'P': return 44;
        case 'Q': return 45;
        case 'D': return 46;
        case '#': return 47;
        case '"': return 48;
        case 'H': return 49;
        case 'U': return 50;
        case 'V': return 51;
        case 'b': return 52;
        case 'h': return 53;
        case 'y': return 54;
        case '<': return 57;
        case 'm': return 58;
        case 'w': return 59;
        case 'x': return 60;
        case 'z': return 61;
        case '=': return 62;
        case 'p': return 63;
        case 'n': return 64;
        case 'r': return 65;
        case 'u': return 66;
        case 'v': return 68;
        case 'g': return 69;
        case 'q': return 70;
        case 'e': return 71;
        case 'o': return 72;
        case 'c': return 73;
        case 's': return 74;
        case 'a': return 75;
        case '-': return 76;
        case ',': return 77;
        case '.': return 78;
        case '_': return 79;
        case '%': return 80;
        case '|': return 81;
        case 'l': return 82;
        case '!': return 83;
        case '`': return 84;
        case 'i': return 85;
        case '(': return 86;
        case '{': return 87;
        case 'j': return 88;
        case 'd': return 89;
        case 'J': return 90;
        case '/': return 91;
        case '~': return 92;
        case '*': return 93;
        case '>': return 94;
        case '+': return 102;
        case ';': return 103;
        case ':': return 104;
        default: return 0;
    }
    // clang-format on
}

int line_num_font_index(int num) {
    // clang-format off
    switch (num) {
        case 0: return 97; 
        case 1: return 56; 
        case 2: return 101; 
        case 3: return 100; 
        case 4: return 67; 
        case 5: return 95; 
        case 6: return 55; 
        case 7: return 96; 
        case 8: return 99; 
        case 9: return 98; 
        default: return 0;
    }
    // clang-format on
}

void draw_text(void) {
    for (int i = 0; i < 14; i++) {
        int line = i + te_state.top_visible_line;
        if (line >= 500)
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
    char *text[] = {
        "ABCDEFGHIJ 123",
        "KLMNOPQRST 456",
        "UVWXYZ    0789",
    };
    for (int t = 0; t < 3; t++) {
        int i = 0;
        for (; text[t][i]; i++)
            se_mem[30][(row_start + (t * 2)) * 32 + i * 2 + 1] = font_index(text[t][i]);
    }
}

void text_editor_init(void) {
    memset(&te_state, 0, sizeof(TextEditorState));
    wait_for_vblank();  // memset takes a while, so wait a frame after it finishes to prevent flickering
    game_state = GS_TEXT_EDITOR;
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32X32;
    oam_init(obj_buffer, 128);

    memcpy(pal_bg_mem, font_palette, FONT_PALETTE_SIZE);
    memcpy(tile_mem[0], font_tiles, FONT_TILES_SIZE);
    memcpy(se_mem[30], font, FONT_MAP_SIZE);

    memcpy(tile_mem[4], text_editor_sprites, TEXT_EDITOR_SPRITES_SIZE);
    memcpy(pal_obj_mem, text_editor_sprites_palette, TEXT_EDITOR_SPRITES_PALETTE_SIZE);

    REG_BG0VOFS = 0;
    REG_BG0HOFS = 0;

    pal_bg_mem[2] = 0x2D6B;

    obj_set_attr(&obj_buffer[0], LOWER_SPRITE_SHAPE, ATTR1_SIZE_32X32, LOWER_PALETTE_ID | LOWER_ID);
    obj_set_pos(&obj_buffer[0], 117, 142);

    te_state = (TextEditorState){.text = {
                                     "hi",
                                     "test",
                                 }};
    draw_text();
    draw_keyboard();
}

void text_editor_update(void) {
    if ((cur_keys & KEY_DOWN) && !(prev_keys & KEY_DOWN)) {
        te_state.top_visible_line++;
        draw_text();
    }

    if ((cur_keys & KEY_UP) && !(prev_keys & KEY_UP) && te_state.top_visible_line > 0) {
        te_state.top_visible_line--;
        draw_text();
    }
}
