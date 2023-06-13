//declaraciones de la tasa de comunicación serial
#define F_CPU 16000000                //Definición de velocidad del cristal

//Librerias para FreRTOS
#include <Arduino_FreeRTOS.h>         //Librerias para FreeRTOS core.
#include "queue.h"                    //Libreria de colas(FreeRTOS).

//Librerias para los sensores

#include <Arduino.h>                  //Libreria Arduino core.
#include <Wire.h>                     //Libreria para comunicacion con dispositivos I2C.
#include <SoftwareSerial.h>           //Libreria para comunicacion serial por medio de pines.
#include "src/MeSingleLineFollower.h" //Libreria para sensor seguidor de linea.
#include "src/MeCollisionSensor.h"    //Libreria para sonsor de colision.
#include "src/MeBarrierSensor.h"      //Libreria para sensor de proximidad IR (Infra Rojo).
#include "src/MeNewRGBLed.h"          //Libreria para leds (RGB).
#include <MeMegaPi.h>                 //Libreria megaPI

//Definiciones sensores
MeBarrierSensor barrier_60(60);       //Sensor de proximidad IR izquierdo.
MeNewRGBLed rgbled_67(67,4);          //Led RGB izquierdo.
MeNewRGBLed rgbled_68(68,4);          //Led RGB derecho.
MeBarrierSensor barrier_61(61);       //Sensor de proximidad IR central.
MeBarrierSensor barrier_62(62);       //Sensor de proximidad IR derecho.
double angle_rad = PI/180.0;
double angle_deg = 180.0/PI;
MeCollisionSensor collision_65(65);   //Sensor de colision izquierdo.
MeCollisionSensor collision_66(66);   //Sensor de colision derecho.

//buffer para el UART
unsigned char mybuffer[25];

//handle para un queue
QueueHandle_t myQueue;

//Se define función para el timer
void T3Delay();

//Se definen funciones movimientos del motor
void forward();
void backward();
void right();
void left();
void stopbot();

//Se definen funciones para UART
void usart_init (void);
void usart_send (unsigned char ch);


void setup() {
  //Uart
  usart_init();    //INICIALIZACION DE USART
  
  //Definiciones robot (llantas).
  DDRB = 0XFF; // PORTB output port.
  DDRC = 0XFF; // PORTB output port.
  DDRH = 0XFF; // PORTB output port.
  DDRG = 0XFF; // PORTB output port.


  //creación de tareas
  xTaskCreate(vTaskMov,"Movimiento TASK",100,NULL,2,NULL);      //Tarea control de robot (Movimiento).
  xTaskCreate(vTaskCollision,"Collision TASK",100,NULL,1,NULL); //Tarea para colision de robot.
  xTaskCreate(vTaskPresence,"Presence TASK",100,NULL,1,NULL);   //Tarea detección de objeto con sesor de proximidad IR.
  
}

//--------------------Tarea movimiento del robot--------------------
void vTaskMov(void * pvParameters){
  //Definiciones de registros
  TCCR1A = 0xA1;   //Estado en el que se va a prender y tipo de onda
  TCCR1B = 0x0C;   //(TCCR1A [Byte más alto] y TCCR1B [Byte más bajo]).
  TCCR4A = 0x29;   //Estado en el que se va a prender y tipo de onda
  TCCR4B = 0x0C;   //(TCCR4A [Byte más alto] y TCCR4B [Byte más bajo]).
  

  
  OCR1A = 128;  //Se determina el valor del PWM.
  OCR1B = 128;  //Se determina el valor del PWM.
  OCR4B = 128;  //Se determina el valor del PWM.
  OCR4C = 128;  //Se determina el valor del PWM.
  
  while(1){
    //Se declara variable tipo char para recibir señal de control.
    unsigned char ch;

    
    while(!(UCSR2A&(1<<RXC2))); //Espera nuevos datos
    
    ch = UDR2;                  //Se lee el dato de UART2
    
    //Comparacion de 'ch' para mobimiento de robot
    if(ch == 'f'){
      //Si 'ch' es igual a 'f' avanza.
      forward();
    }
    else if(ch == 'b'){
      //Si 'ch' es igual a 'b' retrocede.
      backward();
    }
    else if(ch == 'r'){
      //Si 'ch' es igual a 'r' gira a la derecha. 
      right();
    }
    else if(ch == 'l'){
      //Si 'ch' es igual a 'l' gira a la izquierda.
      left();
    }
    else if(ch == 's'){
      //Si 'ch' es igual a 's' se detiene.
      stopbot();
    }

    //bloquea tarea por 150ms de forma periodica.
     vTaskDelay(150/portTICK_PERIOD_MS);    
     
  }
}

