#define TOUCH_MODULES_CST_SELF
#include "Arduino.h"
#include "TFT_eSPI.h" /* Please use the TFT library provided in the library. */
#include "TouchLib.h"
#include "Wire.h"
#include "pin_config.h"
#include "fox.h"    //78x81
#include "tagFont.h" //Rock_Salt_Regular_10
#include <SPI.h>
#include <Adafruit_Sensor.h>  //BME280
#include <Adafruit_BME280.h> //BME280
#include <TaskScheduler.h>

Scheduler ts;

TouchLib touch(Wire1, PIN_IIC_SDA, PIN_IIC_SCL, CTS820_SLAVE_ADDRESS, PIN_TOUCH_RES);
Adafruit_BME280 bme; // I2C
#define SEALEVELPRESSURE_HPA (1013.25) //BME280

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

int deb=0;
bool switch1=true;
bool switch2=false;
int x = 0;
int y = 0;
bool touchFlag = true;

void draw(int x, int y);
void checkTouch();
void printI2CAdresses();
void printValues();
void drawScreen();

//Tasks
Task t1(1000, TASK_FOREVER, &drawScreen, &ts, true);
Task t2(1000, TASK_FOREVER, &printValues, &ts, true);
Task t3(100, TASK_FOREVER, &checkTouch, &ts, true);

void setup() {
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);
  pinMode(PIN_TOUCH_RES, OUTPUT);
  digitalWrite(PIN_TOUCH_RES, LOW);
  delay(500);
  digitalWrite(PIN_TOUCH_RES, HIGH);
  tft.begin();
  tft.setRotation(1);
  touch.setRotation(1);
  sprite.createSprite(320,170);
  sprite.setTextColor(TFT_WHITE,TFT_BLACK);
  Wire.begin(43, 44);
  unsigned status;
  status = bme.begin(0x77, &Wire);
  Wire1.begin(PIN_IIC_SDA, PIN_IIC_SCL);
  sprite.fillSprite(TFT_BLACK);
  sprite.setSwapBytes(true);
  Serial.begin(115200);
  //while(!Serial);
  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }
  ts.startNow();
}

void loop() {

  ts.execute();
  
}


void draw(int x, int y)
{  
  sprite.fillSprite(TFT_BLACK);
 
      if(y<160)
      switch1=!switch1;
      else
      switch2=!switch2;

    sprite.drawString("SWITCH1= "+String(switch1),20,30,4);
    sprite.drawRect(18,58,134,54,TFT_GREEN);
    sprite.fillRect(20+(switch1*80),60,50,50,TFT_GREEN);
    
    sprite.drawString("SWITCH2= "+String(switch2),20,190,4);
    sprite.drawRect(18,218,134,54,TFT_YELLOW);
    sprite.fillRect(20+(switch2*80),220,50,50,TFT_YELLOW);

  sprite.pushSprite(0,0); 
}

void checkTouch() {

  if(touchFlag) {
    if (touch.read()) {
      TP_Point t = touch.getPoint(0);
      x = t.x;
      y = t.y;
    }
  }
}

void printI2CAdresses() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);
}

void printValues() {
    touchFlag = false;
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
    touchFlag = true;
}

void drawScreen() {
  sprite.fillSprite(TFT_BLACK);

  sprite.drawString("PM1", 35, 10, 2);
  sprite.drawString("PM2.5", 100, 10, 2);
  sprite.drawString("PM10", 180, 10, 2);
  sprite.drawString(String(random(30)), 10, 30, 7);
  sprite.drawString(String(random(20)), 80, 30, 7);
  sprite.drawString(String(random(15)), 160, 30, 7);
  sprite.pushImage(242,65,78,81,fox);
  sprite.setFreeFont(&Rock_Salt_Regular_10);
  sprite.drawString("MAR", 270, 146);
  sprite.drawString("Touch x: " + String(x), 20, 100, 2);
  sprite.drawString("Touch y: " + String(y), 20, 120, 2);
  sprite.pushSprite(0,0);
}