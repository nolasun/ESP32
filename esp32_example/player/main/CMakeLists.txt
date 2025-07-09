idf_component_register(SRCS "mjpeg_frame.c" "st7789_cst816t.c" "main.c" "sdcard_file.c" "ui_show.c"
                    INCLUDE_DIRS ".")

spiffs_create_partition_image(ui_img ../img FLASH_IN_PROJECT)