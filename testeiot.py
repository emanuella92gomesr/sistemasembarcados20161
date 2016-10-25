#!/usr/bin/python
import sys
import RPi.GPIO as GPIO
import os
from time import sleep
import Adafruit_DHT #Biblioteca responsável pelo acesso aos dados do sensor DHT11
#Para instalar o Adafruit_DHT no teminal : git clone https://github.com/adafruit/Adafruit_Python_DHT.git
import urllib2 #Biblioteca que possibilita o acesso a um determinado site

DEBUG = 1
DHTPino = 4 #Pino GPIO no qual o pino Signal do DHT11 está conectado
meuAPI ="SUA_CHAVE" #Chave do canal criado no ThingSpeak
meuDelay = 5 #Tempo para o plotamento dos dados nos gráficos do Thingspeak

GPIO.setmode(GPIO.BCM)

def getDadosSensor():
	RHW, TW = Adafruit_DHT.read_retry(Adafruit_DHT.DHT11, DHTPino) #Leitura dos dados obtidos no sensor DHT11
	TWF =9/5*TW+32 #Conversão de Celsius para Fahrenheit
	return (str(RHW), str(TW), str(TWF)) #Conversão para string dos dados que foram obtidos do sensor

def main():
	print 'Iniciando...'
	baseURL = 'https://api.thingspeak.com/update?api_key=%s' % (meuAPI)
	while True:
		try:
			RHW, TW, TWF = getDadosSensor()
			f = urllib2.urlopen(baseURL + "&field1=%s&field2=%s&field3=%s" % (TW, TWF, RHW)) #Prenchimento do gráfico
			print f.read()
			print TW + " " + TWF + " " + RHW + " " #Imprime os dados do sensor no terminal
			f.close()

			sleep(int(meuDelay))
		except : #Finaliza a execução do código (CTRL+C)
			print 'fui'
			break
# chamando o métodos main

if __name__ == '__main__': main() 

