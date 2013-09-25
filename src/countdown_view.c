// ----------------------------------------------------------------------------
// countdown_view - Displays a countdown timer and controls
// Copyright (c) 2013 Jonathan Speicher (jon.speicher@gmail.com)
// Licensed under the MIT license: http://opensource.org/licenses/MIT
// ----------------------------------------------------------------------------

#include <pebble_os.h>
#include <pebble_app.h>
#include <pebble_fonts.h>

// Define the length in characters of the countdown string, including the NULL.

#define COUNTDOWN_STRING_LENGTH 6

// Define the format for the countdown string.

#define COUNTDOWN_STRING_FORMAT "%d:%02d"

// Define the countdown string. The API requires that this not be stack-based.

static char countdown_string[COUNTDOWN_STRING_LENGTH] = "";

// Define the various user interface elements comprising this view.

static TextLayer countdown_text_layer;
static TextLayer prompt_text_layer;
static ActionBarLayer action_bar;

// Define a structure to hold the window's bitmaps in a convenient aggregate.

static struct {
  HeapBitmap start;
  HeapBitmap restart;
  HeapBitmap abort;
} icons;

// Define a variable to hold the previous unload handler for chaining.

static WindowHandler previous_unload_handler;

// Private functions.

static void load_and_add_view(Window* window, ClickConfigProvider provider);
static void remove_and_unload_view(Window* window);

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  text_layer_set_text(&countdown_text_layer, "The curling iron is now off.");
  text_layer_set_text(&prompt_text_layer, "");

  vibes_long_pulse();
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  text_layer_set_text(&countdown_text_layer, "Ignoring...");
  text_layer_set_text(&prompt_text_layer, "");

  vibes_long_pulse();
}

void click_config_provider(ClickConfig **config, Window *window) {

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

// Public functions -----------------------------------------------------------

void countdown_view_init(Window* window, ClickConfigProvider provider) {
  load_and_add_view(window, provider);
  previous_unload_handler = window->window_handlers.unload;
  window_set_window_handlers(window, (WindowHandlers) {
    .unload = remove_and_unload_view
  });
}

void countdown_view_set_time_remaining_sec(unsigned int seconds) {
  unsigned int minutes_left = seconds / 60;
  unsigned int seconds_left = seconds % 60;

  snprintf(countdown_string, COUNTDOWN_STRING_LENGTH, COUNTDOWN_STRING_FORMAT,
           minutes_left, seconds_left);
  //text_layer_set_text(&countdown_text_layer, countdown_string);
}

void countdown_view_show_start() {
  action_bar_layer_set_icon(&action_bar, BUTTON_ID_SELECT, &icons.start.bmp);
}

void countdown_view_show_restart() {
  action_bar_layer_set_icon(&action_bar, BUTTON_ID_SELECT, &icons.restart.bmp);
}

void countdown_view_show_abort() {
  action_bar_layer_set_icon(&action_bar, BUTTON_ID_SELECT, &icons.abort.bmp);
}

// Private functions ----------------------------------------------------------

void load_and_add_view(Window* window, ClickConfigProvider provider) {
  light_enable_interaction();
  vibes_long_pulse();
  heap_bitmap_init(&icons.start, RESOURCE_ID_ICON_START);
  heap_bitmap_init(&icons.restart, RESOURCE_ID_ICON_RESTART);
  heap_bitmap_init(&icons.abort, RESOURCE_ID_ICON_ABORT);

  action_bar_layer_init(&action_bar);
   // action_bar_layer_set_icon(&action_bar, BUTTON_ID_SELECT, &icons.start.bmp);
  action_bar_layer_set_icon(&action_bar, BUTTON_ID_UP, &icons.restart.bmp);
  action_bar_layer_set_icon(&action_bar, BUTTON_ID_DOWN, &icons.abort.bmp);
  action_bar_layer_add_to_window(&action_bar, window);


  window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);

  // TBD: Is there a nicer way to do this? - JRS 8/16
  text_layer_init(&countdown_text_layer, GRect(0, 40, window->layer.frame.size.w - ACTION_BAR_WIDTH, 45));
  text_layer_set_text_alignment(&countdown_text_layer, GTextAlignmentCenter);
  text_layer_set_font(&countdown_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_color(&countdown_text_layer, GColorWhite);
  text_layer_set_background_color(&countdown_text_layer, GColorBlack);
  text_layer_set_overflow_mode(&countdown_text_layer, GTextOverflowModeWordWrap);

  text_layer_set_text(&countdown_text_layer, "You've left your curling iron on.");

  text_layer_init(&prompt_text_layer, GRect(5, 95, window->layer.frame.size.w - ACTION_BAR_WIDTH - 5, 45));
  text_layer_set_text_alignment(&prompt_text_layer, GTextAlignmentCenter);
  text_layer_set_font(&prompt_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_overflow_mode(&prompt_text_layer, GTextOverflowModeWordWrap);
  text_layer_set_text(&prompt_text_layer, "Would you like to turn it off?");

  layer_add_child(&window->layer, &prompt_text_layer.layer);
  layer_add_child(&window->layer, &countdown_text_layer.layer);
}

void remove_and_unload_view(Window* window) {
  layer_remove_from_parent(&countdown_text_layer.layer);
  action_bar_layer_remove_from_window(&action_bar);

  heap_bitmap_deinit(&icons.start);
  heap_bitmap_deinit(&icons.restart);
  heap_bitmap_deinit(&icons.abort);

  if (previous_unload_handler) {
    previous_unload_handler(window);
  }
}
