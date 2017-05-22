#include <pebble.h>
#include <ctype.h>

#define LOCATION_DELAY 4500

char background_color_hex_char[10] = "000000"; 
char font_color_hex_char[10] = "FFFFFF";
char highlight_color_hex_char[10] = "00FFAA";
char location_color_hex_char[10] = "FFFF00";

GColor backgroundGColor;
GColor fontGColor;
GColor locationGColor;

int vibrate_status = 2;
int temp_units = 0;
int location_status = 0;
int zip_code_int = 0;
int background_color_hex_int;
int font_color_hex_int;
int highlight_color_hex_int;
int location_color_hex_int;
int temp_to_store = 0;
int middle_outline_status = 1;
int update_interval_val = 60;
int sleep_status = 0;
int sleep_start_hour = 0;
int sleep_end_hour = 0;
int battery_meter_status = 1;

char latitude_to_store[15] = "0";
char longitude_to_store[15] = "0";

bool is_in_sleep = false;
bool show_bluetooth_status = true;
bool hide_location;

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_POS 2

//settings keys
#define KEY_HIGHLIGHT_COLOR 3
#define KEY_LOCATION_COLOR 4
#define KEY_VIBRATE 5
#define KEY_UNIT 6
#define KEY_LOCATION 7
#define KEY_ZIP_CODE 8
#define KEY_MIDDLE_OUTLINE 9
#define KEY_LATITUDE 10
#define KEY_LONGITUDE 11
#define KEY_UPDATE_INTERVAL 12
#define KEY_SLEEP 13
#define KEY_SLEEP_START 14
#define KEY_SLEEP_END 15
#define KEY_BATTERY_METER 16
#define KEY_JSREADY 17
#define KEY_BACKGROUND_COLOR 18
#define KEY_FONT_COLOR 19
#define KEY_POS_ERROR 20

static Window *window;
static Layer *canvas_layer;
static Layer *text_layer;
static TextLayer *location_text_layer;
static TextLayer *date_text_layer;
static TextLayer *temperature_text_layer;
static TextLayer *steps_text_layer;
static TextLayer *battery_text_layer;

static BitmapLayer *weather_bitmap_layer;
static GBitmap *cloud_bitmap;
static BitmapLayer *steps_bitmap_layer;
static GBitmap *shoe_bitmap;
static BitmapLayer *battery_bitmap_layer;
static GBitmap *battery_bitmap;
static BitmapLayer *pin_bitmap_layer;
static GBitmap *pin_bitmap;

static GFont font_myriad_14;
static GFont font_myriad_13;

static bool js_ready;

char* short_months[] = {"Jan","Feb","Mar","Apr","May","June","July","Aug","Sep","Oct","Nov","Dec"};
char* long_months[] = {"January","February","March","April","May","June","July","August","September","October","November","December"};
static char date_buffer[30];
static char temperature_buffer[8];
static char conditions_buffer[100];
static char location_buffer[100];
static char steps_buffer[10];
static char battery_buffer[10];

int numbers_to_print[4] = {0,0,0,0};

static int digit_matrix[10][7][6] = 
{
  {
    {0, 1, 1, 1, 0}, //0
    {1, 0, 0, 0, 1},
    {1, 0, 0, 1, 1},
    {1, 0, 1, 0, 1},
    {1, 1, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0}
  },
  {
    {0, 0, 1, 0, 0}, // 1
    {0, 1, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 1, 1, 0}
  },
  {
    {0, 1, 1, 1, 0}, //2
    {1, 0, 0, 0, 1},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0},
    {1, 1, 1, 1, 1}
  },
  {
    {1, 1, 1, 1, 1}, //3
    {0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0}
  },
  {
    {0, 0, 0, 1, 0}, //4
    {0, 0, 1, 1, 0},
    {0, 1, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 1, 1, 1, 1},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 1, 0}
  },
  {
    {1, 1, 1, 1, 1}, //5
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0}
  },
  {
    {0, 0, 1, 1, 0}, //6
    {0, 1, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0}
  },
  {
    {1, 1, 1, 1, 1}, //7
    {0, 0, 0, 0, 1},
    {0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0}
  },
  {
    {0, 1, 1, 1, 0}, //8
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0}
  },
  {
    {0, 1, 1, 1, 0}, //9
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 1},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 1, 0},
    {0, 1, 1, 0, 0}  
  } 
};

