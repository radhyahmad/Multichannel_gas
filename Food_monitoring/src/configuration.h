#include <Arduino.h>

// Fill in  your WiFi networks SSID and password
#define SECRET_SSID "LANTAI BAWAH 2"
#define SECRET_PASS "ibudini17"

// Fill in the hostname of your IoT broker
#define SECRET_BROKER "a1BxBIKMsfi.iot-as-mqtt.cn-shanghai.aliyuncs.com" 
 
#define CLIENT_ID "a1BxBIKMsfi.seeed_gasmultisensor|securemode=2,signmethod=hmacsha256,timestamp=1647744414359|" //"a1crwz5OZDi.airQuality|securemode=2,signmethod=hmacsha256,timestamp=2524608000000|" 
#define MQTT_USERNAME "seeed_gasmultisensor&a1BxBIKMsfi" 
#define MQTT_PASSWORD "8e20a19bb86b61f2ca54bcb4a34a34a8d3d4ebdb67ccb25035b59ea71f4a9ef8" //"1904c29b8c9de119fcb311e2765bb69cdd6824293a6a093b6e377ffd886a6f73" 

// Fill in the publisher and subscriber topic
#define PUBLISH_TOPIC "/sys/a1BxBIKMsfi/seeed_gasmultisensor/thing/event/property/post"
#define SUBSCRIBE_TOPIC "/sys/a1BxBIKMsfi/seeed_gasmultisensor/thing/service/property/set"