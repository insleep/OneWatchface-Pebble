#include "pebble.h"
  
// App-specific data
Window *window;
TextLayer *time_layer; 
TextLayer *date_layer;
TextLayer *battery_layer;
TextLayer *bt_layer;

#define KEY_INVERT 0

static void in_recv_handler(DictionaryIterator *iterator, void *context)
{
  // Get Tuple
  Tuple *t = dict_read_first(iterator);
  if(t)
  {
    switch(t->key)
    {
    case KEY_INVERT:
      // KEY_INVERT key
      if(strcmp(t->value->cstring, "on") == 0)
      {
        // Set and save as inverted
        text_layer_set_text_color(time_layer, GColorBlack);
        text_layer_set_text_color(date_layer, GColorBlack);
        text_layer_set_text_color(bt_layer, GColorBlack);
        text_layer_set_text_color(battery_layer, GColorBlack);
        window_set_background_color(window, GColorWhite);

        persist_write_bool(KEY_INVERT, true);
      }
      else if(strcmp(t->value->cstring, "off") == 0)
      {
        // Set and save as not inverted
        text_layer_set_text_color(time_layer, GColorWhite);
        text_layer_set_text_color(date_layer, GColorWhite);
        text_layer_set_text_color(bt_layer, GColorWhite);
        text_layer_set_text_color(battery_layer, GColorWhite);
        window_set_background_color(window, GColorBlack);

        persist_write_bool(KEY_INVERT, false);
      }
      break;
    }
  }
}
  
static void handle_bt(bool connected) {
  text_layer_set_text(bt_layer, connected ? "connected" : "disconnected");
}

static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "charging";

  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "charging");
  } else {
    snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
  }
  text_layer_set_text(battery_layer, battery_text);
}

// Called once per second
static void handle_datetime(struct tm* tick_time, TimeUnits units_changed) {
  static char time_text[] = "00:00"; // Needs to be static because it's used by the system later.

  if(clock_is_24h_style() == true) {
    // Use 12 hour format
    strftime(time_text, sizeof("00:00"), "%H:%M", tick_time);
    text_layer_set_text(time_layer, time_text);
  } else {
    // Use 12 hour format
    strftime(time_text, sizeof("00:00"), "%I:%M", tick_time);
    text_layer_set_text(time_layer, time_text);
  }
  
  static char date_text[] = "Loading...";
  strftime(date_text, sizeof(date_text), "%m-%d-%y", tick_time);
  text_layer_set_text(date_layer, date_text);

    if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(time_layer, time_text);
  
  handle_battery(battery_state_service_peek());
}

// Handle the start-up of the watchface
static void init(void) {

  // Create the watchface's base window
  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);

  Layer *root_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(root_layer);

  #if defined(PBL_ROUND)
    // Initialize the text layer used to show the time
    time_layer = text_layer_create(GRect(0, 50, frame.size.w /* width */, 50 /* height */));
    text_layer_set_text_color(time_layer, GColorWhite);
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
    
    date_layer = text_layer_create(GRect(0, 100, frame.size.w /* width */, 50 /* height */));
    text_layer_set_text_color(date_layer, GColorWhite);
    text_layer_set_background_color(date_layer, GColorClear);
    text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
    text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
    
    bt_layer = text_layer_create(GRect(0, 10, frame.size.w /* width */, 34 /* height */));
    text_layer_set_text_color(bt_layer, GColorWhite);
    text_layer_set_background_color(bt_layer, GColorClear);
    text_layer_set_font(bt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(bt_layer, GTextAlignmentCenter);
    handle_bt(bluetooth_connection_service_peek());
    
    battery_layer = text_layer_create(GRect(0, 150, frame.size.w /* width */, 34 /* height */));
    text_layer_set_text_color(battery_layer, GColorWhite);
    text_layer_set_background_color(battery_layer, GColorClear);
    text_layer_set_font(battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(battery_layer, GTextAlignmentCenter);
    text_layer_set_text(battery_layer, "charging");
  #elif defined(PBL_RECT)
    // Initialize the text layer used to show the time
    time_layer = text_layer_create(GRect(0, 50, frame.size.w /* width */, 50 /* height */));
    text_layer_set_text_color(time_layer, GColorWhite);
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
    
    date_layer = text_layer_create(GRect(0, 100, frame.size.w /* width */, 50 /* height */));
    text_layer_set_text_color(date_layer, GColorWhite);
    text_layer_set_background_color(date_layer, GColorClear);
    text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
    text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
    
    bt_layer = text_layer_create(GRect(0, 0, /* width */ frame.size.w, 34 /* height */));
    text_layer_set_text_color(bt_layer, GColorWhite);
    text_layer_set_background_color(bt_layer, GColorClear);
    text_layer_set_font(bt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(bt_layer, GTextAlignmentLeft);
    handle_bt(bluetooth_connection_service_peek());
    
    battery_layer = text_layer_create(GRect(0, 0, frame.size.w /* width */, 34 /* height */));
    text_layer_set_text_color(battery_layer, GColorWhite);
    text_layer_set_background_color(battery_layer, GColorClear);
    text_layer_set_font(battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(battery_layer, GTextAlignmentRight);
    text_layer_set_text(battery_layer, "charging");
  #endif
  
  // Check for saved option
  bool inverted = persist_read_bool(KEY_INVERT);
  
  // Option-specific setup
  if(inverted == true)
  {
    
    text_layer_set_text_color(time_layer, GColorBlack);
    text_layer_set_text_color(date_layer, GColorBlack);
    text_layer_set_text_color(bt_layer, GColorBlack);
    text_layer_set_text_color(battery_layer, GColorBlack);
    window_set_background_color(window, GColorWhite);
  }
  else
  {
    text_layer_set_text_color(time_layer, GColorWhite);
    text_layer_set_text_color(date_layer, GColorWhite);
    text_layer_set_text_color(bt_layer, GColorWhite);
    text_layer_set_text_color(battery_layer, GColorWhite);
    window_set_background_color(window, GColorBlack);
  }
  
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_datetime(current_time, MINUTE_UNIT);

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_datetime);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bt);

  layer_add_child(root_layer, text_layer_get_layer(time_layer));
  layer_add_child(root_layer, text_layer_get_layer(date_layer));
  layer_add_child(root_layer, text_layer_get_layer(bt_layer));
  layer_add_child(root_layer, text_layer_get_layer(battery_layer));
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(bt_layer);
  text_layer_destroy(battery_layer);
  window_destroy(window);
}

// The main event/run loop for the watchface
int main(void) {
  init();
  app_event_loop();
  deinit();
}