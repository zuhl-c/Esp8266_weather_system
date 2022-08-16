/*ESP8266 microcontroller and ssd1306 128x64 oled screen based IoT weather station*/
/*It will display the current weather prediction from internet through using WiFi network*/
/* www.visualcorssing.com API is used for this project*/
/*This program was written by zuhl , www.github.com/zuhl-c */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <logo.h>
#include <moonicons.h>
#include <weathericons.h>
#include <Fonts/FreeSans12pt7b.h>//mono bold  //ok tested 1
#include <Fonts/FreeSansBold9pt7b.h>//sans bold //ok tested 2

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "JioFi2_0BF789";
const char* password = "q5jbu95gug";
String W_STATUS[]={"WiFi IN PROGRESS","SSID NOT FOUND","SCAN COMPLETED","WiFi CONNECTED","WiFi CONNECTION FAILED","WiFi CONNECTION LOST","WRONG PASSWORD","CONNECTING WiFi"};

String api_key = "7NNBGDJMUFCVAAACKUYP93ERF";
String location = "Nelluwaya";

unsigned long previousTime = 0;
long interval = 0;
int menu = 0;
int button = D5;

float temp;
float moonphase;
String icon;
String condition;
String sunRise;
String sunSet;
int humidity;
int windspeed;
int pressure;
int visibility;
int cloudcover;
int solarRadiation;
int solarEnergy;
int uvIndex;

HTTPClient http;
WiFiClientSecure client;

void setup()
{
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  WiFi.begin(ssid,password);
  delay(500);
  
  printAuthor();
  
  while (WiFi.status() != WL_CONNECTED){
    printWifiStatus();
    delay(500);
  }
  
  printWifiStatus();
  delay(1000);
}

void loop() {
  
  if(WiFi.status()==WL_CONNECTED){
    unsigned long currentTime = millis();
    if(currentTime - previousTime > interval){
      previousTime = currentTime;
      interval = 60000;
      getData();
      }
      else{
        if(digitalRead(button)==HIGH){
          delay(500);
          menu++;
          if(menu>3){menu=0;}
          printData();
        }
       }
    }
    else{
      printWifiStatus();
    }
  }

void printWifiStatus(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(16,32);
  display.print(W_STATUS[WiFi.status()]);
  display.display();
}
void getData(){
  
  String url = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/"+ location+ "/today?unitGroup=metric&include=current&alerts&key="+ api_key +"&contentType=json";
  
  display.clearDisplay();
  display.setCursor(16,20);
  display.print("LOADING DATA");
  display.setCursor(8,32);
  display.printf("AREA : %s",location);
  display.display();
  
  client.setInsecure();
  http.begin(client,url);
  int httpResponse=http.GET();
  
  if(httpResponse!=200){
    display.clearDisplay();
    display.setCursor(16,32);
    display.printf("HTTP %d",httpResponse);
    display.display();
    return;
  }
  String responseData=http.getString();
  http.end();
  
  int capacity = 3072;
  DynamicJsonDocument doc(capacity);
  DeserializationError err = deserializeJson(doc,responseData);
  if(err){
    display.clearDisplay();
    display.setCursor(0,32);
    display.print("DESERIALIZATION ERROR");
    display.setCursor(16,48);
    display.print(err.c_str());
    display.display();
    return;
  }
  
  temp = doc["currentConditions"]["temp"];
  icon = String(doc["currentConditions"]["icon"]);
  condition = String(doc["currentConditions"]["conditions"]);
  humidity = doc["currentConditions"]["humidity"];
  windspeed = doc["currentConditions"]["windspeed"];
  visibility = doc["currentConditions"]["visibility"];
  cloudcover = doc["currentConditions"]["cloudcover"];
  pressure = doc["currentConditions"]["pressure"];
  sunRise = String (doc["currentConditions"]["sunrise"]);
  sunSet = String (doc["currentConditions"]["sunset"]);
  solarRadiation = doc["currentConditions"]["solarradiation"];
  solarEnergy = doc["currentConditions"]["solarenergy"];
  uvIndex = doc["currentConditions"]["uvindex"];
  moonphase = doc["currentConditions"]["moonphase"];

  printData();
  
}

