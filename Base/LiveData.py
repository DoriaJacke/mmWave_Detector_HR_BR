import pandas as pd
from itertools import count
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation 
matplotlib.use('TkAgg')


plt.style.use('fivethirtyeight')

index = count()

def animacion(i):
    data= pd.read_csv('datos.csv')
    x = data['Tiempo']
    card =data['Frecuencia Cardiaca']
    resp = data['Frecuencia Respiratoria']

    plt.cla()
    plt.plot(x,card,label='F. Card')
    plt.plot(x,resp,label='F. Resp')
    plt.title('Frecuencias VS Tiempo')
    plt.xlabel('Tiempo[s]')
    plt.ylabel('Frecuencia')
    plt.legend(loc='upper left')
    plt.tight_layout()



#Main 
ani = FuncAnimation(plt.gcf(),animacion,interval=1000)
plt.tight_layout()
plt.show()

