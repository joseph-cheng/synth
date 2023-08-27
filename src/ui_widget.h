#pragma once

#include <raylib.h>
#include <stdbool.h>
typedef enum WidgetType { SLIDER, BUTTON } WidgetType_e;
typedef enum WidgetScale { LINEAR, LOGARITHMIC } WidgetScale_e;
typedef enum WidgetEventType {
  ON_CLICK,
  ON_RELEASE,
  ON_DRAG
} WidgetEventType_e;

typedef struct UiWidget UiWidget_t;

typedef struct WidgetEvent {
  UiWidget_t *widget;
  WidgetEventType_e event_type;
  union {
    struct {
    } on_click;

    struct {
    } on_release;

    struct {
      double start_percentage;
      double end_percentage;
    } on_drag;

  } event_data;
} WidgetEvent_t;

typedef void (*WidgetCallback_t)(void *udata, WidgetEvent_t *event);

typedef struct UiWidget {
  WidgetType_e type;
  WidgetScale_e scale;
  int x;
  int y;
  double component_value;
  int size;
  bool dragged;
  const char *name;
  WidgetCallback_t callback;
  void *udata;
} UiWidget_t;

UiWidget_t *create_widget(WidgetType_e type, WidgetScale_e scale, int x, int y,
                          int size, const char *name, WidgetCallback_t callback,
                          void *udata);

void destroy_widget(UiWidget_t *widget);

void draw_widget(UiWidget_t *widget, Color color);

void widget_clicked(UiWidget_t *widget, int x, int y);
void widget_released(UiWidget_t *widget, int x, int y);
void widget_dragged(UiWidget_t *widget, int last_x, int last_y, int x, int y);

bool point_inside_widget_clickbox(UiWidget_t *widget, int x, int y);
