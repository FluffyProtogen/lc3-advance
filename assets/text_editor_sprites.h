/*
 * Exported with nin10kit v1.8
 * Invocation command was nin10kit --device=gba --mode=sprites --bpp=4 text_editor_sprites upper.png lower.png symbol.png 
 * Time-stamp: Monday 03/17/2025, 05:44:59
 * 
 * Image Information
 * -----------------
 * upper.png 32@32
 * lower.png 32@32
 * symbol.png 32@32
 * 
 * All bug reports / feature requests are to be filed here https://github.com/TricksterGuy/nin10kit/issues
 */

#ifndef TEXT_EDITOR_SPRITES_H
#define TEXT_EDITOR_SPRITES_H

#define TEXT_EDITOR_SPRITES_PALETTE_TYPE (0 << 13)
#define TEXT_EDITOR_SPRITES_DIMENSION_TYPE (1 << 6)

extern const unsigned short text_editor_sprites_palette[16];
#define TEXT_EDITOR_SPRITES_PALETTE_SIZE 32
#define TEXT_EDITOR_SPRITES_PALETTE_LENGTH 16

extern const unsigned short text_editor_sprites[768];
#define TEXT_EDITOR_SPRITES_SIZE 1536
#define TEXT_EDITOR_SPRITES_LENGTH 768

#define UPPER_PALETTE_ID (0 << 12)
#define UPPER_SPRITE_SHAPE (0 << 14)
#define UPPER_SPRITE_SIZE (2 << 14)
#define UPPER_ID 0

#define LOWER_PALETTE_ID (0 << 12)
#define LOWER_SPRITE_SHAPE (0 << 14)
#define LOWER_SPRITE_SIZE (2 << 14)
#define LOWER_ID 16

#define SYMBOL_PALETTE_ID (0 << 12)
#define SYMBOL_SPRITE_SHAPE (0 << 14)
#define SYMBOL_SPRITE_SIZE (2 << 14)
#define SYMBOL_ID 32

#endif

