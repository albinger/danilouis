#include <pebble.h>
#include <stdlib.h>
  
Window *window;
TextLayer *text_layer;
static PropertyAnimation *s_property_animation;
static int XXX, YYY, minute;
char s_minute[3];

static void get_coordinates() {
  if(minute < 8){
    XXX=60+(65.0/7.0*minute);
    YYY=-7;
  }else if(minute < 10){
    XXX=123;
    YYY = (2+(139.0/16.0 * (minute -8)));
  }else if (minute < 24){
    XXX = 119;
    YYY = (148.0/16.0 * (minute -8));
  }else if (minute < 38){
    YYY = 138;
    XXX = (121 - ( 121.0/14.0 * (minute - 23)));
  }else if (minute < 54){
    XXX = 0;
    YYY = (138 - (140/15 * (minute - 37)));
  }else{
    YYY = -7;
    XXX = 60.0 / 7.0 * (minute - 53);
  }
}

#ifdef PBL_PLATFORM_APLITE
  // Animation handler
static void animationStoppedHandler(struct Animation *animation, bool finished, void *context)
{

  property_animation_destroy((PropertyAnimation *) context);
}
#endif

  static void trigger_custom_animation() {
 
  GRect from_frame = layer_get_frame((Layer *)text_layer);
  GRect to_frame = GRect(XXX, YYY, 25, 30);

  // Create the animation
  s_property_animation = property_animation_create_layer_frame((Layer *)text_layer, &from_frame, &to_frame);

  // Schedule to occur ASAP with default settings
#ifdef PBL_PLATFORM_APLITE
  animation_set_handlers((Animation *)s_property_animation, (AnimationHandlers){
    .stopped = (AnimationStoppedHandler)animationStoppedHandler}, s_property_animation);
#endif
  animation_schedule((Animation*) s_property_animation);

  APP_LOG(APP_LOG_LEVEL_DEBUG,"x:%i y:%i m:%i", XXX, YYY, minute);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(minute == 59){
    minute = 0;
  }else{
    minute++;
  }
  get_coordinates();
  trigger_custom_animation();
  snprintf(s_minute,3,"%i",minute);
  text_layer_set_text(text_layer, s_minute);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  minute = 0;
  get_coordinates();
  trigger_custom_animation();
  snprintf(s_minute,3,"%i",minute);
  text_layer_set_text(text_layer, s_minute);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(minute == 0){
    minute = 59;
  }else{
    minute--;
  }
  get_coordinates();
  trigger_custom_animation();
  snprintf(s_minute,3,"%i",minute);
  text_layer_set_text(text_layer, s_minute);
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void handle_init(void) {
  
  minute = 0;
  XXX = 60;
  YYY=-7;
	// Create a window and text layer
	window = window_create();
	text_layer = text_layer_create(GRect(XXX, YYY, 25, 30));
 
	
	// Set the text, font, and text alignment
	snprintf(s_minute,3,"0");
  text_layer_set_text(text_layer, s_minute);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));

	// Push the window
	window_stack_push(window, true);
  window_set_click_config_provider(window, click_config_provider);	
}

void handle_deinit(void) {
	// Destroy the text layer
	text_layer_destroy(text_layer);
	
	// Destroy the window
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
