#include "ui_widget.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

UiWidget_t *create_widget(WidgetType_e type, WidgetScale_e scale, double *value,
                          double min, double max, int x, int y, int size) {
  UiWidget_t *widget = malloc(sizeof(UiWidget_t));
  widget->type = type;
  widget->scale = scale;

  if (widget->type == BUTTON) {
    if (*value != min && *value != max) {
      *value = min;
    }
  }

  widget->value = value;
  widget->min = min;
  widget->max = max;

  widget->x = x;
  widget->y = y;
  widget->size = size;
  widget->dragged = false;
  return widget;
}

double get_value_percentage(UiWidget_t *widget) {
  switch (widget->scale) {
  case LINEAR: {
    return (*(widget->value) - widget->min) / (widget->max - widget->min);
  }

  case LOGARITHMIC: {
    return log10(*(widget->value) - widget->min + 1.0) /
           log10(widget->max - widget->min);
  }
  }
}

double percentage_to_value(UiWidget_t *widget, double percentage) {
  switch (widget->scale) {
  case LINEAR: {
    return percentage * (widget->max - widget->min) + widget->min;
  }

  case LOGARITHMIC: {
    return pow(10, percentage * log10(widget->max - widget->min)) +
           widget->min - 1.0;
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

    break;
  }
  case BUTTON: {
    DrawCircleLines(widget->x, widget->y, widget->size, WHITE);

    if (widget->dragged) {
      DrawCircle(widget->x, widget->y, (widget->size / 10.0f) * 8.0f, LIGHTGRAY);
    } else {
      // button is pressed
      if (*(widget->value) != widget->min) {
        DrawCircle(widget->x, widget->y, (widget->size / 10.0f) * 9.0f, WHITE);
      }
    }

    break;
  }
  }
}

void widget_clicked(UiWidget_t *widget, int x, int y) {
  widget->dragged = true;

  switch (widget->type) {
  case SLIDER:
    break;
  case BUTTON: {
    break;
  }
  }
}

void widget_released(UiWidget_t *widget, int x, int y) {
  widget->dragged = false;

  switch (widget->type) {
  case SLIDER:
    break;
  case BUTTON: {
    if (*(widget->value) != widget->min)
    {
      *widget->value = widget->min;
    }

    else
    {
      *widget->value = widget->max;
    }
    break;
  }
  }
}

void widget_dragged(UiWidget_t *widget, int last_x, int last_y, int x, int y) {
  switch (widget->type) {
  case SLIDER: {
    int delta = -(y - last_y);
    double percentage_change = (double)delta / (double)widget->size;
    double new_percentage = percentage_change + get_value_percentage(widget);
    double new_value = percentage_to_value(widget, new_percentage);

    if (new_value > widget->max) {
      new_value = widget->max;
    }

    else if (new_value < widget->min) {
      new_value = widget->min;
    }

    *widget->value = new_value;
    break;
  }
  case BUTTON:
    break;
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
  case BUTTON:
  {
    int delta_x = x - widget->x;
    int delta_y = y - widget->y;

    int distance2 = (delta_x * delta_x) + (delta_y * delta_y);
    return distance2 < widget->size * widget->size;
  }
  }
}
