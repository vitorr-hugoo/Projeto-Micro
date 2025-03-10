#include <xc.h>
#define _XTAL_FREQ 48000000

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "i2c.h"

extern const uint8_t FONTE_DATA[];
extern const uint8_t FONT_1[];
extern const uint8_t FONT_2[];
extern const uint8_t FONT_3[];

typedef struct _Current_Font_s
{
    const uint8_t *font;        // Fonts Stored are Const
    uint8_t x_size;
    uint8_t y_size;
    uint8_t offset;
    uint8_t numchars;
    uint8_t inverted;
}Current_Font_s;

#define LEFT                  0
#define RIGHT                 254
#define CENTER                255
#define BLACK                 0
#define WHITE                 1
#define INVERSE               2

//#define SSD1306_ADDR        0x3D
#define SSD1306_ADDR          0x3C

#define SSD1306_128_64
// #define SSD1306_128_32
// #define SSD1306_96_16

#if defined SSD1306_128_64
  #define SSD1306_LCDWIDTH                  128
  #define SSD1306_LCDHEIGHT                 64
#endif
#if defined SSD1306_128_32
  #define SSD1306_LCDWIDTH                  128
  #define SSD1306_LCDHEIGHT                 32
#endif
#if defined SSD1306_96_16
  #define SSD1306_LCDWIDTH                  96
  #define SSD1306_LCDHEIGHT                 16
#endif

#define SSD1306_COMMAND                     0x00
#define SSD1306_DATA                        0xC0
#define SSD1306_DATA_CONTINUE               0x40

// Fundamental Commands
#define SSD1306_SET_CONTRAST_CONTROL                    0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME                   0xA4
#define SSD1306_DISPLAY_ALL_ON                          0xA5
#define SSD1306_NORMAL_DISPLAY                          0xA6
#define SSD1306_INVERT_DISPLAY                          0xA7
#define SSD1306_DISPLAY_OFF                             0xAE
#define SSD1306_DISPLAY_ON                              0xAF
#define SSD1306_NOP                                     0xE3
// Scrolling Commands
#define SSD1306_HORIZONTAL_SCROLL_RIGHT                 0x26
#define SSD1306_HORIZONTAL_SCROLL_LEFT                  0x27
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_RIGHT    0x29
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_LEFT     0x2A
#define SSD1306_DEACTIVATE_SCROLL                       0x2E
#define SSD1306_ACTIVATE_SCROLL                         0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
// Addressing Setting Commands
#define SSD1306_SET_LOWER_COLUMN                        0x00
#define SSD1306_SET_HIGHER_COLUMN                       0x10
#define SSD1306_MEMORY_ADDR_MODE                        0x20
#define SSD1306_SET_COLUMN_ADDR                         0x21
#define SSD1306_SET_PAGE_ADDR                           0x22
// Hardware Configuration Commands
#define SSD1306_SET_START_LINE                          0x40
#define SSD1306_SET_SEGMENT_REMAP                       0xA0
#define SSD1306_SET_MULTIPLEX_RATIO                     0xA8
#define SSD1306_COM_SCAN_DIR_INC                        0xC0
#define SSD1306_COM_SCAN_DIR_DEC                        0xC8
#define SSD1306_SET_DISPLAY_OFFSET                      0xD3
#define SSD1306_SET_COM_PINS                            0xDA
#define SSD1306_CHARGE_PUMP                             0x8D
// Timing & Driving Scheme Setting Commands
#define SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO             0xD5
#define SSD1306_SET_PRECHARGE_PERIOD                    0xD9
#define SSD1306_SET_VCOM_DESELECT                       0xDB

// Prot�tipos de fun��es para controle de display OLED
void OLED_Init(void);              // Inicializa o display OLED (comunica��o e configura��es b�sicas)
uint8_t OLED_Width(void);          // Retorna a largura �til do display em pixels
uint8_t OLED_Height(void);         // Retorna a altura �til do display em pixels
void OLED_Update(void);            // Atualiza o display com o conte�do do buffer gr�fico
void OLED_SetContrast(uint8_t contrast); // Ajusta o contraste do display (0-255)
void OLED_ClearDisplay(void);      // Limpa todo o conte�do do display (preenche com espa�o vazio)
void OLED_FillDisplay(void);       // Preenche todo o display com pixels ativos (cor s�lida)
void OLED_DrawPixel(int16_t x, int16_t y, uint8_t color); // Desenha um pixel nas coordenadas (x,y)
void OLED_InvertDisplay(uint8_t value);   // Inverte as cores do display (0 = normal, 1 = invertido)
void OLED_SetFont(const uint8_t *font);   // Define a fonte de texto a ser utilizada (tipo e tamanho)
void OLED_InvertFont(uint8_t invert_status); // Inverte cores da fonte (0 = normal, 1 = negativo)
void OLED_Line(int16_t x_start, int16_t y_start, int16_t x_end, int16_t y_end, uint8_t color); // Desenha linha entre dois pontos
void OLED_V_Line(int16_t y_start, int16_t y_end, int16_t x_pos, uint8_t color); // Linha vertical
void OLED_H_Line(int16_t x_start, int16_t x_end, int16_t y_pos, uint8_t color); // Linha horizontal
void OLED_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color); // Desenha ret�ngulo vazio
void OLED_FillRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color); // Desenha ret�ngulo preenchido
void OLED_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color); // Desenha tri�ngulo
void OLED_Circle(int16_t x_center, int16_t y_center, int16_t radius, uint8_t color); // Desenha c�rculo vazio
void OLED_Image(const uint8_t *image);    // Exibe uma imagem pr�-definida no display (bitmap)
void OLED_Write(int16_t x, int16_t y, char value); // Escreve um �nico caractere na posi��o (x,y)
void OLED_Write_Text(int16_t x, int16_t y, char *text); // Escreve uma string de texto na posi��o (x,y)