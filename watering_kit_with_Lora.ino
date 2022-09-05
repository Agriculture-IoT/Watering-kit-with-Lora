#include <Wire.h>
#include "Wire.h"
#include "RTClib.h"
RTC_DS1307 RTC;
#include<Arduino.h>
#include "config.h"

/* 
 * LoRaWAN functions
 * Adopted using example code from https://wiki.seeedstudio.com/Grove_LoRa_E5_New_Version/
 * Wio-E5 is connected to Serial1 port of the Elecrow board.
 * Port connections shown in the wiring-diagram.png 
 * 
 * I2C display not used in this project.
 */

static char recv_buf[512];
static bool is_exist = false;
static bool is_join = false;
static int led = 0;
int counter = 0;

 
static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...)
{
    int ch;
    int num = 0;
    int index = 0;
    int startMillis = 0;
    va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    va_start(args, p_cmd);
    Serial1.print(p_cmd);
    Serial.print(p_cmd);
    va_end(args);
    delay(200);
    startMillis = millis();
 
    if (p_ack == NULL)
    {
        return 0;
    }
    do
    {
        while (Serial1.available() > 0)
        {
            ch = Serial1.read();
            recv_buf[index++] = ch;
            Serial.print((char)ch);
            delay(2);
        }
 
        if (strstr(recv_buf, p_ack) != NULL)
        {
            return 1;
        }
 
    } while (millis() - startMillis < timeout_ms);
    return 0;
}
 
static void recv_prase(char *p_msg)
{
    if (p_msg == NULL)
    {
        return;
    }
    char *p_start = NULL;
    int data = 0;
    int rssi = 0;
    int snr = 0;
 
    p_start = strstr(p_msg, "RX");
    if (p_start && (1 == sscanf(p_start, "RX: \"%d\"\r\n", &data)))
    {
        Serial.println(data);
    }
 
    p_start = strstr(p_msg, "RSSI");
    p_start = strstr(p_msg, "SNR");
}

// LoRa functions End


// set all moisture sensors PIN ID
int moisture1 = A0;
int moisture2 = A1;
int moisture3 = A2;
int moisture4 = A3;

// declare moisture values
int moisture1_value = 0;
int moisture2_value = 0;
int moisture3_value = 0;
int moisture4_value = 0;

int mthreshold = soil_moisture_threshold ;

// set water relays
int relay1 = 6;
int relay2 = 8;
int relay3 = 9;
int relay4 = 10;

// set water pump
int pump = 4;

// set button
int button = 12;

//pump state    1:open   0:close
int pump_state_flag = 0;

//relay1 state    1:open   0:close
int relay1_state_flag = 0;

//relay2 state   1:open   0:close
int relay2_state_flag = 0;

//relay3 state  1:open   0:close
int relay3_state_flag = 0;

//relay4 state   1:open   0:close
int relay4_state_flag = 0;

static unsigned long currentMillis_send = 0;
static unsigned long  Lasttime_send = 0;


void setup()
{
  char _cmd[128];
  delay(2000);
  Wire.begin();
  RTC.begin();
  Serial.begin(9600);
  // declare relay as output
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  // declare pump as output
  pinMode(pump, OUTPUT);
  // declare switch as input
  pinMode(button, INPUT);
  //pinMode(ROTARY_ANGLE_SENSOR, INPUT);
  // water_flower();

  // Lora setup start
    Serial1.begin(9600);
    delay(10000);
    Serial.print("E5 LORAWAN TEST\r\n");
    char deveui_cmd[40];
    char appeui_cmd[40];
    char appkey_cmd[40];
    char loraregion_cmd[40];
    char lorachannel_cmd[40];
    if (at_send_check_response("+AT: OK", 100, "AT\r\n"))
    {
        is_exist = true;
        at_send_check_response("+ID: DevEui", 1000, "AT+ID=DevEui\r\n");
        at_send_check_response("+ID: AppEui", 1000, "AT+ID=AppEui\r\n");

        sprintf(deveui_cmd, "AT+ID=DevEUI, \"%s\"\r\n", deveui);
        at_send_check_response("+ID: DevEui", 1000, deveui_cmd);
        
        sprintf(appeui_cmd, "AT+ID=AppEui, \"%s\"\r\n", appeui);
        at_send_check_response("+ID: AppEui", 1000, appeui_cmd);
        
        sprintf(appkey_cmd, "AT+KEY=APPKEY, \"%s\"\r\n", appkey);
        at_send_check_response("+KEY: APPKEY", 1000, appkey_cmd);
        
        sprintf(loraregion_cmd, "AT+DR=%s\r\n", loraregion);
        at_send_check_response("+DR:", 1000, loraregion_cmd);
        
        sprintf(lorachannel_cmd, "AT+CH=NUM,%s\r\n", lorachannel);
        at_send_check_response("+CH: NUM", 1000, lorachannel_cmd);
        
        //at_send_check_response("+ID: DevEui", 1000, "AT+ID=DevEUI, \"2CF7F12032307C03\"\r\n");
        //at_send_check_response("+ID: AppEui", 1000, "AT+ID=AppEui, \"8000000000000006\"\r\n");
        //at_send_check_response("+KEY: APPKEY", 1000, "AT+KEY=APPKEY, \"01EAC9876043F188C5D6E098D6D9C222\"\r\n");
        //at_send_check_response("+DR: US915", 1000, "AT+DR=US915\r\n");
        //at_send_check_response("+CH: NUM", 1000, "AT+CH=NUM,8-15\r\n");
        at_send_check_response("+MODE: LWOTAA", 1000, "AT+MODE=LWOTAA\r\n");
        delay(200);
        is_join = true;
    }
    else
    {
        is_exist = false;
        Serial.print("No E5 module found.\r\n");
    }
  // Lora setup end
}

