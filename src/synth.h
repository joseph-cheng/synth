#pragma once

#include "dynamic_array.h"
#include "filter.h"
#include "oscillator.h"

typedef struct Synth {
  Oscillator_t *oscillators;
  size_t voices;
  double freq_spread;
  double pan_spread;
  double amp_spread;
  DynamicArray_t filters;
} Synth_t;

Synth_t *create_synth(Oscillator_t *oscillator, size_t voices, double freq_spread, double pan_spread, double amp_spread);
void add_filter_to_synth(Synth_t *synth, Filter_t *filter);
void destroy_synth(Synth_t *synth);

void update_oscillator_freq(Synth_t *synth, double new_freq);
void update_oscillator_pan(Synth_t *synth, double new_pan);
void update_oscillator_amp(Synth_t *synth, double new_amp);
void update_oscillator_active(Synth_t *synth, bool new_active);

void synth_write_callback(struct SoundIoOutStream *out_stream,
                          int frame_count_min, int frame_count_max);
