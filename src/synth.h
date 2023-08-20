#pragma once

#include "dynamic_array.h"
#include "filter.h"
#include "oscillator.h"

typedef struct Synth {
  DynamicArray_t *oscillators;
  Oscillator_t *oscillator;
  DynamicArray_t filters;
} Synth_t;

Synth_t *create_synth(Oscillator_t *oscillator);
void add_filter_to_synth(Synth_t *synth, Filter_t *filter);
void destroy_synth(Synth_t *synth);

void synth_write_callback(struct SoundIoOutStream *out_stream,
                          int frame_count_min, int frame_count_max);
