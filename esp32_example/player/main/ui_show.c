#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"

#include "sdcard_file.h"
#include "esp_spiffs.h"
#include "mjpeg_frame.h"
#include "ui_show.h"
#include "jpeg_decoder.h"

static const char* TAG = "ui_show";

static lv_obj_t* lv_file_title = NULL;

static lv_obj_t* lv_file_list = NULL;

static lv_obj_t* lv_file_page = NULL;

static lv_obj_t* lv_player_page = NULL;

static lv_obj_t* lv_player_img = NULL;

static lv_obj_t* lv_player_back = NULL;

static lv_obj_t* lv_player_pause = NULL;


static lv_timer_t* s_player_timer = NULL;

//定义一个标志，用于指示暂停/启动播放
static bool s_pause_flag = false;


void lv_player_timer(struct _lv_timer_t * t)
{
    static jpeg_frame_data_t frame_data = {0,0};
    if(frame_data.frame)
    {
        free(frame_data.frame);
        frame_data.frame = NULL;
        frame_data.len = 0;
    }
    //判断当前是否暂停
    if(s_pause_flag)
        return;
    //获取一帧jpg图像
    jpeg_frame_get_one(&frame_data);
    if(frame_data.len)
    {
        static lv_img_dsc_t img_dsc;
        memset(&img_dsc,0,sizeof(lv_img_dsc_t));
        static uint8_t *rgb565_data = NULL;
        uint16_t width = 0;
        uint16_t height = 0;
        if(rgb565_data)
        {
            free(rgb565_data);
            rgb565_data = NULL;
        }

        // 使用 esp_jpeg 库进行 JPEG 解码
        esp_jpeg_image_cfg_t cfg = {
            .indata = frame_data.frame,
            .indata_size = frame_data.len,
            .out_format = JPEG_IMAGE_FORMAT_RGB565,
            .out_scale = JPEG_IMAGE_SCALE_0,
            .advanced = {
                .working_buffer = NULL,
                .working_buffer_size = 0
            }
        };
        esp_jpeg_image_output_t img_info = {0};

        // 获取 JPEG 图像信息
        esp_err_t err = esp_jpeg_get_image_info(&cfg, &img_info);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get JPEG image info: %d", err);
            return;
        }

        // 分配输出缓冲区
        cfg.outbuf = (uint8_t *)malloc(img_info.output_len);
        if (cfg.outbuf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate output buffer");
            return;
        }
        cfg.outbuf_size = img_info.output_len;

        // 解码 JPEG 图像
        err = esp_jpeg_decode(&cfg, &img_info);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to decode JPEG image: %d", err);
            free(cfg.outbuf);
            return;
        }

        rgb565_data = cfg.outbuf;
        width = img_info.width;
        height = img_info.height;

        img_dsc.header.cf = LV_COLOR_FORMAT_RGB565;
        img_dsc.data = rgb565_data;
        img_dsc.data_size = (uint32_t)width * (uint32_t)height * 2;
        img_dsc.header.h = height;
        img_dsc.header.w = width;
        //ESP_LOGI(TAG,"rgb img,w:%d,h:%d,size:%d",height,width,(size_t)img_dsc.data_size);
        lv_img_set_src(lv_player_img, &img_dsc);
    }
    else
    {
        lv_timer_del(s_player_timer);
        s_player_timer = NULL;
    }
}

void lv_list_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    ESP_LOGI(TAG,"list btn clicked");
    switch(code)
    {
        case LV_EVENT_CLICKED:
        {    
            if(s_player_timer)  //当前存在播放定时器，说明处于播放中，不允许重复再播放
            {
                return;
            }
            const char* text = lv_list_get_btn_text(lv_file_list, obj);
            if(text)
            {
                //判断是否.mjpeg文件
                if(strstr(text,".mjpeg") || strstr(text,".MJPEG") || strstr(text,".MJP"))
                {
                    ESP_LOGI(TAG,"start play file:%s", text);
                    jpeg_frame_start(text);     //启动jpeg解析
                    s_pause_flag = false;       //取消暂停标志
                    s_player_timer = lv_timer_create(lv_player_timer, 5, NULL);  //启动播放定时器
                    lv_scr_load(lv_player_page); //加载视频播放页面
                }
            }
            break;
        }
        default:break;
    }
}

