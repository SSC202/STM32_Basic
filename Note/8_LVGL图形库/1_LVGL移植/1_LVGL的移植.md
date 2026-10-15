# STM32 LVGL 1_LVGL的移植

## 1. 裸机移植LVGL

### 源码下载

https://github.com/lvgl/lvgl

### 新建工程文件夹`GUI`和`GUI_APP`并移植文件

- 在`GUI`下新建`lvgl`和`lvgl_port`文件夹。

> - `lvgl`存放官方各类控件的源程序。
> - `lvgl_port`存放LVGL显示屏驱动、输入设备驱动及文件系统驱动。

- 将源码中的`src`文件夹和`lvgl.h`文件拷贝到`lvgl`文件夹中。
- 将源码中的`examples/porting`路径下的文件拷贝到`lvgl_port`文件夹中，并删除`_template`字段。

> - `lv_port_disp`为LVGL显示驱动。
> - `lv_port_fs`为LVGL文件系统驱动。
> -  `lv_port_indev`为LVGL输入设备驱动。

- 将源码中的`lv_conf_template.h`拷贝到`GUI`文件夹中，并删除`_template`字段。

> - `lv_conf.h`是LVGL库的配置文件，里面有各种宏。

- 添加工程代码和编译路径，增加全局宏定义`STM32F4`。

### 修改`lv_conf.h`

- 使能文件

```c
/**
 * @file lv_conf.h
 * Configuration file for v8.1.0
 */

/*
 * Copy this file as `lv_conf.h`
 * 1. simply next to the `lvgl` folder
 * 2. or any other places and
 *    - define `LV_CONF_INCLUDE_SIMPLE`
 *    - add the path as include path
 */

/* clang-format off */
#if 1 /*Set it to "1" to enable content*/ 
```

- 定义颜色深度，如果为单色屏，改为1.

```c
/*====================
   COLOR SETTINGS
 *====================*/

/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 16
```

- 分配内存

```c
/*=========================
   MEMORY SETTINGS
 *=========================*/

/*1: use custom malloc/free, 0: use the built-in `lv_mem_alloc()` and `lv_mem_free()`*/
#define LV_MEM_CUSTOM 0
#if LV_MEM_CUSTOM == 0
/*Size of the memory available for `lv_mem_alloc()` in bytes (>= 2kB)*/
#  define LV_MEM_SIZE (32U * 1024U)          /*[bytes]*/
```

- 时钟配置

```c
/*Use a custom tick source that tells the elapsed time in milliseconds.
 *It removes the need to manually update the tick with `lv_tick_inc()`)*/
 /* 时钟源提供器，如果LV_TICK_CUSTOM==1，那么你就不用lv_tick_inc()提供时钟了 */
#define LV_TICK_CUSTOM     1
#if LV_TICK_CUSTOM
/* ↓ 这里可以指定时钟源提供器，例如STM32的HAL库的HAL_GetTick() */
#define LV_TICK_CUSTOM_INCLUDE  "stm32f4xx_hal.h"     /*Header for the system time function*/
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (HAL_GetTick()) 
#endif   /*LV_TICK_CUSTOM*/

/*Default Dot Per Inch. Used to initialize default sizes such as widgets sized, style paddings.
 *(Not so important, you can adjust it to modify default sizes and spaces)*/
#define LV_DPI_DEF 130     /*[px/inch]*/
//用来调节界面缩放比例的，此值越大，控件分布的就越散，控件自身的间隔也会变大。可根据实际情况进行更改，例如128x128分辨率1.44英寸的屏幕，那么 DPI = ((√128*128) / 1.44) ≈ 89
```

- 使能DMA2D

```c
/*Use STM32's DMA2D (aka Chrom Art) GPU*/
#define LV_USE_GPU_STM32_DMA2D 1
#if LV_USE_GPU_STM32_DMA2D
    /*Must be defined to include path of CMSIS header of target processor
    e.g. "stm32f769xx.h" or "stm32f429xx.h"*/
    #define LV_GPU_DMA2D_CMSIS_INCLUDE "stm32f429xx.h"
#endif
```

