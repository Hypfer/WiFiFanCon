#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"


#define FIRMWARE_PREFIX "wifi-fancon"
#define AVAILABILITY_ONLINE "online"
#define AVAILABILITY_OFFLINE "offline"
char MQTT_TOPIC_AVAILABILITY[128];
char MQTT_TOPIC_STATE[128];
char MQTT_TOPIC_COMMAND[128];

char MQTT_TOPIC_AUTOCONF_WIFI_SENSOR[128];

char MQTT_TOPIC_AUTOCONF_FAN1[128];
char MQTT_TOPIC_AUTOCONF_FAN2[128];
char MQTT_TOPIC_AUTOCONF_FAN3[128];
char MQTT_TOPIC_AUTOCONF_FAN4[128];


unsigned long lastMqttConnectionAttempt = 0;
const long mqttConnectionInterval = 60000;

unsigned long stateUpdatePreviousMillis = millis();
const long stateUpdateInterval = 30000;

PubSubClient mqttClient;
WiFiClient wifiClient;


void initMQTT(void) {
  snprintf(MQTT_TOPIC_AVAILABILITY, 127, "%s/%s/status", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_STATE, 127, "%s/%s/state", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_COMMAND, 127, "%s/%s/command", FIRMWARE_PREFIX, identifier);

  snprintf(MQTT_TOPIC_AUTOCONF_WIFI_SENSOR, 127, "homeassistant/sensor/%s/%s_wifi/config", FIRMWARE_PREFIX, identifier);

  snprintf(MQTT_TOPIC_AUTOCONF_FAN1, 127, "homeassistant/fan/%s/%s_fan1/config", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_FAN2, 127, "homeassistant/fan/%s/%s_fan2/config", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_FAN3, 127, "homeassistant/fan/%s/%s_fan3/config", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_FAN4, 127, "homeassistant/fan/%s/%s_fan4/config", FIRMWARE_PREFIX, identifier);


  mqttClient.setClient(wifiClient);
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setKeepAlive(10);
  mqttClient.setBufferSize(2048);
  mqttClient.setCallback(mqttCallback);

  Serial.println("MQTT client initialized");
}


void publishState(void) {
  DynamicJsonDocument wifiJson(192);
  DynamicJsonDocument fan1Json(128);
  DynamicJsonDocument fan2Json(128);
  DynamicJsonDocument fan3Json(128);
  DynamicJsonDocument fan4Json(128);
  DynamicJsonDocument stateJson(1024);
  char payload[512];

  wifiJson["ssid"] = WiFi.SSID();
  wifiJson["ip"] = WiFi.localIP().toString();
  wifiJson["rssi"] = WiFi.RSSI();
  stateJson["wifi"] = wifiJson.as<JsonObject>();


  fan1Json["tacho"] = getTacho1();
  fan1Json["dutyCycle"] = getDutyCycle1();
  fan1Json["state"] = getDutyCycle1() > 0 ? "ON" : "OFF";

  stateJson["FAN1"] = fan1Json.as<JsonObject>();

  fan2Json["tacho"] = getTacho2();
  fan2Json["dutyCycle"] = getDutyCycle2();
  fan2Json["state"] = getDutyCycle2() > 0 ? "ON" : "OFF";

  stateJson["FAN2"] = fan2Json.as<JsonObject>();

  fan3Json["tacho"] = getTacho3();
  fan3Json["dutyCycle"] = getDutyCycle3();
  fan3Json["state"] = getDutyCycle3() > 0 ? "ON" : "OFF";

  stateJson["FAN3"] = fan3Json.as<JsonObject>();

  fan4Json["tacho"] = getTacho4();
  fan4Json["dutyCycle"] = getDutyCycle4();
  fan4Json["state"] = getDutyCycle4() > 0 ? "ON" : "OFF";

  stateJson["FAN4"] = fan4Json.as<JsonObject>();


  serializeJson(stateJson, payload);
  mqttClient.publish(MQTT_TOPIC_STATE, payload, true);

  stateUpdatePreviousMillis = millis();
}