unsigned int HexStringToUInt(char const* hexstring)
{
  unsigned int result = 0;
  char const *c = hexstring;
  unsigned char thisC;
  while( (thisC = *c) != 0 )
  {
    thisC = toupper(thisC);
    result <<= 4;
    if( isdigit(thisC))
      result += thisC - '0';
    else if(isxdigit(thisC))
      result += thisC - 'A' + 10;
    else
    {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "ERROR: Unrecognized hex character \"%c\"", thisC);
      return 0;
    }
    ++c;
  }
  return result;  
}

static void requestWeather(void) 
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "requestWeather()");

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  dict_write_uint32(iter, KEY_UNIT, temp_units);  
  dict_write_uint32(iter, KEY_LOCATION, location_status);  
  dict_write_uint32(iter, KEY_ZIP_CODE, zip_code_int);  
  dict_write_cstring(iter, KEY_LATITUDE, latitude_to_store);
  dict_write_cstring(iter, KEY_LONGITUDE, longitude_to_store);  

  app_message_outbox_send();
}

static void health_handler(HealthEventType event, void *context) {
  int today_steps = (int)health_service_sum_today(HealthMetricStepCount);
  snprintf(steps_buffer, sizeof(steps_buffer), "%d", today_steps);
  text_layer_set_text(steps_text_layer, steps_buffer);
}

