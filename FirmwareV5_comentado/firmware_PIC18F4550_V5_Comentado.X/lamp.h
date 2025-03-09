

/* Defini��es de pinos */
#define pino_lamp PORTBbits.RB3       // Porta f�sica do rel�
#define pino_lamp_TRIS TRISBbits.TRISB3 // Registrador de dire��o do pino

/* Prot�tipos de fun��es */
void acionaLamp(void);  // Liga a l�mpada
void desligaLamp(void); // Desliga a l�mpada

// L�gica invertida devido ao uso de rel� NC (Normalmente Fechado)
void acionaLamp(void){    
    pino_lamp = 0;        // Energiza o rel� -> abre o circuito (desliga)
    __delay_ms(200);      // delay 200 ms
}

void desligaLamp(void){    
    pino_lamp = 1;        // Desenergiza o rel� -> fecha o circuito (liga)
    __delay_ms(200);      // delay 200 ms
}
 
 