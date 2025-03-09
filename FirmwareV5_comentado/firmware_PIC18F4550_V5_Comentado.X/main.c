/*
 * File:   main.c
 * Author: vitor hugo pavanelli dos reis
 *
 * Created on 31 de Janeiro de 2025, 10:02
 */


// Define a frequência do sistema para uso nas funções de delay
#define _XTAL_FREQ 48000000      // Clock de 48MHz (compatível com operação USB)

// Configurações de clock e divisores de frequência
#pragma config PLLDIV = 2       // Divide o clock de entrada por 2 antes do PLL (ex: 8MHz -> 4MHz)
#pragma config CPUDIV = OSC1_PLL2 // Clock da CPU = (Clock do PLL)/2 (ex: 96MHz PLL -> 48MHz CPU)
#pragma config USBDIV = 2        // Clock USB = (Clock do PLL)/2 (48MHz necessário para USB)

// Configurações do oscilador
#pragma config FOSC = HSPLL_HS   // Usa oscilador HS externo com PLL habilitado
#pragma config FCMEN = OFF       // Desabilita monitor de fail-safe clock
#pragma config IESO = OFF        // Desabilita troca automática entre clock interno/externo

// Gerenciamento de energia
#pragma config PWRT = OFF        // Desabilita timer de power-up
#pragma config BOR = OFF         // Desabilita Brown-Out Reset
#pragma config BORV = 3          // Nível de voltagem para BOR (2.0V)
#pragma config VREGEN = OFF      // Desabilita regulador de voltagem USB interno

// Watchdog Timer
#pragma config WDT = OFF         // Desabilita Watchdog Timer
#pragma config WDTPS = 32768     // Prescaler do WDT (1:32768) - não afeta com WDT desligado

// Configurações de pinos e periféricos
#pragma config CCP2MX = ON       // Habilita multiplexação do CCP2 no pino RC1
#pragma config PBADEN = OFF      // Porta B digital (desabilita conversores A/D)
#pragma config LPT1OSC = OFF     // Timer1 opera em modo de alta frequência
#pragma config MCLRE = ON        // Habilita função de reset no pino MCLR

// Segurança e depuração
#pragma config STVREN = ON       // Habilita reset em overflow/underflow da pilha
#pragma config LVP = OFF         // Desabilita programação em baixa voltagem
#pragma config ICPRT = OFF       // Desabilita depuração via ICSP
#pragma config XINST = OFF       // Desabilita conjunto de instruções estendido

// Proteção de código (todas desabilitadas)
#pragma config CP0 = OFF         // Sem proteção de código na memória 0
#pragma config CP1 = OFF         // Sem proteção de código na memória 1
#pragma config CP2 = OFF         // Sem proteção de código na memória 2
#pragma config CP3 = OFF         // Sem proteção de código na memória 3
#pragma config CPB = OFF         // Sem proteção de código na área de bootloader
#pragma config CPD = OFF         // Sem proteção de dados EEPROM

// Proteção de escrita (todas desabilitadas)
#pragma config WRT0 = OFF        // Sem proteção de escrita na memória 0
#pragma config WRT1 = OFF        // Sem proteção de escrita na memória 1
#pragma config WRT2 = OFF        // Sem proteção de escrita na memória 2
#pragma config WRT3 = OFF        // Sem proteção de escrita na memória 3
#pragma config WRTC = OFF        // Sem proteção de escrita na configuração
#pragma config WRTB = OFF        // Sem proteção de escrita na área de bootloader
#pragma config WRTD = OFF        // Sem proteção de escrita na EEPROM

// Proteção contra leitura (todas desabilitadas)
#pragma config EBTR0 = OFF       // Sem proteção contra leitura na memória 0
#pragma config EBTR1 = OFF       // Sem proteção contra leitura na memória 1
#pragma config EBTR2 = OFF       // Sem proteção contra leitura na memória 2
#pragma config EBTR3 = OFF       // Sem proteção contra leitura na memória 3
#pragma config EBTRB = OFF       // Sem proteção contra leitura na área de bootloader




#include <xc.h>       // Biblioteca essencial para compilação XC8 
#include <stdio.h>    // Biblioteca padrão de entrada/saída 
#include "dht11.h"    // Biblioteca para o sensor DHT11
#include "cooler.h"   // Biblioteca para o cooler
#include "lamp.h"     // Biblioteca para a lâmpanda
#include "i2c.h"      // Biblioteca para a comunicação I2C
#include "ssd1306_oled.h" // Biblioteca para o display OLED
#include "servo.h"    // Biblioteca para o servo
#include "ds3231.h"   // Biblioteca para o RTC ds3231

//Declaração de variáveis
short dht_ok;                               // flag de vericação da resposta do DHT
float temperatura;                          // armazena valor da temperatura
float umidade;                              // armazena valor da umidade
char buffer[20];                            // buffer que converte os valores (float) em Char
uint8_t day, mth, year, dow, hr, min, sec;  // Variaveis para armazenar data e hora
uint8_t flag1_rtc = 0, flag2_rtc = 0;       // flags de verificação para controle do mecanismo do comedouro

//Protótipo das funções
void exibeData_e_Hora();                    // função que retorna data e hora do ds3231
void inicia_Sistema();                      // responsavel por incializar pinos, protocolos e dispositivos
void verificaHorario();                     // função que verifica o horario e aciona o servo quando necessario

//void exibeTemp_e_Umid();

