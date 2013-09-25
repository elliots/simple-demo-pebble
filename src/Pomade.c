// ----------------------------------------------------------------------------
// Pomade - A pomodoro-tracking Pebble watchapp with statistics on your wrist
// Copyright (c) 2013 Jonathan Speicher (jon.speicher@gmail.com)
// Licensed under the MIT license: http://opensource.org/licenses/MIT
// ----------------------------------------------------------------------------

#include <pebble_os.h>
#include <pebble_app.h>

#include "cookies.h"
#include "pomodoro_controller.h"
#include "version.h"

// Define the app's unique identifier and metadata.

#define APP_UUID { 0xEB, 0x43, 0x36, 0xB1, 0x3B, 0x99, 0x49, 0xC3, 0x90, 0x9E, 0x81, 0x6B, 0x19, 0xF1, 0x6A, 0xCE }

PBL_APP_INFO(APP_UUID, APP_NAME, APP_AUTHOR,
             APP_VERSION_MAJOR, APP_VERSION_MINOR,
             RESOURCE_ID_APP_MENU_ICON, APP_INFO_STANDARD_APP);

// Event handlers -------------------------------------------------------------

void handle_init(AppContextRef ctx) {
  resource_init_current_app(&APP_RESOURCES);
  pomodoro_controller_init(ctx);
}

void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
  pomodoro_controller_timer_event(handle, cookie);
}

// main -----------------------------------------------------------------------

void pbl_main(void* params) {
  PebbleAppHandlers handlers = {
    .init_handler = handle_init,
    .timer_handler = handle_timer
  };
  app_event_loop(params, &handlers);
}
