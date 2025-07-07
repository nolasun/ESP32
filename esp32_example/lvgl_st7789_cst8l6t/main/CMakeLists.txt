idf_component_register(SRCS "st7789_cst816t.c" "main.c" "ws2812.c" "ui_show.c"
                    INCLUDE_DIRS ".")

lvgl_port_create_c_image("images/humidity.png" "images/" "ARGB8888" "NONE")
lvgl_port_add_images(${COMPONENT_LIB} "images/")

# lvgl_port_create_c_image("images/temperature.png" "images/" "ARGB8888" "NONE")
# lvgl_port_add_images(${COMPONENT_LIB} "images/")
