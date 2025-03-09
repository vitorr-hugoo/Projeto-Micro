/* 
 * File:   cooler.h
 * Author: vitor
 *
 * Created on 31 de Janeiro de 2025, 16:49
 */

/* Definições de pinos */
#define pino_cooler PORTBbits.RB4       // Porta física do cooler
#define pino_cooler_TRIS TRISBbits.TRISB4 // Registrador de direção do pino

/* Protótipos de funções */
void acionaCooler(void);  // Liga o cooler
void desligaCooler(void); // Desliga o cooler

/* Implementação das funções
 * Lógica Inversa
 */
void acionaCooler(void) {
    pino_cooler_TRIS = 0;   // Configura pino como saída
    pino_cooler = 0;        // Coloca pino em LOW (liga o cooler)
    __delay_ms(200);        // Espera 200ms 
}

void desligaCooler(void) {
    pino_cooler = 1;        // Coloca pino em HIGH (desliga o cooler)
    __delay_ms(200);        // Espera 200ms 
}


