/*
 * File:   main.c
 * Author: vitor
 *
 * Created on 31 de Janeiro de 2025, 10:02
 */

#pragma config PLLDIV = 2, CPUDIV = OSC1_PLL2, USBDIV = 2
#pragma config FOSC = HSPLL_HS, FCMEN = OFF, IESO = OFF
#pragma config PWRT = OFF, BOR = OFF, BORV = 3, VREGEN = OFF
#pragma config WDT = OFF
#pragma config WDTPS = 32768
#pragma config CCP2MX = ON, PBADEN = OFF, LPT1OSC = OFF, MCLRE = ON
#pragma config STVREN = ON, LVP = OFF, ICPRT = OFF, XINST = OFF
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF
#pragma config CPB = OFF, CPD = OFF
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF
#pragma config WRTC = OFF, WRTB = OFF, WRTD = OFF
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF
#pragma config EBTRB = OFF

#define _XTAL_FREQ 48000000 //48000000
#include <xc.h>
#include <stdio.h>

#include "LCD16x2_config.h"                        // Libreria de la pantalla lcd
#include "dht11.h"                      // Libreria del sensor DHT11
#include "cooler.h"
#include "lamp.h"
#include "i2c.h"                                // Libreria del protocolo i2c
#include "ssd1306_oled.h" 
#include "servo.h"
#include "ds3231.h"


short dht_ok;                           // Flag de verificacion del bit de paridad
float temperatura;                      // Almacena la temperatura
float humedad;                          // Almacena la humedad
char buffer[20];
uint8_t day, mth, year, dow, hr, min, sec;
uint8_t flag1_rtc = 0, flag2_rtc = 0;


void exibeData_e_Hora();
void exibeTemp_e_Umid();
void inicia_Sistema();
void verificaHorario();


void iniciaTMR0(){
    
    
}

void iniciaTMR1(){
    //INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    //PIE1bits.TMR1IE=1;//habililita tmr1
    //IPR1bits.TMR1IP=0; //alta prioridade
    PIR1bits.TMR1IF=0;
    //T1CON.RD16=8;
    //T1CON.TMR1CS=0;
    T1CON=0b10110000;
    T1CONbits.TMR1ON=1;
    TMR1=64030; //int a cada 10ms -> 100x10ms
    
}

void main()
{
    inicia_Sistema();
    Servo_1_Write(100);
    
    
    while(1)
    {   
        
        
        
        //Servo_1_Write(0);
        exibeData_e_Hora();
        verificaHorario();
        exibeTemp_e_Umid();
        //Servo_1_Write(90);
        
        //__delay_ms(1500);
        
        
    }
}

void inicia_Sistema(){
    pino_lamp_TRIS=0; //configura para saida
    Servo_Init();
    I2C_Init_Master(I2C_400KHZ);                // Inicializa el protocolo i2c
    OLED_Init();
    OLED_SetFont(FONT_1);
    OLED_Write_Text(5, 7, "INICIALIZANDO...");
    OLED_Update();
    //iniciaTMR0();
    iniciaTMR1();
    DS3231_Set_Date_Time(10,2,25,2,13,30,0);  //// Day,Month,Year,Day_Week,Hour,Minute,Second
    __delay_ms(1500);
    acionaLamp();
    OLED_ClearDisplay();
}

void exibeData_e_Hora(){
        
        DS3231_Get_Date(&day, &mth, &year, &dow);
        DS3231_Get_Time(&hr, &min, &sec);
        sprintf(buffer, "%02u/%02u/20%02u", day, mth, year);
        OLED_SetFont(FONT_1);
        OLED_Write_Text(1, 1, buffer);
        OLED_Update();
        
        sprintf(buffer, "| %02u:%02u:%02u", hr, min, sec);
        OLED_SetFont(FONT_1);
        OLED_Write_Text(65, 1, buffer);
        OLED_Line(1, 12, 127, 12,1);
        OLED_Update();
        
        
}

void verificaHorario(){
    if(hr == 13 && min == 31 && (sec >25 && sec<30 ) && flag1_rtc == 0 && flag_rtc == 1) {
            flag_rtc = 1;
            Servo_1_Write(188);
            __delay_ms(3000);
            Servo_1_Write(100);      
        }
    
        
        if(hr == 13 && min == 30 && (sec >25 && sec<30 ) && flag2_rtc == 0) {
            flag2_rtc = 1;
            Servo_1_Write(188);
            __delay_ms(3000);
            Servo_1_Write(100);
        }
        
        if(hr != 13 || min != 31 || sec>35 ) flag1_rtc = 0;
        if(hr != 13 || min != 30 || sec>35 ) flag2_rtc = 0;
}

void exibeTemp_e_Umid(){
    
    dht_ok = DHT11_Read_Data(&temperatura, &humedad);
        
        if(dht_ok == 1)
        {
            //Limpa_display_LCD();
            //Posicao_LCD(1,1);
            sprintf(buffer, "Hum: %0.1f %%", humedad);
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 26, buffer);
            OLED_Update();
            //__delay_ms(1500);
            //Escreve_texto_LCD(buffer);
            //Posicao_LCD(2,1);
            sprintf(buffer, "Tem: %0.1f C", temperatura);
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 47 , buffer);
            OLED_Update();
            //__delay_ms(1500);
            //Escreve_texto_LCD(buffer);(buffer);
            if(temperatura > 29.0){
                
                //acionaCooler();
                desligaLamp();
                __delay_ms(500);
            }
            if(temperatura < 28.0){
                desligaCooler();
                acionaLamp();
                __delay_ms(500);
            }
        }
}
