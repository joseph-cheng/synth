#include "oscillator.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const double PI = 3.14159265358979;

Oscillator_t *make_oscillator(OscType_e type) {
  Oscillator_t *oscillator = malloc(sizeof(Oscillator_t));
  oscillator->type = type;
  oscillator->params.freq = 2000.0;
  oscillator->params.amplitude = 0.2;
  oscillator->params.active = false;
  oscillator->offset = 0.0;
  oscillator->prev_sample = 0.0f;
  oscillator->prev_filtered_sample = 0.0f;
  oscillator->left_pan = 0.5;
  oscillator->right_pan = 0.5;
  return oscillator;
}

void destroy_oscillator(Oscillator_t *oscillator) { free(oscillator); }

double sine_gen(Oscillator_t *oscillator, double seconds) {
  double radians_per_second = oscillator->params.freq * 2.0f * PI;
  return sinf(seconds * radians_per_second) * oscillator->params.amplitude;
}

double saw_gen(Oscillator_t *oscillator, double seconds) {
  double period = 1.0 / oscillator->params.freq;
  double position_in_period = fmod(seconds, period);
  return ((position_in_period / period) * 2.0 - 1.0) *
         oscillator->params.amplitude;
}

double square_gen(Oscillator_t *oscillator, double seconds) {
  double period = 1.0 / oscillator->params.freq;
  double position_in_period = fmod(seconds, period);

  if (position_in_period / period < 0.5) {
    return oscillator->params.amplitude;
  }

  else {
    return -oscillator->params.amplitude;
  }
}

double rand_gen(Oscillator_t *oscillator, double seconds) {
  return ((double)rand() / (double)RAND_MAX) * oscillator->params.amplitude;
}
