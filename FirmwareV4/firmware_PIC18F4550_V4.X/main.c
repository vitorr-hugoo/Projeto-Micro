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

#define _XTAL_FREQ 48000000 
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


void main()
{
    inicia_Sistema();
    Servo_1_Write(2);
    
    
    while(1)
    {   
        exibeData_e_Hora();
        verificaHorario();
        //exibeTemp_e_Umid();
        dht_ok = DHT11_Read_Data(&temperatura, &humedad);
        
        if(dht_ok == 1)

            sprintf(buffer, "Hum: %0.1f %%", humedad);
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 26, buffer);
            OLED_Update();
            sprintf(buffer, "Tem: %0.1f C", temperatura);
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 47 , buffer);
            OLED_Update();
            if(temperatura > 27.5){
                
                acionaCooler();
                desligaLamp();
            }
            else if(temperatura < 27.5 && temperatura >26.5){
                acionaCooler();
                acionaLamp();
            }
                            
            else{
                desligaCooler();
                acionaLamp();
            }
            
        __delay_ms(500);
      
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
    
    DS3231_Set_Date_Time(10,2,25,2,13,30,0);  //// Day,Month,Year,Day_Week,Hour,Minute,Second
    __delay_ms(500);
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
    if(hr == 13 && min == 30 && (sec >25 && sec<30 ) && flag1_rtc == 0 ) {
            flag1_rtc = 1; // se entrar nessa condição, ativa essa flag
            OLED_ClearDisplay();
            Servo_1_Write(108); //função que movimenta o servo para 90° (108° obtido experimentalmente)
            
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 26, "COMIDA ACIONADA"); // exibe nos display quando aciona o servo
            OLED_Update();
            __delay_ms(2000); //delay de 2 seg para liberação da comida
            OLED_ClearDisplay();
            Servo_1_Write(2);  //volta à posição incial do servo   
        }
    
        //horario pre definido 2: 13:31:01
        if(hr == 13 && min == 31 && (sec >00 && sec<05 ) && flag2_rtc == 0 ) {
            flag2_rtc = 1;
            OLED_ClearDisplay();
            Servo_1_Write(108);
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 26, "COMIDA ACIONADA");
            OLED_Update();
            __delay_ms(2000);
            OLED_ClearDisplay();
            Servo_1_Write(2);
        }
        
        if(hr != 13 || min != 30 || sec>30) flag1_rtc = 0; //se o horario de ativação ja passou, desabilita flag para que possa ser acionado novamente  
        if(hr != 13 || min != 31 || sec>05 ) flag2_rtc = 0; 
}

void exibeTemp_e_Umid(){
    
    dht_ok = DHT11_Read_Data(&temperatura, &humedad);
        
        if(dht_ok == 1)

            sprintf(buffer, "Hum: %0.1f %%", humedad);
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 26, buffer);
            OLED_Update();
            sprintf(buffer, "Tem: %0.1f C", temperatura);
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 47 , buffer);
            OLED_Update();
            if(temperatura > 27.5){
                
                acionaCooler();
                desligaLamp();
            }
            if(temperatura < 25){
                desligaCooler();
                acionaLamp();
            }
        }

