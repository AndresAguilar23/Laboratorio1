#include "stm32f7xx.h"
#include <stdio.h>
#include "math.h"
#include <cmath>

//RESULTADO SENSORES
//int valores[8] = {1, 2, 3, 4, 5, 6, 7, 8};
int valores[4] = {1, 2, 3, 4};
void enviar();
int datos = 0;

// Interrupcion TIMER
extern "C"{
	void TIM2_IRQHandler(void){
		TIM2 -> SR &= ~(1<<0);
		GPIOB -> ODR ^= (1UL << 0);
		for(int i = 0; i < 4; i++){
			UART4 -> TDR = valores[i] + 48;
			while ((UART4->ISR &= 0X80) == 0);
		}
	}
}	

int main (void){
	RCC -> APB1ENR = 0X1; //HABILITAR TIM2
	RCC -> AHB1ENR |= 0X6; // B Y C
	RCC -> APB1ENR |= 0X80000; //HABILITAR UART4
	RCC -> APB2ENR |= 0X100; // ADC1
	
	GPIOB -> MODER = 0X55; // PUESTO EN SALIDA  
	GPIOB -> OTYPER = 0X0; // PUSH PULL  
	GPIOB -> OSPEEDR = 0X55; // VELOCIDAD MEDIA  
	GPIOB -> PUPDR = 0X55; // PULL UP 
	
	TIM2 -> CR1 = 0x1; // CONTADOR HABILITADO
	TIM2 -> DIER = 0x1; // HABILITACION DE INTERRUPCION AL TERMINAR CONTEO
	TIM2 -> ARR = 39999; // 
	TIM2 -> PSC = 1; // PARA 1 Hz
	NVIC_EnableIRQ(TIM2_IRQn); // HABILITACION DE INTERRUPCION

	//UART
	// HABITILITA UART, TRANSMISOR, RECEPTOR Y LA INTERRUPCION POR RECEPCION
	UART4 -> CR1 |= 0X2D;	 
	UART4 -> BRR |= 0X683; // 9600 BAUDIOS
	NVIC_EnableIRQ(UART4_IRQn);
	// MODO ALTERNANTE(UART) Y MODO ANALOGO PUERTO C (PIN 0 - PIN 1 - PIN 2)
	GPIOC -> MODER |= 0XA0003F; 
	// CONFIGURACION DEL REGISTRO ALTERNO PARA EL TX Y RX
	GPIOC -> AFR[1] |= 0X8800;  

	//ADCS
	ADC1 -> CR1 |= 0X2000000;//RESOLUCION DE 12 BITS
	ADC1 -> CR2 |= 0X401;//ENCENDER EL ADC (END OF CONVERSION, HABILITAR ADC)
	ADC1 -> SQR3 = 10;// CANAL 10 DEL ADC
	
	while(true){
		ADC1->CR2 |= 0X40000000; // INICIAR LA CONVERSION EN CANAL REGULARES

		while ((ADC1 -> SR &= 0X2) == 1); 
		datos = ((ADC1 -> DR) * 3.3 / 255) * 1000;
		
		valores[0] = (datos) / 1000;
		valores[1] = (datos % 1000) / 100;
		valores[2] = (datos % 100) / 10;
		valores[3] = (datos % 10);
	}
}




//# import serial, time
//# import threading
//#
//# data = []
//#
//#
//# def DATOSMICRO():
//#     global dato
//#     Conexion = serial.Serial("COM6", 9600)
//#     file = open("8bits_40hz.txt", "w")
//#     i = 0
//#     while i < 4000:
//#         data = Conexion.read(4).decode('gbk')  # Datos recibidos en una lista
//#         dato = int(data[0] + data[1] + data[2] + data[3]) * 0.001
//#         file.write("\n" + str(dato))
//#         i += 1
//#         print(dato)
//#         dato = 0
//#
//#     Conexion.close()
//#
//#
//# Hilo1 = threading.Thread(target=DATOSMICRO)
//# Hilo1.start()

