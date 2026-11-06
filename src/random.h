#ifndef RANDOM_H

#define RANDOM_H

#include <stdint.h>

typedef struct mt_state mt_state;

void initialize_state(mt_state* state, uint32_t seed);
uint32_t random_uint32(mt_state* state);

#endif
