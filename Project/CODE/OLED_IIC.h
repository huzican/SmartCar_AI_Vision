#ifndef _OLED_IIC_h
#define _OLED_IIC_h

#define high 1
#define low 0

#define	Brightness_1	0xCF
#define X_WIDTH 	128
#define Y_WIDTH 	64

extern unsigned char BMP0[];

void delay();
void IIC_Start();
void IIC_Stop();
void Write_IIC_Byte(unsigned char IIC_Byte);
void OLED_Wrdat(unsigned char IIC_Data);
void OLED_Set_POS(unsigned char x, unsigned char y);
void OLED_fill(unsigned char bmp_dat);
void OLED_CLS(void);
void OLED_Init(void);
void OLED_P6x8Str_Y(unsigned char x, unsigned char y,unsigned char *ch);
void Draw_BMP_Y(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char  y1,unsigned char BMP[]);

void LED_Cursor(uint8 cursor_column, uint8 cursor_row);
void LED_PrintValueF(uint8 x, uint8 y, float data, uint8 num);
void LED_PrintValueFP(uint8 x, uint8 y, uint16 data, uint8 num);
void LED_PrintValueI5(uint8 x, uint8 y, int data);
void LED_PrintsignValueF4(uint8 x, uint8 y, float data);
void LED_PrintValueI4(uint8 x, uint8 y, int data);
void LED_PrintValueI3(uint8 x, uint8 y, int data);
void LED_PrintsignValueI4(uint8 x, uint8 y, int data);
void LED_PrintValueI2(uint8 x, uint8 y, int data);
void LED_PrintValueI(uint8 x, uint8 y, int data);
void LED_PrintValueC(uint8 x, uint8 y, char data);
void LED_PrintBMP(uint8 x0,uint8 y0,uint8 x1,uint8 y1,uint8 bmp[]);
void LED_P6x8Char(uint8 x,uint8 y,uint8 ch);
#endif
