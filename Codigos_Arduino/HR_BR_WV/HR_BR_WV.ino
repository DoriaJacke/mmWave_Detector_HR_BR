//Librerias
#include "Arduino.h"
#include <60ghzbreathheart.h>
#include <SoftwareSerial.h>
#include <ArduinoBLE.h>

//Se determina el UIDD del servidio de BLE
BLEService VSMonitor("19B10005-E8F2-537E-4F6C-D104768A1214");

//Se determina el UIDD de caracteristicas de Frecuencia respiratoria, Cardiaca y Estado(determina en que estado esta la persona o si no esta)
BLEStringCharacteristic FCardiaca("19B10006-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 16);
BLEStringCharacteristic FRespiratoria("19B10007-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 16);

//Hearth wave
BLEStringCharacteristic HWP1("19B10008-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 16);
BLEStringCharacteristic HWP2("19B10009-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 16);
BLEStringCharacteristic HWP3("19B10010-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 16);
BLEStringCharacteristic HWP4("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 16);
BLEStringCharacteristic HWP5("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 16);

bool envio = false;


// Se eligen los pines del sofwareserial donde se conecto el sensor mWave
#define RX_Pin 16
#define TX_Pin 17
SoftwareSerial mySerial = SoftwareSerial(RX_Pin, TX_Pin);

// Se incia el sensor mWave
BreathHeart_60GHz radar = BreathHeart_60GHz(&mySerial);

//Se inicializan las variables
int resp = 0;
int card = 0;
int Dist = 0;
String FCardiac;
String FBreath;
String HP1;
String HP2;
String HP3;
String HP4;
String HP5;
float timer = 0;

//Setup void
void setup() {

  //Inicializacion de puertos
  Serial.begin(115200);
  mySerial.begin(115200);


  //Incio BLE
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1)
      ;
  }

  //Se determina el nombre del equipo a la hora de mostrarse en BLE
  BLE.setLocalName("VSMonitor");
  BLE.setAdvertisedService(VSMonitor);

  // agrega las carateristicas a la transmision de BLE
  VSMonitor.addCharacteristic(FCardiaca);
  VSMonitor.addCharacteristic(FRespiratoria);

  VSMonitor.addCharacteristic(HWP1);
  VSMonitor.addCharacteristic(HWP2);
  VSMonitor.addCharacteristic(HWP3);
  VSMonitor.addCharacteristic(HWP4);
  VSMonitor.addCharacteristic(HWP5);
  
  // Agrega el servicio
  BLE.addService(VSMonitor);

  // Se muestra el equipo a traves del BLE y se inicia el emparejamiento
  BLE.advertise();

  //Inicio del programa loop
  while (!Serial)
    ;
  Serial.println("Ready");
}


//Main
void loop() {

  BLEDevice central = BLE.central();
  
  radar.Breath_Heart();
    switch (radar.sensor_report) {
      case HEARTRATEVAL:
        card = radar.heart_rate, DEC;
        FCardiac = String(card);
        FCardiaca.writeValue(FCardiac);
        break;
      case HEARTRATEWAVE:  //Valid only when real-time data transfer mode is on
        HP1 = String(radar.heart_point_1,DEC);
        HWP1.writeValue(HP1);
        HP2 = String(radar.heart_point_2,DEC);
        HWP2.writeValue(HP2);
        HP3 = String(radar.heart_point_3,DEC);
        HWP3.writeValue(HP3);
        HP4 = String(radar.heart_point_4,DEC);
        HWP4.writeValue(HP4);
        HP5 = String(radar.heart_point_5,DEC);
        HWP5.writeValue(HP5);
        break;
      case BREATHVAL:
        resp = radar.breath_rate, DEC;
        FBreath = String(resp);
        FRespiratoria.writeValue(FBreath);
        break;
  }
  //Contador el cual se usa para volver a detectar presncia en caso de que esta haya cambiado
  timer += 1;
  Serial.print(timer);
  Serial.println(" ciclos");
  delay(200);  // delay
};