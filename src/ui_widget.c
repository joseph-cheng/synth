#include "ui_widget.h"
#include <stdlib.h>

UiWidget_t *create_widget(WidgetType_e type, double *value, double min,
                          double max, int x, int y, int size) {
  UiWidget_t *widget = malloc(sizeof(UiWidget_t));
  widget->type = type;
  widget->value = value;
  widget->min = min;
  widget->max = max;
  widget->x = x;
  widget->y = y;
  widget->size = size;
  return widget;
}

void destroy_widget(UiWidget_t *widget) { free(widget); }

void draw(UiWidget_t *widget) {}

void update(UiWidget_t *widget, int last_x, int last_y, int x, int y) {
  switch (widget->type) {
  case SLIDER: {
    int delta = y - last_y;
    double percentage_change = (double)delta / (double)widget->size;
    double value_change = percentage_change * (widget->max - widget->min);
    *widget->value += value_change;
  }
  }
}