void printData(){
  
  display.clearDisplay();
  if(menu==0){
      if(icon=="clear-day"){
    display.drawBitmap(2,0,clear_day,48,48,1);
    display.display();
  }
  else if(icon=="clear-night"){
    display.drawBitmap(2,0,clear_night,48,48,1);
    display.display();
  }
  else if(icon=="cloudy"){
    display.drawBitmap(2,0,cloud,48,48,1);
    display.display();
  }
  else if(icon=="partly-cloudy-day"){
    display.drawBitmap(2,0,partly_cloudy_day,48,48,1);
    display.display();
  
  }
  else if(icon="partly-cloudy-night"){
    display.drawBitmap(2,0,partly_cloudy_night,48,48,1);
    display.display();
  }
  else if(icon=="wind"){
    display.drawBitmap(2,0,wind,48,48,1);
    display.display();
  }
  else if(icon=="fog"){
    display.drawBitmap(2,0,fog,48,48,1);
    display.display();
  }
  else if(icon=="rain"){
    display.drawBitmap(2,0,rain,48,48,1);
    display.display();
  }
  else if(icon=="showers-day"){
    display.drawBitmap(2,0,showers_day,48,48,1);
    display.display();
  }
  else if(icon=="showers-night"){
    display.drawBitmap(2,0,showers_night,48,48,1);
    display.display();
  }
  else if(icon=="thunderstrom"){
    display.drawBitmap(2,0,thunderstorm,48,48,1);
    display.display();
  }
  else if(icon=="thunder-rain"){
    display.drawBitmap(2,0,thunderstorm_rain,48,48,1);
    display.display();
  }
  
  display.setCursor(56,28);
  display.setFont(&FreeSans12pt7b);
  display.printf("%.1f",temp);
  display.print("C");
  display.display();
  display.setFont();
  display.setTextSize(1);
  display.setCursor(12,52);
  display.print(condition);
  display.display();
  }
  
  else if(menu==1){
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(4,4);
    display.printf("HUMIDITY   : %d %%",humidity);
    display.setCursor(4,16);
    display.printf("PRESSURE   : %d Mb",pressure);
    display.setCursor(4,28);
    display.printf("VISIBILITY : %d Km",visibility);
    display.setCursor(4,40);
    display.printf("WINDSPEED  : %d Kmph",windspeed);
    display.setCursor(4,52);
    display.printf("CLOUDCOVER : %d %%",cloudcover);
    display.display();
  }
  
  else if(menu==2){
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(2,4);
    display.printf("SUNRISE  : %S ",sunRise);
    display.setCursor(2,16);
    display.printf("SUNSET   : %S ",sunSet);
    display.setCursor(2,28);
    display.printf("SOLAR ENERGY : %d MJ",solarEnergy);
    display.setCursor(2,40);
    display.printf("RADIATION : %d W",solarRadiation);
    display.setCursor(2,52);
    display.printf("UV INDEX  : %d ",uvIndex);
    display.display();
  }
  
  else if(menu==3){
    display.clearDisplay();
    if(moonphase==0){
      display.drawBitmap(0,0,new_moon,48,48,1);
      display.display();
    }
    else if(moonphase>0&&moonphase<0.25){
      display.drawBitmap(0,0,waxing_crescent_moon,48,48,1);
      display.display();
    }
    else if(moonphase==0.25){
      display.drawBitmap(0,0,first_half_moon,48,48,1);
      display.display();
    }
    else if(moonphase>0.25&&moonphase<0.5){
      display.drawBitmap(0,0,waxing_gibbous_moon,48,48,1);
      display.display();
    }
    else if(moonphase==0.5){
      display.drawBitmap(0,0,full_moon,48,48,1);
      display.display();
    }
    else if(moonphase>0.5&&moonphase<0.75){
      display.drawBitmap(0,0,waning_gibbous_moon,48,48,1);
      display.display();
    }
    else if(moonphase==0.75){
      display.drawBitmap(0,0,last_half_moon,48,48,1);
      display.display();
    }
    else if(moonphase>0.5&&moonphase<1){
      display.drawBitmap(0,0,waning_crescent_moon,48,48,1);
      display.display();
    }
    display.setTextSize(1);
    display.setCursor(56,16);
    display.print("MOON PHASE");
    display.setCursor(56,28);
    display.printf("%.2f Degree",moonphase);
    display.display();
  }
}

void printAuthor(){
  display.clearDisplay();
  display.drawBitmap(0,0,zuhl_logo,48,48,1);
  display.setTextColor(WHITE);
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(56,32);
  display.print("Zuhl");
  display.display();
  display.setFont();
  display.setCursor(56,40);
  display.print("www.github.c");
  display.setCursor(56,48);
  display.print("om/zuhl-c");
  display.display();
  delay(2000);
  display.clearDisplay();
}
