# securitysystem
ESP32 program to make a HomeKit Security Accessory accessory over MQTT and Homebridge 

![Diragram](https://diysmartmatter.com/images/20220916230522.png)

![Actual circuit](https://diysmartmatter.com/wp-content/uploads/2023/03/securitysystem.jpg)


Homebridge example.

<pre>
{
    "type": "securitySystem",
    "name": "Security System",
    "url": "mqtt://localhost:1883",
    "logMqtt": true,
    "topics": {
        "getOnline": "mqttthing/security/getOnline",
        "setTargetState": "mqttthing/security/setTargetState",
        "getTargetState": "mqttthing/security/getTargetState",
        "getCurrentState": "mqttthing/security/getCurrentState",
        "getAltSensorState": "mqttthing/security/getAltSensorState"
    },
    "restrictTargetState": [
        1,
        3
    ],
    "accessory": "mqttthing"
},
{
    "type": "switch",
    "name": "Alarm Buzzer",
    "url": "mqtt://localhost:1883",
    "topics": {
        "getOn": "mqttthing/security/getCurrentState",
        "setOn": "mqttthing/security/setTargetState"
    },
//    "accessory": "mqttthing"
//},
</pre>
