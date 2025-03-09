/* 
 * File:   dht11.h
 * Author: vitor
 *
 * Created on 15 de Janeiro de 2025, 15:56
 */

// Defini��es de pinos para o sensor DHT11
#define DHT11_PIN_PORT 	PORTBbits.RB2  // Porta de leitura do pino de dados
#define DHT11_PIN_LAT 	LATBbits.LB2    // Latch para controle de sa�da
#define DHT11_PIN_DIR 	TRISBbits.TRISB2 // Registrador de dire��o (input/output)

// Prot�tipos de fun��es
void DHT11_Start(void);          // Inicia a comunica��o com o sensor
void DHT11_Response(void);       // Aguarda resposta do sensor
int DHT11_Read_Byte(void);       // L� um byte de dados
short DHT11_Read_Data(float *temp, float *hum); // Fun��o principal de leitura
unsigned DHT11_Join_Data(unsigned h, unsigned l); // Processa dados inteiros e decimais

// Fun��o de inicializa��o da comunica��o
void DHT11_Start(void)
{
    DHT11_PIN_DIR = 0;           // Configura pino como sa�da
    DHT11_PIN_LAT = 0;           // Puxa o pino para LOW
    __delay_ms(20);              // Delay de 18ms m�nimo requerido pelo protocolo
    DHT11_PIN_LAT = 1;           // Libera o pino (HIGH)
    __delay_us(30);              // Espera 20-40us conforme especifica��o
    DHT11_PIN_DIR = 1;           // Muda pino para entrada
}

// Fun��o que verifica a resposta do sensor
void DHT11_Response(void)
{
    while(DHT11_PIN_PORT == 1);  // Aguarda sensor puxar o pino LOW
    while(DHT11_PIN_PORT == 0);  // Aguarda sinal LOW inicial (80us)
    while(DHT11_PIN_PORT == 1);  // Aguarda sinal HIGH (80us)
}

// Fun��o de leitura de um byte (8 bits)
int DHT11_Read_Byte(void)
{
    int i,data = 0;
    for(i=0;i<8;i++){
        while((DHT11_PIN_PORT) == 0); // Aguarda pulso LOW de prepara��o
        __delay_us(30);           // Atraso cr�tico para distinguir 0 e 1
        if((DHT11_PIN_PORT) == 1){ // Se ainda HIGH ap�s 30us, � bit 1
            data = ((data<<1) | 1); // Desloca e adiciona 1
        }else{                    // Sen�o, � bit 0
            data = (data<<1);     // Apenas desloca
        }
        while((DHT11_PIN_PORT) == 1); // Aguarda final do bit
    }
    return data;
}

// Fun��o principal de leitura dos dados
short DHT11_Read_Data(float *tem, float *hum)
{
    int temp = 0;
    int info[5];                // Array para armazenar os 5 bytes recebidos
    
    DHT11_Start();              // Inicia comunica��o
    DHT11_Response();           // Verifica resposta
    
    // Leitura dos 5 bytes de dados:
    info[0] = DHT11_Read_Byte();   // Umidade inteira
    info[1] = DHT11_Read_Byte();   // Umidade decimal (n�o usado no DHT11)
    info[2] = DHT11_Read_Byte();   // Temperatura inteira
    info[3] = DHT11_Read_Byte();   // Temperatura decimal (n�o usado no DHT11)
    info[4] = DHT11_Read_Byte();   // Checksum
    
    // Processamento dos dados:
    *hum = (float)((DHT11_Join_Data(info[0], info[1])) / 10.0f); // Combina e converte umidade
    *tem = (float)((DHT11_Join_Data(info[2], info[3])) / 10.0f); // Combina e converte temperatura
    
    temp = info[0] + info[1] + info[2] + info[3]; // C�lculo do checksum
    if(temp == info[4]){        // Verifica��o de integridade
        return 1;               // Dados v�lidos
    }else{
        return 0;               // Erro de checksum
   }
}

// Fun��o auxiliar para combinar partes inteira e decimal
unsigned DHT11_Join_Data(unsigned h, unsigned l)
{
    unsigned pow = 10;
    while(l >= pow)             // Ajusta a pot�ncia para combinar os valores
        pow *= 10;
    return h * pow + l;         // Retorna valor combinado (ex: 24 + 5 = 245)
}