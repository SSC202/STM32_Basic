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

/**********************
 *  STATIC PROTOTYPES
 **********************/
/**
 * @brief 自定义Switch GUI函数
 */
static void My_Demo_Switch_GUI(void)
{
  lv_obj_t *switch_1 = lv_switch_create(lv_scr_act());
  lv_obj_set_size(switch_1, 100, 50);                                            // 设置开关大小
  lv_obj_set_style_bg_color(switch_1, lv_color_hex(0xffe1d4), LV_STATE_DEFAULT); // 设置开关颜色
  lv_obj_add_state(switch_1, LV_STATE_CHECKED | LV_STATE_EDITED);                // 默认打开且可修改
}

/**
 * @brief 自定义Button GUI函数
 */
static void My_Demo_Button_GUI(void)
{
  lv_obj_t *button = lv_button_create(lv_scr_act());
  lv_obj_set_size(button, 100, 50);                                            // 设置按钮大小
  lv_obj_set_style_bg_color(button, lv_color_hex(0xffe1d4), LV_STATE_DEFAULT); // 设置按钮颜色
  lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);                              // 开启状态切换
}

/**
 * @brief 自定义Label GUI函数
 */
static void My_Demo_Label_GUI(void)
{
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello, World!");
  lv_obj_set_style_opa(label, 100, LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(label, lv_color_hex(0xffe1d3), LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(label, lv_color_hex(0xf7b37b), LV_STATE_DEFAULT);
}

/**
 * @brief 自定义CheckBox GUI函数
 */
static void My_Demo_CheckBox_GUI(void)
{
  lv_obj_t *checkbox = lv_checkbox_create(lv_scr_act());
  lv_checkbox_set_text(checkbox, "check it!");
  lv_obj_set_style_pad_column(checkbox, 20, LV_STATE_DEFAULT); // 设置文本和复选框的间距
}

/**
 * @brief 自定义Bar GUI函数
 */
static void My_Demo_Bar_GUI(void)
{
  lv_obj_t *bar = lv_bar_create(lv_scr_act());
  lv_obj_set_style_anim_time(bar, 500, LV_STATE_DEFAULT); // 设置动画时间
  lv_bar_set_range(bar, -100, 100);                       // 设置范围值
  lv_bar_set_value(bar, 50, LV_ANIM_ON);                  // 设置当前值
}

/**
 * @brief 自定义Spinner GUI函数
 */
static void My_Demo_Spinner_GUI(void)
{
  lv_obj_t *spinner = lv_spinner_create(lv_scr_act());
  lv_obj_set_style_arc_color(spinner, lv_color_hex(0x4a9f00), LV_PART_MAIN);      // 设置主体圆弧颜色
  lv_obj_set_style_arc_color(spinner, lv_color_hex(0x83bd55), LV_PART_INDICATOR); // 设置指示器圆弧颜色
  lv_obj_set_style_arc_width(spinner, 30, LV_PART_MAIN);                          // 设置主体圆弧宽度
  lv_obj_set_style_arc_width(spinner, 30, LV_PART_INDICATOR);                     // 设置指示器圆弧宽度
}

/**
 * @brief 自定义LED GUI函数
 */
static void My_Demo_Led_GUI(void)
{
  lv_obj_t *led = lv_led_create(lv_scr_act());
  lv_led_set_color(led, lv_color_hex(0xff0000)); // 设置LED颜色
  lv_led_set_brightness(led, 0);                 // 设置LED亮度
  lv_led_toggle(led);                            // 翻转LED
}

/**
 * @brief 自定义List GUI函数
 */
static void My_Demo_List_GUI(void)
{
  lv_obj_t *list = lv_list_create(lv_scr_act());
  lv_list_add_text(list, "Settings");          // 设置列表文本
  lv_list_add_btn(list, LV_SYMBOL_GPS, "GPS"); // 设置列表按键图标和文本
  lv_list_add_btn(list, LV_SYMBOL_WIFI, "WIFI");
}

/**
 * @brief 自定义Dropdown GUI函数
 */
static void My_Demo_Dropdown_GUI(void)
{
  lv_obj_t *dropdown = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_options(dropdown, "a\nb\nc\nd\n");
}

/**
 * @brief 自定义Roller GUI函数
 */
static void My_Demo_Roller_GUI(void)
{
  lv_obj_t *roller = lv_roller_create(lv_scr_act());
  lv_obj_set_style_text_line_space(roller, 30, LV_STATE_DEFAULT);     // 设置选项间隔
  lv_roller_set_options(roller, "a\nb\nc\nd", LV_ROLLER_MODE_NORMAL); // 设置选项内容和显示模式
  lv_roller_set_visible_row_count(roller, 2);                         // 设置可见行数
}

/**
 * @brief 自定义Slider GUI函数
 */
static void My_Demo_Slider_GUI(void)
{
  lv_obj_t *slider = lv_slider_create(lv_scr_act());
  lv_obj_set_size(slider, 20, 200);
  lv_slider_set_value(slider, 50, LV_ANIM_OFF);      // 设置当前值
  lv_slider_set_range(slider, -100, 100);            // 设置范围值
  lv_slider_set_mode(slider, LV_SLIDER_MODE_NORMAL); // 设置模式
  lv_slider_set_left_value(slider, 20, LV_ANIM_OFF); // 设置左值
}

/**
 * @brief 自定义Arc GUI函数
 */
static void My_Demo_Arc_GUI(void)
{
  lv_obj_t *arc = lv_arc_create(lv_scr_act());
  lv_obj_set_pos(arc, 100, 100);
  lv_arc_set_value(arc, 10);                // 设置当前值
  lv_arc_set_range(arc, 0, 200);            // 设置范围值
  lv_arc_set_angles(arc, 135, 45);          // 设置前景弧角度
  lv_arc_set_bg_angles(arc, 135, 45);       // 设置背景弧角度
  lv_arc_set_rotation(arc, 0);              // 旋转角度设置
  lv_arc_set_mode(arc, LV_ARC_MODE_NORMAL); // 设置模式
  lv_arc_set_change_rate(arc, 90);          // 速率设置
}

/**
 * @brief 自定义Line GUI函数
 */
static void My_Demo_Line_GUI(void)
{
  lv_obj_t *line = lv_line_create(lv_scr_act());
  static lv_point_t line_points[] = {{150, 50}, {250, 200}, {50, 200}, {150, 50}};
  lv_line_set_points(line, line_points, 4);
  lv_obj_set_style_line_width(line, 8, LV_PART_MAIN);      // 设置线宽
  lv_obj_set_style_line_rounded(line, true, LV_PART_MAIN); // 设置圆角
  lv_line_set_y_invert(line, true);                        // Y轴反转
}

/**
 * @brief 自定义Img GUI函数
 */
static void My_Demo_Img_GUI(void)
{
  lv_obj_t *img = lv_img_create(lv_scr_act());
  LV_IMG_DECLARE(img_qq);
  lv_img_set_src(img, &img_qq);
  lv_img_set_offset_x(img, 100); // x轴偏移100
  lv_img_set_offset_y(img, 20);  // y轴偏移20
  lv_obj_set_style_image_recolor(img, lv_color_hex(0xffe1d2), LV_PART_MAIN);
  lv_obj_set_style_image_recolor_opa(img, 150, LV_PART_MAIN);
  lv_img_set_zoom(img, 512);  // 放大2倍
  lv_img_set_angle(img, 900); // 顺时针方向旋转90°
  // lv_obj_update_layout(img);   // 更新图片布局信息
  lv_img_set_pivot(img, 200, 200); // 设置中心点
}

/**
 * @brief 自定义ColorWheel GUI函数
 */
static void My_Demo_ColorWheel_GUI(void)
{
}

/**
 * @brief 自定义Btnmatrix GUI函数
 */
static void My_Demo_Btnmatrix_GUI(void)
{
  lv_obj_t *btnm = lv_btnmatrix_create(lv_scr_act());
  static const char *map[] = {"btn1", "\n", "btn2", "btn3", ""};   // 定义按钮数组，最后一个元素必须为空
  lv_btnmatrix_set_map(btnm, map);                                 // 设置按钮
  lv_btnmatrix_set_btn_width(btnm, 0, 5);                          // 设置按钮相对宽度
  lv_btnmatrix_set_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKABLE); // 设置单个按钮属性
  // lv_btnmatrix_clear_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKABLE); // 清除单个按钮属性
  lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE); // 设置所有按钮属性
  lv_btnmatrix_set_one_checked(btnm, true);
}

