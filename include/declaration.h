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
// header file

#ifndef declaration_h_
#define declaration_h_

void Init_DS18B20(int precision);
void read_dallas_sensors();
void periodicMesure();
void periodicDisplay();
void oneClick();


#endif