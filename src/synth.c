#include "synth.h"
#include "filter.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const double PI = 3.14159265358979;

Synth_t *create_synth(Oscillator_t *oscillator)
{
  Synth_t *synth = malloc(sizeof(Synth_t));
  synth->oscillator = oscillator;
  synth->filters = create_array(10, sizeof(Filter_t *));
  return synth;
}

void destroy_synth(Synth_t *synth)
{
  free_array(&synth->filters);
}

void add_filter_to_synth(Synth_t *synth, Filter_t *filter)
{
  array_append(&synth->filters, &filter);
}

void synth_write_callback(struct SoundIoOutStream *out_stream,
                               int frame_count_min, int frame_count_max) {
  Synth_t *synth = (Synth_t *)out_stream->userdata;
  Oscillator_t *oscillator = synth->oscillator;
  const struct SoundIoChannelLayout *layout = &out_stream->layout;
  float float_sample_rate = out_stream->sample_rate;
  float seconds_per_frame = 1.0f / float_sample_rate;
  struct SoundIoChannelArea *areas;
  int frames_left = frame_count_max;
  int err;


  while (frames_left > 0) {
    int frame_count = frames_left;

    if ((err =
             soundio_outstream_begin_write(out_stream, &areas, &frame_count))) {
      fprintf(stderr, "%s\n", soundio_strerror(err));
      exit(1);
    }

    if (!frame_count)
      break;

    float pitch = 440.0f;
    float radians_per_second = pitch * 2.0f * PI;
    for (int frame = 0; frame < frame_count; frame += 1) {
      double seconds = oscillator->offset + frame * seconds_per_frame;
      float sample = 0.0f;
      if (oscillator->params.active) {
        switch (oscillator->type) {
        case SIN:
          sample = sine_gen(oscillator, seconds);
          break;

        case SAW:
          sample = saw_gen(oscillator, seconds);
          break;

        case SQUARE:
          sample = square_gen(oscillator, seconds);
          break;

        case RAND:
          sample = rand_gen(oscillator, seconds);
        }
      }


      Filter_t **filters = synth->filters.elements;

      float filtered_sample = sample;

      for (int ii = 0; ii < synth->filters.num_elements; ii++)
      {
        filtered_sample = apply_filter(filters[ii], filtered_sample, oscillator->prev_filtered_sample, oscillator->prev_sample, float_sample_rate);
      }

      for (int channel = 0; channel < layout->channel_count; channel += 1) {
        float *ptr =
            (float *)(areas[channel].ptr + areas[channel].step * frame);
        *ptr = filtered_sample;
        oscillator->prev_sample = sample;
        oscillator->prev_filtered_sample = filtered_sample;
      }
    }
    oscillator->offset =
        fmod(oscillator->offset + seconds_per_frame * frame_count, 1.0f);

    if ((err = soundio_outstream_end_write(out_stream))) {
      fprintf(stderr, "%s\n", soundio_strerror(err));
      exit(1);
    }

    frames_left -= frame_count;
  }
}