/**
 * @brief 自定义Areatext GUI函数
 */
static void My_Demo_TextArea_GUI(void)
{
  lv_obj_t *ta = lv_textarea_create(lv_scr_act());
  lv_textarea_add_char(ta, 'A');                         // 添加一个字符到当前光标处
  lv_obj_t *keyboard = lv_keyboard_create(lv_scr_act()); // 创建键盘部件
  lv_keyboard_set_textarea(keyboard, ta);                // 关联键盘和文本区域部件
  // lv_textarea_set_cursor_pos(ta, 0);                  // 0：最左侧， LV_TEXTAREA_CURSOR_LAST：最右侧
  lv_textarea_set_one_line(ta, true);          // 单行模式
  lv_textarea_set_password_mode(ta, true);     // 密码模式（隐藏输入的内容）
  lv_textarea_set_password_show_time(ta, 100); // 密码显示时间
  // lv_textarea_set_accepted_chars(ta, "0123456789"); // 限制接收的字符
  lv_textarea_set_max_length(ta, 6);                // 限制字符长度
  lv_textarea_set_placeholder_text(ta, "password"); // 提示输入密码
  // const char *txt = lv_textarea_get_text(ta);       // 获取文本框文本
}

/**
 * @brief 自定义Keyboard GUI函数
 */
static void My_Demo_Keyboard_GUI(void)
{
  lv_obj_t *kb = lv_keyboard_create(lv_scr_act());
  lv_obj_t *ta = lv_textarea_create(lv_scr_act());
  lv_keyboard_set_textarea(kb, ta);                  // 关联键盘和文本区域部件
  lv_keyboard_set_popovers(kb, true);                // 允许按键弹窗提示
  lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUMBER); // 数字键盘模式
}

