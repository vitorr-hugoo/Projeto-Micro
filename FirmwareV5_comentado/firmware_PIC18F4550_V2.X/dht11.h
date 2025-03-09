/* 
 * File:   dht11.h
 * Author: vitor
 *
 * Created on 15 de Janeiro de 2025, 15:56
 */

#include <xc.h>
#define _XTAL_FREQ 48000000

#define DHT11_PIN_PORT 	PORTBbits.RB2
#define DHT11_PIN_LAT 	LATBbits.LB2
#define DHT11_PIN_DIR 	TRISBbits.TRISB2

void DHT11_Start(void);
void DHT11_Response(void);
int DHT11_Read_Byte(void);
short DHT11_Read_Data(float *temp, float *hum);
unsigned DHT11_Join_Data(unsigned h, unsigned l);

void DHT11_Start(void)
{
    DHT11_PIN_DIR = 0;
    DHT11_PIN_LAT = 0;
    __delay_ms(20);
    DHT11_PIN_LAT = 1;
    __delay_us(30);
    DHT11_PIN_DIR = 1;
}

void DHT11_Response(void)
{
    while(DHT11_PIN_PORT == 1);
    while(DHT11_PIN_PORT == 0);
    while(DHT11_PIN_PORT == 1);
}

int DHT11_Read_Byte(void)
{
    int i,data = 0;
    for(i=0;i<8;i++){
        while((DHT11_PIN_PORT) == 0);
        __delay_us(30);
        if((DHT11_PIN_PORT) == 1){
            data = ((data<<1) | 1);
        }else{
            data = (data<<1);
        }
        while((DHT11_PIN_PORT) == 1);
    }
    return data;
}

short DHT11_Read_Data(float *tem, float *hum)
{
    int temp = 0;
    int info[5];
    DHT11_Start();
    DHT11_Response();
    info[0] = DHT11_Read_Byte();   // Humedad entero
    info[1] = DHT11_Read_Byte();   // Humedad decimal
    info[2] = DHT11_Read_Byte();   // Temp entero
    info[3] = DHT11_Read_Byte();   // Temp decimal
    info[4] = DHT11_Read_Byte();   // Paridad
    *hum = (float)((DHT11_Join_Data(info[0], info[1])) / 10.0f);
    *tem = (float)((DHT11_Join_Data(info[2], info[3])) / 10.0f);
    temp = info[0] + info[1] + info[2] + info[3];
    if(temp == info[4]){
        return 1;
    }else{
        return 0;
   }
}

unsigned DHT11_Join_Data(unsigned h, unsigned l)
{
    unsigned pow = 10;
    while(l >= pow)
        pow *= 10;
    return h * pow + l;        
}