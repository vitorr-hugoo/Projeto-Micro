

#define pino_lamp PORTBbits.RB3       // Porta física do relé
#define pino_lamp_TRIS TRISBbits.TRISB3 // Registrador de direção do pino

#include <xc.h>

void acionaLamp(void);
void desligaLamp(void);

//Logica da Lampada é invertida -> no rele, NC 

void acionaLamp(void){
    
    pino_lamp=0;
    __delay_ms(200);
    
}

void desligaLamp(void){
    
    pino_lamp=1;
    __delay_ms(200);
    
}
 
 