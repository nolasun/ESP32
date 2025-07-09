#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"

#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "st7789_cst816t.h"
#include "sdcard_file.h"
#include "ui_show.h"
#include "esp_spiffs.h"


static const char *TAG = "main";

esp_err_t bsp_spiffs_init(char *partition_label, char *mount_point, size_t max_files)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = mount_point,
        .partition_label = partition_label,
        .max_files = max_files,
        .format_if_mount_failed = false,
    };

    esp_err_t ret_val = esp_vfs_spiffs_register(&conf);

    if (ESP_OK != ret_val) {
        ESP_LOGW(TAG, "spiffs register fail!");
        return ret_val;
    }

    size_t total = 0, used = 0;
    ret_val = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret_val != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret_val));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ret_val;
}

void lvgl_task(void* param)
{
    lv_mjpeg_create();
    while(1)
    {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}


void app_main(void)
{
    bsp_spiffs_init("ui_img","/img",5);

        /* LCD HW initialization */
    ESP_ERROR_CHECK(app_lcd_init());

    /* Touch initialization */
    ESP_ERROR_CHECK(app_touch_init());

    /* LVGL initialization */
    ESP_ERROR_CHECK(app_lvgl_init());


    xTaskCreatePinnedToCore(lvgl_task,"lvgl",8192,NULL,5,NULL,1);
}

