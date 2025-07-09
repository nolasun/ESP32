#ifndef __ST7789_CST816T_H__
#define __ST7789_CST816T_H__

#include "esp_err.h"

/* LCD size */
#define LCD_H_RES              (320)
#define LCD_V_RES              (240)

/* LCD settings */
#define LCD_SPI_NUM         (SPI2_HOST)
#define LCD_PIXEL_CLK_HZ    (40 * 1000 * 1000)
#define LCD_CMD_BITS        (8)
#define LCD_PARAM_BITS      (8)
#define LCD_COLOR_SPACE     (ESP_LCD_COLOR_SPACE_BGR)
#define LCD_BITS_PER_PIXEL  (16)
#define LCD_DRAW_BUFF_DOUBLE (1)
#define LCD_DRAW_BUFF_HEIGHT (40)
#define LCD_BL_ON_LEVEL     (1)

/* LCD pins */
#define LCD_GPIO_SCLK       (GPIO_NUM_19)
#define LCD_GPIO_MOSI       (GPIO_NUM_18)
#define LCD_GPIO_RST        (GPIO_NUM_17)
#define LCD_GPIO_DC         (GPIO_NUM_16)
#define LCD_GPIO_CS         (GPIO_NUM_5)
#define LCD_GPIO_BL         (GPIO_NUM_4)

/* Touch settings */
#define TOUCH_I2C_NUM       (0)
#define TOUCH_I2C_CLK_HZ    (400000)

/* LCD touch pins */
#define TOUCH_I2C_SCL       (GPIO_NUM_21)
#define TOUCH_I2C_SDA       (GPIO_NUM_22)
#define TOUCH_GPIO_INT      (GPIO_NUM_23)

esp_err_t app_lcd_init(void);
esp_err_t app_touch_init(void);
esp_err_t app_lvgl_init(void);

#endif
