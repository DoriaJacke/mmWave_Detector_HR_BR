import asyncio
from bleak import BleakClient

async def main():
    ble_address = "08:D1:F9:26:11:D6"

    async with BleakClient(ble_address) as client:
        # we’ll do the read/write operations here
        print("Connected to BLE device")
        print(client.is_connected)        

asyncio.run(main())


#BLE Addres: 08:D1:F9:26:11:D6:    08:D1:F9:26:11:D6: