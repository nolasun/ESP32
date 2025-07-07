#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "led_strip.h"
#include "dht.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "st7789_cst816t.h"
#include "ws2812.h"
#include "ui_show.h"

// LVGL image declare
LV_IMG_DECLARE(humidity);
// LV_IMG_DECLARE(temperature);

// static lv_obj_t* temperature_img_logo;
static lv_obj_t* humidity_img_logo;
static lv_obj_t* s_temp_label;
static lv_obj_t* s_humidity_label;
static lv_obj_t* s_light_slider;
static lv_timer_t* s_dht22_timer;

extern led_strip_handle_t led_strip;


void light_slider_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
        case LV_EVENT_VALUE_CHANGED:
        {       
            lv_obj_t* slider_obj = lv_event_get_target(e);
            int32_t value = lv_slider_get_value(slider_obj);
            uint32_t rgb_value = 150*value/100;
            for(uint32_t led_index = 0 ;led_index < LED_STRIP_LED_COUNT;led_index++)
            {
                led_strip_set_pixel(led_strip, led_index, rgb_value, rgb_value, rgb_value);
                led_strip_refresh(led_strip);
                ESP_LOGI("led_strip", "led_index: %lu, rgb_value: %lu", led_index, rgb_value);
            }
            break;
        }
        default:
            break;
    }
}

void dht22_timer_cb(struct _lv_timer_t *t)
{
    float temperature;
    float humidity;
    if(dht_read_float_data(DHT_TYPE_AM2301, DHT22_GPIO_PIN, &humidity, &temperature) == ESP_OK)
    {
        char disp_buf[32];
        snprintf(disp_buf,sizeof(disp_buf),"%.1f",temperature);
        lv_label_set_text(s_temp_label,disp_buf);

        snprintf(disp_buf,sizeof(disp_buf),"%.1f",humidity);
        lv_label_set_text(s_humidity_label,disp_buf);
        ESP_LOGI("dht22", "Humidity: %f, Temperature: %f", humidity, temperature);
    }
}

void app_main_display(void)
{
    lv_obj_t *scr = lv_scr_act();

    /* Task lock */
    lvgl_port_lock(0);

    //创建调光用的进度条
    s_light_slider = lv_slider_create(scr);
    lv_obj_align(s_light_slider, LV_ALIGN_TOP_MID, 0, 80);
    lv_obj_set_size(s_light_slider, 150, 15);
    lv_slider_set_range(s_light_slider, 0, 100);
    lv_obj_add_event_cb(s_light_slider, light_slider_event_cb, LV_EVENT_VALUE_CHANGED,NULL);

    lv_slider_set_value(s_light_slider, 100, LV_ANIM_OFF);
    int32_t value = lv_slider_get_value(s_light_slider);
    uint32_t rgb_value = 150*value/100;
    for(uint32_t led_index = 0 ;led_index < LED_STRIP_LED_COUNT;led_index++)
    {
        led_strip_set_pixel(led_strip, led_index, rgb_value, rgb_value, rgb_value);
        led_strip_refresh(led_strip);
        ESP_LOGI("led_strip", "led_index: %lu, rgb_value: %lu", led_index, rgb_value);
    }

    /* Create image */
    humidity_img_logo = lv_img_create(scr);
    lv_img_set_src(humidity_img_logo, &humidity);
    lv_obj_align(humidity_img_logo, LV_ALIGN_TOP_MID, 0, 20);

    // temperature_img_logo = lv_img_create(scr);
    // lv_img_set_src(temperature_img_logo, &temperature);
    // lv_obj_align(temperature_img_logo, LV_ALIGN_TOP_MID, 0, 50);
    
    //创建湿度label
    s_humidity_label = lv_label_create(scr);
    lv_obj_align(s_humidity_label, LV_ALIGN_TOP_MID, 40, 20);
    lv_obj_set_style_text_font(s_humidity_label, &lv_font_montserrat_14, 0);

    //创建温度label
    s_temp_label = lv_label_create(scr);
    lv_obj_align(s_temp_label, LV_ALIGN_TOP_MID, 40, 50);
    lv_obj_set_style_text_font(s_temp_label, &lv_font_montserrat_14, 0);

    //创建定时器
    s_dht22_timer = lv_timer_create(dht22_timer_cb,2000,NULL);

    /* Task unlock */
    lvgl_port_unlock();
}