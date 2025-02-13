

#define pino_lamp PORTBbits.RB3
#define pino_lamp_TRIS TRISBbits.TRISB3

#include <xc.h>

void acionaLamp(void);
void desligaLamp(void);

//Logica da Lampada é invertida -> no rele, NC 

void acionaLamp(void){
    
    pino_lamp=0;
    __delay_ms(1000);
    
}

void desligaLamp(void){
    
    pino_lamp=1;
    __delay_ms(1000);
    
}
 
 