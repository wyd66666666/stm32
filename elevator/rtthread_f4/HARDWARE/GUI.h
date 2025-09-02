#ifndef __GUI_H
#define __GUI_H
#include "stm32f4xx_hal.h"

uint32_t oled_pow(uint8_t m,uint8_t n);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode);
void GUI_DrawFont16(uint16_t x, uint16_t y, uint8_t *s,uint8_t mode);
void Show_Str(uint16_t x, uint16_t y,uint8_t *str,uint8_t size,uint8_t mode);

void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t BMP[]);
void Disp_Title(void);

#endif
