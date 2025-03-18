/*
 * Exported with nin10kit v1.8
 * Invocation command was nin10kit --device=gba --mode=0 --bpp=4 font font.png
 * Time-stamp: Tuesday 03/18/2025, 22:18:46
 *
 * Image Information
 * -----------------
 * font.png 256@256
 *
 * All bug reports / feature requests are to be filed here https://github.com/TricksterGuy/nin10kit/issues
 */

#ifndef FONT_H
#define FONT_H

extern const unsigned short font_palette[16];
#define FONT_PALETTE_SIZE 32
#define FONT_PALETTE_LENGTH 16

#define FONT_PALETTE_TYPE (0 << 7)

extern const unsigned short font_tiles[1712];
#define FONT_TILES 107
#define FONT_TILES_SIZE 3424
#define FONT_TILES_LENGTH 1712

extern const unsigned short font[1024];
#define FONT_MAP_WIDTH 32
#define FONT_MAP_HEIGHT 32
#define FONT_MAP_SIZE 2048
#define FONT_MAP_LENGTH 1024
#define FONT_MAP_TYPE (0 << 14)

int font_index(char c);
int line_num_font_index(int num);

#endif
