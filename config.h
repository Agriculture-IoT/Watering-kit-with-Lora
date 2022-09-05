/*
 * Create DevEui from ThingStack GUI or use the DevEui from output of AT-command AT+ID=DevEui
 * Create AppEui from ThingStack GUI or use the AppEui from output of AT-command AT+ID=AppEui
 * Create AppKey from ThingStack GUI
 * Set Channel 902-928 MHz (FSB 2) with ThingStack and use AT+CH=NUM,8-15 
 */
#define loraregion "US915"
#define lorachannel "8-15"
#define deveui "2CF7F12032307C03"
#define appeui "8000000000000006"
#define appkey "01EAC9876043F188C5D6E098D6D9C222"
#define datarate 120     // Time in seconds
