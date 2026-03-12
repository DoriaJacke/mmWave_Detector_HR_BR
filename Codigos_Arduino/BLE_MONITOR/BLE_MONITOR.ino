#include "Arduino.h"
#include <60ghzbreathheart.h>
#include <SoftwareSerial.h>
#include <ArduinoBLE.h>

//BLE
BLEService VSMonitor("19B10005-E8F2-537E-4F6C-D104768A1214");

BLEStringCharacteristic FCardiaca("19B10006-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 16);
BLEStringCharacteristic FRespiratoria("19B10007-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify,16);
bool envio = false;


// Choose any two pins that can be used with SoftwareSerial to RX & TX
#define RX_Pin 16
#define TX_Pin 17
SoftwareSerial mySerial = SoftwareSerial(RX_Pin, TX_Pin);
// we'll be using software serial
BreathHeart_60GHz radar = BreathHeart_60GHz(&mySerial);
int resp=0;
int card =0;
String FCardiac;
String FBreath;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  mySerial.begin(115200);


//BLE

  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1)
      ;
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("VSMonitor");
  BLE.setAdvertisedService(VSMonitor);

  // add the characteristic to the service
  VSMonitor.addCharacteristic(FCardiaca);
  VSMonitor.addCharacteristic(FRespiratoria);

  // add service
  BLE.addService(VSMonitor);

  // set the initial value for the characeristic:
  //radarCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();


  while(!Serial);   //When the serial port is opened, the program starts to execute.
  Serial.println("Ready");
}

void loop(){

  BLEDevice central = BLE.central();
  radar.Breath_Heart(); 
  Serial.println(radar.sensor_report);
  if(radar.sensor_report != 0x00){
    switch(radar.sensor_report){
      
      case HEARTRATEVAL:
        //Serial.print("Sensor monitored the current heart rate value is: ");
        //Serial.println(radar.heart_rate, DEC);
        //Serial.println("----------------------------");
        card =radar.heart_rate, DEC;
        FCardiac = String(card);
        FCardiaca.writeValue(FCardiac);
        break;
      case BREATHVAL:
        //Serial.print("Sensor monitored the current breath rate value is: ");
        //Serial.println(radar.breath_rate, DEC);
        //Serial.println("----------------------------");
        resp =radar.breath_rate, DEC;
        FBreath = String(resp);
        FRespiratoria.writeValue(FBreath);
        break;
    }
  }
  delay(100);                       //Add time delay to avoid program jam
}