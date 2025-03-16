/*
 * Exported with nin10kit v1.8
 * Invocation command was nin10kit --device=gba --mode=sprites --bpp=4 title_screen_sprites lc3-advance-split-1.png lc3-advance-split-2.png press_start.png 
 * Time-stamp: Sunday 03/16/2025, 04:44:35
 * 
 * Image Information
 * -----------------
 * lc3-advance-split-1.png 64@32
 * lc3-advance-split-2.png 64@32
 * press_start.png 64@32
 * 
 * All bug reports / feature requests are to be filed here https://github.com/TricksterGuy/nin10kit/issues
 */

#ifndef TITLE_SCREEN_SPRITES_H
#define TITLE_SCREEN_SPRITES_H

#define TITLE_SCREEN_SPRITES_PALETTE_TYPE (0 << 13)
#define TITLE_SCREEN_SPRITES_DIMENSION_TYPE (1 << 6)

extern const unsigned short title_screen_sprites_palette[16];
#define TITLE_SCREEN_SPRITES_PALETTE_SIZE 32
#define TITLE_SCREEN_SPRITES_PALETTE_LENGTH 16

extern const unsigned short title_screen_sprites[1536];
#define TITLE_SCREEN_SPRITES_SIZE 3072
#define TITLE_SCREEN_SPRITES_LENGTH 1536

#define LC3ADVANCESPLIT1_PALETTE_ID (0 << 12)
#define LC3ADVANCESPLIT1_SPRITE_SHAPE (1 << 14)
#define LC3ADVANCESPLIT1_SPRITE_SIZE (3 << 14)
#define LC3ADVANCESPLIT1_ID 0

#define LC3ADVANCESPLIT2_PALETTE_ID (0 << 12)
#define LC3ADVANCESPLIT2_SPRITE_SHAPE (1 << 14)
#define LC3ADVANCESPLIT2_SPRITE_SIZE (3 << 14)
#define LC3ADVANCESPLIT2_ID 32

#define PRESS_START_PALETTE_ID (0 << 12)
#define PRESS_START_SPRITE_SHAPE (1 << 14)
#define PRESS_START_SPRITE_SIZE (3 << 14)
#define PRESS_START_ID 64

#endif