### 修改驱动接口

- 使能并添加头文件

```c
 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "lvgl.h"
#include "lcd.h"
```

- 修改屏幕参数

```c
/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES     (800)    // 屏幕像素高度
#define MY_DISP_VER_RES     (480)    // 屏幕像素宽度


/*Set the resolution of the display*/
disp_drv.hor_res = MY_DISP_HOR_RES;
disp_drv.ver_res = MY_DISP_VER_RES;
```

- 修改初始化显示函数

```c
/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    LCD_Init();
}
```

- 修改显示驱动函数

```c
void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();  // 显示屏驱动初始化

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    // 显示缓冲区初始化有三种方式，这里使用的是方式1，方式1消耗的硬件资源最小，但运行效果也是最差的，三种方式如下
    // 第一种只创建一个缓存区，长度是横轴像素长度的 10 倍
    // 第二种创建两个缓存区，长度都是横轴的 10 倍，可以结合 DMA 加快写入速度
    // 第三种则是创建两个，大小是横轴乘以纵轴，相当于整个屏幕大小
    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    // 显示缓冲区
    static lv_color_t buf_1[MY_DISP_HOR_RES * 40];                          /*A buffer for 10 rows*/
    // 初始化显示缓冲区
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 40);   /*Initialize the display buffer*/

    /* Example for 2) */
//    static lv_disp_draw_buf_t draw_buf_dsc_2;
//    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*A buffer for 10 rows*/
//    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                        /*An other buffer for 10 rows*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
//    static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
//    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*An other screen sized buffer*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, MY_DISP_VER_RES * LV_VER_RES_MAX);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    // 初始化显示设备
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    // 注册显示回调函数
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    // 注册显示缓冲区
    disp_drv.draw_buf = &draw_buf_dsc_1;

    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    // 注册显示设备
    lv_disp_drv_register(&disp_drv);
}
```

- 修改色块填充函数

```c
/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

	LCD_Color_Fill(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_p);

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}
```

- 头文件中使能并声明`lv_port_disp_init`函数。

### 修改输入设备驱动接口

- 使能并添加头文件

```c
 /*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "lvgl.h"
#include "lcd.h"
```

- 修改触摸屏初始化函数

```c
/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    LCD_Touch_Init();
}
```

- 修改初始化函数并裁剪相关函数定义

```c
// 初始化输入设备驱动和在LVGL中注册一个输入设备。输入设备可以是触摸屏、鼠标、键盘、编码器、按键，这里只使用触摸屏，其余的删除。
// 还需要删除相关的函数定义
void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    // 注册输入设备
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);
}
```

- 修改`touchpad_is_pressed()`函数

```c
static bool touchpad_is_pressed(void)
{
    /*Your code comes here*/
    static LCD_touch_point_t point[1];
    if (LCD_Touch_Scan(point, 1)) {
        return true;
    }
    return false;
}
```

- 修改`touchpad_get_xy()`函数

```c
static void touchpad_get_xy(lv_coord_t *x, lv_coord_t *y)
{
    /*Your code comes here*/
    static LCD_touch_point_t point[1];
    if (LCD_Touch_Scan(point, 1)) {
        (*x) = point[0].x;
        (*y) = point[0].y;
    }
}
```

- 头文件中使能并声明`lv_port_indev_init`函数。

### 主程序修改

- 添加头文件

```c
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
```

- 添加初始化函数

```c
lv_init();            // lvgl 系统初始化
lv_port_disp_init();  // lvgl 显示接口初始化,放在 lv_init()的后面
lv_port_indev_init(); // lvgl 输入接口初始化,放在 lv_init()的后面
```

- 添加自己的demo。
- 在循环中执行`lv_task_handler`函数。