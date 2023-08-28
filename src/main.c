#include "filter.h"
#include "oscillator.h"
#include "synth.h"
#include "ui_manager.h"
#include <math.h>
#include <raylib.h>
#include <soundio/soundio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  bool *value;
} ButtonCallbackData_t;

typedef struct
{
  double *value;
  double min;
  double max;
} SliderCallbackData_t;

void basic_value_callback(void *udata, WidgetEvent_t *event)
{
  switch (event->widget->type)
  {
  case SLIDER:
  {
    SliderCallbackData_t *data = udata;

    switch (event->event_type)
    {
    case ON_CLICK:
      break;

    case ON_RELEASE:
    {
      break;
    }

    case ON_DRAG:
    {
      double percentage = event->event_data.on_drag.percentage;

      switch (event->widget->scale)
      {
      case LINEAR:
        *(data->value) = percentage * (data->max - data->min) + data->min;
        break;
      case LOGARITHMIC:
        *(data->value) = pow(10, percentage * log10(data->max - data->min)) +
                         data->min - 1.0;
        break;
      }

      break;
    }
    }

    break;
  }
  case BUTTON:
  {
    ButtonCallbackData_t *data = udata;

    switch (event->event_type)
    {
    case ON_CLICK:
      break;

    case ON_RELEASE:
    {
      *(data->value) = !*(data->value);
      break;
    }

    case ON_DRAG:
      break;
    }

    break;
  }
  }
}

typedef enum
{
  FREQ,
  AMPLITUDE,
  ACTIVE,
} ParamType_e;

typedef struct
{
  Synth_t *synth;
  ParamType_e param_type;
} SynthParamData_t;

void oscillator_param_callback(void *udata, WidgetEvent_t *event)
{
  SynthParamData_t *data = udata;
  if (event->event_type == ON_DRAG && event->widget->type == SLIDER)
  {

    switch (data->param_type)
    {
    case FREQ:
    {
      double max = 20000.0;
      double min = 30.0;
      double freq =
          pow(10, event->event_data.on_drag.percentage * log10(max - min)) +
          min - 1.0;
      update_oscillator_freq(data->synth, freq);
      break;
    }
    case AMPLITUDE:
    {
      update_oscillator_amp(data->synth, event->event_data.on_drag.percentage);
      break;
    }
    case ACTIVE:
      break;
    }
  }

  if (event->event_type == ON_RELEASE && event->widget->type == BUTTON)
  {
    if (data->param_type == ACTIVE)
    {
      bool new_active = !data->synth->oscillators[0].params.active;
      printf("%d\n", new_active);
      update_oscillator_active(data->synth, new_active);
    }
  }
}

int main(int argc, char **argv)
{
  int err;
  struct SoundIo *soundio = soundio_create();
  if (!soundio)
  {
    fprintf(stderr, "out of memory\n");
    return 1;
  }

  if ((err = soundio_connect(soundio)))
  {
    fprintf(stderr, "error connecting: %s", soundio_strerror(err));
    return 1;
  }

  soundio_flush_events(soundio);

  int default_out_device_index = soundio_default_output_device_index(soundio);
  if (default_out_device_index < 0)
  {
    fprintf(stderr, "no output device found");
    return 1;
  }

  struct SoundIoDevice *device =
      soundio_get_output_device(soundio, default_out_device_index);
  if (!device)
  {
    fprintf(stderr, "out of memory");
    return 1;
  }

  fprintf(stderr, "Output device: %s\n", device->name);

  Oscillator_t *oscillator = make_oscillator(SAW);
  Filter_t *filter = create_filter(LOWPASS, 1000.0f);

  Synth_t *synth = create_synth(oscillator, 5, 0.1, 0.4, 0.0);
  add_filter_to_synth(synth, filter);

  struct SoundIoOutStream *outstream = soundio_outstream_create(device);
  outstream->format = SoundIoFormatFloat32NE;
  outstream->userdata = synth;
  outstream->write_callback = synth_write_callback;
  outstream->software_latency = 0.017;

  if ((err = soundio_outstream_open(outstream)))
  {
    fprintf(stderr, "unable to open device: %s", soundio_strerror(err));
    return 1;
  }

  if (outstream->layout_error)
    fprintf(stderr, "unable to set channel layout: %s\n",
            soundio_strerror(outstream->layout_error));

  if ((err = soundio_outstream_start(outstream)))
  {
    fprintf(stderr, "unable to start device: %s", soundio_strerror(err));
    return 1;
  }

  InitWindow(800, 600, "synth");
  UiManager_t *ui_manager = create_ui_manager();
  SynthParamData_t freq_callback_data = {synth, FREQ};
  SynthParamData_t amp_callback_data = {synth, AMPLITUDE};
  SynthParamData_t active_callback_data = {synth, ACTIVE};
  SliderCallbackData_t lpf_cutoff_callback_data = {&filter->cutoff_freq, 30.0,
                                                   20000.0};
  add_widget(ui_manager,
             create_widget(SLIDER, LOGARITHMIC, 100, 400, 150, "Freq",
                           oscillator_param_callback, &freq_callback_data));
  add_widget(ui_manager,
             create_widget(SLIDER, LINEAR, 300, 400, 150, "Amplitude",
                           oscillator_param_callback, &amp_callback_data));
  add_widget(ui_manager,
             create_widget(SLIDER, LOGARITHMIC, 300, 100, 150, "LP Cutoff",
                           basic_value_callback, &lpf_cutoff_callback_data));

  add_widget(ui_manager,
             create_widget(BUTTON, LINEAR, 500, 400, 20, "Active",
                           oscillator_param_callback, &active_callback_data));

  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);
    update_ui(ui_manager);
    draw_ui(ui_manager);

    EndDrawing();
    soundio_flush_events(soundio);
  }

  CloseWindow();

  soundio_outstream_destroy(outstream);
  soundio_device_unref(device);
  soundio_destroy(soundio);
  return 0;
}
