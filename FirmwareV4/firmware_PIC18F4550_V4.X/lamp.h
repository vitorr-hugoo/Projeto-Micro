

#define pino_lamp PORTBbits.RB3       // Porta f�sica do rel�
#define pino_lamp_TRIS TRISBbits.TRISB3 // Registrador de dire��o do pino

#include <xc.h>

void acionaLamp(void);
void desligaLamp(void);

//Logica da Lampada � invertida -> no rele, NC 

void acionaLamp(void){
    
    pino_lamp=0;
    __delay_ms(200);
    
}

void desligaLamp(void){
    
    pino_lamp=1;
    __delay_ms(200);
    
}
 
 