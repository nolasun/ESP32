#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "dht.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "st7789_cst816t.h"
#include "ws2812.h"
#include "ui_show.h"


led_strip_handle_t led_strip;


void app_main(void)
{
    led_strip = configure_ws2812(); 

    /* LCD HW initialization */
    ESP_ERROR_CHECK(app_lcd_init());

    /* Touch initialization */
    ESP_ERROR_CHECK(app_touch_init());

    /* LVGL initialization */
    ESP_ERROR_CHECK(app_lvgl_init());

    /* Show LVGL objects */
    app_main_display();

    ESP_LOGI("main", "all init finsh!");

    while (1) {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

