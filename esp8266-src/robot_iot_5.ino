
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <time.h>
#include "robosend.h"

#include "secrets.h"

#define emptyString String()

WiFiClientSecure net;

BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

MQTTClient client(32);

time_t now;
time_t nowish = 1510592825;

void NTPConnect(void)
{
  Serial.print(F("Setting time using SNTP"));
  configTime(TIME_ZONE * 3600, DST * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(F("."));
    now = time(nullptr);
  }
  Serial.println(F("done!"));
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

void connectToWiFi(String init_str)
{
  if (init_str != emptyString)
    Serial.print(init_str);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(F("."));
    delay(1000);
  }
  if (init_str != emptyString)
    Serial.println(F("ok!"));
}

void connectToMqtt() {
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  client.subscribe(MQTT_SUB_TOPIC);
}

void checkWiFiThenMQTT(void)
{
  connectToWiFi(F("Checking WiFi"));
  connectToMqtt();
}

#define LED 2  //On board LED

void messageReceived(String &topic, String &payload)
{
  digitalWrite(LED,0);
  Serial.println("Received [" + topic + "]: " + payload);
  
  char robo_cmd_str[sizeof(payload)];
  payload.toCharArray(robo_cmd_str, sizeof(robo_cmd_str));

  u_char robo_cmd = (u_char)strtol(robo_cmd_str, NULL, 16);
    
  robo_send(robo_cmd);
  digitalWrite(LED,1);
}

void changeState()
{
  digitalWrite(LED, !(digitalRead(LED)));  //Invert Current State of LED  
}

void setup()
{
  pinMode(LED,OUTPUT);
  digitalWrite(LED,1);
  
  Serial.begin(115200);
  delay(500);
  Serial.println("");
  Serial.println("RUN SETUP");
  Serial.print("D2 is");
  Serial.println(D2);
  
  WiFi.hostname(THINGNAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  connectToWiFi(String(F("Attempting to connect to SSID: ")) + String(ssid));

  NTPConnect();
  
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
  
  client.begin(MQTT_HOST, MQTT_PORT, net);
  client.onMessage(messageReceived);
  
  print_info();
  
  Serial.println(F("Start MQTT Setup *******"));
  connectToMqtt();
  Serial.println(F("MQTT Setup DONE *******"));
  
  digitalWrite(LED,0);
  ESP.wdtFeed();
  delay(1000);
  ESP.wdtFeed();
  delay(1000);
  ESP.wdtFeed();
  delay(1000);
  ESP.wdtFeed();
  delay(1000);
  ESP.wdtFeed();
  digitalWrite(LED,1);
  
  Serial.println(F("Start Robo IR Setup *******"));
  robo_setup();
  Serial.println(F("Robo IR Setup DONE*******"));
}

int led_count = 0;

void loop()
{
  if (!client.connected())
  {
    Serial.print(F("Not Connected"));
    checkWiFiThenMQTT();
  }
  else
  {
    client.loop();
    delay(10);
    
    led_count++;
    if (led_count < 50) {
      digitalWrite(LED, 0);
    }
    else {
      digitalWrite(LED, 1);
    }
    if (led_count > 750) {
      led_count = 0;
    }
  }
}
