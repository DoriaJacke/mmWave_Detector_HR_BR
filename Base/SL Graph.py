# Read the current state of a characteristic on a BLE device

import asyncio
import streamlit as st
from bleak import BleakClient
import plotly.express as px
import time 
import pandas as pd
import plotly.graph_objects as go

st.set_page_config(page_title="Monitor", layout="wide")

async def main():
    ble_address = "08:D1:F9:26:11:D6"
    characteristic_uuid1 = "19B10006-E8F2-537E-4F6C-D104768A1214"
    characteristic_uuid2 = "19B10007-E8F2-537E-4F6C-D104768A1214"

    async with BleakClient(ble_address) as client:
        data1 = await client.read_gatt_char(characteristic_uuid1)
        card =int(data1.decode('utf-8').strip())
        base.append(card)

        data2 = await client.read_gatt_char(characteristic_uuid2)
        resp =int(data2.decode('utf-8').strip())

def graph(x,y):
    dict= {'tiempo':x,'Frecuencia':y}
    df = pd.DataFrame(dict)
    fig = px.line(df,x='tiempo',y='Frecuencia',line_shape="spline", render_mode="svg",title="F Cardiaca")
    st.plotly_chart(fig,)


def updategraph(x,y):
    dict= {'tiempo':x,'Frecuencia':y}
    df = pd.DataFrame(dict)
    fig = px.line(df, x='tiempo', y='Frecuencia')
    fig.update_xaxes(title_font={"size": 10}, tickfont={"size": 12})
    fig.update_yaxes(title_font={"size": 14}, tickfont={"size": 12},range=(0,10))
    st.plotly_chart(fig)



#Main 
cont=0
base =[]
tiempo =[]
estado = True
while(cont <20):
    if (len(base)>9):
        asyncio.run(main())
        del base[0]
        tiempo.append(cont)
        del tiempo[0]
        #if estado:
        graph(tiempo,base)
            #estado = False
        #else:
           # updategraph(tiempo,base)
    else:
        asyncio.run(main())
        tiempo.append(cont)
    print(base)
    cont+= 1
