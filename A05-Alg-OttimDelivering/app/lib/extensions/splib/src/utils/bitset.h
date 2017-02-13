#ifndef GOL_UTILS_BITSET_H
#define GOL_UTILS_BITSET_H

#include <stdint.h>
#include <stdbool.h>

namespace gol {

#define BITSET_NONE UINT32_MAX

typedef struct bitset_s bitset_t;
struct bitset_s {
    uint32_t capacity;
    uint64_t *chunks;
    uint32_t nchunks;
};

bitset_t *bitset_new(uint32_t capacity);
void bitset_reset(bitset_t *self); // rename to bitset_clear?
void bitset_set(bitset_t *self, uint32_t index);
void bitset_unset(bitset_t *self, uint32_t index);
bool bitset_get(bitset_t *self, uint32_t index);
void bitset_dump(bitset_t *self);
uint32_t bitset_enumerate(bitset_t *self);
void bitset_destroy(bitset_t *self);
uint32_t bitset_next_set_bit(bitset_t*, uint32_t index);

}

#endif // GOL_UTILS_BITSET_H
