/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "lcd.h"

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Board_LED.h"                  // ::Board Support:LED
#include "adc.h"                  // ::Board Support:A/D Converter
#include "RTC.h"
#include "sntp.h"
#include "stm32f4xx_it.h"



#include "Board_Buttons.h"              // ::Board Support:Buttons

//#include "Board_ADC.h"                  // ::Board Support:A/D Converter
//#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
//#include "GLCD_Config.h"                // Keil.MCBSTM32F400::Board Support:Graphic LCD

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

//extern GLCD_FONT GLCD_Font_6x8;
//extern GLCD_FONT GLCD_Font_16x24;

extern uint16_t AD_in          (uint32_t ch1);
//extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);
void adc_in (void *argument); 

extern bool LEDrun;
extern char lcd_text[2][20+1];

extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;

bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };


uint8_t aShowTime[16] = {0};
uint8_t aShowDate[16] = {0}; 

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;
osThreadId_t TID_RTC;
osThreadId_t TID_Alarma;
osThreadId_t TID_Pulsador;
										

//TIMER
osTimerId_t id_tim_1s;
osTimerId_t id_tim_6s;
osTimerId_t id_tim_3m;
uint16_t timer_1seg=0;

/* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);
static void ActualizacionHora (void *arg);
static void AlarmaMinuto (void *arg);
static void Pulsador(void *arg);


//CALLBACKS TIMERS
static void SetTimers (void);
void Timer_Callback_1s (void);
void Timer_Callback_6s (void);
void Timer_Callback_3m (void);


__NO_RETURN void app_main (void *arg);

/* Read analog inputs */
uint16_t AD_in (uint32_t ch1) {
 
	static float raw = 0;
  static uint16_t value;
	ADC_HandleTypeDef adchandle;
	
	
  if (ch1 == 0) {
			ADC_Init_Single_Conversion(&adchandle, ADC1);
  }
	raw=ADC_getVoltage(&adchandle,10);
	value=(uint16_t)raw;
  return (uint16_t)(raw*1250);
}

///* Read digital inputs */
//uint8_t get_button (void) {
//  return ((uint8_t)Buttons_GetState ());
//}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
   osThreadFlagsSet (TID_Display, 0x01);
  }
}

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Display (void *arg) {

  LCD_reset();
	LCD_init();
	clean_buffer();
	
	uint32_t flagLCD = 0x00;
  while(1) {
    /* Wait for signal from DHCP */
    flagLCD= osThreadFlagsWait (0x50, osFlagsWaitAll, osWaitForever); 
	 if (flagLCD== 0x50){
		write_LCD(lcd_text[0],1,true);
    write_LCD(lcd_text[1],2,false);
	 }
	 LCD_update();
  }
}

/*----------------------------------------------------------------------------
  Thread 'RTC': RTC handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void ActualizacionHora (void *arg) {
	
	//osTimerStart(id_tim_6s, 6000);

  while(1) {
		RTC_Show(aShowTime, aShowDate);
		
		
		memcpy(lcd_text[0], aShowTime, sizeof(aShowTime));
    lcd_text[0][sizeof(aShowTime)] = '\0';
    
    memcpy(lcd_text[1], aShowDate, sizeof(aShowDate));
    lcd_text[1][sizeof(aShowDate)] = '\0';
    
    
    //Envía al hilo del LCD para escribir
    osThreadFlagsSet (TID_Display, 0x50); 
	
		osDelay(250);//para no estar representando infinitamente, el lcd se ve mal sino
		
  }
}
/*----------------------------------------------------------------------------
  Thread 'ALARMA': RTC handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void AlarmaMinuto (void *arg) {
	
	uint8_t FlagAlarma= 0x0U;
//SI NO PONES LA INTERRUPCION DE RTC JAMAS SALTA EL LED
  while(1) {
		FlagAlarma= osThreadFlagsWait(0x01U, osFlagsWaitAll,osWaitForever);
		
		if(FlagAlarma== 0x01){
			RTC_hora(0x00,0x00,0x00);
			RTC_fecha(0x01,0x01,0x01,0x00);
			for(int CincoSegundos=0; CincoSegundos<10; CincoSegundos++){
			    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
          osDelay(500);
			}
		
		}
		
  }
}
/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) {
  const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
  uint32_t cnt = 0U;

  (void)arg;

  LEDrun = true;
  while(1) {
    /* Every 100 ms */
    if (LEDrun == true) {
      LED_SetOut (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
  }
}

/*----------------------------------------------------------------------------
  Thread 'pulsador': Funcionalidad de pulsador incluida
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Pulsador (void *arg) {
	
	(void)arg;
	uint8_t flagPulsador= 0x0U;
	
  while(1) {
		flagPulsador= osThreadFlagsWait(0x01U, osFlagsWaitAll,osWaitForever);
		
		if(flagPulsador==0x01){
		  RTC_hora(0,0,0);
			RTC_fecha(1,1,0,0);
			for(int i = 0; i < 10; i++){
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);//Led azul, no lo piden
        osDelay(100);
      }
		}
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;

  LED_Initialize();
	
  netInitialize ();

  //Buttons_Initialize();
  ADC1_pins_F429ZI_config ();
	
	//inicializacion RTC
  RTC_Init();
	
	//Alarma(); ACTIVAR PARA ALARMA DE LED
	
	//Lanzar timers
	  SetTimers();
	osTimerStart(id_tim_6s, 6000);
	//boton
	init_Boton();

  //TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);
	TID_RTC			= osThreadNew (ActualizacionHora,  NULL, NULL);
  TID_Alarma	 = osThreadNew (AlarmaMinuto,  NULL, NULL);
	TID_Pulsador = osThreadNew (Pulsador,  NULL, NULL);
	osThreadExit();
}


//MANEJO TIMERS

static void SetTimers (void){ 
	id_tim_1s= osTimerNew((osTimerFunc_t)Timer_Callback_1s, osTimerPeriodic, NULL,NULL);
  id_tim_6s= osTimerNew((osTimerFunc_t)Timer_Callback_6s, osTimerOnce, NULL,NULL);
	id_tim_3m= osTimerNew((osTimerFunc_t)Timer_Callback_3m, osTimerPeriodic, NULL,NULL);
	

	
}
//manejo de lo quiero que haga durante el tiempo que dure el timer
//en este caso cada vez que se actualice la hora, 
//que obviamnete sera cada 1seg debe hacer el led rojo toggle durante 2 seg
void Timer_Callback_1s (void){
 
 if(timer_1seg<40){//4 segundos porque 40veces 100ms
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);//Led red
	   timer_1seg++;
 }else{
   osTimerStop(id_tim_1s);
	 timer_1seg=0;
 }
}

void Timer_Callback_6s (void){
	//parpadeo 100ms 5Hz
	osTimerStart(id_tim_1s, 100);
	Init_sntp(); //reinicio por primera vez del SNTP como dice el enunciado (re-configuracion)
	
	//3mins=3*60*1000=180.000ms
	osTimerStart(id_tim_3m,180000); //18000ms
}


void Timer_Callback_3m (void){
	timer_1seg = 0; // Reset contador por si acaso
	osTimerStart(id_tim_1s, 100);
	Init_sntp(); //re sincronizacion a los 3 minutos
	
}
