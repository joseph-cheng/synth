#pragma once

#include "dynamic_array.h"
#include "ui_widget.h"
#include <stddef.h>
typedef struct UiManager {
  DynamicArray_t widgets;
  UiWidget_t *dragged_widget;
  int last_mouse_x;
  int last_mouse_y;
} UiManager_t;

UiManager_t *create_ui_manager();

void add_widget(UiManager_t *ui_manager, UiWidget_t *widget);

void draw_ui(UiManager_t *ui_manager);

void update_ui(UiManager_t *ui_manager);
