/* 
 * File:   cooler.h
 * Author: vitor
 *
 * Created on 31 de Janeiro de 2025, 16:49
 */

#define pino_cooler PORTBbits.RB4
#define pino_cooler_TRIS TRISBbits.TRISB4

#include <xc.h>

void acionaCooler(void);
void desligaCooler(void);

void acionaCooler(void){
    pino_cooler_TRIS = 0;
    pino_cooler = 0;
    __delay_ms(1000);
    
}

void desligaCooler(void){
    pino_cooler = 1;
    __delay_ms(1000);
}


