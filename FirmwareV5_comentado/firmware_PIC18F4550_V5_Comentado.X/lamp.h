

/* Definições de pinos */
#define pino_lamp PORTBbits.RB3       // Porta física do relé
#define pino_lamp_TRIS TRISBbits.TRISB3 // Registrador de direção do pino

/* Protótipos de funções */
void acionaLamp(void);  // Liga a lâmpada
void desligaLamp(void); // Desliga a lâmpada

// Lógica invertida devido ao uso de relé NC (Normalmente Fechado)
void acionaLamp(void){    
    pino_lamp = 0;        // Energiza o relé -> abre o circuito (desliga)
    __delay_ms(200);      // delay 200 ms
}

void desligaLamp(void){    
    pino_lamp = 1;        // Desenergiza o relé -> fecha o circuito (liga)
    __delay_ms(200);      // delay 200 ms
}
 
 