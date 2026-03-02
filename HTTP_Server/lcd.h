#ifndef __LCD_H
#define __LCD_H


//includes
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h" 
#include "Driver_SPI.h"
//#include "stm32f4xx_hal_tim.h"

extern unsigned char buffer[512];

//extern osThreadId_t tid_LCD;
//extern osMessageQueueId_t id_MsgQueueLCD;
//osMessageQueueId_t getQueueIdLCD(void);
//void LCD_update(void);

//typedef struct{
//	char cadena[40];
//	int linea;
//	bool reset;
//}MSGQUEUELCD_OBJ_t;

//funciones
//int Init_LCD (void) ;
extern void LCD_reset(void);
extern void LCD_wr_data(unsigned char data);
extern void LCD_wr_cmd(unsigned char cmd);
extern void LCD_init(void);
extern void LCD_update(void);
extern void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
extern void LCD_symbolToLocalBuffer_L2(uint8_t symbol);
extern void LCD_update_L1(void);
extern void LCD_update_L2(void);
extern void symbolToLocalBuffer(uint8_t line,uint8_t symbol, bool reset);
extern void write_LCD(char cadena[], int linea, bool reset);
extern void clean_buffer(void);
 





#endif
