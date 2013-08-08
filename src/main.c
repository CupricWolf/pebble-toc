#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x00, 0xAB, 0x2D, 0x4E, 0x22, 0xA5, 0x4E, 0xA6, 0xA7, 0x28, 0x0A, 0x64, 0x29, 0xAC, 0xC1, 0xED }
PBL_APP_INFO(MY_UUID,
             "Pebble-Toc", "Happydude11209",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

BmpContainer background_image_container;

RotBmpPairContainer minute_hand_image_container;
RotBmpPairContainer hour_hand_image_container;

void update_watch(PblTm* t){

minute_hand_image_container.layer.white_layer.rotation = TRIG_MAX_ANGLE * (t->tm_min * 6) / 360;
minute_hand_image_container.layer.black_layer.rotation = TRIG_MAX_ANGLE * (t->tm_min * 6) / 360;
minute_hand_image_container.layer.layer.frame.origin.x = (144/2) - (minute_hand_image_container.layer.layer.frame.size.w/2);
minute_hand_image_container.layer.layer.frame.origin.y = (168/2) - (minute_hand_image_container.layer.layer.frame.size.h/2);
layer_mark_dirty(&minute_hand_image_container.layer.layer);	

hour_hand_image_container.layer.white_layer.rotation = TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 30) + (t->tm_min/2)) / 360;
hour_hand_image_container.layer.black_layer.rotation = TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 30) + (t->tm_min/2)) / 360;
hour_hand_image_container.layer.layer.frame.origin.x = (144/2) - (hour_hand_image_container.layer.layer.frame.size.w/2);
hour_hand_image_container.layer.layer.frame.origin.y = (168/2) - (hour_hand_image_container.layer.layer.frame.size.h/2);
layer_mark_dirty(&hour_hand_image_container.layer.layer);

}

// Called once per second
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
update_watch(t->tick_time);
}


// Handle the start-up of the app
void handle_init(AppContextRef app_ctx) {

// Create our app's base window
window_init(&window, "Trails Watch");
window_stack_push(&window, true);
window_set_background_color(&window, GColorBlack);

resource_init_current_app(&APP_RESOURCES);

// Set up a layer for the static watch face background
bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image_container);
layer_add_child(&window.layer, &background_image_container.layer.layer);

// Set up a layer for the minute hand with transparency, ensure the source image has a transparent area
rotbmp_pair_init_container(RESOURCE_ID_IMAGE_MINUTE_ROTOR_WHITE, RESOURCE_ID_IMAGE_MINUTE_ROTOR_BLACK, &minute_hand_image_container);
rotbmp_pair_layer_set_src_ic(&minute_hand_image_container.layer, GPoint(111, 111));
layer_add_child(&window.layer, &minute_hand_image_container.layer.layer);	

// Set up a layer for the hour hand with transparency, ensure the source image has a transparent area
rotbmp_pair_init_container(RESOURCE_ID_IMAGE_HOUR_ROTOR_WHITE, RESOURCE_ID_IMAGE_HOUR_ROTOR_BLACK, &hour_hand_image_container);
rotbmp_pair_layer_set_src_ic(&hour_hand_image_container.layer, GPoint(55, 55));
layer_add_child(&window.layer, &hour_hand_image_container.layer.layer);

PblTm t;
get_time(&t);
update_watch(&t);

}

void handle_deinit(AppContextRef ctx) {

bmp_deinit_container(&background_image_container);
rotbmp_pair_deinit_container(&minute_hand_image_container);
rotbmp_pair_deinit_container(&hour_hand_image_container);
}


// The main event/run loop for our app
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}