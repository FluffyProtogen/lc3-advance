#pragma once

#include <stdint.h>

#define REG_VCOUNT *(volatile uint16_t *)0x04000006

#define MEM_IO 0x04000000
#define MEM_PAL 0x05000000
#define MEM_VRAM 0x06000000
#define MEM_OAM 0x07000000

#define REG_DISPCNT *(volatile uint32_t *)(MEM_IO + 0x0000)
#define REG_DISPSTAT *(volatile uint16_t *)(MEM_IO + 0x0004)
#define REG_KEYINPUT *(volatile uint16_t *)(MEM_IO + 0x0130)

typedef struct {
    uint32_t data[8];
} TILE;

typedef TILE CHARBLOCK[512];
#define tile_mem ((CHARBLOCK *)MEM_VRAM)

#define PAL_BG_SIZE 0x00200
#define MEM_PAL_OBJ (MEM_PAL + PAL_BG_SIZE)

#define pal_obj_mem ((uint16_t *)MEM_PAL_OBJ)

#define oam_mem ((OBJ_ATTR *)MEM_OAM)

typedef struct OBJ_ATTR {
    uint16_t attr0;
    uint16_t attr1;
    uint16_t attr2;
    int16_t fill;
} __attribute__((aligned(4))) OBJ_ATTR;

typedef struct OBJ_AFFINE {
    uint16_t fill0[3];
    int16_t pa;
    uint16_t fill1[3];
    int16_t pb;
    uint16_t fill2[3];
    int16_t pc;
    uint16_t fill3[3];
    int16_t pd;
} __attribute__((aligned(4))) OBJ_AFFINE;

extern OBJ_ATTR obj_buffer[128];
extern OBJ_AFFINE *obj_aff_buffer;

#define ATTR0_HIDE 0x0200

void oam_copy(OBJ_ATTR *dst, const OBJ_ATTR *src, uint32_t count);

void oam_init(OBJ_ATTR *obj, uint32_t count);

#define DCNT_OBJ 0x1000
#define DCNT_OBJ_1D 0x0040

#define ATTR0_SQUARE 0
#define ATTR1_SIZE_32X32 0x8000
#define ATTR1_SIZE_64X64 0xC000
#define ATTR1_SIZE_64X32 0xC000

#define ATTR2_PALBANK_SHIFT 12
#define ATTR2_PALBANK(n) ((n) << ATTR2_PALBANK_SHIFT)

OBJ_ATTR *obj_set_attr(OBJ_ATTR *obj, uint16_t a0, uint16_t a1, uint16_t a2);

#define ATTR0_Y_MASK 0x00FF
#define ATTR0_Y_SHIFT 0
#define ATTR0_Y(n) ((n) << ATTR0_Y_SHIFT)
#define ATTR0_WIDE 0x4000

#define ATTR1_X_MASK 0x01FF
#define ATTR1_X_SHIFT 0
#define ATTR1_X(n) ((n) << ATTR1_X_SHIFT)

#define BFN_SET(y, x, name) (y = ((y) & ~name##_MASK) | BFN_PREP(x, name))
#define BFN_PREP(x, name) (((x) << name##_SHIFT) & name##_MASK)

void obj_set_pos(OBJ_ATTR *obj, int x, int y);

#define se_mem ((SCREENBLOCK *)MEM_VRAM)
#define pal_bg_mem ((uint16_t *)MEM_PAL)

#define BG_CBB_MASK 0x000C
#define BG_CBB_SHIFT 2
#define BG_CBB(n) ((n) << BG_CBB_SHIFT)

#define BG_SBB_MASK 0x1F00
#define BG_SBB_SHIFT 8
#define BG_SBB(n) ((n) << BG_SBB_SHIFT)

typedef uint16_t SCREENBLOCK[1024];

#define REG_BG0CNT *(volatile uint16_t *)(MEM_IO + 0x0008)
#define REG_BG0HOFS *(volatile uint16_t *)(MEM_IO + 0x0010)
#define REG_BG0VOFS *(volatile uint16_t *)(MEM_IO + 0x0012)
//
#define DCNT_MODE0 0x0000
#define DCNT_BG0 0x0100
#define BG_4BPP 0
#define BG_8BPP 0x0080
#define BG_REG_64X32 0x4000
#define BG_REG_32X32 0
#define BG_REG_32X64 0x8000
#define BG_REG_64X64 0xC000

#define KEY_A 0x0001
#define KEY_B 0x0002
#define KEY_SELECT 0x0004
#define KEY_START 0x0008
#define KEY_RIGHT 0x0010
#define KEY_LEFT 0x0020
#define KEY_UP 0x0040
#define KEY_DOWN 0x0080
#define KEY_R 0x0100
#define KEY_L 0x0200

// from GBATEK
#define FLASH_MEM ((volatile uint8_t *)0xE000000)

extern uint32_t frame_count;
extern uint16_t prev_keys;
extern uint16_t cur_keys;

void wait_for_vblank(void);

#define KEY_PRESSED(key) ((cur_keys & key) && !(prev_keys & key))

#define REG_DMA *((volatile DMA_REC *)0x040000B0)
#define DMA_ENABLE (1 << 31)
typedef struct DMA_REC {
    const void *src;
    void *dst;
    uint32_t cnt;
} DMA_REC;
