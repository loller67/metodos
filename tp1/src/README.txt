Para correr los scripts, desde una consola ejecutar:

$ python metnum.py opcion

donde opcion puede ser:

compile: Compila todos los *.cpp que hayan en el directorio.
link: Genera el ejecutable en base a los *.o generados previamente.
build: compile + link
clean: borra los *.o y el ejecutable.
test: hace el build, busca lo archivos *.in en la carpeta tests/, ejecuta el programa y guarda el
resultado para cada corrida en el correspondiente .out. Despues, chequea que el resultado sea el "mismo" que el
.expected, tambien del directorio test. En este caso, la comparacion es por tolerancia coordenada a coordeanda del vector
solucion.

Ejecutable
==========

Para poder correr el ejecutable de acuerdo a las especificaciones del TP, se debe ejecutar
'./tp entrada salida metodo' donde
-entrada: nombre archivo de entrada
-salida: nombre archivo de salida
-metodo: es un número [0=CMM, 1=WP, 2=CMM_ATP]

Experimentos
============

Para reproducir los experimentos del análisis cuantitativo y cualitativo (sin considerar justicia/estrategias), se proveen los Jupyter Notebook experimentos/AnalisisCuantitativo.ipynb y experimentos/ComparacionRakings.ipynb respectivamente.

Dado que los experimentos de justicia/estrategia utilizan únicamente la salida del ejecutable, estos se pueden reproducir corriendo los ejemplos provistos en las carpetas justice/ y strategy/ respectivamente.
