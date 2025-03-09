/* #include "servo.h"
*
* Creada por: Ing. Abiezer Hernandez O.
* Fecha de creacion: 10/01/2021
* Electronica y Circuitos
*
*/

#include <xc.h>
#define _XTAL_FREQ 48000000

#define v_min 14
#define v_max 60
#define SERVO_HARDWARE

#define SERVO_1_PIN LATDbits.LD0
#define SERVO_1_DIR TRISDbits.RD0

//#define SERVO_2_PIN LATDbits.LD1
//#define SERVO_2_DIR TRISDbits.RD1

//#define SERVO_3_PIN LATDbits.LD2
//#define SERVO_3_DIR TRISDbits.RD2

//#define SERVO_4_PIN LATDbits.LD3
//#define SERVO_4_DIR TRISDbits.RD3

//#define SERVO_5_PIN LATDbits.LD4
//#define SERVO_5_DIR TRISDbits.RD4

//#define SERVO_6_PIN LATDbits.LD5
//#define SERVO_6_DIR TRISDbits.RD5

//#define SERVO_7_PIN LATDbits.LD6
//#define SERVO_7_DIR TRISDbits.RD6

//#define SERVO_8_PIN LATDbits.LD7
//#define SERVO_8_DIR TRISDbits.RD7

void Servo_Init(void);

#ifdef SERVO_1_PIN
void Servo_1_Write(unsigned char ang);
#endif

#ifdef SERVO_2_PIN
void Servo_2_Write(unsigned char ang);
#endif

#ifdef SERVO_3_PIN
void Servo_3_Write(unsigned char ang);
#endif

#ifdef SERVO_4_PIN
void Servo_4_Write(unsigned char ang);
#endif

#ifdef SERVO_5_PIN
void Servo_5_Write(unsigned char ang);
#endif

#ifdef SERVO_6_PIN
void Servo_6_Write(unsigned char ang);
#endif

#ifdef SERVO_7_PIN
void Servo_7_Write(unsigned char ang);
#endif

#ifdef SERVO_8_PIN
void Servo_8_Write(unsigned char ang);
#endif

unsigned int cont_sr = 0;
unsigned char data_s[8];
unsigned int cont_rtc = 0;
unsigned int flag_rtc = 0;
void Servo_Init(void)
{
    #ifdef SERVO_1_PIN
    SERVO_1_DIR = 0;
    SERVO_1_PIN = 0;
    #endif

    #ifdef SERVO_2_PIN
    SERVO_2_DIR = 0;
    SERVO_2_PIN = 0;
    #endif

    #ifdef SERVO_3_PIN
    SERVO_3_DIR = 0;
    SERVO_3_PIN = 0;
    #endif

    #ifdef SERVO_4_PIN
    SERVO_4_DIR = 0;
    SERVO_4_PIN = 0;
    #endif

    #ifdef SERVO_5_PIN
    SERVO_5_DIR = 0;
    SERVO_5_PIN = 0;
    #endif

    #ifdef SERVO_6_PIN
    SERVO_6_DIR = 0;
    SERVO_6_PIN = 0;
    #endif

    #ifdef SERVO_7_PIN
    SERVO_7_DIR = 0;
    SERVO_7_PIN = 0;
    #endif

    #ifdef SERVO_8_PIN
    SERVO_8_DIR = 0;
    SERVO_8_PIN = 0;
    #endif
    
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    //INTCON2bits.TMR0IP = 1;
    T0CON = 0x50; //0b0101 0000 
    //-> config 8 bits 
    //-> transiçao do clock de high p low
    // prescaler 1:2
    T0CONbits.TMR0ON = 1; //ativa bit de controle do tmr0
    TMR0 = 50;
    
}

#ifdef SERVO_1_PIN
void Servo_1_Write(unsigned char ang)
{
    data_s[0] = ((ang-0)*(v_max-v_min))/(180-0)+v_min;
}
#endif

#ifdef SERVO_2_PIN
void Servo_2_Write(unsigned char ang)
{
    data_s[1] = (ang-0)*(v_max-v_min)/(180-0)+v_min;
}
#endif

#ifdef SERVO_3_PIN
void Servo_3_Write(unsigned char ang)
{
    data_s[2] = (ang-0)*(v_max-v_min)/(180-0)+v_min;
}
#endif

#ifdef SERVO_4_PIN
void Servo_4_Write(unsigned char ang)
{
    data_s[3] = (ang-0)*(v_max-v_min)/(180-0)+v_min;
}
#endif

#ifdef SERVO_5_PIN
void Servo_5_Write(unsigned char ang)
{
    data_s[4] = (ang-0)*(v_max-v_min)/(180-0)+v_min;
}
#endif

#ifdef SERVO_6_PIN
void Servo_6_Write(unsigned char ang)
{
    data_s[5] = (ang-0)*(v_max-v_min)/(180-0)+v_min;
}
#endif

#ifdef SERVO_7_PIN
void Servo_7_Write(unsigned char ang)
{
    data_s[6] = (ang-0)*(v_max-v_min)/(180-0)+v_min;
}
#endif

#ifdef SERVO_8_PIN
void Servo_8_Write(unsigned char ang)
{
    data_s[7] = (ang-0)*(v_max-v_min)/(180-0)+v_min;
}
#endif

#ifdef SERVO_HARDWARE
void __interrupt() TMR0_ISR(void)
{
        
    if(INTCONbits.TMR0IF  == 1)
    {   
        //PIE1bits.TMR1IE=1;
        cont_sr = cont_sr + 1;
        if(cont_sr > 490){
            cont_sr = 0;
        }
        #ifdef SERVO_1_PIN
        if(cont_sr < data_s[0]){SERVO_1_PIN = 1;} else{SERVO_1_PIN = 0;}
        #endif

        TMR0 = 50;
        INTCONbits.TMR0IF = 0;
    }
}
#endif

