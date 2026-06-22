
#ifndef SORT_H
#define SORT_H

#include "common/common.h"
const uint32_t BLOCK_SIZE = 64;
const uint32_t NUM_KEYS = 1 << 12;
const uint32_t NUM_BLOCKS = NUM_KEYS / BLOCK_SIZE;

bool sort_init();
void sort_free();
void radix_sort(GLuint particles, vec3 axis, float z_min, float z_max);

#endif
