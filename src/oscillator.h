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
  bool active;
} OscParams_t;

typedef struct Oscillator {
  OscType_e type;
  OscParams_t params;
  double offset;
  float prev_sample;
  float prev_filtered_sample;
  double left_pan;
  double right_pan;
} Oscillator_t;

Oscillator_t *make_oscillator(OscType_e type);
void destroy_oscillator(Oscillator_t *oscillator);

double sine_gen(Oscillator_t *oscillator, double seconds);
double saw_gen(Oscillator_t *oscillator, double seconds);
double square_gen(Oscillator_t *oscillator, double seconds);
double rand_gen(Oscillator_t *oscillator, double seconds);