static void update_time()
{
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  int normal_hours = 0;

  if (clock_is_24h_style())
  {
    numbers_to_print[0] = tick_time->tm_hour/10;
    numbers_to_print[1] = tick_time->tm_hour-(10*numbers_to_print[0]);
  }
  else
  {
    normal_hours = tick_time->tm_hour;

    if (tick_time->tm_hour > 12)
    {    
      normal_hours = tick_time->tm_hour-12;
    }
    if (tick_time->tm_hour == 0)
    {
      normal_hours = 12;
    }

    numbers_to_print[0] = (normal_hours/10);
    numbers_to_print[1] = (normal_hours-(10*numbers_to_print[0]));
  }

  numbers_to_print[2] = tick_time->tm_min/10;
  numbers_to_print[3] = tick_time->tm_min-(10*numbers_to_print[2]);

  strftime(date_buffer, sizeof(date_buffer), "%A, %B %e",tick_time);
  text_layer_set_text(date_text_layer,date_buffer);

  if(canvas_layer)
    layer_mark_dirty(canvas_layer);

  if(tick_time->tm_min % update_interval_val == 0 && tick_time->tm_sec % 60 == 0 && is_in_sleep != true)
  {
    requestWeather();
  }

  if(tick_time->tm_min % 60 == 0 && tick_time->tm_sec % 60 == 0 && is_in_sleep != true)
  {
    switch (vibrate_status)
    {
      case 0:
        vibes_short_pulse();
        break;
      case 1:
        vibes_long_pulse();
        break;
      default:
        break;
    }
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{  
  Tuple *ready = dict_find(iterator, KEY_JSREADY);
  Tuple *pos_error = dict_find(iterator, KEY_POS_ERROR);

  if(ready && js_ready != true)
  {
    APP_LOG(APP_LOG_LEVEL_INFO, "JS ready!");
    js_ready = true;
    requestWeather();
  }
  else if(pos_error)
  {
    APP_LOG(APP_LOG_LEVEL_INFO, "Error getting position");
    snprintf(location_buffer, sizeof(location_buffer), "Set Location Manually");
  }
  else
  {
    Tuple *t = dict_read_first(iterator);

    APP_LOG(APP_LOG_LEVEL_INFO, "AppMessage Received");

    while(t != NULL) {
      switch (t->key) {
        case KEY_TEMPERATURE:
          temp_to_store = (int)t->value->int32;
          if(temp_to_store < 100)
            snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", (int)t->value->int32);
          else
            snprintf(temperature_buffer, sizeof(temperature_buffer), "%d", (int)t->value->int32);
          break;

        case KEY_CONDITIONS:
          snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
          break;

        case KEY_POS:
          snprintf(location_buffer, sizeof(location_buffer), "%s", t->value->cstring);
          break;

        case KEY_HIGHLIGHT_COLOR:
          snprintf(highlight_color_hex_char, sizeof(highlight_color_hex_char), "%s", t->value->cstring);    
          break;

        case KEY_LOCATION_COLOR:
          snprintf(location_color_hex_char, sizeof(location_color_hex_char), "%s", t->value->cstring);    
          break;

        case KEY_VIBRATE:
          vibrate_status = (int)t->value->int32;
          break;

        case KEY_UNIT:
          temp_units = (int)t->value->int32;
          break;
        
        case KEY_LOCATION:
          location_status = (int)t->value->int32;
          break;      
          
        case KEY_ZIP_CODE:
          zip_code_int = (int)t->value->int32;
          break;
        
        case KEY_MIDDLE_OUTLINE:
          middle_outline_status = (int)t->value->int32;
          break;
          
        case KEY_LATITUDE:
          snprintf(latitude_to_store, sizeof(latitude_to_store), "%s", t->value->cstring);
          break;
          
        case KEY_LONGITUDE:
          snprintf(longitude_to_store, sizeof(longitude_to_store), "%s", t->value->cstring);
          break;
        
        case KEY_UPDATE_INTERVAL:
          update_interval_val = (int)t->value->int32;
          break;
        
        case KEY_SLEEP:
          sleep_status = (int)t->value->int32;
          break;
        
        case KEY_SLEEP_START:
          sleep_start_hour = (int)t->value->int32;
          break;
        
        case KEY_SLEEP_END:
          sleep_end_hour = (int)t->value->int32;
          break;
        
        case KEY_BATTERY_METER:
          battery_meter_status = (int)t->value->int32;
          APP_LOG(APP_LOG_LEVEL_INFO, "battery_meter: %d", battery_meter_status);
          break;
        
        default:
          APP_LOG(APP_LOG_LEVEL_ERROR, "Key%d not recognized", (int)t->key);
          break;
      }
      t = dict_read_next(iterator);
    }
  }

  layer_mark_dirty(canvas_layer);
  layer_mark_dirty(text_layer);

  //update_time();
}

static void inbox_dropped_callback(AppMessageResult reason, void *context)
{ 
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void canvas_layer_update_proc(Layer *this_layer, GContext *ctx)
{
  background_color_hex_int = HexStringToUInt(background_color_hex_char);
  font_color_hex_int = HexStringToUInt(font_color_hex_char);
  highlight_color_hex_int = HexStringToUInt(highlight_color_hex_char);
  location_color_hex_int = HexStringToUInt(location_color_hex_char);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);

  fontGColor = GColorWhite;
  backgroundGColor = GColorBlack;

  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorFromHEX(highlight_color_hex_int));
  #else
    graphics_context_set_fill_color(ctx, fontGColor);
  #endif

  int x_start[4] = {10, 40, 80, 110};
  int y_start[4] = {62, 62, 62, 62};
  int x_offset = 4;
  int y_offset = 4;

  for (int x=0; x<5; x++)
  {
    for (int y=0; y<7; y++)
    {
      for (int k=0; k<4; k++)
      {
        if (digit_matrix[numbers_to_print[k]][y][x] == 1)
        {
          graphics_fill_rect(ctx, GRect(x_start[k]+x_offset*x+x, y_start[k]+y_offset*y+y, x_offset, y_offset), 0, GCornerNone);  
        }
      }
    }
  }
  graphics_fill_rect(ctx, GRect(70,72,4,4), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(70,92,4,4), 0, GCornerNone);

  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorFromHEX(highlight_color_hex_int));
  #else
    graphics_context_set_fill_color(ctx, fontGColor);
  #endif

  graphics_fill_circle(ctx, GPoint(24,30),21);
  graphics_fill_circle(ctx, GPoint(72,30),21);
  graphics_fill_circle(ctx, GPoint(120,30),21);

  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorBlack);
  #else
    graphics_context_set_fill_color(ctx, backgroundGColor);
  #endif

  graphics_fill_circle(ctx, GPoint(24,30),19);
  graphics_fill_circle(ctx, GPoint(72,30),19);
  graphics_fill_circle(ctx, GPoint(120,30),19);

  if(temp_to_store < 100 && temp_units == 1)
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°F", temp_to_store);
  else if(temp_to_store < 100 && temp_units == 0)
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°C", temp_to_store);
  else if(temp_to_store > 100)
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", temp_to_store);
  
  text_layer_set_text(temperature_text_layer, temperature_buffer);
  text_layer_set_text(location_text_layer, location_buffer);

  BatteryChargeState charge_state = battery_state_service_peek();
  snprintf(battery_buffer, sizeof(battery_buffer), "%d%%", charge_state.charge_percent);
  text_layer_set_text(battery_text_layer, battery_buffer);


  #ifdef PBL_COLOR
    text_layer_set_text_color(date_text_layer, GColorWhite);
    text_layer_set_text_color(location_text_layer, GColorWhite);
    text_layer_set_text_color(temperature_text_layer, GColorWhite);
    text_layer_set_text_color(steps_text_layer, GColorWhite);
    text_layer_set_text_color(battery_text_layer, GColorWhite);
    text_layer_set_text_color(location_text_layer, GColorFromHEX(location_color_hex_int));
  #else
    text_layer_set_text_color(date_text_layer, fontGColor);
    text_layer_set_text_color(location_text_layer, fontGColor);
    text_layer_set_text_color(temperature_text_layer, fontGColor);
    text_layer_set_text_color(steps_text_layer, fontGColor);
    text_layer_set_text_color(battery_text_layer, fontGColor);
    text_layer_set_text_color(location_text_layer, fontGColor);
  #endif
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  text_layer = window_get_root_layer(window);

  canvas_layer = layer_create(GRect(0, 0, 144, 168));
  layer_add_child(window_layer, canvas_layer);
  layer_set_update_proc(canvas_layer, canvas_layer_update_proc);

  date_text_layer = text_layer_create(GRect(0, 105, 144, 25));
  text_layer_set_text_alignment(date_text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(date_text_layer, GColorClear);
  text_layer_set_font(date_text_layer, font_myriad_14);
  layer_add_child(text_layer, text_layer_get_layer(date_text_layer));

  location_text_layer = text_layer_create(GRect(0, 145, 144, 25));
  text_layer_set_text_alignment(location_text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(location_text_layer, GColorClear);
  text_layer_set_font(location_text_layer, font_myriad_14);
  layer_add_child(text_layer, text_layer_get_layer(location_text_layer));

  weather_bitmap_layer = bitmap_layer_create(GRect(11,14,25,15));
  cloud_bitmap = gbitmap_create_with_resource(RESOURCE_ID_cloud);
  bitmap_layer_set_bitmap(weather_bitmap_layer, cloud_bitmap);
  bitmap_layer_set_alignment(weather_bitmap_layer, GAlignCenter);
  bitmap_layer_set_compositing_mode(weather_bitmap_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(weather_bitmap_layer));
  
  temperature_text_layer = text_layer_create(GRect(10, 29, 29, 20));
  text_layer_set_text_alignment(temperature_text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(temperature_text_layer, GColorClear);
  text_layer_set_font(temperature_text_layer, font_myriad_13);
  layer_add_child(text_layer, text_layer_get_layer(temperature_text_layer));
  
  steps_bitmap_layer = bitmap_layer_create(GRect(56,14,32,15));
  shoe_bitmap = gbitmap_create_with_resource(RESOURCE_ID_shoe);
  bitmap_layer_set_bitmap(steps_bitmap_layer, shoe_bitmap);
  bitmap_layer_set_alignment(steps_bitmap_layer, GAlignCenter);
  bitmap_layer_set_compositing_mode(steps_bitmap_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(steps_bitmap_layer));

  steps_text_layer = text_layer_create(GRect(56, 29, 33, 20));
  text_layer_set_text_alignment(steps_text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(steps_text_layer, GColorClear);
  text_layer_set_font(steps_text_layer, font_myriad_13);
  layer_add_child(text_layer, text_layer_get_layer(steps_text_layer));

  battery_bitmap_layer = bitmap_layer_create(GRect(105,15,32,15));
  battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_battery);
  bitmap_layer_set_bitmap(battery_bitmap_layer, battery_bitmap);
  bitmap_layer_set_alignment(battery_bitmap_layer, GAlignCenter);
  bitmap_layer_set_compositing_mode(battery_bitmap_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(battery_bitmap_layer));
  
  battery_text_layer = text_layer_create(GRect(103, 29, 38, 20));
  text_layer_set_text_alignment(battery_text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(battery_text_layer, GColorClear);
  text_layer_set_font(battery_text_layer, font_myriad_13);
  layer_add_child(text_layer, text_layer_get_layer(battery_text_layer));

  pin_bitmap_layer = bitmap_layer_create(GRect(0,130,144,18));
  pin_bitmap = gbitmap_create_with_resource(RESOURCE_ID_pin);
  bitmap_layer_set_bitmap(pin_bitmap_layer, pin_bitmap);
  bitmap_layer_set_alignment(pin_bitmap_layer, GAlignCenter);
  bitmap_layer_set_compositing_mode(pin_bitmap_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(pin_bitmap_layer));
}

static void window_unload(Window *window) {
  layer_destroy(canvas_layer);
  text_layer_destroy(date_text_layer);
  text_layer_destroy(temperature_text_layer);
  gbitmap_destroy(cloud_bitmap);
  bitmap_layer_destroy(weather_bitmap_layer);
  gbitmap_destroy(shoe_bitmap);
  bitmap_layer_destroy(steps_bitmap_layer);
  gbitmap_destroy(battery_bitmap);
  bitmap_layer_destroy(battery_bitmap_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) 
{
  if(tick_time->tm_sec % 60 == 0) 
  {
    update_time();
  }
}

static void init(void) {
  if (persist_exists(KEY_TEMPERATURE))
  {
    temp_to_store = persist_read_int(KEY_TEMPERATURE);
  }
  if(persist_exists(KEY_POS))
  {
    persist_read_string(KEY_POS, location_buffer, 100);
  }
  if (persist_exists(KEY_UNIT))
  {
    temp_units = persist_read_int(KEY_UNIT);
  }
  if (persist_exists(KEY_VIBRATE))
  {
    vibrate_status = persist_read_int(KEY_VIBRATE);
  }
  if (persist_exists(KEY_LOCATION))
  {
    location_status = persist_read_int(KEY_LOCATION);
  }
  if (persist_exists(KEY_ZIP_CODE))
  {
    zip_code_int = persist_read_int(KEY_ZIP_CODE);
  }
  if (persist_exists(KEY_HIGHLIGHT_COLOR))
  {
    persist_read_string(KEY_HIGHLIGHT_COLOR, highlight_color_hex_char, 10);
  }
  if (persist_exists(KEY_LOCATION_COLOR))
  {
    persist_read_string(KEY_LOCATION_COLOR, location_color_hex_char, 10);
  }
  if (persist_exists(KEY_BACKGROUND_COLOR))
  {
    persist_read_string(KEY_BACKGROUND_COLOR, background_color_hex_char, 10);
  }
  if (persist_exists(KEY_FONT_COLOR))
  {
    persist_read_string(KEY_FONT_COLOR, font_color_hex_char, 10);
  }
  if (persist_exists(KEY_MIDDLE_OUTLINE))
  {
    middle_outline_status = persist_read_int(KEY_MIDDLE_OUTLINE);
  }
  if (persist_exists(KEY_LATITUDE))
  {
    persist_read_string(KEY_LATITUDE, latitude_to_store, 15);
  }
  if (persist_exists(KEY_LONGITUDE))
  {
    persist_read_string(KEY_LONGITUDE, longitude_to_store, 15);
  }  
  if (persist_exists(KEY_UPDATE_INTERVAL))
  {
    update_interval_val = persist_read_int(KEY_UPDATE_INTERVAL);
  }

  if (persist_exists(KEY_SLEEP))
  {
    sleep_status = persist_read_int(KEY_SLEEP);
  }
  if (persist_exists(KEY_SLEEP_START))
  {
    sleep_start_hour = persist_read_int(KEY_SLEEP_START);
  }
  if (persist_exists(KEY_SLEEP_END))
  {
    sleep_end_hour = persist_read_int(KEY_SLEEP_END);
  }
  if (persist_exists(KEY_BATTERY_METER))
  {
    battery_meter_status = persist_read_int(KEY_BATTERY_METER);
  }

  background_color_hex_int = HexStringToUInt(background_color_hex_char);
  font_color_hex_int = HexStringToUInt(font_color_hex_char);

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  font_myriad_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MYRIAD_LIGHT_14));
  font_myriad_13 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MYRIAD_LIGHT_13));

  window_stack_push(window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  health_service_events_subscribe(health_handler, NULL);
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  #ifdef PBL_PLATFORM_APLITE
  app_message_open(64, 64);
  #else
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  #endif
  
  update_time();
}

static void deinit(void) {
  persist_write_int(KEY_TEMPERATURE, temp_to_store);
  persist_write_string(KEY_POS, location_buffer);
  persist_write_int(KEY_UNIT, temp_units);
  persist_write_int(KEY_VIBRATE, vibrate_status);
  persist_write_int(KEY_LOCATION, location_status);
  persist_write_int(KEY_ZIP_CODE, zip_code_int);
  persist_write_string(KEY_HIGHLIGHT_COLOR, highlight_color_hex_char);
  persist_write_string(KEY_LOCATION_COLOR, location_color_hex_char);
  persist_write_string(KEY_BACKGROUND_COLOR, background_color_hex_char);
  persist_write_string(KEY_FONT_COLOR, font_color_hex_char);
  persist_write_int(KEY_MIDDLE_OUTLINE, middle_outline_status);
  persist_write_string(KEY_LATITUDE, latitude_to_store);
  persist_write_string(KEY_LONGITUDE, longitude_to_store);
  persist_write_int(KEY_UPDATE_INTERVAL, update_interval_val);
  persist_write_int(KEY_SLEEP, sleep_status);
  persist_write_int(KEY_SLEEP_START, sleep_start_hour);
  persist_write_int(KEY_SLEEP_END, sleep_end_hour);
  persist_write_int(KEY_BATTERY_METER, battery_meter_status);

  window_destroy(window);
  tick_timer_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