//--------------------Tarea para deteccion de colision--------------------/
void vTaskCollision(void * pvParameters){
  //Definicion de variable tipo char para mandar señal en caso de colision.
  unsigned char choque;
 
  while(1){
 
    //Verifica choque en ambos  sensores.
    if( collision_65.isCollision() || collision_66.isCollision()){
        choque = 'c';         //Se le otorga un valor a la señal de envio.
        usart_send(choque);   //Se envia señal.
        usart_send('\n');     //Se envia salto de linea.
      }

      //bloquea tarea por 300 ms de forma periodica.
      vTaskDelay(150/portTICK_PERIOD_MS);  
   }  
}

//---------------------Tarea para deteccion de obstaculo IR--------------------/
void vTaskPresence(void * pvParameters){
  //Definicion de variable tipo char para la mandar señal en caso de obstaculo
  unsigned char presencia;
  
  while(1){
 
    //Verifican ambos sensores de proximidad IR.
    if( barrier_62.isBarried() || barrier_60.isBarried()){
        presencia = 'p';        //Se le otorga un valor a la señal de envio.
        usart_send(presencia);  //Se envia señal.
        usart_send('\n');       //Se envia salto de linea.
      }

      //bloquea tarea por 150ms periodicos.
      vTaskDelay(150/portTICK_PERIOD_MS);    
   }  
}



//Funcion de movimiento para robot (Enfrente).
void forward(){
  PORTC  =  0b00010101; // (Input (High/Low) I1 e I2 Motores 1 y 2)
  PORTG  =  0b00000001; // (Input (High/Low) I1 e I2 Motores 3 y 4)
  
}

//Funcion de movimiento para robot (Atras).
void backward(){
  PORTC  =  0b00101010; // (Input (High/Low) I1 e I2 Motores 1 y 2)
  PORTG  =  0b00000010; // (Input (High/Low) I1 e I2 Motores 3 y 4)
  
}

//Funcion de movimiento para robot (Derecha).
void right(){
  PORTC  =  0b00101001; // (Input (High/Low) I1 e I2 Motores 1 y 2)
  PORTG  =  0b00000001; // (Input (High/Low) I1 e I2 Motores 3 y 4)
  
}

//Funcion de movimiento para robot (Izquierda).
void left(){
  PORTC  =  0b00010110; // (Input (High/Low) I1 e I2 Motores 1 y 2)
  PORTG  =  0b00000010; // (Input (High/Low) I1 e I2 Motores 3 y 4)
}

//Funcion de movimiento para robot (Detenido).
void stopbot(){
  PORTC  =  0b00000000; // (Input (High/Low) I1 e I2 Motores 1 y 2)
  PORTG  =  0b00000000; // (Input (High/Low) I1 e I2 Motores 3 y 4)
}

// Función para el Delay en Timer 3.
void T3Delay(){
  TCNT3H = 0xA4;                  // Establece el byte alto del Timer/Counter 3 en 0xA4.
  TCNT3L = 0x73;                  // Establece el byte bajo del Timer/Counter 3 en 0x73.
  TCCR3A = 0;                     // Limpia el Registro de Control A del Timer/Counter 3.
  TCCR3B = 0x05;                  // Establece el Registro de Control B del Timer/Counter 3 en 0x05.
  while((TIFR3 & (1<<TOV3))==0);  // Espera hasta que se establezca el indicador de desbordamiento del Timer/Counter 3.
  TCCR3B = 0;                     // Limpia el Registro de Control B del Timer/Counter 3.
  TIFR3 = (1<<TOV3);              // Limpia el indicador de desbordamiento del Timer/Counter 3.
}

// Función para inicializar la transmisión por UART.
void usart_init (void){
  UCSR2B = (1<<TXEN2 | 1<<RXEN2);       // Habilita la transmisión y la recepción en la UART2.
  UCSR2C = (1<< UCSZ21) | (1<< UCSZ20); // Configurar el tamaño de los datos transmitidos/recibidos en la UART2.
  UBRR2L = 103;                         // Transmisión de 9600 bps.
} 

// Función para inicializar la transmisión por UART.
void usart_send (unsigned char ch){
  while (!(UCSR2A &(1<< UDRE2))); //WAIT UNTIL UDR0 IS EMPTY
  UDR2 = ch;            //TRANSMIT CH
}

//IDLE TASK VACIO
////////////////////////////////////////////////////////////////////////////
void loop() {}
