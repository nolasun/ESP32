# 一、硬件

ESP32开发板

# 二、调试用的软件

阿里云（https://iot.aliyun.com）（2025年2月1日起不再支持新购！）


# 三、软件功能

该项目基于 ESP32 平台和 ESP - IDF 框架开发，主要实现与阿里云物联网平台的连接和数据交互功能，同时具备 Wi - Fi 连接管理和设备动态注册等能力，下面为你详细总结：

## 1. Wi - Fi 连接管理

初始化：在 wifi_manager_init 函数中创建事件组，启动 wifi_manager_task 任务。

配置加载：从 NVS（非易失性存储）读取 Wi - Fi 的 SSID 和密码，若未设置则使用默认值。

连接管理：wifi_manager_task 任务负责处理 Wi - Fi 连接，监听连接成功和失败事件，连接失败时会尝试重新连接。

时间同步：连接成功后调用 initialize_sntp 函数进行 SNTP 时间同步。

## 2. 设备动态注册

启动注册：aliot_start_register 函数创建 aliot_register_task 任务，发起动态注册。

MQTT 连接：aliot_register_task 任务初始化 MQTT 客户端，注册事件处理函数，启动 MQTT 连接。

结果查询：提供 aliot_register_result 函数查询注册结果。

## 3. 阿里云物联网平台交互

连接管理

启动连接：aliot_start 函数创建 aliot_mqtt_run 任务，启动与阿里云物联网平台的连接。

回调设置：aliot_set_property_cb 函数设置属性下发控制回调函数。

数据上报

单个属性上报：提供 aliot_post_property_int、aliot_post_property_double 和 aliot_post_property_str 函数，分别用于上报整型、浮点型和字符串类型的单个属性值。

批量属性上报：aliot_property_report 函数创建物模型，添加多个属性值后上报。

指令接收

属性设置：aliot_mqtt_event_handler 函数监听 MQTT 消息，当收到属性设置指令时，解析参数并调用回调函数处理，同时返回属性设置确认。

## 4. 物模型处理

创建物模型：aliot_malloc_dm 函数根据不同类型（属性上报、属性设置回复、事件上报）创建物模型。

添加数据：aliot_set_dm_int、aliot_set_dm_double、aliot_set_dm_str 和 aliot_set_dm_json 函数用于向物模型添加不同类型的数据。

序列化与释放：aliot_dm_serialize 函数将物模型序列化为 JSON 字符串，aliot_free_dm 函数释放物模型内存。

## 5. 系统初始化

板级初始化：在 app_main 函数中调用 board_init 函数进行板级初始化。

命令行初始化：调用 initialize_console 和 set_cmd_init 函数初始化命令行。

启动流程：等待 Wi - Fi 连接成功后启动阿里云物联网平台连接，之后进入主循环执行其他任务。





