#include <Arduino.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <Secrets.h>

U8G2_SSD1322_NHD_256X64_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/15, /* dc=*/4, /* reset=*/5);
const int CHIP_ID = ESP.getChipId();
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void drawModeNormal(int tempInside, int tempOutside, const char *time, const char *data, int contrast)
{
  u8g2.firstPage();
  u8g2.setContrast(contrast);
  do
  {
    u8g2.setFont(u8g2_font_logisoso34_tf);
    u8g2.setCursor(0, 34);
    u8g2.print(tempInside);
    u8g2.setFont(u8g2_font_logisoso16_tf);
    u8g2.print("°C");

    u8g2.setFont(u8g2_font_logisoso34_tf);
    u8g2.setCursor(69, 34);
    u8g2.print(tempOutside);
    u8g2.setFont(u8g2_font_logisoso16_tf);
    u8g2.print("°C");

    u8g2.setFont(u8g2_font_logisoso38_tf);
    u8g2.setCursor(142, 38);
    u8g2.print(time);

    u8g2.drawLine(0, 45, 255, 45);

    u8g2.setFont(u8g2_font_8x13_t_symbols);
    u8g2.drawUTF8(0, 62, data);
  } while (u8g2.nextPage());
}

void loop(void)
{
  mqttClient.loop();
}

void logT(const char *s)
{
  Serial.println(s);
  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_luRS10_te);
    u8g2.setCursor(0, 24);
    u8g2.print(s);
  } while (u8g2.nextPage());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);
  const char *time = doc["time"];
  const char *data = doc["data"];
  int inside = doc["inside"];
  int outside = doc["outside"];
  int contrast = doc["contrast"];
  bool disable = doc["disabled"];
  if (!disable)
  {
    drawModeNormal(inside, outside, time, data, contrast);
  }
  else
  {
    u8g2.firstPage();
    do
    {
    } while (u8g2.nextPage());
  }
}

void startWifi(void)
{
  WiFi.begin(wifiAP, wifiPassword);
  logT("Wifi connecting ...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  logT("Wifi connected");
}

void startMqtt()
{
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
  while (!mqttClient.connected())
  {
    String clientId = "iotdisplay-";
    clientId += String(random(0xffff), HEX);
    Serial.printf("MQTT connecting as client %s...\n", clientId.c_str());
    if (mqttClient.connect(clientId.c_str()))
    {
      logT("MQTT connected");
      delay(1000);
      mqttClient.subscribe(mqttTopicText);
      logT("MQTT subscribed");
    }
    else
    {
      Serial.println("MQTT failed, retrying..." + mqttClient.state());
      delay(2500);
    }
  }
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println(CHIP_ID + " starting");

  u8g2.begin();
  u8g2.enableUTF8Print();

  startWifi();
  delay(1000);
  startMqtt();
  delay(1000);
}