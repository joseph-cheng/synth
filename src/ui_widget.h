#pragma once

#include <raylib.h>
#include <stdbool.h>
typedef enum WidgetType { SLIDER } WidgetType_e;
typedef enum WidgetScale { LINEAR, LOGARITHMIC } WidgetScale_e;

typedef struct UiWidget {
  WidgetType_e type;
  WidgetScale_e scale;
  double *value;
  double min;
  double max;
  int x;
  int y;
  int size;
} UiWidget_t;

UiWidget_t *create_widget(WidgetType_e type, WidgetScale_e scale, double *value, double min,
                          double max, int x, int y, int size);
void destroy_widget(UiWidget_t *widget);

void draw_widget(UiWidget_t *widget, Color color);

void update_widget(UiWidget_t *widget, int last_x, int last_y, int x, int y);

bool point_inside_widget_clickbox(UiWidget_t *widget, int x, int y);
