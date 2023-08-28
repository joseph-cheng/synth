#include "synth.h"
#include "filter.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const double PI = 3.14159265358979;

Synth_t *create_synth(Oscillator_t *oscillator, size_t voices,
                      double freq_spread, double pan_spread, double amp_spread) {
  Synth_t *synth = malloc(sizeof(Synth_t));
  synth->oscillators = malloc(sizeof(Oscillator_t) * voices);
  synth->freq_spread = freq_spread;
  synth->pan_spread = pan_spread;
  synth->amp_spread = amp_spread;
  synth->voices = voices;
  synth->filters = create_array(10, sizeof(Filter_t *));

  for (int ii = 0; ii < voices; ii++) {
    synth->oscillators[ii] = *oscillator;
  }

  update_oscillator_freq(synth, oscillator->params.freq);
  update_oscillator_pan(synth, oscillator->params.pan);
  update_oscillator_amp(synth, oscillator->params.amplitude);

  return synth;
}

void update_oscillator_freq(Synth_t *synth, double new_freq)
{
  double spread = synth->freq_spread * new_freq;

  for (int ii=0; ii < synth->voices; ii++)
  {
    double voice_position = (ii - (synth->voices / 2.0)) / (double)(synth->voices / 2.0);
    synth->oscillators[ii].params.freq = new_freq + spread * voice_position;
  }
}

void update_oscillator_amp(Synth_t *synth, double new_amp)
{
  for (int ii=0; ii < synth->voices; ii++)
  {
    synth->oscillators[ii].params.amplitude = new_amp;
  }
}

void update_oscillator_pan(Synth_t *synth, double new_pan)
{
  double spread = synth->pan_spread;

  for (int ii=0; ii < synth->voices; ii++)
  {
    double voice_position = (ii - (synth->voices / 2.0)) / (double)(synth->voices / 2.0);
    synth->oscillators[ii].params.pan = new_pan + spread * voice_position;
  }
}

void update_oscillator_active(Synth_t *synth, bool new_active)
{
  for (int ii=0; ii < synth->voices; ii++)
  {
    synth->oscillators[ii].params.active = new_active;
  }
}

void destroy_synth(Synth_t *synth) {
  free_array(&synth->filters);
  free(synth->oscillators);
  free(synth);
}

void add_filter_to_synth(Synth_t *synth, Filter_t *filter) {
  array_append(&synth->filters, &filter);
}

void synth_write_callback(struct SoundIoOutStream *out_stream,
                          int frame_count_min, int frame_count_max) {
  Synth_t *synth = (Synth_t *)out_stream->userdata;
  Oscillator_t *oscillators = synth->oscillators;
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
      float left_channel_sample = 0.0;
      float right_channel_sample = 0.0;
      for (int ii=0; ii < synth->voices; ii++)
      {
        Oscillator_t *oscillator = &oscillators[ii];
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

        for (int ii = 0; ii < synth->filters.num_elements; ii++) {
          filtered_sample = apply_filter(
              filters[ii], filtered_sample, oscillator->prev_filtered_sample,
              oscillator->prev_sample, float_sample_rate);
        }

        double right_pan = (oscillator->params.pan + 1.0) / 2.0;
        double left_pan = 1.0 - right_pan;
        left_channel_sample += filtered_sample * left_pan;
        right_channel_sample += filtered_sample * right_pan;

        oscillator->prev_sample = sample;
        oscillator->prev_filtered_sample = filtered_sample;

      }

      for (int channel = 0; channel < layout->channel_count; channel += 1) {
        float *ptr =
            (float *)(areas[channel].ptr + areas[channel].step * frame);
        if (channel == 0)
        {
          *ptr = left_channel_sample;
        }

        else if (channel == 1)
        {
          *ptr = right_channel_sample;
        }
        
        else
        {
          *ptr = 0.0f;
        }
        
      }
    }

    for (int ii=0; ii < synth->voices; ii++)
    {
      oscillators[ii].offset =
          fmod(oscillators[ii].offset + seconds_per_frame * frame_count, 1.0f);
    }

    if ((err = soundio_outstream_end_write(out_stream))) {
      fprintf(stderr, "%s\n", soundio_strerror(err));
      exit(1);
    }

    frames_left -= frame_count;
  }
}
