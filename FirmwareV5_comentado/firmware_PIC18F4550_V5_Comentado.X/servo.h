/* #include "servo.h"
*
* Creada por: Ing. Abiezer Hernandez O.
* Fecha de creacion: 10/01/2021
* Electronica y Circuitos
*
*/

/* Configura��es b�sicas */
#include <xc.h>                // Biblioteca do compilador XC8
#define _XTAL_FREQ 48000000    // Frequ�ncia do clock principal (48MHz)

/* Par�metros do servo (valores em unidades de timer) */
#define v_min 14               // Largura de pulso m�nima (0�)
#define v_max 60               // Largura de pulso m�xima (180�)
#define SERVO_HARDWARE         // Habilita controle por hardware

/* Defini��o dos pinos (exemplo para servo 1) */
#define SERVO_1_PIN LATDbits.LD0  // Porta f�sica do servo 1
#define SERVO_1_DIR TRISDbits.RD0  // Dire��o do pino

/* Prot�tipos de fun��es */
void Servo_Init(void);         // Inicializa��o do sistema
#ifdef SERVO_1_PIN
void Servo_1_Write(unsigned char ang); // Controle do servo 1
#endif

/* Vari�veis globais */
unsigned int cont_sr = 0;      // Contador de ciclos PWM
unsigned char data_s[8];       // Armazena tempos de pulso para at� 8 servos
unsigned int cont_rtc = 0;     // Contador auxiliar (n�o utilizado)
unsigned int flag_rtc = 0;     // Flag auxiliar (n�o utilizado)


void Servo_Init(void)
{
    /* Configura��o dos pinos como sa�da */
    #ifdef SERVO_1_PIN
    SERVO_1_DIR = 0;  // Configura pino RD0 como sa�da
    SERVO_1_PIN = 0;   // Inicia com sinal LOW
    #endif

    /* Configura��o do Timer0 */
    INTCONbits.GIE = 1;        // Habilita interrup��es globais
    INTCONbits.TMR0IE = 1;     // Habilita interrup��o do Timer0
    INTCONbits.TMR0IF = 0;     // Limpa flag de interrup��o
    T0CON = 0x50;              // Configura Timer0:
                               // - Modo 8-bit
                               // - Clock interno
                               // - Prescaler 1:2
                               // - Borda de descida
    T0CONbits.TMR0ON = 1;      // Liga Timer0
    TMR0 = 50;                 // Valor inicial do timer
}

/*********************
* Fun��o para escrever �ngulo no servo 1
*********************/
#ifdef SERVO_1_PIN
void Servo_1_Write(unsigned char ang)
{
    /* Mapeamento angular para largura de pulso */
    data_s[0] = ((ang-0)*(v_max-v_min))/(180-0)+v_min;
}
#endif

/*********************
* Rotina de servi�o de interrup��o (Timer0)
*********************/
#ifdef SERVO_HARDWARE
void __interrupt() TMR0_ISR(void)
{        
    if(INTCONbits.TMR0IF  == 1)
    {   
        cont_sr = cont_sr + 1;  // Incrementa contador de pulsos
        
        if(cont_sr > 490){       // Reset a cada ~16.74ms (per�odo total)
            cont_sr = 0;
        }
        
        /* Gera PWM para servo 1 */
        #ifdef SERVO_1_PIN
        if(cont_sr < data_s[0]){SERVO_1_PIN = 1;} 
        else{SERVO_1_PIN = 0;}
        #endif

        TMR0 = 50;               // Recarrega timer
        INTCONbits.TMR0IF = 0;   // Limpa flag de interrup��o
    }
}
#endif

