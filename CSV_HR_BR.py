
#Librerias
import asyncio
from bleak import BleakClient
import csv
from itertools import count
import time
from datetime import date


index = count()
tiempo = 1

#Se crea un Archivo CSV con los encabezados
Nombres=["Index","Hora","Frecuencia Cardiaca","Frecuencia Respiratoria"]
Name='datos_'+str(date.today())+"_"+str(time.strftime("%H-%M", time.localtime()))+'.csv'

with open(Name,'w',newline='') as file:
    csv_write = csv.DictWriter(file,fieldnames=Nombres)
    csv_write.writeheader()


#Se defina la funcion asincronica para la extraccion de informacion proveniente desde el BLE de la placa
async def main():

    #Se determina la MAC Addres del dispositivo y los UUID de cada dato que se desea extraer 
    ble_address = "08:D1:F9:26:11:D6"
    characteristic_uuid1 = "19B10006-E8F2-537E-4F6C-D104768A1214"
    characteristic_uuid2 = "19B10007-E8F2-537E-4F6C-D104768A1214"

   

    #Se extraen los datos y se retornan  al ciclo principal.
    async with BleakClient(ble_address) as client:
        data1 = await client.read_gatt_char(characteristic_uuid1)
        card =int(data1.decode('utf-8').strip())
        data2 = await client.read_gatt_char(characteristic_uuid2)
        resp =int(data2.decode('utf-8').strip())

        return [card,resp]

while True:
    #try permite que el programa siga corriendo aunque detecte un error, este no evita errores fatales
    try:
        #Se habre el Archivo CSV creado previamente, y se le agrega una linea con los datos obtenidos desde el sensor
        with open(Name,'a') as csv_file:
            csv_write = csv.DictWriter(csv_file,fieldnames=Nombres)
            data = asyncio.run(main())
            info = {
                "Index": tiempo,
                "Hora": time.strftime("%H:%M:%S", time.localtime()),
                "Frecuencia Cardiaca":data[0],
                "Frecuencia Respiratoria":data[1],
            }
            csv_write.writerow(info)
            print(time.strftime("%H:%M:%S", time.localtime()))
            tiempo +=1
            

    #En caso de error se vuelve a intentar luego de mostrar el error y la hora en la cual se produjo
    except Exception as e:
        print("Function errored out!", e)
        print(time.strftime("%H:%M:%S", time.localtime()))
        print("Retrying ... ")
    

