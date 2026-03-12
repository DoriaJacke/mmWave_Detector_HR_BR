#include "Arduino.h"
#include <60ghzbreathheart.h>
#include <SoftwareSerial.h>
#include "BluetoothSerial.h"

// Choose any two pins that can be used with SoftwareSerial to RX & TX
#define RX_Pin 16
#define TX_Pin 17

String device_name = "ESP32-BT-Monitor";

SoftwareSerial mySerial = SoftwareSerial(RX_Pin, TX_Pin);

// we'll be using software serial
BreathHeart_60GHz radar = BreathHeart_60GHz(&mySerial);

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mySerial.begin(115200);

  //Inicializacion dispositivo BT
  SerialBT.begin(device_name);  //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());


  while(!Serial);   //When the serial port is opened, the program starts to execute.
  Serial.println("Ready");
}

void loop()
{
  // put your main code here, to run repeatedly:
  radar.Breath_Heart();           //Breath and heartbeat information output
  if(radar.sensor_report != 0x00){
    switch(radar.sensor_report){
      case HEARTRATEVAL:
        Serial.print("Sensor monitored the current heart rate value is: ");
        Serial.println(radar.heart_rate, DEC);
        Serial.println("----------------------------");
        SerialBT.print("Sensor monitored the current heart rate value is: ");
        SerialBT.println(radar.heart_rate);
        SerialBT.println("----------------------------");
        break;
    }
  }
  delay(200);                       //Add time delay to avoid program jam
}