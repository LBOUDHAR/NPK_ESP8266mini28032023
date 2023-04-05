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
  Revision: 05/04/2023 at 02:35
  Microcontroller: ESP8266 D1 mini wemos
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
//#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Ticker.h>
#include "OneButton.h"
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

#define Button01 0                      // GPIO0 attached to button 
uint8_t k = 1;
OneButton button01(Button01, true);     // external wiring sets the button to LOW when pressed

#define DE_RE 15                // GPIO15 
const byte rxPin = 3;           // GPIO3 
const byte txPin = 1;           // GPIO1 
HardwareSerial mod(0);

const byte nitr[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

void setup() {
    //Serial.begin(9600);
    u8g2.begin();

    mod.begin(9600);
    pinMode(DE_RE, OUTPUT);
    digitalWrite(DE_RE, LOW);   // put RS-485 into receive mode

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

    button01.attachClick(oneClick);

}

void loop() {
    button01.tick();

    if(m){
        read_dallas_sensors();
        soilMoistureValue = analogRead(SensorPin);  
        soilmoisture = map(soilMoistureValue, 710, 277, 0, 100);
        Serial.printf("Soil Moisture : %u %%\n", soilmoisture);
        m = 0;
    }


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
    if(k == 1){
        u8g2.clearBuffer();
        u8g2.setCursor(2, 10);
        u8g2.print("- Soil Temperature");
        u8g2.setCursor(2, 22);
        u8g2.print("==================");
        u8g2.setCursor(2, 34);
        u8g2.printf("Temp01 = %2.2f C\n", T_18B20[0]);
        u8g2.setCursor(2, 46);
        u8g2.printf("Temp02 = %2.2f C\n", T_18B20[1]);
        u8g2.sendBuffer();
    }else if(k == 2){
        u8g2.clearBuffer();
        u8g2.setCursor(2, 10);
        u8g2.print("- Soil Moisture"); 
        u8g2.setCursor(2, 34);
        u8g2.printf("Moisture = %u %%\n", soilmoisture);
        u8g2.sendBuffer();
    }else{
        u8g2.clearBuffer();
        u8g2.setCursor(2, 10);
        u8g2.print("- Soil NPK");
        u8g2.setCursor(2, 22);
        u8g2.printf("N = %.2f mg/kg\n", T_18B20[0]);
        u8g2.setCursor(2, 34);
        u8g2.printf("P = %.2f mg/kg\n", T_18B20[0]);
        u8g2.setCursor(2, 46);
        u8g2.printf("K = %.2f mg/kg\n", T_18B20[1]);    
        u8g2.sendBuffer();        
    }

}

void oneClick(){
    k++;
    if(k > 3) k = 1;
}



