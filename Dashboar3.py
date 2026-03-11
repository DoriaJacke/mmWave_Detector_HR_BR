#Librerias
import pandas as pd
import streamlit as st
from streamlit_autorefresh import st_autorefresh
import os
import glob



#Funciones 
def clasCar(x):
    '''Entrega el rango donde se encuentra la Frecuencia cardiaca'''
    if (60<=x<=100):
        return 'Normal'
    elif (x<60):
        return 'Baja'
    else:
        return 'Alta'

def clasRes(x):
    '''Entrega el rango donde se encuentra la Frecuencia Respiratoria'''
    if (12<=x<=20):
        return 'Normal'
    elif (x<12):
        return 'Baja'
    else:
        return 'Alta'

def mean(comlumn):
    '''Entrega el promedio actual de la columna entregada'''
    return round(data.tail(100)[comlumn].mean(),0)


#Main
list_of_files = glob.glob(r"Datos\datos_*.csv") # Carpeta donde se encuentran los archivos csv
latest_file = max(list_of_files, key=os.path.getctime)
data= pd.read_csv(latest_file)

#Creacion de pagina/Dashboard
st.set_page_config(layout='wide', initial_sidebar_state='expanded')


with open('style.css') as f:
    st.markdown(f'<style>{f.read()}</style>', unsafe_allow_html=True)

#Presentacion y ajustes del menu desplegable
st.sidebar.header('Dashboard `version 3`')

st.sidebar.subheader('Line chart parameters')
plot_data = st.sidebar.multiselect('Select data', ['Frecuencia Cardiaca', 'Frecuencia Respiratoria'], 
                                   ['Frecuencia Cardiaca', 'Frecuencia Respiratoria'])
plot_height = st.sidebar.slider('Specify plot height', 300, 500, 350)

st.sidebar.markdown('''
---
Created by Andres Calderon.
''')


#Implementacion de cuadro con parametro a monitorear
st.markdown('### Parametros')
col1, col2,col3,col4,col5= st.columns(5)

col1.metric("F.Cardiaca", str(data["Frecuencia Cardiaca"][len(data)-1]),
            (data["Frecuencia Cardiaca"][len(data)-1])-mean("Frecuencia Cardiaca"))

col2.metric("Promedio FC",mean('Frecuencia Cardiaca'),
           clasCar(mean('Frecuencia Cardiaca')),delta_color='off')

col3.metric("F.Respiratoria", str(data['Frecuencia Respiratoria'][len(data)-1]), 
            (data['Frecuencia Respiratoria'][len(data)-1]) - mean('Frecuencia Respiratoria'))

col4.metric("Promedio FR",mean('Frecuencia Respiratoria'),
           clasRes(mean('Frecuencia Respiratoria')),delta_color='off')

col5.metric("Hora medicion",str(data['Hora'][len(data)-1]))



# Implementacion de grafico de lineas
st.markdown('### Grafico')
st.line_chart(data.tail(100), x = 'Hora', y = plot_data, height = plot_height)


#Grafico linea 2
st.markdown('### Grafico 2')
info = {
       "":128,
        "Punto 1":data["Punto 1"][len(data)-1],
       "":128,
        "Punto 2":data["Punto 2"][len(data)-1],
        "Punto 3":data["Punto 3"][len(data)-1],
        "Punto 4":data["Punto 4"][len(data)-1],
        "":128,
        "Punto 5":data["Punto 5"][len(data)-1],  
        "":128 
        }
ref=[data["Punto 1"],data["Punto 2"],data["Punto 3"],data["Punto 4"],data["Punto 5"]]
st.line_chart(info)


st_autorefresh(interval=1000, limit=5000, key="fizzbuzzcounter")