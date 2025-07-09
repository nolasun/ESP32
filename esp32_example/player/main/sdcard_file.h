#ifndef __SDCARD_FILE_H__
#define __SDCARD_FILE_H__
#include "esp_err.h"

/** 检测SD卡
 * @param 无
 * @return 成功或失败，只有成功检测到SD卡，才可以获取文件列表
*/
esp_err_t sdcard_check(void);

/** 获取SD卡的文件列表
 * @param file 返回的文件列表
 * @return 文件列表长度
*/
int sdcard_filelist(const char (**file)[256]);

#endif
