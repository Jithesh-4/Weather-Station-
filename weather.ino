#include "Arial_black_16.h"
#include "Arial_Black_16_ISO_8859_1.h"
#include "Arial14.h"
#include "SystemFont5x7.h"

#include <DMD.h> 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DMDESP.h>
#include <fonts/ElektronMart6x8.h>
#include <fonts/Mono5x7.h>

const char* ssid = "CIT-Campus";
const char* password = "citchennai";
const char* apiKey = "f844ea07cffb5358e45a8b0a876b189b";
const char* city = "Chennai";
const char* country = "IN";
const char* server = "api.openweathermap.org";

#define DISPLAYS_WIDE 2 // Panel Columns
#define DISPLAYS_HIGH 1 // Panel Rows
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);  // Number of Panels P10 used (Column, Row)

String weatherData = "";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  
  Disp.start(); // Run the DMDESP library
  Disp.setBrightness(50); // Brightness level
  Disp.setFont(SystemFont5x7); // Determine the font used

  fetchWeatherData();
}

void loop() {
  Disp.loop(); // Run "Disp.loop" to refresh the LED
  //Disp.drawText(0, 0, "CIT WEATHER"); // Display static text at the top
  Scrolling_Text(4, 25); // Show running text "Scrolling_Text(y position, speed);"

  static unsigned long lastFetchTime = 0;
  if (millis() - lastFetchTime > 600000) { // Fetch new data every 10 minutes
    fetchWeatherData();
    lastFetchTime = millis();

  }
}


void fetchWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    String url = String("http://") + server + "/data/2.5/weather?q=" + city + "," + country + "&appid=" + apiKey;
    http.begin(client, url);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, payload);
      
      const char* description = doc["weather"][0]["description"];
      float tempK = doc["main"]["temp"];
      float tempC = tempK - 273.15; // Convert from Kelvin to Celsius
      int humidity = doc["main"]["humidity"];
      int pressure = doc["main"]["pressure"];
      float speed = doc["wind"]["speed"];
      int sunrise = doc["sys"]["sunrise"];
      int sunset = doc["sys"]["sunset"];
      
      // Create a single string containing all the data
      weatherData = "CIT IOT WEATHER STATION     Weather: " +   String(description) +   " Temp: " + String(tempC, 2) + "°C Humidity:" + String(humidity) + "%  Pressure: " + String(pressure) + "hPa Wind: " + String(speed) + " m/s";
      Serial.println(weatherData);
    } else {
      Serial.println("Error in HTTP request");
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void Scrolling_Text(int y, uint8_t scrolling_speed) {
  static uint32_t pM;
  static uint32_t x;
  int width = Disp.width();
  Disp.setFont(SystemFont5x7);
  int fullScroll = Disp.textWidth(weatherData) + width;
  if((millis() - pM) > scrolling_speed) { 
    pM = millis();
    if (x < fullScroll) {
      ++x;
    } else {
      x = 0;
      return;
    }
    Disp.drawText(width - x, y, weatherData.c_str());
  }  
}
