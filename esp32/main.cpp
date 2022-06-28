#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>

HTTPClient http;

String FirmwareVer = "1.2";
const char *currentVer = FirmwareVer.c_str();

#define wifi_ssid "THANAKON 2G"
#define wifi_pass "07082543"

#define url_firmware_ver "https://raw.githubusercontent.com/JameWasin/jws_auto_update/main/esp32/fw_version.txt"
#define url_firmware_bin "https://raw.githubusercontent.com/JameWasin/jws_auto_update/main/esp32/fw.bin"

byte connState = 0;
void wifiConnect()
{
  if (connState == 0)
  {
    WiFi.begin(wifi_ssid, wifi_pass);
    byte timeoutConn = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
      timeoutConn++;
      if (timeoutConn == 15)
        ESP.restart();
    }
    Serial.println("");
    Serial.println("WiFi connected");
    connState = 1;
  }
}

int checkVersion()
{
  wifiConnect();

  http.begin(url_firmware_ver);
  int httpCode = http.GET();
  String newVersion;
  Serial.print("\n Current  Version is : " + String(currentVer));
  // Serial.print("\n httpCode  is : " + String(httpCode));
  if (httpCode == HTTP_CODE_OK)
  { // received ok code 200
    newVersion = http.getString();
    const char *newVer = newVersion.c_str();
    Serial.print("\n New Version is : " + String(newVer));
    // Serial.print("\n strcmp Ver is : " + String(strcmp(currentVer, newVer)));
    if (strcmp(currentVer, newVer) == 0)
    {
      Serial.println("\n Device already on latest firmware version");
      return 0;
    }
    else
      return 1;
  }
  else
  {
    Serial.print("\n error in downloading version file:" + String(httpCode));
  }
  http.end();
}

void firmwareUpdate()
{
  if ((WiFi.status() == WL_CONNECTED))
  {
    Serial.println("\n Start update firmware");
    t_httpUpdate_return ret = ESPhttpUpdate.update(url_firmware_bin);
    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\nStart Device.\n");
}

// unsigned long previousMillis = 0;
// const long interval = 2000;

void loop()
{
  // unsigned long currentMillis = millis();
  // if ((currentMillis - previousMillis) >= interval)
  //{
  // previousMillis = currentMillis;
  //  if (checkVersion() == 1) {
  //    firmwareUpdate();
  //  }
  //}

  if (Serial.available() > 0)
  {
    String command = Serial.readString();
    command.trim();
    Serial.println(command);
    if (command == "data")
    {
      checkVersion();
    }
    else if (command == "update")
    {
      if (checkVersion() == 1)
      {
        firmwareUpdate();
      }
    }
    else
      Serial.println("unknown command");
  }
}