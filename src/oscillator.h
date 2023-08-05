#pragma once
#include <soundio/soundio.h>

typedef enum OscType {
  SIN,
  SAW,
  SQUARE,
  RAND,
} OscType_e;

typedef struct OscParams {
  double freq;
  double amplitude;
} OscParams_t;

typedef struct Oscillator {
  OscType_e type;
  OscParams_t params;
  double offset;
} Oscillator_t;

void oscillator_write_callback(struct SoundIoOutStream *out_stream,
                               int frame_count_min, int frame_count_max);

Oscillator_t *make_oscillator(OscType_e type);
void destroy_oscillator(Oscillator_t *oscillator);

double sine_gen(Oscillator_t *oscillator, double seconds);
double saw_gen(Oscillator_t *oscillator, double seconds);
double square_gen(Oscillator_t *oscillator, double seconds);
double rand_gen(Oscillator_t *oscillator, double seconds);
