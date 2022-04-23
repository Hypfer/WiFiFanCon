#include "config.h"

#include <WiFiManager.h>


WiFiManager wifiManager;

WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, sizeof(mqtt_server));
WiFiManagerParameter custom_mqtt_user("user", "MQTT username", mqtt_username, sizeof(mqtt_username));
WiFiManagerParameter custom_mqtt_pass("pass", "MQTT password", mqtt_password, sizeof(mqtt_password));


unsigned long lastLog = millis();
const long logInterval = 1000;


char identifier[24];




bool shouldSaveConfig = false;

void saveConfigCallback () {
  shouldSaveConfig = true;
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Hello there");



  //wifiManager.resetSettings(); //TODO


  snprintf(identifier, sizeof(identifier), "FANCON-%X", getChipId());

  Serial.println(identifier);

  loadConfig();

  setupWifi();

  initTacho();
  initPWM();

  initOTA();
  initMQTT();
}


void setupWifi() {
  wifiManager.setDebugOutput(false);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);

  WiFi.setHostname(identifier);
  wifiManager.autoConnect(identifier);

  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_username, custom_mqtt_user.getValue());
  strcpy(mqtt_password, custom_mqtt_pass.getValue());

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  if (shouldSaveConfig) {
    saveConfig();
  } else {
    //For some reason, the read values get overwritten in this function
    //To combat this, we just reload the config
    //This is most likely a logic error which could be fixed otherwise
    loadConfig();
  }
}

void loop() {
  updateTacho();

  loopMQTT();
  loopOTA();

  if ( (logInterval <= (millis() - lastLog)) )  {
    lastLog = millis();

    Serial.print("FAN1: ");
    Serial.print(getDutyCycle1());
    Serial.print("% ");
    Serial.print(getTacho1());
    Serial.println(" (Tacho)");

    Serial.print("FAN2: ");
    Serial.print(getDutyCycle2());
    Serial.print("% ");
    Serial.print(getTacho2());
    Serial.println(" (Tacho)");

    Serial.print("FAN3: ");
    Serial.print(getDutyCycle3());
    Serial.print("% ");
    Serial.print(getTacho3());
    Serial.println(" (Tacho)");

    Serial.print("FAN4: ");
    Serial.print(getDutyCycle4());
    Serial.print("% ");
    Serial.print(getTacho4());
    Serial.println(" (Tacho)");
  }

}
