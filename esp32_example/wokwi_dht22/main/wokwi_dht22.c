#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "dht.h"

#define DHT22_GPIO GPIO_NUM_14
static const char *TAG = "dht22";

int16_t humidity = 0;
int16_t temperature = 0;

void app_main(void)
{
    while (1)
    {
        dht_read_data(DHT_TYPE_AM2301, GPIO_NUM_14, &humidity, &temperature);
        ESP_LOGI(TAG, "humidity: %d, temperature: %d", humidity, temperature);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
