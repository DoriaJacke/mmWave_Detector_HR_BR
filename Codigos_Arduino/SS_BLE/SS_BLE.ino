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
BLEStringCharacteristic State("19B10008-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 16);
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
String FCardiac;
String FBreath;
bool move = false;
float timer = 0;

//Pin y valor sensor luminico
const int LuxPin = 4;
int LuxValue = 0;

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
  VSMonitor.addCharacteristic(State);
  
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

  //Lectura Sensor Luminico
  LuxValue = analogRead(LuxPin);
  Serial.print("Luminosidad = ");
  Serial.println(LuxValue);
  //
  BLEDevice central = BLE.central();

  // Filtro para determinar el estado y la presencia de individuos
  if (timer == 500 || !move) timer = 0;  //2 minutos  (60x0.2 manera de calcular )
  radar.HumanExis_Func();

  if (timer == 0 || !move) {
    if (radar.sensor_report != 0x00) {
      //Caso 1: deteccion de individuo en habitacion
      if (1 <= (radar.bodysign_val, DEC) && !move && LuxValue < 2000) {  //condicion luz apagada y estancia
        Serial.println("Deteccion presencia en escuridad");
        move = true;
        Serial.println("----------------------------");
      }
      //Caso 2: Deteccion de persona despierta
      if (1 <= (radar.bodysign_val, DEC) && !move && LuxValue > 2000) {  //condicion luz apagada y estancia
        Serial.println("Deteccion de presencia, despierto");
        move = true;
        Serial.println("----------------------------");
      }
    }
    //Caso 3: No hay nadie en la habitacion
    if ((radar.sensor_report == 0x00) || (1 > (radar.bodysign_val, DEC) && move)) {  //y condicion luz encendida
      Serial.println("No hay nadie.");
      move = false;
      Serial.println("----------------------------");
      FCardiaca.writeValue("0");
      FRespiratoria.writeValue("0");
    }

    Serial.println("*");
    Serial.println(move);
  }


  //Medicion de signos vitales, esto solo se medira cuando se detecte presencia de alguien en la habitacion
  radar.Breath_Heart();
  if (radar.sensor_report != 0x00 && move) {
    switch (radar.sensor_report) {
      case HEARTRATEVAL:
        card = radar.heart_rate, DEC;
        FCardiac = String(card);
        FCardiaca.writeValue(FCardiac);
        break;
      case BREATHVAL:
        resp = radar.breath_rate, DEC;
        FBreath = String(resp);
        FRespiratoria.writeValue(FBreath);
        break;
    }
  }
  //Contador el cual se usa para volver a detectar presncia en caso de que esta haya cambiado
  timer += 1;
  Serial.print(timer);
  Serial.println(" ciclos");
  //Envio del estado en el cual se encuentra
  if (move && LuxValue<2000) State.writeValue("Durmiendo");
  if (move && LuxValue>2000) State.writeValue("En habitacion");
  if (!move) State.writeValue("Vacio");