void publishAutoConfig(void) {
  char mqttPayload[2048];
  DynamicJsonDocument device(256);
  DynamicJsonDocument autoconfPayload(1024);
  StaticJsonDocument<64> identifiersDoc;
  JsonArray identifiers = identifiersDoc.to<JsonArray>();

  identifiers.add(identifier);

  device["identifiers"] = identifiers;
  device["manufacturer"] = "Hypfer";
  device["model"] = "WiFiFanCon";
  device["name"] = identifier;
  device["sw_version"] = "2022.03.0";


  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["name"] = identifier + String(" WiFi");
  autoconfPayload["value_template"] = "{{value_json.wifi.rssi}}";
  autoconfPayload["unique_id"] = identifier + String("_wifi");
  autoconfPayload["unit_of_measurement"] = "dBm";
  autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["json_attributes_template"] = "{\"ssid\": \"{{value_json.wifi.ssid}}\", \"ip\": \"{{value_json.wifi.ip}}\"}";
  autoconfPayload["icon"] = "mdi:wifi";

  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(MQTT_TOPIC_AUTOCONF_WIFI_SENSOR, mqttPayload, true);

  autoconfPayload.clear();


  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["name"] = identifier + String(" FAN1");
  autoconfPayload["unique_id"] = identifier + String("_fan1");

  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["state_value_template"] = "{{value_json.FAN1.state}}";
  autoconfPayload["command_topic"] = MQTT_TOPIC_COMMAND;
  autoconfPayload["command_template"] = "{\"FAN1\": {\"state\": \"{{value}}\"}}";

  autoconfPayload["percentage_state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["percentage_command_topic"] = MQTT_TOPIC_COMMAND;
  autoconfPayload["percentage_value_template"] = "{{value_json.FAN1.dutyCycle}}";
  autoconfPayload["percentage_command_template"] = "{\"FAN1\": {\"dutyCycle\": {{value}}}}";

  autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["json_attributes_template"] = "{\"tacho\": \"{{value_json.FAN1.tacho}}\"}";


  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(MQTT_TOPIC_AUTOCONF_FAN1, mqttPayload, true);

  autoconfPayload.clear();

  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["name"] = identifier + String(" FAN2");
  autoconfPayload["unique_id"] = identifier + String("_fan2");

  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["state_value_template"] = "{{value_json.FAN2.state}}";
  autoconfPayload["command_topic"] = MQTT_TOPIC_COMMAND;
  autoconfPayload["command_template"] = "{\"FAN2\": {\"state\": \"{{value}}\"}}";

  autoconfPayload["percentage_state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["percentage_command_topic"] = MQTT_TOPIC_COMMAND;
  autoconfPayload["percentage_value_template"] = "{{value_json.FAN2.dutyCycle}}";
  autoconfPayload["percentage_command_template"] = "{\"FAN2\": {\"dutyCycle\": {{value}}}}";

  autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["json_attributes_template"] = "{\"tacho\": \"{{value_json.FAN2.tacho}}\"}";


  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(MQTT_TOPIC_AUTOCONF_FAN2, mqttPayload, true);

  autoconfPayload.clear();


  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["name"] = identifier + String(" FAN3");
  autoconfPayload["unique_id"] = identifier + String("_fan3");

  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["state_value_template"] = "{{value_json.FAN3.state}}";
  autoconfPayload["command_topic"] = MQTT_TOPIC_COMMAND;
  autoconfPayload["command_template"] = "{\"FAN3\": {\"state\": \"{{value}}\"}}";

  autoconfPayload["percentage_state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["percentage_command_topic"] = MQTT_TOPIC_COMMAND;
  autoconfPayload["percentage_value_template"] = "{{value_json.FAN3.dutyCycle}}";
  autoconfPayload["percentage_command_template"] = "{\"FAN3\": {\"dutyCycle\": {{value}}}}";

  autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["json_attributes_template"] = "{\"tacho\": \"{{value_json.FAN3.tacho}}\"}";


  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(MQTT_TOPIC_AUTOCONF_FAN3, mqttPayload, true);

  autoconfPayload.clear();


  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["name"] = identifier + String(" FAN4");
  autoconfPayload["unique_id"] = identifier + String("_fan4");

  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["state_value_template"] = "{{value_json.FAN4.state}}";
  autoconfPayload["command_topic"] = MQTT_TOPIC_COMMAND;
  autoconfPayload["command_template"] = "{\"FAN4\": {\"state\": \"{{value}}\"}}";

  autoconfPayload["percentage_state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["percentage_command_topic"] = MQTT_TOPIC_COMMAND;
  autoconfPayload["percentage_value_template"] = "{{value_json.FAN4.dutyCycle}}";
  autoconfPayload["percentage_command_template"] = "{\"FAN4\": {\"dutyCycle\": {{value}}}}";

  autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["json_attributes_template"] = "{\"tacho\": \"{{value_json.FAN4.tacho}}\"}";


  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(MQTT_TOPIC_AUTOCONF_FAN4, mqttPayload, true);

  autoconfPayload.clear();


}

void reconnectMQTT(void) {
  for (int attempt = 0; attempt < 3; ++attempt) {
    Serial.println("Connecting to MQTT Broker...");

    if (mqttClient.connect(identifier, mqtt_username, mqtt_password, MQTT_TOPIC_AVAILABILITY, 1, true, AVAILABILITY_OFFLINE)) {
      Serial.println("Successfully connected to MQTT broker");

      mqttClient.publish(MQTT_TOPIC_AVAILABILITY, AVAILABILITY_ONLINE, true);
      publishAutoConfig();

      mqttClient.subscribe(MQTT_TOPIC_COMMAND);
      break;
    } else {
      Serial.print("Failed to connect to MQTT broker: ");
      Serial.println(mqttClient.state());

      delay(5000);
    }
  }
}


void loopMQTT(void) {
  mqttClient.loop();

  if (
    !mqttClient.connected() &&
    (
      (mqttConnectionInterval <= (millis() - lastMqttConnectionAttempt)) ||
      lastMqttConnectionAttempt == 0
    )
  )  {
    lastMqttConnectionAttempt = millis();
    reconnectMQTT();
  }

  if (stateUpdateInterval <= (millis() - stateUpdatePreviousMillis)) {
    publishState();
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  boolean somethingChanged = false;

  if (strcmp(topic, MQTT_TOPIC_COMMAND) == 0) {
    DynamicJsonDocument commandJson(1024);
    char payloadText[length + 1];

    snprintf(payloadText, length + 1, "%s", payload);

    DeserializationError err = deserializeJson(commandJson, payloadText);

    if (!err) {
      JsonObject fan1Command = commandJson["FAN1"].as<JsonObject>();

      if (!fan1Command.isNull()) {
        if (fan1Command.containsKey("dutyCycle")) {
          int fan1DutyCycleCommand = fan1Command["dutyCycle"];

          Serial.print("Setting FAN1 duty cycle to ");
          Serial.println(fan1DutyCycleCommand);

          setDutyCycle1(fan1DutyCycleCommand);
          somethingChanged = true;
        } else if (fan1Command.containsKey("state")) {
          String fan1StateCommand = fan1Command["state"].as<String>();

          if (fan1StateCommand == "ON") {
            if (getDutyCycle1() <= 0) {
              Serial.println("Setting FAN1 to ON");

              setDutyCycle1(initialDutyCycle);
              somethingChanged = true;
            } else {
              Serial.println("FAN1 is already ON");
            }
          } else if (fan1StateCommand == "OFF") {
            Serial.println("Setting FAN1 to OFF");

            setDutyCycle1(0);

            somethingChanged = true;
          }
        }
      }

      JsonObject fan2Command = commandJson["FAN2"].as<JsonObject>();

      if (!fan2Command.isNull()) {
        if (fan2Command.containsKey("dutyCycle")) {
          int fan2DutyCycleCommand = fan2Command["dutyCycle"];

          Serial.print("Setting FAN2 duty cycle to ");
          Serial.println(fan2DutyCycleCommand);

          setDutyCycle2(fan2DutyCycleCommand);
          somethingChanged = true;
        } else if (fan2Command.containsKey("state")) {
          String fan2StateCommand = fan2Command["state"].as<String>();

          if (fan2StateCommand == "ON") {
            if (getDutyCycle2() <= 0) {
              Serial.println("Setting FAN2 to ON");

              setDutyCycle2(initialDutyCycle);
              somethingChanged = true;
            } else {
              Serial.println("FAN2 is already ON");
            }
          } else if (fan2StateCommand == "OFF") {
            Serial.println("Setting FAN2 to OFF");

            setDutyCycle2(0);

            somethingChanged = true;
          }
        }
      }

      JsonObject fan3Command = commandJson["FAN3"].as<JsonObject>();

      if (!fan3Command.isNull()) {
        if (fan3Command.containsKey("dutyCycle")) {
          int fan3DutyCycleCommand = fan3Command["dutyCycle"];

          Serial.print("Setting FAN3 duty cycle to ");
          Serial.println(fan3DutyCycleCommand);

          setDutyCycle3(fan3DutyCycleCommand);
          somethingChanged = true;
        } else if (fan3Command.containsKey("state")) {
          String fan3StateCommand = fan3Command["state"].as<String>();

          if (fan3StateCommand == "ON") {
            if (getDutyCycle3() <= 0) {
              Serial.println("Setting FAN3 to ON");

              setDutyCycle3(initialDutyCycle);
              somethingChanged = true;
            } else {
              Serial.println("FAN3 is already ON");
            }
          } else if (fan3StateCommand == "OFF") {
            Serial.println("Setting FAN3 to OFF");

            setDutyCycle3(0);

            somethingChanged = true;
          }
        }
      }

      JsonObject fan4Command = commandJson["FAN4"].as<JsonObject>();

      if (!fan4Command.isNull()) {
        if (fan4Command.containsKey("dutyCycle")) {
          int fan4DutyCycleCommand = fan4Command["dutyCycle"];

          Serial.print("Setting FAN4 duty cycle to ");
          Serial.println(fan4DutyCycleCommand);

          setDutyCycle4(fan4DutyCycleCommand);
          somethingChanged = true;
        } else if (fan4Command.containsKey("state")) {
          String fan4StateCommand = fan4Command["state"].as<String>();

          if (fan4StateCommand == "ON") {
            if (getDutyCycle4() <= 0) {
              Serial.println("Setting FAN4 to ON");

              setDutyCycle4(initialDutyCycle);
              somethingChanged = true;
            } else {
              Serial.println("FAN4 is already ON");
            }
          } else if (fan4StateCommand == "OFF") {
            Serial.println("Setting FAN4 to OFF");

            setDutyCycle4(0);

            somethingChanged = true;
          }
        }
      }
    }
  }

  if (somethingChanged) {
    publishState();
  }
}