/**
 * @brief 自定义Imgbtn GUI函数
 */
static void My_Demo_Imgbtn_GUI(void)
{
  lv_obj_t *imgbtn = lv_imgbtn_create(lv_scr_act());
  LV_IMG_DECLARE(img_qq);
  lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, &img_qq, NULL);
  lv_obj_set_size(imgbtn, 36, 44);
  lv_obj_set_pos(imgbtn, 200, 200);
}

/**
 * @brief 自定义Tabview GUI函数
 */
static void My_Demo_Tabview_GUI(void)
{
  lv_obj_t *tv = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
  lv_obj_t *tab1 = lv_tabview_add_tab(tv, "Tab 1");
  lv_obj_t *tab2 = lv_tabview_add_tab(tv, "Tab 2");
  lv_tabview_set_act(tv, 0, LV_ANIM_ON);
  lv_obj_t *label = lv_label_create(tab1);
  lv_label_set_text(label, "Test");
}

/**
 * @brief 自定义Tabview GUI函数
 */
static void My_Demo_Tileview_GUI(void)
{
  lv_obj_t *tileview = lv_tileview_create(lv_scr_act());
  lv_obj_t *tile1 = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_RIGHT);
  lv_obj_t *tile2 = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_LEFT);
  lv_obj_update_layout(tileview);                // 更新参数
  lv_obj_set_tile(tileview, tile2, LV_ANIM_OFF); // 根据页面对象设置
  // lv_obj_set_tile_id(tileview, 1, 0, LV_ANIM_OFF); // 根据页面行列设置
}

/**
 * @brief 自定义Win GUI函数
 */
static void My_Demo_Win_GUI(void)
{
  lv_obj_t *win = lv_win_create(lv_scr_act());
  lv_obj_t *title = lv_win_add_title(win, "Setting");
  lv_obj_t *btn = lv_win_add_button(win, LV_SYMBOL_BELL, 20);
  lv_obj_t *content = lv_win_get_content(win); // 获取主体
  lv_obj_t *label = lv_label_create(content);  // 添加内容
  lv_label_set_text(label, "Test");
}

/**
 * @brief 自定义Msgbox GUI函数
 */
static void My_Demo_Msgbox_GUI(void)
{
  static const char *btns[] = {"Continue", "Close", ""};
  lv_obj_t *msgbox = lv_msgbox_create(lv_scr_act(), "Notice", "Do you want to continue?", btns, true);
  // lv_msgbox_close(msgbox);
}

/**
 * @brief 自定义Spinbox GUI函数
 */
static void My_Demo_Spinbox_GUI(void)
{
  lv_obj_t *spinbox = lv_spinbox_create(lv_scr_act());
  // lv_spinbox_increment(spinbox);              // 递增
  // lv_spinbox_decrement(spinbox);              // 递减
  lv_spinbox_set_step(spinbox, 200);          // 设置步进值，默认为1
  lv_spinbox_set_range(spinbox, -1000, 1000); // 设置范围值，默认±99999
  lv_spinbox_set_value(spinbox, 400);
  lv_spinbox_set_digit_format(spinbox, 4, 2); // 设置数字位数、小数点位置
  lv_spinbox_set_pos(spinbox, 3);
  lv_spinbox_get_value(spinbox); // 返回值为整数，而非小数
}

/**
 * @brief 自定义Table GUI函数
 */
static void My_Demo_Table_GUI(void)
{
  lv_obj_t *table = lv_table_create(lv_scr_act());
  lv_table_set_row_cnt(table, 2);        // 设置行数
  lv_table_set_col_cnt(table, 2);        // 设置列数
  lv_table_set_col_width(table, 1, 200); // 设置列的宽度
  lv_table_set_cell_value(table, 0, 0, "123");
  lv_table_set_cell_value_fmt(table, 1, 0, "%d", 100);
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
  // My_Demo_Label_GUI();
  // My_Demo_Button_GUI();
  // My_Demo_CheckBox_GUI();
  // My_Demo_Bar_GUI();
  // My_Demo_Spinner_GUI();
  // My_Demo_Led_GUI();
  // My_Demo_List_GUI();
  // My_Demo_Dropdown_GUI();
  // My_Demo_Roller_GUI();
  // My_Demo_Slider_GUI();
  // My_Demo_Arc_GUI();
  // My_Demo_Line_GUI();
  // My_Demo_Img_GUI();
  // My_Demo_Btnmatrix_GUI();
  // My_Demo_TextArea_GUI();
  // My_Demo_Keyboard_GUI();
  // My_Demo_Imgbtn_GUI();
  // My_Demo_Tabview_GUI();
  // My_Demo_Tileview_GUI();
  // My_Demo_Win_GUI();
  // My_Demo_Msgbox_GUI();
  // My_Demo_Spinbox_GUI();
  My_Demo_Table_GUI();

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
