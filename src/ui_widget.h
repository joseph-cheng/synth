#pragma once

typedef enum WidgetType { SLIDER } WidgetType_e;

typedef struct UiWidget {
  WidgetType_e type;
  double *value;
  double min;
  double max;
  int x;
  int y;
  int size;
} UiWidget_t;

UiWidget_t *create_widget(WidgetType_e type, double *value, double min,
                          double max, int x, int y, int size);
void destroy_widget(UiWidget_t *widget);

void draw(UiWidget_t *widget);

void update(UiWidget_t *widget, int last_x, int last_y, int x, int y);
