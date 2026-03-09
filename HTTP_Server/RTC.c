#include "RTC.h"


//estructura que almacena toda la informacion de configuracion y estado de RTC
RTC_HandleTypeDef RtcHandle;
RTC_TimeTypeDef RTC_Time;
RTC_DateTypeDef RTC_Date;
RTC_AlarmTypeDef alarma;

//funciones

void RTC_Show(uint8_t *showtime, uint8_t *showdate);
void Init_LSE (void);
void RTC_hora(uint8_t hh, uint8_t mm, uint8_t ss);



//inicializacion del reloj LSE para el correcto funcionamiento de RTC
void Init_LSE (void)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-2- Configure LSE as RTC clock source ###################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  //RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	
	
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
   while(1);
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    while(1);
  }
  
  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
}



//inicializaci n del RTC
void RTC_Init(void){
	
  Init_LSE();
	
	HAL_PWR_EnableBkUpAccess();/********************/
	
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  
	
	if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
		while(1);
  
  }

//	RTC_hora(0x16	,0x04,0x14);
//	RTC_fecha(0x01,0x01,0x01,0x01);
//	

	if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    Alarma();
		RTC_hora(0x16,0x04,0x14);
		RTC_fecha(0x06,0x03,0x26,0x01);

		
	
  }
  else      //  ESTO NO PERMITIA QUE SE CAMBIASE DE HORA SE QUEDABA EN EL MINUTO 59 ESTANCADO!!!!!
  {
//    /* Check if the Power On Reset flag is set */
 __HAL_RCC_CLEAR_RESET_FLAGS();
 }

}

//configuracion hora del sistema RTC
void RTC_hora(uint8_t hh, uint8_t mm, uint8_t ss){
	RTC_Time.Hours= hh;
	RTC_Time.Minutes=mm;
	RTC_Time.Seconds=ss;
	RTC_Time.TimeFormat= RTC_HOURFORMAT_24;
	RTC_Time.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
	RTC_Time.StoreOperation=RTC_STOREOPERATION_RESET;
	
	  if (HAL_RTC_SetTime(&RtcHandle, &RTC_Time, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}


//configuracion fecha del sistema RTC
//ojo tengo que poner la variable mth sino no actualiza bien la hora, ya que si le impongo el mes
//no lo va a actualizar nunca
void RTC_fecha(uint8_t day, uint8_t mth, uint8_t year, uint8_t wday){
	RTC_Date.Date= day;
	RTC_Date.Month= mth;
	RTC_Date.Year=year;
	RTC_Date.WeekDay=RTC_WEEKDAY_FRIDAY;

  
  if(HAL_RTC_SetDate(&RtcHandle,&RTC_Date,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */

    while(1);
  }
	
	  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}



//    OJO!!!! para sacar la info la fecha y hora al final tenemos que usar las estructuras que usamos en 
//las funciones de arriba que son las que almacenan esa informacion

 void RTC_Show(uint8_t *showtime, uint8_t *showdate)
{
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &RTC_Time, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &RTC_Date, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char *)showtime, "%02d:%02d:%02d", RTC_Time.Hours, RTC_Time.Minutes, RTC_Time.Seconds);
  /* Display date Format : mm-dd-yy */
  sprintf((char *)showdate, "%02d-%02d-%04d", RTC_Date.Month, RTC_Date.Date, 2000 + RTC_Date.Year);
}



//ALARMA igual que la hora y fecha, existe una estructura para definirla 
 //HAL_RTC_GetTime funcion clave para la hora actual 
void Alarma (void){
	 HAL_RTC_GetTime(&RtcHandle, &RTC_Time, RTC_FORMAT_BIN); //obtengo la hora
	alarma.AlarmTime.Hours= RTC_Time.Hours;// RTC_Time.Hours;
	alarma.AlarmTime.Minutes=RTC_Time.Minutes;//RTC_Time.Minutes;
	alarma.AlarmTime.Seconds=0;
	
	alarma.AlarmTime.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
	alarma.AlarmTime.StoreOperation=RTC_STOREOPERATION_RESET;
	alarma.AlarmMask= RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES; //en mascara pongo lo que no quiero tener en cuenta ('enmascarar')
	alarma.AlarmDateWeekDaySel=RTC_ALARMDATEWEEKDAYSEL_DATE; 
	alarma.AlarmDateWeekDay=0x1;  //monday
	alarma.Alarm=RTC_ALARM_A;
	
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn); //Habilita ambas alarmas
	
  __HAL_RTC_ALARM_EXTI_ENABLE_IT();       // Habilita la interrupci n
	__HAL_RTC_ALARM_EXTI_ENABLE_RISING_EDGE();  // Sensibilidad a flanco de subi
    // Configura la alarma en el RTC
    if (HAL_RTC_SetAlarm_IT(&RtcHandle, &alarma, RTC_FORMAT_BIN) != HAL_OK) {
       while (1);
    }

}

