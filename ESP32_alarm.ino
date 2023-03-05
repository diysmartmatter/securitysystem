//ESP32 security system (AA Alarm) with EspMQTTClient library.
#include "EspMQTTClient.h"
EspMQTTClient *client;
//input & output pins and values
#define ALARMOUT 13 //GPIO for a relay to activate alarm buzzer.
#define ARMEDLED 12  //GPIO for armed-indicator LED.
//security level. Only AA (Away Arm) and D (Disarm) are used.
#define level_SA 0  //Stay Arm, not used in this version
#define level_AA 1  //Away Arm
#define level_NA 2  //Night Arm, not used in this version
#define level_D 3   //Disarm
int currentLevel=level_D; //current security level 0=SA, 1=AA, 2=NA, 3=D
//WiFi
const char SSID[] = "XXXXXXXX"; //WiFi SSID
const char PASS[] = "xxxxxxxx"; //WiFi password
//MQTT
char CLIENTID[] = "ESP32_xx:xx:xx:xx:xx:xx"; //MAC address is set in setup()
//for example, this will be set to "ESP32_84:CC:A8:7A:5F:44"
const char  MQTTADD[] = "192.168.xxx.xxx"; //Broker IP address
const short MQTTPORT = 1883; //Broker port
const char  MQTTUSER[] = "";//Can be omitted if not needed
const char  MQTTPASS[] = "";//Can be omitted if not needed
const char  SUBTOPIC[] = "mqttthing/security/setTargetState"; //mqtt topic to subscribe
const char  PUBTARGET[] = "mqttthing/security/getTargetState"; //mqtt topic to publish
const char  PUBCURRENT[] = "mqttthing/security/getCurrentState"; //mqtt topic to publish
const char  PUBDEBUG[] = "mqttthing/security/debug"; //for debug message

void setup() {
  //Digital I/O
  pinMode(ALARMOUT, OUTPUT);
  pinMode(ARMEDLED, OUTPUT);
  digitalWrite(ALARMOUT, LOW); //set alarm off
  digitalWrite(ARMEDLED, LOW); //set LED off
  currentLevel=level_D; //set disarm
  //Serial
  Serial.begin(115200);
  while (!Serial);
  Serial.println("ESP32 Security System started.");
  //MQTT
  String wifiMACString = WiFi.macAddress(); //WiFi MAC address
  wifiMACString.toCharArray(&CLIENTID[6], 18, 0); //"ESP32_xx:xx:xx:xx:xx:xx"
  Serial.print("SSID: ");Serial.println(SSID);
  Serial.print("MQTT broker address: ");Serial.println(MQTTADD);
  Serial.print("MQTT clientID: ");Serial.println(CLIENTID);
  client = new EspMQTTClient(SSID,PASS,MQTTADD,MQTTUSER,MQTTPASS,CLIENTID,MQTTPORT);
}

void onConnectionEstablished() {
  Serial.println("WiFi/MQTT onnection established.");
  client->subscribe(SUBTOPIC, onMessageReceived); //set callback function
  client->publish(PUBDEBUG, "ESP32 Security System is ready.");
}

void onMessageReceived(const String& msg) { // topic = mqttthing/security/setTargetState
//Serial.println(msg);
  client->publish(PUBDEBUG, "Set TS received.");
  if(msg.compareTo("AA")==0) { //is set AA (Away Arm)
    currentLevel = level_AA;
    //inicate with a 0.2 second beep
    digitalWrite(ALARMOUT, HIGH); delay(200); digitalWrite(ALARMOUT, LOW);
    digitalWrite(ARMEDLED, HIGH); //turn LED on
    client->publish(PUBCURRENT,"false"); //turn off the button JIC it is on
    client->publish(PUBCURRENT,"AA");
  }
  else if(msg.compareTo("D")==0){ //target state is D (Disarm)
    currentLevel = level_D;
    digitalWrite(ARMEDLED, LOW); //turn the LED off
    digitalWrite(ALARMOUT, LOW); //turn the alarm off JIC it is on
    client->publish(PUBCURRENT,"false"); //turn the button off JIC it is on
    client->publish(PUBCURRENT,"D");
  }
  else if(msg.compareTo("false")==0) { //a sensor turned off the switch
    digitalWrite(ALARMOUT, LOW); //turn the alarm off, anyway
    if(currentLevel == level_AA){ // if AA, alarm likely to have been triggered
      client->publish(PUBCURRENT,"AA"); //return to AA from T
    }
  }
  else if(msg.compareTo("true")==0) { //sensor turned on the switch
    if(currentLevel == level_AA){ //if it is armed
      client->publish(PUBCURRENT,"T"); //trigger the security system
      digitalWrite(ALARMOUT, HIGH); //turn on the alarm buzzer
    }
    else{ //if currentLevel is level_D
      delay(500); //do nothing but just delay for 500 ms for sensor test
      client->publish(PUBCURRENT,"false"); //turn off the button on the Home.app
    }
  }
}

void loop() {
  client->loop();
}
