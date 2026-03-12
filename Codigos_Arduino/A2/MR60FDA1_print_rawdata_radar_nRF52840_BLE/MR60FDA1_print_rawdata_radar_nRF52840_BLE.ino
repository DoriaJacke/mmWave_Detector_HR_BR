#include "Arduino.h"
#include <60ghzfalldetection.h>
#include <ArduinoBLE.h>

BLEService FallService("19B10000-E8F2-537E-4F6C-D104768A1214");

BLEStringCharacteristic radarCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 16);

bool envio = false;

// can also try hardware serial with
FallDetection_60GHz radar = FallDetection_60GHz(&Serial1);

byte datosSensor[10];
String tempString;
String stringFall;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);

  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1)
      ;
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Fallsensor");
  BLE.setAdvertisedService(FallService);

  // add the characteristic to the service
  FallService.addCharacteristic(radarCharacteristic);

  // add service
  BLE.addService(FallService);

  // set the initial value for the characeristic:
  //radarCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Ready");
}

void loop() {
  BLEDevice central = BLE.central();
  radar.Fall_Detection();
  // put your main code here, to run repeatedly:
  radar.recvRadarBytes();  //Receive radar data and start processing
  radar.showData();        //Serial port prints a set of received data frames
  delay(200);
  /*int cantidad = Serial1.available();
  Serial.print("la cantidad de datos serial es: ");
  Serial.println(cantidad);*/
  if (Serial1.available() > 0) {
    for (int i = 0; i < 10; i++) {
      datosSensor[i] = Serial1.read();
      if (i > 1 && i < 7) {
        if (datosSensor[i] < 0x10) {
          tempString = "0";
        } else {
          tempString = "";
        }
        stringFall += tempString + String(datosSensor[i], HEX) + " "; 
      }
      Serial.print(datosSensor[i] < 0x10 ? " 0" : " ");
      Serial.print(datosSensor[i], HEX);
    }
    Serial.println("");
    Serial.println(stringFall);
    radarCharacteristic.writeValue(stringFall);
    /*if (datosSensor[0] == 0x00 && datosSensor[1] == 0x00) {
      Serial.println(datosSensor[6], HEX);
    } else {
      for (int i = 0; i < 10; i++) {
        Serial.print(datosSensor[i] < 0x10 ? " 0" : " ");
        Serial.print(datosSensor[i], HEX);
        //Serial.print("; ");
      }
      Serial.println("");
    }*/
  }

  /*if (Serial1.available() > 0) {        // Si hay datos disponibles
    byte valor_bruto = Serial1.read();  // Lee un byte
    Serial.print("valor_bruto: ");
    Serial.println(valor_bruto, HEX);
  }*/
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    radarCharacteristic.writeValue(stringFall);
  }
  //delay(200);
  stringFall = "";
}