#include "ui_widget.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

UiWidget_t *create_widget(WidgetType_e type, WidgetScale_e scale, int x, int y,
                          int size, const char *name, WidgetCallback_t callback,
                          void *udata) {
  UiWidget_t *widget = malloc(sizeof(UiWidget_t));
  widget->type = type;
  widget->scale = scale;
  widget->component_value = 0.0;

  widget->x = x;
  widget->y = y;
  widget->size = size;
  widget->dragged = false;
  widget->name = name;
  widget->callback = callback;
  widget->udata = udata;
  return widget;
}

/*
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
*/

void destroy_widget(UiWidget_t *widget) { free(widget); }

void draw_widget(UiWidget_t *widget, Color color) {
  switch (widget->type) {
  case SLIDER: {

    int slider_y =
        -(int)((widget->component_value - 0.5) * (double)widget->size) +
        widget->y;
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

    int text_width = MeasureText(widget->name, widget->size / 5);
    DrawText(widget->name, widget->x - text_width / 2,
             widget->y - widget->size / 2 - widget->size / 5, widget->size / 5,
             WHITE);

    break;
  }
  case BUTTON: {
    DrawCircleLines(widget->x, widget->y, widget->size, WHITE);

    if (widget->dragged) {
      DrawCircle(widget->x, widget->y, (widget->size / 10.0f) * 8.0f,
                 LIGHTGRAY);
    } else {
      // button is pressed
      if (widget->component_value) {
        DrawCircle(widget->x, widget->y, (widget->size / 10.0f) * 9.0f, WHITE);
      }
    }

    int text_width = MeasureText(widget->name, widget->size);
    DrawText(widget->name, widget->x - text_width / 2,
             widget->y - widget->size * 2, widget->size, WHITE);

    break;
  }
  }
}

void widget_clicked(UiWidget_t *widget, int x, int y) {
  widget->dragged = true;

  WidgetEvent_t event;
  event.widget = widget;
  event.event_type = ON_CLICK;

  switch (widget->type) {
  case SLIDER:
    break;
  case BUTTON: {
    break;
  }
  }

  if (widget->callback) {
    widget->callback(widget->udata, &event);
  }
}

void widget_released(UiWidget_t *widget, int x, int y) {
  widget->dragged = false;

  WidgetEvent_t event;
  event.widget = widget;
  event.event_type = ON_RELEASE;

  switch (widget->type) {
  case SLIDER:
    break;
  case BUTTON: {
    widget->component_value = !widget->component_value;
    break;
  }
  }

  if (widget->callback) {
    widget->callback(widget->udata, &event);
  }
}

void widget_dragged(UiWidget_t *widget, int last_x, int last_y, int x, int y) {
  WidgetEvent_t event;
  event.widget = widget;
  event.event_type = ON_DRAG;
  event.event_data.on_drag.start_percentage = widget->component_value;

  switch (widget->type) {
  case SLIDER: {
    widget->component_value =
        (double)(widget->y - y + widget->size / 2.0) / (double)widget->size;

    if (widget->component_value < 0.0) {
      widget->component_value = 0.0;
    }

    else if (widget->component_value > 1.0) {
      widget->component_value = 1.0;
    }
    break;
  }
  case BUTTON:
    break;
  }

  event.event_data.on_drag.end_percentage = widget->component_value;

  if (widget->callback) {
    widget->callback(widget->udata, &event);
  }
}

bool point_inside_widget_clickbox(UiWidget_t *widget, int x, int y) {
  switch (widget->type) {
  case SLIDER: {
    int slider_y =
        -(int)((widget->component_value - 0.5) * (double)widget->size) +
        widget->y;
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
  case BUTTON: {
    int delta_x = x - widget->x;
    int delta_y = y - widget->y;

    int distance2 = (delta_x * delta_x) + (delta_y * delta_y);
    return distance2 < widget->size * widget->size;
  }
  }
}
