#include "filter.h"
#include <math.h>
#include <stdlib.h>

float apply_filter(Filter_t *filter, float sample, float prev_sample, float prev_input_sample,
                   float sample_rate) {
  float dt = 1.0 / sample_rate;
  float rc = 1.0 / (filter->cutoff_freq * 2 * M_PI);
  float alpha = dt / (rc + dt);

  switch (filter->type) {
  case LOWPASS: {
    return (1 - alpha) * prev_sample + (sample  * prev_input_sample) / 2.0f * alpha;
  }
  }
}

Filter_t *create_filter(FilterType_e type, double cutoff_freq) {
  Filter_t *filter = malloc(sizeof(Filter_t));
  filter->type = type;
  filter->cutoff_freq = cutoff_freq;
  return filter;
}

void destroy_filter(Filter_t *filter) { free(filter); }
