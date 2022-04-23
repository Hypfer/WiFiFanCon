#include <FS.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>

void saveConfig() {
  DynamicJsonDocument json(512);
  json["mqtt_server"] = mqtt_server;
  json["username"] = mqtt_username;
  json["password"] = mqtt_password;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    return;
  }

  serializeJson(json, configFile);
  configFile.close();
}

void loadConfig() {
  if (SPIFFS.begin(true)) {

    if (SPIFFS.exists("/config.json")) {
      File configFile = SPIFFS.open("/config.json", "r");

      if (configFile) {
        const size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument json(512);

        if (DeserializationError::Ok == deserializeJson(json, buf.get())) {
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_username, json["username"]);
          strcpy(mqtt_password, json["password"]);
        }
      }
    }
  }
}
