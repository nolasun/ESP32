# 一、硬件

ESP32开发板

# 二、调试用的软件

APP: ESPtouchV2 

windows10客户端软件: MQTTX

# 三、软件功能

ESP32通过MQTT协议订阅/test/topic2主题的内容，通过MQTT协议往主题/test/topic1发送内容（延时2秒发布一条消息到/test/topic1主题）

# 四、调试方法

1. 打开手机热点，给ESP32上电（注意代码里面设置的热点名和密码必须与真是手机热点和密码一致）。同时需要确保能通过该热点访问MQTT服务器mqtt://broker-cn.emqx.io

2. 在MQTTX上往/test/topic2主题发送内容，可以看到ESP32立即打印了该内容

3. 在MQTTX上订阅了/test/topic1的内容，可以看到MQTTX显示了ESP32发布的内容



