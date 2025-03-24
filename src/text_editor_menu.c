#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../assets/font.h"
#include "../assets/text_editor_sprites.h"
#include "assembler/object.h"
#include "assembler/parser.h"
#include "assembler/symbol.h"
#include "assembler/token.h"
#include "game.h"
#include "gba.h"
#include "obj_viewer.h"
#include "text_editor.h"
#include "text_editor_menu.h"
#include "vm_viewer.h"

typedef struct {
    int cursor_pos;
    bool obj_menu_active;
} TextEditorMenuState;

TextEditorMenuState tm_state;

void write_line_num(int num, int row, int col) {
    if (num >= 100) {
        se_mem[30][row * 32 + col] = font_index(num / 100 + '0');
        se_mem[30][row * 32 + 1 + col] = font_index((num % 100) / 10 + '0');
        se_mem[30][row * 32 + 2 + col] = font_index(num % 10 + '0');
    } else if (num >= 10) {
        se_mem[30][row * 32 + col] = font_index(num / 10 + '0');
        se_mem[30][row * 32 + 1 + col] = font_index(num % 10 + '0');
    } else {
        se_mem[30][row * 32 + col] = font_index(num + '0');
    }
}

#define ERR_ROW 12
bool try_assemble() {
    bool success = false;
    LineTokensList token_list;
    size_t lines_read;
    LineTokenizerResult lt_result = tokenize_lines(&token_list, te_state.text, &lines_read);
    if (lt_result != LT_SUCCESS) {
        wait_for_vblank();
        char *text = "Tokenizing failed at line";
        for (int i = 0; text[i]; i++)
            se_mem[30][32 * ERR_ROW + i] = font_index(text[i]);
        write_line_num(lines_read, ERR_ROW, strlen(text) + 1);
        // clang-format off
        switch (lt_result) {
            case LT_INVALID_INTEGER: text = "Invalid integer"; break;
            case LT_INTEGER_TOO_LARGE: text = "Integer too large"; break;
            case LT_BAD_PSEUDOOP: text = "Bad pseudo-op"; break;
            default: break;
        }
        // clang-format on
        for (int i = 0; text[i]; i++)
            se_mem[30][32 * (ERR_ROW + 2) + i] = font_index(text[i]);
        goto free_tokens;
    }

    SymbolTable symbol_table;
    SymbolTableResult st_result = generate_symbol_table(&symbol_table, &token_list, &lines_read);
    if (st_result != ST_SUCCESS) {
        wait_for_vblank();
        char *text = "Sym Table failed at line";
        for (int i = 0; text[i]; i++)
            se_mem[30][32 * ERR_ROW + i] = font_index(text[i]);
        write_line_num(lines_read, ERR_ROW, strlen(text) + 1);
        // clang-format off
        switch (st_result) {
            case ST_ORIG_NO_START_ADDR: text = "Orig without start address"; break;
            case ST_NEGATIVE_ORIG: text = "Negative orig address"; break;
            case ST_NO_ORIG_NUMBER: text = "No orig address"; break;
            case ST_TOKEN_BEFORE_ORIG: text = "Token before orig"; break;
            case ST_OVERLAPPING_MEM: text = "Overlapping memory"; break;
            case ST_NO_BLKW_AMOUNT: text = "No blkw amount"; break;
            case ST_BAD_BLKW_AMOUNT: text = "Bad blkw amount"; break;
            case ST_BAD_STRINGZ: text = "Bad stringz"; break;
            case ST_BAD_STRING_ESCAPE: text = "Bad string escape"; break;
            case ST_ORIG_INSIDE_ORIG: text = "Orig inside orig"; break;
            case ST_NO_END: text = "Missing .end"; break;
            case ST_SYMBOL_ALREADY_EXISTS: text = "Symbol already exists"; break;
            case ST_NO_MORE_TOKENS: text = "No more tokens"; break;
            default: break;
        }
        // clang-format on
        for (int i = 0; text[i]; i++)
            se_mem[30][32 * (ERR_ROW + 2) + i] = font_index(text[i]);
        goto free_symbols;
    }

    Instructions instructions;
    ParserResult ps_result = parse_instructions(&instructions, &token_list, &symbol_table, &lines_read);
    if (ps_result != PS_SUCCESS) {
        wait_for_vblank();
        char *text = "Parser failed at line";
        for (int i = 0; text[i]; i++)
            se_mem[30][32 * ERR_ROW + i] = font_index(text[i]);
        write_line_num(lines_read, ERR_ROW, strlen(text) + 1);
        // clang-format off
        switch (ps_result) {
            case PS_TOKEN_BEFORE_ORIG: text = "Token before orig"; break;
            case PS_BAD_TOKEN: text = "Bad token"; break;
            case PS_NEGATIVE_ORIG: text = "Negative orig"; break;
            case PS_BAD_BLKW: text = "Bad blkw"; break;
            case PS_BAD_STRING_ESCAPE: text = "Bad string escape"; break;
            case PS_TRAILING_TOKENS: text = "Trailing tokens"; break;
            case PS_NUMBER_TOO_LARGE: text = "Number too large"; break;
            case PS_SYMBOL_NOT_PRESENT: text = "Symbol not present"; break;
            case PS_OVERFLOWING_ADDR: text = "Overflowing address"; break;
            case PS_NO_MORE_TOKENS: text = "Expected another token"; break;
            default: break;
        }
        // clang-format on
        for (int i = 0; text[i]; i++)
            se_mem[30][32 * (ERR_ROW + 2) + i] = font_index(text[i]);
        goto free_instructions;
    }

    success = write_to_object(&instructions);
    wait_for_vblank();

free_instructions:
    free(instructions.instructions);
free_symbols:
    free_symbol_table(&symbol_table);
free_tokens:
    free_tokens_list(&token_list);
    return success;
}

