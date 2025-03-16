#include "gba.h"

uint32_t frame_count;

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE *)obj_buffer;

void oam_copy(OBJ_ATTR *dst, const OBJ_ATTR *src, uint32_t count) {
    while (count--) *dst++ = *src++;
}

void wait_for_vblank(void) {
    while (REG_VCOUNT >= 160);
    while (REG_VCOUNT < 160);
    frame_count++;
}

void oam_init(OBJ_ATTR *obj, uint32_t count) {
    uint32_t nn = (count + 3) >> 2;
    uint32_t *dst = (uint32_t *)obj;

    while (nn--) {
        *dst++ = ATTR0_HIDE;
        *dst++ = 0x0100 << 16;
        *dst++ = ATTR0_HIDE;
        *dst++ = 0;
        *dst++ = ATTR0_HIDE;
        *dst++ = 0;
        *dst++ = ATTR0_HIDE;
        *dst++ = 0x0100 << 16;
    }
    oam_copy(oam_mem, obj, count);
}

OBJ_ATTR *obj_set_attr(OBJ_ATTR *obj, uint16_t a0, uint16_t a1, uint16_t a2) {
    obj->attr0 = a0;
    obj->attr1 = a1;
    obj->attr2 = a2;
    return obj;
}

void obj_set_pos(OBJ_ATTR *obj, int x, int y) {
    BFN_SET(obj->attr0, y, ATTR0_Y);
    BFN_SET(obj->attr1, x, ATTR1_X);
}
