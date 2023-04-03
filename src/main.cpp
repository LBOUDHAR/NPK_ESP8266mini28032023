/**************************************************
  COPYRIGHT (c) Lies BOUDHAR 1982-2022
                           
  Project name: IoT in Agriculture
  Date: 28/03/2023
  Version: 1.0
  Circuit: 
  CODE NAME: LiesB_IoT032023_28A
  Coder: Lies BOUDHAR
  programmer: Lies BOUDHAR
  maintainer: Lies BOUDHAR
  Revision: 01/04/2023 at 16:53
  Microcontroller: ESP8266Mod mini
  instagram: soko.009
  youtube: lies boudhar
**************************************************/

/* 
   system composed with: SH1106_128X64, BME280, DS18B20, NPK sensor
   Connect Vin to 3-5VDC
   Connect GND to ground
   Connect SCL to I2C clock pin (GPIO5(D1) on ESP8266 mini)
   Connect SDA to I2C data pin  (GPIO4(D2) on ESP8266 mini) 
   
   Connect DI to (GPIO5(D1) on ESP8266 mini)
   Connect RO to (GPIO5(D1) on ESP8266 mini)
   Connect DE and RE to (GPIO0(D3) on ESP8266 mini)
*/

/*
 * Project Description:
 * this project use ESP8266 mini, SH1106_128X64
 * sensors(DS18B20, BMP280, capacitve soil moisture, NPK 
 * 
 * data are sent to the Cloud (ThingSpeak), to be stored and retrieved remotely 
 * by any suitable application. then the data can be processed to predict things, 
 * or for direct control of the system. 
 * 
 */


#include <Arduino.h>
#include <SoftwareSerial.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Ticker.h>
#include "declaration.h"


#define ONE_WIRE_BUS 13                    // GPIO16 (D0) on ESP8266 mini

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// U8G2_R0 ou U8G2_R2: mode paysage, U8G2_R1 ou U8G2_R3: mode portrait

OneWire oneWire(ONE_WIRE_BUS);            
DallasTemperature sensors(&oneWire);      
DeviceAddress DS18B20[4];                 // Create an array for DS18B20 sensor addresses

int NumOfDevices = 0;                     // Number of dallas temperature devices found
float T_18B20[4];                         // To store value of temperature

Ticker mesureTicker;
Ticker dispayTicker;
bool m = 0;

#define SensorPin A0
int soilMoistureValue = 0;
int soilmoisture;

void setup() {
    Serial.begin(9600);
    u8g2.begin();

    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_7x13B_tf);               
    u8g2.setCursor(2, 10); 
    u8g2.print("IoT in Agriculture"); 
    u8g2.setCursor(2, 22);
    u8g2.print("Lies BOUDHAR");
    u8g2.setCursor(2, 34);
    u8g2.print("ALGIERS 03/2023");
    u8g2.sendBuffer();
    delay(2000); 

    Wire.begin();
    Init_DS18B20(9);      // Initialize DS18B20 temperature sensors with precision set to 9
    u8g2.clearBuffer(); 
    u8g2.setCursor(2, 10);
    u8g2.print(NumOfDevices);
    u8g2.print(" DS18B20 faound");
    u8g2.sendBuffer();
    delay(2000);  

    mesureTicker.attach(4, periodicMesure);
    dispayTicker.attach(1, periodicDisplay);

}

void loop() {
    if(m){
        read_dallas_sensors();
        m = 0;
    }
    soilMoistureValue = analogRead(SensorPin);  
    int soilmoisture = map(soilMoistureValue, 710, 277, 0, 100);
    Serial.printf("Soil Moisture : %u %%\n", soilmoisture);
    delay(500);

   //Serial.printf("CpuFreqMHz: %u MHz\n", ESP.getCpuFreqMHz());
}

void Init_DS18B20(int precision){
  sensors.begin();
  NumOfDevices = sensors.getDeviceCount();
  for(int sensor_num = 0; sensor_num!= NumOfDevices; sensor_num++){
    if(sensors.getAddress(DS18B20[sensor_num], sensor_num)){
      sensors.setResolution(DS18B20[sensor_num], precision);
    }
  }
}

void read_dallas_sensors(){
    for(int sensor_num = 0; sensor_num<NumOfDevices; sensor_num++){
        sensors.requestTemperatures();
        T_18B20[sensor_num] = sensors.getTempC(DS18B20[sensor_num]);
    }
}

void periodicMesure(){
    m = 1;
}

void periodicDisplay(){
    u8g2.clearBuffer();
    u8g2.setCursor(2, 10);
    u8g2.print("- Soil Temperature");
    u8g2.setCursor(2, 22);
    u8g2.print("==================");
    u8g2.setCursor(2, 34);
    u8g2.printf("Temp01 : %2.2f C\n", T_18B20[0]);
    u8g2.setCursor(2, 46);
    u8g2.printf("Temp02 : %2.2f C\n", T_18B20[1]);
    u8g2.sendBuffer();
}