void text_editor_menu_update(void) {
    if (KEY_PRESSED(KEY_UP) && tm_state.cursor_pos > 0)
        tm_state.cursor_pos--;
    if (KEY_PRESSED(KEY_DOWN) && ((tm_state.obj_menu_active && tm_state.cursor_pos < 1) ||
                                  (!tm_state.obj_menu_active && tm_state.cursor_pos < 2)))
        tm_state.cursor_pos++;

    if (KEY_PRESSED(KEY_A) && !tm_state.obj_menu_active) {
        if (tm_state.cursor_pos == 0) {
            for (int j = 0; j < 999; j++)
                for (int i = 0; i < 27; i++)
                    FLASH_MEM[j * 27 + i] = te_state.text[j][i];

            for (int i = 0; i < 30; i++)
                se_mem[30][32 * ERR_ROW + i] = font_index(' ');
            for (int i = 0; i < 30; i++)
                se_mem[30][32 * (ERR_ROW + 2) + i] = font_index(' ');

            char *text = "saved";
            for (int i = 0; text[i]; i++)
                se_mem[30][32 * ERR_ROW + i + 12] = font_index(text[i]);
        } else if (tm_state.cursor_pos == 1) {
            if (try_assemble()) {
                memcpy(se_mem[30], font, FONT_MAP_SIZE);

                char *text[] = {"assembled successfully", "execute", "view obj file"};
                for (int i = 0; i < (int)(sizeof(text) / sizeof(*text)); i++) {
                    int offset = (30 - strlen(text[i])) / 2;
                    for (int j = 0; text[i][j]; j++)
                        se_mem[30][32 * (2 + i * 2 + (i == 0 ? 0 : 1)) + offset + j] = font_index(text[i][j]);
                }

                tm_state.cursor_pos = 0;
                tm_state.obj_menu_active = true;
            }
        } else {
            memset(&te_state.text, 0, sizeof(te_state.text));
            te_state.cursor_x = 0;
            te_state.cursor_y = 0;
            te_state.top_visible_line = 0;
            text_editor_init(false);
            return;
        }
    } else if (KEY_PRESSED(KEY_A) && tm_state.obj_menu_active) {
        if (tm_state.cursor_pos == 0) {
            vm_viewer_init();
            return;
        } else {
            obj_viewer_init();
            return;
        }
    }

    if (KEY_PRESSED(KEY_B)) {
        text_editor_init(false);
        return;
    }

    if (tm_state.obj_menu_active) {
        if (tm_state.cursor_pos == 0)
            obj_set_pos(&obj_buffer[0], 2 + 8 * 10, 8 * 5);
        else
            obj_set_pos(&obj_buffer[0], 2 + 8 * 7, 8 * 7);
    } else {
        if (tm_state.cursor_pos == 0)
            obj_set_pos(&obj_buffer[0], 2 + 8 * 12, 8 * 4);
        else if (tm_state.cursor_pos == 1)
            obj_set_pos(&obj_buffer[0], 2 + 8 * 10, 8 * 6);
        else
            obj_set_pos(&obj_buffer[0], 2 + 8 * 9, 8 * 8);
    }

    oam_copy(oam_mem, obj_buffer, 1);
}

void text_editor_menu_init(void) {
    tm_state = (TextEditorMenuState){0};
    update_fn = text_editor_menu_update;
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

    obj_set_attr(&obj_buffer[0], KEYBOARDCURSOR_SPRITE_SHAPE, ATTR1_SIZE_32X32,
                 KEYBOARDCURSOR_PALETTE_ID | KEYBOARDCURSOR_ID);
    obj_set_pos(&obj_buffer[0], 2 + 8 * 12, 8 * 4);

    char *text[] = {"SETTINGS", "save", "assemble", "clear text"};
    for (int i = 0; i < (int)(sizeof(text) / sizeof(*text)); i++) {
        int offset = (30 - strlen(text[i])) / 2;
        for (int j = 0; text[i][j]; j++)
            se_mem[30][32 * (2 + i * 2) + offset + j] = font_index(text[i][j]);
    }
}
