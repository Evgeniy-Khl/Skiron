/*****************************************************
Project : Skiron
Version : 0.0.0
Chip type           : ATmega32
Program type        : Application
Clock frequency     : 16 MHz
Programm size       : 11358 words (22716 bytes), 69,3% of FLASH 11.09.2022
*****************************************************/

#include <mega32.h>

#include <stdio.h>      // Standard Input/Output functions
#include <spi.h>        // SPI functions
#include <1wire.h>
#include <delay.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "usart_9600.h"

#include "fonts.h"
#include "tft.h"
#include "9341.h"

#define CONECT          PINB.3    // если bluetooth подключен то здесь +3,3В
#define MAX_DEVICES     3
#define MAX_MENU        5
#define MAX_SET         4
#define MAX_DATE        5

#define ON              1
#define OFF             0
#define UNCHANGED       2
#define MISTAKE         3

#define SBLK		    0x91      // Начало блока данных
#define BSTR		    0x81      // Начало строки данных
#define EBLK            0xF1      // Конец блока данных
#define DS3231          0xD0      // Часовая микросхема

#define TWI_CLK_RATE    100000    // TWI clock rate [bps]
#define EEPROM_TWI_BUS_ADDRESS (0xA0 >> 1)// 7 bit TWI bus slave address of the AT24C16B 2kbyte EEPROM
#define ADC_VREF_TYPE   0x40

// Declare your global variables here
//flash float A1=1.8, A2=0.81, A3=0.01;  // порядок a=0.9 (A1=2a; A2=a^2; A3=(1-a)^2)
//flash float A1=1.6, A2=0.64, A3=0.04;  // порядок a=0.8 (A1=2a; A2=a^2; A3=(1-a)^2)
//flash float A1=1.2, A2=0.36, A3=0.16;  // порядок a=0.6 (A1=2a; A2=a^2; A3=(1-a)^2)
flash float A1=0.8, A2=0.16, A3=0.36;  // порядок a=0.4 (A1=2a; A2=a^2; A3=(1-a)^2)

//--------- Registr region ----------------------------------------------------
#pragma regalloc-
//register char relay @2;/* allocate the variable ‘relay’ to the register pair R2*/
#pragma regalloc+
//unsigned char *ptr_to_ram;
//unsigned char eeprom *ptr_to_eeprom;
unsigned char /*counter0, counter1,*/ BeepT;
//****************
unsigned char displ_num, newButton, newSetButt, pointY, error;
unsigned char error, devices, signchar, intval, frcval;

signed char numMenu;

unsigned char devices, portOut;
signed char numMenu, numSet, displCO2, timerCO2;
unsigned char *ptr_char;
const char* setMenu[MAX_MENU]={"Температура","Влажность","Таймер","Настройки","Время и Дата"};
const char* setName0[MAX_SET]={"Задание","Отклон.","Гистер.","Режим"};
const char* setName1[MAX_SET]={"Включен","Выключен", "Размерн.","Шаг"};
const char* setName2[MAX_SET]={"Корекция Т","Корекция Вл","Датчик Вл","Резерв"};
const char* setName3[MAX_DATE]={"минуты","часы","день","месяц","год"};
unsigned int max_X, max_Y, fillScreen = BLACK;
signed int pvT=1990, offsetT, pvCO2, newval[MAX_DATE];
unsigned char alarm[4]={2,2,2,2}; // alarm[0] - датчик №1; alarm[1] - датчик Вл; alarm[2] - датчик №2; alarm[3] - датчик №3;
unsigned char pvRH=100, DHTexist, offsetRH, relOut[4]={0}, buff[40], familycode[MAX_DEVICES][9], clock_buffer[7], analogPV[4]={87,65,15,100}, analogOut[4];
//--------------- union declaration -----------------------------------------------
union {signed int point[MAX_DEVICES]; unsigned char buff[];} t; // буффер значений температур
union {unsigned char buffer[8]; unsigned int pvT;} ds;          // буффер микросхемы DS18B20
union {unsigned char buffer[4]; unsigned int val[2];} in;
union {unsigned char buffer[4]; unsigned int val[2];} out;
//---------------------------------------------------------------------------------
signed char analog[4]={-1,30,90,-1};
signed char relay[4]={2,2,2,2};
float Told1[5], Told2[5];

//-------------------------
eeprom signed int set[5][4]={
                     {270,10, 5, 0},  // Tset; dTalarm; hysteresis;  mode=0 -> нагрев / mode=1 -> охлаждение;
                     { 47, 5, 5, 0},  // RHset; dRHalarm; hysteresis;  mode=0 -> увлажнение / mode=1 -> осушение;
                     {250, 8, 4, 0},  // Tset; dTalarm; hysteresis;  mode=0 -> увлажнение / mode=1 -> осушение;
                     {  5,10, 1, 0},  // tmOn; tmOff; dimension=0 -> секунды / direction=1 -> минуты; ??;
                     {  0, 0, 0, 0}}; // корекция датчика T; корекция датчика Вл; тип датчика = 0 -> DHT21; Other = 1 -> DHT11

