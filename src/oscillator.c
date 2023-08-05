#include "oscillator.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const double PI = 3.14159265358979;
void oscillator_write_callback(struct SoundIoOutStream *out_stream,
                               int frame_count_min, int frame_count_max) {
  Oscillator_t *oscillator = (Oscillator_t *)out_stream->userdata;
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
      double sample = 0.0f;
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
      for (int channel = 0; channel < layout->channel_count; channel += 1) {
        float *ptr =
            (float *)(areas[channel].ptr + areas[channel].step * frame);
        *ptr = sample;
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

Oscillator_t *make_oscillator(OscType_e type) {
  Oscillator_t *oscillator = malloc(sizeof(Oscillator_t));
  oscillator->type = type;
  oscillator->params.freq = 440.0;
  oscillator->params.amplitude = 0.2;
  oscillator->offset = 0.0;
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
