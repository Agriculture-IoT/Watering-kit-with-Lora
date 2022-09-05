#include <Wire.h>
#include "Wire.h"
#include "RTClib.h"
RTC_DS1307 RTC;
//#include<SoftwareSerial.h>
#include <ArduinoJson.h>

#include <Arduino.h>
#include "disk91_LoRaE5.h"

Disk91_LoRaE5 lorae5(false);


#define Frequency DSKLORAE5_ZONE_US915


char deveui[] = "2CF7F12032307C03";
char appeui[] = "0000000000000000";
char appkey[] = "01EAC9876043F188C5D6E098D6D9C222";



void setup() {

  Serial.begin(9600);
  uint32_t start = millis();
  while ( !Serial && (millis() - start) < 1500 );  // Open the Serial Monitor to get started or wait for 1.5"

  // init the library, search the LORAE5 over the different WIO port available
  Serial1.begin(9600);
  
  while(!Serial1) {
    Serial.println("LoRa E5 Init Failed");
    while(1); 
  }

  // Setup the LoRaWan Credentials
  if ( ! lorae5.setup(
        Frequency,
        deveui,
        appeui,
        appkey
     ) ){
    Serial.println("LoRa E5 Setup Failed");
    while(1);         
  }
}

void loop() {

}
