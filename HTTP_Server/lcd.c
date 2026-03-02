
//#include "stm32f4xx_hal_tim.h"
#include "lcd.h"
#include "Arial12x12.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


#define TRANSFERDONESPI 0x01U


extern osThreadId_t TID_Display;


//osMessageQueueId_t id_MsgQueueLCD;   //id de la cola

 //buffer para escribir en LCD
unsigned char buffer[512];


//estructura GPIO para los pines 
GPIO_InitTypeDef GPIO_InitStruct;

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv= &Driver_SPI1;



//Funciones LCD
void LCD_reset(void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void LCD_init(void);
void LCD_update(void);
void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
void LCD_symbolToLocalBuffer_L2(uint8_t symbol);
void LCD_update_L1(void);
void LCD_update_L2(void);
void symbolToLocalBuffer(uint8_t line,uint8_t symbol, bool reset);
void write_LCD(char cadena[], int linea, bool reset);
void clean_buffer(void);
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
//void LCD (void *argument);                   // thread function
//osThreadId_t tid_LCD;                        // thread id




//int Init_LCD (void) {
//	
//	id_MsgQueueLCD=osMessageQueueNew(16,sizeof(MSGQUEUELCD_OBJ_t),NULL);
//  tid_LCD = osThreadNew(LCD, NULL, NULL);
//	
//  if (tid_LCD == NULL) {
//    return(-1);
//  }
// 
//  return(0);
//}
// 
//void LCD (void *argument) {
//	
//	MSGQUEUELCD_OBJ_t infoLCD;

//	LCD_reset();
//	LCD_init();
//	clean_buffer();
//	
//  while (1) {
//		 osMessageQueueGet(id_MsgQueueLCD,&infoLCD, NULL,osWaitForever);
//		 write_LCD(infoLCD.cadena, infoLCD.linea, infoLCD.reset);


//	}
//}



void clean_buffer(void){//Sirve para limpiar el buffer
	int i=0;
	
	for(i=0;i<512;i++){
		buffer[i]=0x00;
	}
	
	LCD_update();
}

//PRACTICA LCD
void write_LCD(char cadena[], int linea, bool reset){
	int j=0;
	
	if(reset){
		clean_buffer();
	}
	
	for (j=0; j < strlen(cadena);j++){
		symbolToLocalBuffer(linea,cadena[j], reset);
		reset=false;
	}
	
	if(linea==1)
		LCD_update_L1();
	else
		LCD_update_L2();
}


void symbolToLocalBuffer(uint8_t line,uint8_t symbol, bool reset){
	uint8_t  i, value1, value2;
	uint16_t offset=0;
	static uint16_t positionL1=0;//Pagina 0
	static uint16_t positionL2=256;//Pagina 2
	uint16_t position=0;
	
	if(reset){
		positionL1=0;
		positionL2=256;
	}
	
	position=(line == 1) ? positionL1 : positionL2;//Sustituye al if else
	
	//	if(line == 1)
	//		position=positionL1;
	//	else
	//		position=positionL1;
	
	offset=25*(symbol - ' ');
	
	for(i=0;i<12;i++){
		value1=Arial12x12[offset+i*2+1];
		value2=Arial12x12[offset+i*2+2];
		
		buffer[i+position]=value1;
		buffer[i+128+position]=value2;
	}
	
	if(line==1)
		positionL1=positionL1+Arial12x12[offset];
	else
		positionL2=positionL2+Arial12x12[offset];
	
}


void LCD_symbolToLocalBuffer_L1(uint8_t symbol){
	uint8_t  i, value1, value2;
	uint16_t offset=0;
	static uint16_t positionL1=0;
	
	offset=25*(symbol - ' ');
	
	for(i=0;i<12;i++){
		value1=Arial12x12[offset+i*2+1];
		value2=Arial12x12[offset+i*2+2];
		
		buffer[i+positionL1]=value1;
		buffer[i+128+positionL1]=value2;
	}
	
	positionL1=positionL1+Arial12x12[offset];
}	

void LCD_symbolToLocalBuffer_L2(uint8_t symbol){
	uint8_t  i, value1, value2;
	uint16_t offset=0;
	static uint16_t positionL2=0;
	
	offset=25*(symbol - ' ');
	
	for(i=0;i<12;i++){
		value1=Arial12x12[offset+i*2+1];
		value2=Arial12x12[offset+i*2+2];
		
		buffer[i+256+positionL2]=value1;//Define posici n en la pagina 2
		buffer[i+384+positionL2]=value2;//Define posici n en la pagina 3
	}
	
	positionL2=positionL2+Arial12x12[offset];
}	

void LCD_update_L1(void)
{
 int i;
 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci n a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci n a 0
 LCD_wr_cmd(0xB0); // P gina 0

 for(i=0;i<128;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci n a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci n a 0
 LCD_wr_cmd(0xB1); // P gina 1

 for(i=128;i<256;i++){
 LCD_wr_data(buffer[i]);
 }
}


void LCD_update_L2(void)
{
 int i;
	
 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB2); //P gina 2
	
 for(i=256;i<384;i++){
	LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB3); // Pagina 3


 for(i=384;i<512;i++){
	LCD_wr_data(buffer[i]);
 }
}

void LCD_update(void)
{
 int i;
 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci n a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci n a 0
 LCD_wr_cmd(0xB0); // P gina 0

 for(i=0;i<128;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci n a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci n a 0
 LCD_wr_cmd(0xB1); // P gina 1

 for(i=128;i<256;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB2); //P gina 2
 for(i=256;i<384;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB3); // Pagina 3


 for(i=384;i<512;i++){
 LCD_wr_data(buffer[i]);
 }
}

void LCD_init(void){
	LCD_wr_cmd(0xAE);//Display off
	LCD_wr_cmd(0xA2);//Fija el valor de la relaci n de la tensi n de polarizaci n del LCD a 1/9 
	LCD_wr_cmd(0xA0);//El direccionamiento de la RAM de datos del display es la normal
	LCD_wr_cmd(0xC8);//El scan en las salidas COM es el normal
	LCD_wr_cmd(0x22);//Fija la relaci n de resistencias interna a 2
	LCD_wr_cmd(0x2F);//Power on
	LCD_wr_cmd(0x40);//Display empieza en la linea 0
	LCD_wr_cmd(0xAF);//Display ON
	LCD_wr_cmd(0x81);//Contraste
	LCD_wr_cmd(0x15);//Valor Contraste (debe determinarlo y configurarlo a su gusto)
	LCD_wr_cmd(0xA4);//Display all points normal
	LCD_wr_cmd(0xA6);//LCD Display normal / A7 reversa
}

void LCD_reset(void){
	/* Initialize the SPI driver */
	SPIdrv->Initialize(NULL);
	/* Power up the SPI peripheral */
	SPIdrv->PowerControl(ARM_POWER_FULL);
	/* Configure the SPI to Master, 8-bit mode @10000 kBits/sec */
	SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);
	
	__HAL_RCC_GPIOA_CLK_ENABLE();	
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	
	GPIO_InitStruct.Pin=GPIO_PIN_6;//PA6-->RST
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin=GPIO_PIN_13;//PF13-->A0
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	GPIO_InitStruct.Pin=GPIO_PIN_14;//PD14-->CS
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);
	
	
	
	//SENAL DE RESET
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
	HAL_Delay(10) ;
}

void LCD_wr_data(unsigned char data){
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_SET);
	
	SPIdrv->Send(&data,sizeof(data));
	
	while(SPIdrv->GetStatus().busy==1){};
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);
	
}

void LCD_wr_cmd(unsigned char cmd){
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_RESET);
	
	SPIdrv->Send(&cmd,sizeof(cmd));
	
	while(SPIdrv->GetStatus().busy==1){};
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);
}

//osMessageQueueId_t getQueueIdLCD(void){
//	return id_MsgQueueLCD;
//}

