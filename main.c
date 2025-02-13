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

// Interrupção para leitura da temperatura
void __interrupt(high_priority) Low_ISR(void) {
    if (PIR1bits.TMR1IF) {
        // Código do Timer1 (Leitura do DHT11)
        TMR1 = 3036;
        dht_ok = DHT11_Read_Data(&temperatura, &humedad);
        PIR1bits.TMR1IF = 0;
    }
}

void main()
{
    //ADCON1bits.PCFG = 0x0F;             // Coloca todos los pines como digitales
    //Inicializa_LCD();                         // Inicializa la pantalla lcd
    pino_lamp_TRIS=0; //configura para saida
    
    T1CON=0b11111000;     //config do registrador TIMER1
    T1CONbits.TMR1ON = 1; // habilita TIMER1
    PIE1bits.TMR1IE = 1;  //habilita interrupção TIMER1
    PIR1bits.TMR1IF = 0;  // flag = 0 
    INTCONbits.PEIE = 1;  //habilita interrupções de periféricos
    TMR1=3036;
    
    RCONbits.IPEN = 1;   // Habilita sistema de prioridades

    // Configura prioridade das interrupções
    INTCON2bits.TMR0IP = 0; // Timer0 -> Alta prioridade
    IPR1bits.TMR1IP = 1;    // Timer1 -> Baixa prioridade

    // Configura interrupções individuais
    INTCONbits.GIEH = 1;  // Habilita interrupções de alta prioridade
    INTCONbits.GIEL = 1;  // Habilita interrupções de baixa prioridade
    
    Servo_Init();
    I2C_Init_Master(I2C_400KHZ);                // Inicializa o protocolo i2c
    OLED_Init();
    OLED_SetFont(FONT_1);
    OLED_Write_Text(5, 7, "INICIALIZANDO...");
    OLED_Update();
    
    DS3231_Set_Date_Time(10,2,25,2,13,30,0);  //// Day,Month,Year,Day_Week,Hour,Minute,Second
    __delay_ms(1500);
    acionaLamp();
    OLED_ClearDisplay();
    
    
    while(1)
    {   
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
        
        Servo_1_Write(0);
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
            if(temperatura > 34.0){
                
                acionaCooler();
                desligaLamp();
            }
            if(temperatura < 32.0){
                desligaCooler();
                acionaLamp();
            }
        }
        
        Servo_1_Write(108);
        
        __delay_ms(1200);
    }
}
