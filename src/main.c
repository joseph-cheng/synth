#include "oscillator.h"
#include "ui_manager.h"
#include <math.h>
#include <raylib.h>
#include <soundio/soundio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  int err;
  struct SoundIo *soundio = soundio_create();
  if (!soundio) {
    fprintf(stderr, "out of memory\n");
    return 1;
  }

  if ((err = soundio_connect(soundio))) {
    fprintf(stderr, "error connecting: %s", soundio_strerror(err));
    return 1;
  }

  soundio_flush_events(soundio);

  int default_out_device_index = soundio_default_output_device_index(soundio);
  if (default_out_device_index < 0) {
    fprintf(stderr, "no output device found");
    return 1;
  }

  struct SoundIoDevice *device =
      soundio_get_output_device(soundio, default_out_device_index);
  if (!device) {
    fprintf(stderr, "out of memory");
    return 1;
  }

  fprintf(stderr, "Output device: %s\n", device->name);

  Oscillator_t *oscillator = make_oscillator(SQUARE);

  struct SoundIoOutStream *outstream = soundio_outstream_create(device);
  outstream->format = SoundIoFormatFloat32NE;
  outstream->userdata = oscillator;
  outstream->write_callback = oscillator_write_callback;
  outstream->software_latency = 0.017;

  if ((err = soundio_outstream_open(outstream))) {
    fprintf(stderr, "unable to open device: %s", soundio_strerror(err));
    return 1;
  }

  if (outstream->layout_error)
    fprintf(stderr, "unable to set channel layout: %s\n",
            soundio_strerror(outstream->layout_error));

  if ((err = soundio_outstream_start(outstream))) {
    fprintf(stderr, "unable to start device: %s", soundio_strerror(err));
    return 1;
  }

  InitWindow(800, 600, "synth");
  UiManager_t *ui_manager = create_ui_manager();
  add_widget(ui_manager, create_widget(SLIDER, LOGARITHMIC, &oscillator->params.freq, 30.0, 20000.0, 100, 400, 200));
  add_widget(ui_manager, create_widget(SLIDER, LINEAR, &oscillator->params.amplitude, 0.0, 1.0, 300, 400, 200));
  add_widget(ui_manager, create_widget(BUTTON, LINEAR, &oscillator->params.active, 0.0, 1.0, 500, 400, 20));

  while (!WindowShouldClose()) {
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
