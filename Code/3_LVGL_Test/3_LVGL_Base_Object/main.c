/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_display_t *hal_init(lv_coord_t w, lv_coord_t h);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/
static lv_style_t switch_1_style;
/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * @brief 按键回调函数
 */
static void lv_switch_1_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    LV_LOG_USER("Clicked");
  }
}

/**
 * @brief 自定义GUI函数
 */
static void My_GUI(void)
{
  // 初始化样式
  lv_style_init(&switch_1_style);
  lv_style_set_bg_color(&switch_1_style, lv_color_hex(0xf4b183));

  lv_obj_t *switch_1 = lv_switch_create(lv_scr_act());
  lv_obj_set_size(switch_1, 80, 48);                                    // 设置按钮大小
  lv_obj_set_pos(switch_1, 100, 100);                                   // 设置按钮位置
  lv_obj_add_style(switch_1, &switch_1_style, LV_STATE_DEFAULT);        // 设置按钮样式
  lv_obj_add_event(switch_1, lv_switch_1_event_cb, LV_EVENT_ALL, NULL); // 设置按钮事件

  lv_obj_t *switch_2 = lv_switch_create(lv_scr_act());
  lv_obj_set_size(switch_2, 80, 48);
  lv_obj_align_to(switch_2, switch_1, LV_ALIGN_CENTER, 0, 100); // 设置对齐方式
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();
  hal_init(800, 480);
  My_GUI();

  while (1)
  {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    lv_timer_handler();
    usleep(1 * 1000);
  }

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static lv_display_t *hal_init(lv_coord_t w, lv_coord_t h)
{

  lv_group_set_default(lv_group_create());

  lv_display_t *disp = lv_sdl_window_create(w, h);
  lv_indev_t *mouse = lv_sdl_mouse_create();
  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_disp(mouse, disp);
  lv_display_set_default(disp);

  LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  lv_obj_t *cursor_obj;
  cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
  lv_image_set_src(cursor_obj, &mouse_cursor_icon); /*Set the image source*/
  lv_indev_set_cursor(mouse, cursor_obj);           /*Connect the image  object to the driver*/

  lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
  lv_indev_set_disp(mousewheel, disp);

  lv_indev_t *keyboard = lv_sdl_keyboard_create();
  lv_indev_set_disp(keyboard, disp);
  lv_indev_set_group(keyboard, lv_group_get_default());
  return disp;
}
