# BLE.py
import asyncio
from bleak import BleakScanner, BleakClient
import threading

class BLEReceiver:
    def __init__(self, data_queue):
        self.UUID_TO_KEY = {
            "19B10006-E8F2-537E-4F6C-D104768A1214": "FCard",
            "19B10007-E8F2-537E-4F6C-D104768A1214": "FResp"
        }

        self.data_queue = data_queue
        self.client = None
        self.running = False

        # Inicializar el diccionario para almacenar los datos recibidos
        self.received_data = {}
        # Inicializar un lock para manejar el acceso concurrente
        self.data_lock = threading.Lock()

    def notification_handler(self, uuid_name):
        def handler(sender, data):
            try:
                # Procesar los datos para esta clave
                received_fragment = data.decode('utf-8').strip()
                with self.data_lock:
                    self.received_data[uuid_name] = received_fragment
                    # Verificar si hemos recibido datos de todas las características
                    if all(key in self.received_data for key in self.UUID_TO_KEY.values()):
                        # Ensamblar los datos en orden
                        data_order = ['FCard', 'FResp']
                        complete_string = ','.join(self.received_data[key] for key in data_order)
                        # Reiniciar los datos recibidos
                        self.received_data.clear()
                        # Procesar el string completo
                        data_list = complete_string.split(',')
                        print(data_list, len(data_list))

                        if len(data_list) == 15:  
                            variable_names = ['Tiempo', 'BPM','IR']
                            data_dict = dict(zip(variable_names, data_list))
                            # Colocar el diccionario en la cola
                            self.data_queue.put(data_dict)
                        else:
                            print(f"Datos incompletos o incorrectos recibidos: {complete_string}")
            except Exception as e:
                print(f"Error al procesar la notificación: {e}")
        return handler

    async def run_ble(self):
        reintento = 0
        esp32_device = None
        while esp32_device is None:
            try:
                # Escanear dispositivo BLE 
                print(f"Intentando conectar... (Reintento #{reintento})")
                devices = await BleakScanner.discover()

                # Buscar el dispositivo
                for device in devices:
                    if device.name == "VSMonitor":
                        esp32_device = device
                        break

                if esp32_device is None:
                    print("Dispositivo BLE no encontrado. Reintentando en 5 segundos...")
                    await asyncio.sleep(5)
                    reintento += 1
                else:
                    print(f"Conectando a {esp32_device.name} con dirección {esp32_device.address}")
            except Exception as e:
                print(f"Error durante la búsqueda BLE: {e}")
                await asyncio.sleep(5)

        # Conectar al dispositivo
        async with BleakClient(esp32_device.address) as client:
            self.client = client
            print("Conectado al ESP32")

            # Suscribirse a las notificaciones de cada característica
            for uuid, key in self.UUID_TO_KEY.items():
                try:
                    handler = self.notification_handler(key)
                    await client.start_notify(uuid, handler)
                    print(f"Suscrito a la característica {uuid} con clave {key}")
                except Exception as e:
                    print(f"Error suscribiendo a {uuid}: {e}")

            # Mantener la conexión abierta para recibir datos
            print("Esperando datos... (Ctrl+C para salir)")
            while self.running:
                await asyncio.sleep(0.1)

    def start(self):
        self.running = True
        # Ejecutar run_ble en un hilo separado de CPU
        threading.Thread(target=self.run, daemon=True).start()

    def run(self):
        asyncio.run(self.run_ble())

    async def cleanup(self):
        if self.client and self.client.is_connected:
            for uuid in self.UUID_TO_KEY.keys():
                try:
                    await self.client.stop_notify(uuid)
                except Exception as e:
                    print(f"Error al detener notificaciones de {uuid}: {e}")
            await self.client.disconnect()

    def stop(self):
        print("Deteniendo BLE Receiver...")
        self.running = False
        asyncio.run(self.cleanup())
