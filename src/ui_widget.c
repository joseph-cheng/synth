#include "ui_widget.h"
#include <stdio.h>
#include <math.h>
#include <raylib.h>
#include <stdlib.h>

UiWidget_t *create_widget(WidgetType_e type, WidgetScale_e scale, double *value,
                          double min, double max, int x, int y, int size) {
  UiWidget_t *widget = malloc(sizeof(UiWidget_t));
  widget->type = type;
  widget->scale = scale;
  widget->value = value;
  widget->min = min;
  widget->max = max;
  widget->x = x;
  widget->y = y;
  widget->size = size;
  return widget;
}

double get_value_percentage(UiWidget_t *widget) {
  switch (widget->scale) {
  case LINEAR: {
    return (*(widget->value) - widget->min) / (widget->max - widget->min);
  }

  case LOGARITHMIC: {
    return log10(*(widget->value) - widget->min + 1.0) / log10(widget->max - widget->min);
  }
  }
}

double percentage_to_value(UiWidget_t *widget, double percentage) {
  switch (widget->scale) {
  case LINEAR: {
    return percentage * (widget->max - widget->min) + widget->min;
  }

  case LOGARITHMIC: {
    return pow(10, percentage * log10(widget->max - widget->min)) + widget->min - 1.0;
  }
  }
}

void destroy_widget(UiWidget_t *widget) { free(widget); }

void draw_widget(UiWidget_t *widget, Color color) {
  switch (widget->type) {
  case SLIDER: {

    double percentage = get_value_percentage(widget);
    int slider_y =
        -(int)((percentage - 0.5) * (double)widget->size) + widget->y;
    int slider_x = widget->x;

    int slider_width = widget->size / 5;
    int slider_height = widget->size / 20;

    int slider_left = slider_x - slider_width / 2;
    int slider_right = slider_x + slider_width / 2;
    int slider_bottom = slider_y + slider_height / 2;
    int slider_top = slider_y - slider_height / 2;

    DrawLine(widget->x, widget->y - widget->size / 2, widget->x,
             widget->y + widget->size / 2, color);
    DrawRectangle(slider_left, slider_top, slider_width, slider_height, color);
  }
  }
}

void update_widget(UiWidget_t *widget, int last_x, int last_y, int x, int y) {
  switch (widget->type) {
  case SLIDER: {
    int delta = -(y - last_y);
    double percentage_change = (double)delta / (double)widget->size;
    double new_percentage = percentage_change + get_value_percentage(widget);
    double new_value = percentage_to_value(widget, new_percentage);
    printf("%lf, %lf, %lf\n", percentage_change, new_percentage, new_value);

    if (new_value > widget->max) {
      new_value = widget->max;
    }

    else if (new_value < widget->min) {
      new_value = widget->min;
    }

    *widget->value = new_value;
  }
  }
}

bool point_inside_widget_clickbox(UiWidget_t *widget, int x, int y) {
  switch (widget->type) {
  case SLIDER: {
    double percentage = get_value_percentage(widget);
    int slider_y =
        -(int)((percentage - 0.5) * (double)widget->size) + widget->y;
    int slider_x = widget->x;

    int slider_width = widget->size / 5;
    int slider_height = widget->size / 20;

    int slider_left = slider_x - slider_width / 2;
    int slider_right = slider_x + slider_width / 2;
    int slider_bottom = slider_y + slider_height / 2;
    int slider_top = slider_y - slider_height / 2;

    return x > slider_left && x < slider_right && y > slider_top &&
           y < slider_bottom;
  }
  }
}