//----------------------------------Таблица влажности ------------------------------------------------------
eeprom unsigned char tabRH[420]={
95,90,86,81,77,72,68,64,60,56,52,48,44,40,36,32,29,25,22,18,
95,91,86,82,77,73,69,65,61,57,53,49,45,42,38,34,31,27,24,20,
95,91,87,82,78,74,70,66,62,58,54,50,47,43,40,36,33,29,26,23,
96,91,87,83,79,75,71,67,63,59,55,52,48,45,41,38,34,31,28,25,
96,91,87,83,79,75,71,67,64,60,56,53,49,46,43,39,36,33,30,26,
96,92,88,84,80,76,72,68,65,61,57,54,51,47,44,41,37,34,31,28,
96,92,88,84,80,76,73,69,65,62,58,55,52,48,45,42,39,36,33,30,
96,92,88,84,80,77,73,70,66,63,59,56,53,50,46,43,40,37,34,32,
96,92,88,85,81,77,74,70,67,64,60,57,54,51,48,45,42,39,36,33,
96,92,89,85,81,78,74,71,68,64,61,58,55,52,49,46,43,40,37,35,
96,92,89,85,82,78,75,71,68,65,62,59,56,53,50,47,44,41,39,36,
96,93,89,85,82,79,75,72,69,66,63,60,57,54,51,48,45,42,40,37,
96,93,89,86,82,79,76,73,69,66,63,60,57,55,52,49,46,44,41,38,
96,93,89,86,83,79,76,73,70,67,64,61,58,55,53,50,47,45,42,40,
96,93,90,86,83,80,77,74,71,68,65,62,59,56,54,51,48,46,43,41,
97,93,90,87,83,80,77,74,71,68,65,62,60,57,54,52,49,47,44,42,
97,93,90,87,84,80,77,74,72,69,66,63,60,58,55,53,50,48,45,43,
97,93,90,87,84,81,78,75,72,69,66,64,61,58,56,53,51,48,46,44,
97,93,90,87,84,81,78,75,72,70,67,64,62,59,57,54,52,49,47,45,
97,94,90,87,84,81,79,76,73,70,67,65,62,60,57,55,52,50,48,46,
97,94,91,88,85,82,79,76,73,71,68,65,63,60,58,56,53,51,49,46
};
//----------------------------------------------------------------------------------------------------------
bit Sec;
bit Clock_Ok;
bit Dht;
bit Soil;
bit pHsensor;
bit typeS;
bit CO2module;          // подключен измеритель СО2
bit CheckCO2;           // разрешено использование данных измерителя СО2

//- prototypes ------
char buttonCheck(void);

#include "9341.c"
#include "tft.c"
#include "twi.c"
#include "dht11.c"
#include "buttons.c"
#include "proc.c"
#include "control.c"
#include "touchpad.c"
#include "displ.c"


// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void){
 char status,data;
 status=UCSRA;
 data=UDR;
 if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0){
   rx_buffer[rx_wr_index++]=data;
#if RX_BUFFER_SIZE == 256
   // special case for receiver buffer size=256
   if (++rx_counter == 0){
#else
       if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
       if (++rx_counter == RX_BUFFER_SIZE){
          rx_counter=0;
#endif
          rx_buffer_overflow=1;
       }
   }
}

// USART Transmitter interrupt service routine
interrupt [USART_TXC] void usart_tx_isr(void){
 if (tx_counter){
   --tx_counter;
   UDR=tx_buffer[tx_rd_index++];
#if TX_BUFFER_SIZE != 256
   if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
#endif
   }
}

// External Interrupt 0 service routine (управляет часовая микросхема)
interrupt [EXT_INT0] void ext_int0_isr(void){
 static unsigned int count1;
 count1++;
}
// 0.25 Hz timer interrupt generated by Timer1 overflow
interrupt [TIM1_OVF] void timer_comp_isr(void){
 Sec=1;
}                 


void main(void){
// Declare your local variables here
char byte;
#include "init.c"

while (1){                       
   //----------- функция 1 секунда ---------------------------
   if(Sec){                     
     Sec=0; 
     if (devices) temperature_check();
     if (Dht){                                         // присутствует датчик влажности
       if(readDHT()) DHTexist = 3; 
       else if(DHTexist) DHTexist--;                   // датчик влажности работает? 
       else {pvT = 1900; pvRH = 190;}
     }
     else {
       byte = tableRH(t.point[0],t.point[1]);          // если отсутствует то ...
       if(byte>100) pvRH = 999; else pvRH = byte;
     }     
     // --------КАНАЛ 1 ---------
     if(Dht){
       RelayControl(pvT,0);
       RelayControl(pvRH,1);
     } 
     else {
       RelayControl(t.point[0],0);
       RelayControl(t.point[1],1);
     }
     // --------КАНАЛ 2 --------- 
     relayHadl();
     analogHadl();
     if (displ_num<3) display();
     //-- CO2 ----------------------------------
     if(CO2module){
       byte = readCO2();     // если модуль подключен то обмениваемся командами и обновляем только pvCO2
       if(byte) pvCO2 = LowPassF2(pvCO2,3);// если идет измерение СО2 то обновляем Tf[3]
       controlCompressorCO2(4);  // Компрессор СО2.
     }
   }
   //---------------------------------------------------------
   if(newButton==100){
     if(checkTouch()){
         byte = (checkButton(buttonCount));
         if (byte < buttonCount){ touchpad(byte); display();}
     }
   }
   else {
    touchpad(newButton);
    newButton=100;
    display();
   }
   /*
   if(NewMin){
     NewMin = 0;
     if(++timerCO2>(set[3][0] + set[3][1] - 1)) timerCO2=0;// компрессор СО2
   }
   */
//   sprintf(buff,"y%4u  x%4u  val%4u",val0,val1,val2);
//   TFT_DrawString(buff,10,20,1,0,WHITE,BLACK); 
//   sprintf(buff,"X%6u  Y%6u D%4u",point_X,point_Y, displ_num);
//   TFT_DrawString(buff,10,80,1,0,WHITE,BLACK);
 };
}