void lv_player_btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED)
    {
        if(obj == lv_player_back)        //点击了返回按键，停止播放，关闭jpeg解析任务，返回文件列表
        {
            if(s_player_timer)
                lv_timer_del(s_player_timer);
            s_player_timer = NULL;
            jpeg_frame_stop();
            lv_scr_load(lv_file_page);
        }
        else if(obj == lv_player_pause)  //暂停按键
        {
            s_pause_flag = !s_pause_flag;
        }
    }
}


void lv_mjpeg_create(void)
{
    // 获取屏幕大小
    uint32_t screen_width = lv_disp_get_hor_res(NULL);
    uint32_t screen_height = lv_disp_get_ver_res(NULL);
    
    //创建屏幕与控件(文件标题、文件列表)
    lv_file_page = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(lv_file_page, lv_color_black(), LV_PART_MAIN);

    lv_file_title = lv_label_create(lv_file_page);
    lv_label_set_text(lv_file_title, "SDCard file");
    lv_obj_set_size(lv_file_title, screen_width-60, 30);
    lv_obj_align(lv_file_title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(lv_file_title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(lv_file_title, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(lv_file_title, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(lv_file_title, LV_OPA_100, LV_PART_MAIN);

    lv_file_list = lv_list_create(lv_file_page);
    lv_obj_set_size(lv_file_list, screen_width-60, screen_height-90);
    lv_obj_align(lv_file_list, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_obj_set_style_text_font(lv_file_list, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lv_file_title, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(lv_file_title, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(lv_file_title, LV_OPA_100, LV_PART_MAIN);

    //添加文件列表
    const char (*filelist)[256] = NULL;
    sdcard_check();
    int filenum = sdcard_filelist(&filelist);
    for(int i = 0;i < filenum;i++)
    {
        lv_obj_t* btn = lv_list_add_btn(lv_file_list,NULL,*filelist);
        lv_obj_set_style_text_font(btn,&lv_font_montserrat_18,0);
        filelist++;
        lv_obj_add_event_cb(btn,lv_list_event_cb,LV_EVENT_CLICKED,NULL);
    }
    
    //创建屏幕与控件(视频播放、返回按钮、暂停按钮)
    lv_player_page = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(lv_player_page, lv_color_black(), LV_PART_MAIN);

    lv_player_img = lv_img_create(lv_player_page);
    lv_obj_align(lv_player_img, LV_ALIGN_TOP_MID, 0, 35);

    lv_player_back = lv_imgbtn_create(lv_player_page);
    lv_obj_set_size(lv_player_back, 48, 48);
    lv_obj_align(lv_player_back, LV_ALIGN_BOTTOM_LEFT, 30, -20);
    lv_imgbtn_set_src(lv_player_back, LV_IMGBTN_STATE_RELEASED, NULL, "/img/back_img_48.png", NULL);
    //添加点击事件处理
    lv_obj_add_event_cb(lv_player_back,lv_player_btn_event_cb,LV_EVENT_CLICKED,NULL);

    lv_player_pause = lv_imgbtn_create(lv_player_page);
    lv_obj_set_size(lv_player_pause, 48, 48);
    lv_obj_align(lv_player_pause, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_imgbtn_set_src(lv_player_pause, LV_IMGBTN_STATE_RELEASED, NULL, "/img/pause_img_48.png", NULL);
    //添加点击事件处理
    lv_obj_add_event_cb(lv_player_pause,lv_player_btn_event_cb,LV_EVENT_CLICKED,NULL);

    jpeg_frame_cfg_t  frame_cfg = 
    {
        .buff_size = 100*1024,
    };
    jpeg_frame_config(&frame_cfg);
    
    lv_scr_load(lv_file_page);

}


