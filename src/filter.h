#pragma once

typedef enum FilterType {
  LOWPASS,
} FilterType_e;

typedef struct Filter {
  FilterType_e type;
  double cutoff_freq;
} Filter_t;

Filter_t *create_filter(FilterType_e type, double cutoff_freq);
void destroy_filter(Filter_t *filter);

float apply_filter(Filter_t *filter, float sample, float prev_sample, float prev_input_sample, float sample_rate);
