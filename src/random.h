#ifndef RANDOM_H

#define RANDOM_H

#include <stdint.h>

typedef struct
{
    uint32_t state_array[624];        // the array for the state vector 
    int state_index;                 // index into state vector array, 0 <= state_index <= n-1   always
} mt_state;

void initialize_state(mt_state* state, uint32_t seed);
uint32_t random_uint32(mt_state* state);

#endif
