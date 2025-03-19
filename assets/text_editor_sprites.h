/*
 * Exported with nin10kit v1.8
 * Invocation command was nin10kit --device=gba --mode=sprites --bpp=4 text_editor_sprites KeyboardCursor.png TextCursor.png upper.png lower.png symbol.png KeyboardEnabled.png CursorEnabled.png 
 * Time-stamp: Wednesday 03/19/2025, 04:19:45
 * 
 * Image Information
 * -----------------
 * KeyboardCursor.png 32@32
 * TextCursor.png 32@32
 * upper.png 32@32
 * lower.png 32@32
 * symbol.png 32@32
 * KeyboardEnabled.png 32@32
 * CursorEnabled.png 32@32
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

extern const unsigned short text_editor_sprites[1792];
#define TEXT_EDITOR_SPRITES_SIZE 3584
#define TEXT_EDITOR_SPRITES_LENGTH 1792

#define KEYBOARDCURSOR_PALETTE_ID (0 << 12)
#define KEYBOARDCURSOR_SPRITE_SHAPE (0 << 14)
#define KEYBOARDCURSOR_SPRITE_SIZE (2 << 14)
#define KEYBOARDCURSOR_ID 0

#define TEXTCURSOR_PALETTE_ID (0 << 12)
#define TEXTCURSOR_SPRITE_SHAPE (0 << 14)
#define TEXTCURSOR_SPRITE_SIZE (2 << 14)
#define TEXTCURSOR_ID 16

#define UPPER_PALETTE_ID (0 << 12)
#define UPPER_SPRITE_SHAPE (0 << 14)
#define UPPER_SPRITE_SIZE (2 << 14)
#define UPPER_ID 32

#define LOWER_PALETTE_ID (0 << 12)
#define LOWER_SPRITE_SHAPE (0 << 14)
#define LOWER_SPRITE_SIZE (2 << 14)
#define LOWER_ID 48

#define SYMBOL_PALETTE_ID (0 << 12)
#define SYMBOL_SPRITE_SHAPE (0 << 14)
#define SYMBOL_SPRITE_SIZE (2 << 14)
#define SYMBOL_ID 64

#define KEYBOARDENABLED_PALETTE_ID (0 << 12)
#define KEYBOARDENABLED_SPRITE_SHAPE (0 << 14)
#define KEYBOARDENABLED_SPRITE_SIZE (2 << 14)
#define KEYBOARDENABLED_ID 80

#define CURSORENABLED_PALETTE_ID (0 << 12)
#define CURSORENABLED_SPRITE_SHAPE (0 << 14)
#define CURSORENABLED_SPRITE_SIZE (2 << 14)
#define CURSORENABLED_ID 96

#endif

