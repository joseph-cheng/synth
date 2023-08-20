#include "ui_manager.h"
#include <stdlib.h>

UiManager_t *create_ui_manager() {
  UiManager_t *ui_manager = malloc(sizeof(UiManager_t));

  ui_manager->widgets = create_array(10, sizeof(UiWidget_t *));
  ui_manager->dragged_widget = 0;
  ui_manager->last_mouse_x = 0;
  ui_manager->last_mouse_y = 0;

  return ui_manager;
}

void add_widget(UiManager_t *ui_manager, UiWidget_t *widget) {
  array_append(&ui_manager->widgets, &widget);
}

void draw_ui(UiManager_t *ui_manager) {
  UiWidget_t **widgets = ui_manager->widgets.elements;
  for (int ii = 0; ii < ui_manager->widgets.num_elements; ii++) {
    draw_widget(widgets[ii], WHITE);
  }
}

void update_ui(UiManager_t *ui_manager) {
  UiWidget_t **widgets = ui_manager->widgets.elements;

  int mouse_x = GetMouseX();
  int mouse_y = GetMouseY();

  if (IsMouseButtonReleased(0)) {
    if (ui_manager->dragged_widget)
    {
      widget_released(ui_manager->dragged_widget, mouse_x, mouse_y);
      ui_manager->dragged_widget = NULL;
    }
  }

  if (ui_manager->dragged_widget) {
    widget_dragged(ui_manager->dragged_widget, ui_manager->last_mouse_x,
                  ui_manager->last_mouse_y, mouse_x, mouse_y);
  }

  if (IsMouseButtonPressed(0)) {
    for (int ii = 0; ii < ui_manager->widgets.num_elements; ii++) {
      if (point_inside_widget_clickbox(widgets[ii], mouse_x, mouse_y)) {
        ui_manager->dragged_widget = widgets[ii];
        widget_clicked(ui_manager->dragged_widget, mouse_x, mouse_y);
      }
    }
  }

  ui_manager->last_mouse_x = mouse_x;
  ui_manager->last_mouse_y = mouse_y;
}
