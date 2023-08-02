
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Keypad.h"

//FOR KEYPAD
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {D0, D3, D4, D5};
byte colPins[COLS] = {D6, D7, D8, D9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//for wifi
const char* ssid = "RR";
const char* password = "ravi9999";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://djcollection.pythonanywhere.com/cam/1/";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 9000;

String sensorReadings;
float sensorReadingsArr[3];
int run_setup = 0;
void setup() {
  lcd.begin();
  lcd.backlight();
  if (run_setup == 0)
    lcd.print("starting machine");
  Serial.begin(115200);
  delay(1000);
  lcd.clear();

  WiFi.begin(ssid, password);
  lcd.print("Conn..g to WiFi");
  lcd.setCursor(0, 1);
  while (WiFi.status() != WL_CONNECTED) {

    lcd.print(".");
    delay(600);
  }
  Serial.println("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.print("Wifi Connected");
  Serial.println("Wifi Setup completed");
  run_setup = 1;
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(client, serverName);

  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    delay(1000);
  }
  // Free resources
  http.end();

  return payload;
}








int pass = 0, flag = 0;
String star="";
void func1() {

  lcd.clear();
  char key = keypad.getKey();

  if (key) {
       Serial.print(key);
    if (key == 'B') {
      pass = 0;
      star="";
      lcd.print("Cleared");
      delay(500);
      lcd.clear();
      return;
    }
    else if(key=='A'){
      
      return;
      }
 
    lcd.clear();
    pass = 10 * pass + ((int)key - 48);
    star+="*";
    lcd.print(star);
    delay(500);
    if (pass == 1745) {
      flag = 1;
      lcd.clear();
      lcd.print("User Id Matched");
      return;
    }
    else if (pass > 999) {
      pass = 0;
      star="";
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Retry Again");
      delay(500);
    }


  }
}















void loop() {
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.print("Welcome");
  lcd.setCursor(0, 1);
  lcd.print("Major Project");
  delay(3000);
  lcd.clear();
  if (pass != 1745) {
    lcd.print("Enter Userid");
    delay(1000);
    lcd.clear();
  }
  while (flag == 0)
    func1();
  Serial.print("Correct");
  delay(1000);
  if (pass == 1745) {
    //person 1;
  }
  else if (pass == 1690) {
    //person 2;
  }
  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/json");
      //get id from pass;
      int httpResponseCode = 0;
//      httpResponseCode = http.PUT("{\"cam_access\":\"1\"}");
      httpResponseCode = http.PUT("{\"cam_access\":\"1\", \"identified_face\":\"noface\",\"location\":\"0\"}");

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      delay(5000);
      lcd.clear();
      Serial.print("camera request done");
      lcd.print("Opening Camera");


      int timer = 25;
      while (timer--) {
        lcd.print(timer);
        delay(1000);
        lcd.clear();
      }


      sensorReadings = httpGETRequest(serverName);

      Serial.println(sensorReadings);
      JSONVar myObject = JSON.parse(sensorReadings);

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        lcd.print("Error at server");
        Serial.println("Parsing input failed!");
        return;
      }

      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.println(myObject["identified_face"]);
      JSONVar face = myObject["identified_face"],app_location=myObject["location"];
      String jsonString = String(face),jsonlocation=String(app_location);
      Serial.print(jsonString);
      Serial.print(jsonlocation);
      Serial.print(app_location);
      if (jsonString == "face0" and jsonlocation=="1") {
        lcd.clear();
        lcd.print("Face Matched");
        lcd.setCursor(0, 1);
        lcd.print("Location Matched");
        delay(3000);
      }
      else if(jsonString == "face0"){
          lcd.clear();
        lcd.print("Location NOT");
        lcd.setCursor(0, 1);
        lcd.print("Matched");
        delay(3000);
        
        }
      else if (jsonString == "noface") {
        lcd.clear();
        lcd.print("NO Face Detected");
        delay(1000);
      }
      else {
        lcd.clear();
        Serial.print("New Face");
        lcd.print("Face Not Matched");
        delay(3000);
      }
      // Serial.println(myObject["f_name"]);
      //Serial.println(myObject[0]["balance"]);

    }
    else {
      lcd.clear();
      lcd.println("WiFi Disconnected");
      delay(1000);
    }
    lastTime = millis();
  }
  pass = 0; flag = 0;
  delay(5000);
//  lcd.clear();
//  lcd.print("Namaste");
//  lcd.setCursor(0, 1);
//  lcd.print("Welcome");
//
//  delay(3000);
}
