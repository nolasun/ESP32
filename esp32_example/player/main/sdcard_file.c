#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "sdcard_file.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include <dirent.h>

static const char *TAG = "sdcard";

#define MOUNT_POINT "/sdcard"   //挂载点名称

/** 检测SD卡
 * @param 无
 * @return 成功或失败，只有成功检测到SD卡，才可以获取文件列表
*/
esp_err_t sdcard_check(void)
{
    esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,     //挂载失败是否执行格式化
        .max_files = 5,                     //最大可打开文件数
        .allocation_unit_size = 16 * 1024   //执行格式化时的分配单元大小（分配单元越大，读写越快）
    };
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    ESP_LOGI(TAG, "Using SDMMC peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

    //默认的IO管脚配置
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    //4位数据
    slot_config.width = 4;

    //管脚启用内部上拉
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. ");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ESP_OK;
    }
    ESP_LOGI(TAG, "Filesystem mounted");
    return ESP_FAIL;
}

/** 获取SD卡的文件列表
 * @param file 返回的文件列表
 * @return 文件列表长度
*/
int sdcard_filelist(const char (**file)[256])
{
    DIR *dir;
    struct dirent *entry;
    static char filename[20][256] = {0};

    // 打开目录
    dir = opendir(MOUNT_POINT);
    if (dir == NULL) {
        return 0;
    }

    // 读取目录中的文件列表
    int file_cnt = 0;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
        snprintf(&filename[file_cnt][0],256,"%s",entry->d_name);
        file_cnt++;
        if(file_cnt >= 20)
            break;
    }
    closedir(dir);
    *file = filename;
    return file_cnt;
}