void loop()
{

  water_flower();
//  int button_state = digitalRead(button);
//  if (button_state == 1)
//  {
//    read_value();
//  }

  // Lora start send data
  if (is_exist)
  {
      int ret = 0;
      if (is_join)
      {

          ret = at_send_check_response("+JOIN: Network joined", 12000, "AT+JOIN\r\n");
          delay(200);
          if (ret)
          {
              is_join = false;
          }
          else
          {
              at_send_check_response("+ID: AppEui", 1000, "AT+ID\r\n");
              Serial.print("JOIN failed!\r\n\r\n");
              delay(5000);
          }
      }
      else
      {
           // read the value from the moisture sensors:
          read_value();
          
          Serial.println();
          Serial.print("moisture1_value = ");
          Serial.print(moisture1_value);
          Serial.println();
          Serial.print("moisture2_value = ");
          Serial.print(moisture2_value);
          Serial.println();
          Serial.print("moisture3_value = ");
          Serial.print(moisture3_value);
          Serial.println();
          Serial.print("moisture4_value = ");
          Serial.print(moisture4_value);
          Serial.println();
          
          char cmd[128];
          sprintf(cmd, "AT+CMSGHEX=\"%04X%04X%04X%04X\"\r\n", moisture1_value, moisture2_value, moisture3_value, moisture4_value);
          ret = at_send_check_response("Done", 10000, cmd);
          delay(200);
          if (ret)
          {
              Serial.print("Send failed!\r\n\r\n");
          }
          else
          {
              recv_prase(recv_buf);
          }
      }
  }
  // Delay to datarate set
  delay(90000);
  // Lora end send data
}

//Set moisture value
void read_value()
{
/************These is for capacity moisture sensor*********/
 float value1 = analogRead(A0);
  moisture1_value =map(value1,590,360,0,100); delay(20);
  if(moisture1_value<0){
    moisture1_value=0;
  }
  float value2 = analogRead(A1);
  moisture2_value =map(value2,600,360,0,100); delay(20);
  if(moisture2_value<0) {
    moisture2_value=0;
  }
  float value3 = analogRead(A2);
  moisture3_value =map(value3,600,360,0,100); delay(20);
  if(moisture3_value<0){
    moisture3_value=0;
  }
  float value4 = analogRead(A3);
  moisture4_value =map(value4,600,360,0,100); delay(20);
  if(moisture4_value<0) {
    moisture4_value=0;
  }
}

void water_flower()
{
//  if (moisture1_value < 30)
//  Serial.print("moisture1_value = ");
//  Serial.print(moisture1_value);
//  Serial.println();
  if (moisture1_value < mthreshold)
  {
    digitalWrite(relay1, HIGH);
    relay1_state_flag = 1;
    delay(50);
    if (pump_state_flag == 0)
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }
  }
  else if (moisture1_value > 55)
  {
    digitalWrite(relay1, LOW);
    relay1_state_flag = 0;
    delay(50);
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
  }

//  if (moisture2_value < 30)
//  Serial.print("moisture2_value = ");
//  Serial.print(moisture2_value);
//  Serial.println();
  if (moisture2_value < mthreshold)
  {
    digitalWrite(relay2, HIGH);
    relay2_state_flag = 1;
    delay(50);
    if (pump_state_flag == 0)
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }
  }
  else if (moisture2_value > 55)
  {
    digitalWrite(relay2, LOW);
    relay2_state_flag = 0;
    delay(50);
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
  }

//  if (moisture3_value < 30)
//    Serial.print("moisture3_value = ");
//    Serial.print(moisture3_value);
//    Serial.println();
  if (moisture3_value < mthreshold)
  {
    digitalWrite(relay3, HIGH);
    relay3_state_flag = 1;
    delay(50);
    if (pump_state_flag == 0)
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }
  }
  else if (moisture3_value > 55)
  {
    digitalWrite(relay3, LOW);
    relay3_state_flag = 0;
    delay(50);
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
  }

//  if (moisture4_value < 30)
//    Serial.print("moisture4_value = ");
//    Serial.print(moisture4_value);
//    Serial.println();
  if (moisture4_value < mthreshold)
  {
    digitalWrite(relay4, HIGH);
    relay4_state_flag = 1;
    delay(50);

    if (pump_state_flag == 0)
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }
  }
  else if (moisture4_value > 55)
  {
    digitalWrite(relay4, LOW);
    relay4_state_flag = 0;
    delay(50);
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
  }
delay(1000);
}
