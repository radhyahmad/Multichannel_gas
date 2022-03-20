#include <Arduino.h>
#include <Wire.h>
#include "WiFiNINA.h"
#include "ArduinoJson.h"
#include "ArduinoMqttClient.h"
#include "Multichannel_Gas_GMXXX.h"
#include "SparkFun_GridEYE_Arduino_Library.h"
#include "configuration.h"

void initialize_sensors();
void initialize_wifi();
void initialize_mqtt();
void connect_mqtt();
void on_mqtt_message(int messageSize);
void publish_message();

WiFiClient net;
MqttClient mqttClient(net);

char ssid[] = SECRET_SSID;
char password[] = SECRET_PASS;
char broker[] = SECRET_BROKER;

String subscribe_topic = SUBSCRIBE_TOPIC;
String measurementTopic = PUBLISH_TOPIC;

GAS_GMXXX<TwoWire> gas;
GridEYE grideye;
float testPixelValue;
float hotPixelValue;
int hotPixelIndex;

#define INTERVAL (uint32_t)10000
uint32_t last_millis = 0;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  initialize_sensors();
  initialize_wifi();
  initialize_mqtt();
}
 
void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    initialize_wifi();
  }

  if (WiFi.status() == WL_CONNECTED && !(mqttClient.connected()))
  {
    connect_mqtt();
  }
  
  mqttClient.poll();

  if (millis() - last_millis > INTERVAL)
  {
    last_millis = millis();
    publish_message();
  }
}

void initialize_sensors()
{
  gas.begin(Wire, 0x08);
  grideye.begin();
}

void initialize_wifi()
{
  delay(100);
  WiFi.disconnect();
  Serial.println();
  Serial.print("Connecting WiFi to: ");
  Serial.println(ssid);

  while(WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    Serial.print("Attempting WiFi connection .....");
    
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.println("\nWiFi RSSI: ");
      Serial.println(WiFi.RSSI());
      Serial.print("Firmware version: ");
      Serial.println(WiFiClass::firmwareVersion());
    }

    else
    {
      Serial.print("Failed to connect to WiFi");
      Serial.println(", Try again in 5 seconds");
      delay(5000);
    }
    
  }
}

void initialize_mqtt()
{
  mqttClient.setId(CLIENT_ID);
  mqttClient.setUsernamePassword(MQTT_USERNAME, MQTT_PASSWORD);
  mqttClient.onMessage(on_mqtt_message);
}

void connect_mqtt()
{
  Serial.println("Connecting MQTT .....");

  while (!mqttClient.connected())
  {
    Serial.print("Attempting to MQTT broker: ");
    //initializeMqtt();

    if (!mqttClient.connect(broker, (uint32_t)1883))
    {
      Serial.print("MQTT connection failed! Error code: ");
      Serial.println(mqttClient.connectError());
      Serial.println(" Try again in 5 seconds");
      digitalWrite(LED_BUILTIN, LOW);
      delay(5000);
    }

    else
    {
      Serial.println("Connected to MQTT broker");
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println();
    }
    
    mqttClient.subscribe(subscribe_topic.c_str());
  }
}

void on_mqtt_message(int messageSize) 
{
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  while (mqttClient.available() != (uint32_t)0)
  {
    Serial.print((char)mqttClient.read());
  }

  Serial.println();
}

void publish_message()
{
  static char payload[256];
  StaticJsonDocument<256>doc;

  doc["id"] = "a1BxBIKMsfi.seeed_gasmultisensor";
  JsonObject params = doc.createNestedObject("params");
  params["NO2"] = gas.measure_NO2();
  params["C2H5OH"] = gas.measure_C2H5OH();
  params["VOC"] = gas.measure_VOC();
  params["CO"] = gas.measure_CO();

  testPixelValue = 0;
  hotPixelValue = 0;
  hotPixelIndex = 0;

  for (unsigned char i = 0; i < 64; i++)
  {
    testPixelValue = grideye.getPixelTemperature(i);
    if (testPixelValue > hotPixelValue)
    {
      hotPixelValue = testPixelValue;
      hotPixelIndex = i;
    }
    
  }
  params["temperature"] = hotPixelValue;
  
  doc["method"] = "thing.event.property.post";
  serializeJson(doc, payload);

  Serial.println("Publishing message: ");
  mqttClient.beginMessage(measurementTopic.c_str());
  mqttClient.print(payload);
  Serial.println(payload);
  mqttClient.endMessage();
}