void main()
{
    inicia_Sistema(); //chama função de inicio do sistema
    Servo_1_Write(2); // define a posição inicial do servo (referencia 0°)
    
    
    while(1)
    {   
        
        exibeData_e_Hora(); //retorna data e hora
        verificaHorario();  //verifica se o horario atual é igual ao horario pre determinado
        
        dht_ok = DHT11_Read_Data(&temperatura, &umidade); //armazena flag da resposta do DHT
        if(dht_ok == 1)                                   //verifica se a resposta do DHT é válida
                                                          //se sim, entra no condicional; se não erro no sensor
            sprintf(buffer, "Umid: %0.1f %%", umidade);   //converte dados de umidade de float p char
            OLED_SetFont(FONT_1);                         //determina fonte
            OLED_Write_Text(5, 26, buffer);               //função de escrita de texto no display -> coluna 5, linha 26
            OLED_Update();                                //função que atualiza o display
            sprintf(buffer, "Temp: %0.1f C", temperatura);//converte dados de temperatura de float p char
            OLED_SetFont(FONT_1);                         //determina fonte
            OLED_Write_Text(5, 47 , buffer);              //função de escrita de texto no display -> coluna 5, linha 47
            OLED_Update();                                //função que atualiza o display
            
            //condicional para controle da temperatura do sistema
            if(temperatura > 32){
                
                acionaCooler(); //liga o cooler via rele
                desligaLamp();  //desliga a lampada
                  
            }
            else if(temperatura <= 32 && temperatura >31){ //zona morta para evitar trocar de estado súbitas ou imprecisoes do sensor
                
                desligaCooler();//desliga o cooler 
                desligaLamp();  //desliga a lampada 
            }
                            
            else{
                desligaCooler(); //desliga o cooler 
                acionaLamp();    //liga a lampada via rele
            }
       
        __delay_ms(500); //delay de 500 ms apenas para evitar travamentos do sistema
      
    }
}

void inicia_Sistema(){
    pino_lamp_TRIS=0; //configura pino da lmapda para saida
    Servo_Init();     //Inicia o servomotor 
    I2C_Init_Master(I2C_400KHZ); // Inicia protocolo I2C
    OLED_Init(); // Inicia OLED
    OLED_SetFont(FONT_1); // Define o tamanho da fonte
    OLED_Write_Text(5, 7, "INICIALIZANDO..."); //Imprime na tela
    OLED_Update(); // Atualiza display
    
    DS3231_Set_Date_Time(10,2,25,2,13,30,0);  //define os parametros inciais de Dia,Mes,Ano,Dia da semana,Hora,Minuto,Segundo
    __delay_ms(500); // delay de 500 ms
    acionaLamp();    // aciona a lampada ao inicializar o sistema
    OLED_ClearDisplay(); // limpa o display
}

void exibeData_e_Hora(){
        
        DS3231_Get_Date(&day, &mth, &year, &dow); //função que retorna a data
        DS3231_Get_Time(&hr, &min, &sec);         //função que retorna o horario
        sprintf(buffer, "%02u/%02u/20%02u", day, mth, year); //converte a data para o formato dd/mm/aa
        OLED_SetFont(FONT_1); //define fonte
        OLED_Write_Text(1, 1, buffer); //função de escrita de texto no display -> coluna 1, linha 1
        OLED_Update(); //atualiza display
        
        sprintf(buffer, "| %02u:%02u:%02u", hr, min, sec); //converte o horario para o formato hr : min : seg
        OLED_SetFont(FONT_1); // determina a fonte
        OLED_Write_Text(65, 1, buffer); //função de escrita de texto no display -> coluna 65, linha 1
        OLED_Line(1, 12, 127, 12,1); // desenho de uma linha para separar o horario e data da temp e umid
        OLED_Update(); //atualiza display
        
        
}

void verificaHorario(){
    //horario pre definido 1: 13:30:26
    if(hr == 13 && min == 30 && (sec >25 && sec<30 ) && flag1_rtc == 0 ) {
            flag1_rtc = 1; // se entrar nessa condição, ativa essa flag
            OLED_ClearDisplay();
            Servo_1_Write(108); //função que movimenta o servo para 90° (108° obtido experimentalmente)
            
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 26, "COMIDA ACIONADA"); // exibe nos display quando aciona o servo
            OLED_Update();
            __delay_ms(2000); //delay de 2 seg para liberação da comida
            OLED_ClearDisplay();
            Servo_1_Write(2);  //volta à posição incial do servo   
        }
    
        //horario pre definido 2: 13:31:01
        if(hr == 13 && min == 31 && (sec >00 && sec<05 ) && flag2_rtc == 0 ) {
            flag2_rtc = 1;
            OLED_ClearDisplay();
            Servo_1_Write(108);
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 26, "COMIDA ACIONADA");
            OLED_Update();
            __delay_ms(2000);
            OLED_ClearDisplay();
            Servo_1_Write(2);
        }
        
        if(hr != 13 || min != 30 || sec>30) flag1_rtc = 0; //se o horario de ativação ja passou, desabilita flag para que possa ser acionado novamente  
        if(hr != 13 || min != 31 || sec>05 ) flag2_rtc = 0; 
}

/*void exibeTemp_e_Umid(){
    
    dht_ok = DHT11_Read_Data(&temperatura, &umidade);
        
        if(dht_ok == 1)

            sprintf(buffer, "Umid: %0.1f %%", umidade);
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 26, buffer);
            OLED_Update();
            sprintf(buffer, "Temp: %0.1f C", temperatura);
            OLED_SetFont(FONT_1);
            OLED_Write_Text(5, 47 , buffer);
            OLED_Update();
            if(temperatura > 27.5){
                
                acionaCooler();
                desligaLamp();
            }
            if(temperatura < 25){
                
                desligaCooler();
                acionaLamp();
            }
        }*/

