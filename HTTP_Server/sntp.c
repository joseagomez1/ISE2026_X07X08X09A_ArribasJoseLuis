#include "sntp.h"

struct tm t_SNTP; //estructura para manejar la hora de time.h

netStatus SNTP_OK;

//uint8_t errorSNTP = 0;
//uint8_t errorSNTP_2 = 0;
//uint8_t errorHora_sntp = 0;
//uint8_t errorDia_sntp = 0;
const NET_ADDR4 ntp_server = { NET_ADDR_IP4, 0, 216, 239, 35, 0 };
static void netSNTP_Callback(uint32_t seconds, uint32_t seconds_fraction);

//comprobar que nos conectamos
void Init_sntp (void) {
  if (netSNTPc_GetTime ((NET_ADDR *)&ntp_server, netSNTP_Callback) == netOK) {
   // printf ("SNTP request sent.\n");
  }
  else {
   // printf ("SNTP not ready or bad parameters.\n");
  }
}

static void netSNTP_Callback(uint32_t seconds, uint32_t seconds_fraction){
  if(seconds == 0){
     //printf ("Server not responding or bad response.\n");
    
  }else{
    //time_t segundosEpoch = (time_t)seconds; //Cambiamos formato variable que almacena los seg. desde 1970 a tipo time_t
                                            //para pasarlo por parámetro a la función localtime
    //En horario de verano, habrá que sumar dos horas
		//almacena la información de la fecha y hora
    t_SNTP = *localtime(&seconds); //Segundos desde 1 enero de 1970. Se castea a la estructura de tipo tm y lo almacena en la estructura
    
    /* Configure Date */
		
		
    RTC_Date.Year = t_SNTP.tm_year - 100; ///PASAMOS A FROMATO RTC
    RTC_Date.Month = t_SNTP.tm_mon + 1; //FORMATO RTC
    RTC_Date.Date = t_SNTP.tm_mday;
    RTC_Date.WeekDay = t_SNTP.tm_wday;
    
    if(HAL_RTC_SetDate(&RtcHandle, &RTC_Date, RTC_FORMAT_BIN) != HAL_OK)
    {
      /* Initialization Error */
     // Error_Handler();
			printf ("Error de dia\n");
		 //errorDia_sntp += 1;
		// while(1);
    }
    
        /* Configure Time */
    RTC_Time.Hours = (t_SNTP.tm_hour > 23) ? 0 : (t_SNTP.tm_hour + 1);
    RTC_Time.Minutes = t_SNTP.tm_min;
    RTC_Time.Seconds = t_SNTP.tm_sec;
    RTC_Time.TimeFormat = (t_SNTP.tm_hour < 12) ? RTC_HOURFORMAT12_AM : RTC_HOURFORMAT12_PM;
    RTC_Time.DayLightSaving = t_SNTP.tm_isdst;
    RTC_Time.StoreOperation = RTC_STOREOPERATION_RESET;
    
    if (HAL_RTC_SetTime(&RtcHandle, &RTC_Time, RTC_FORMAT_BIN) != HAL_OK)
    {
    /* Initialization Error */
    printf ("Error de hora\n");

//			errorHora_sntp += 1;
    }
    
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
  }

}

//BOTON
void init_Boton (void){

  GPIO_InitTypeDef GPIO_InitStruct;

  /*Enable clock to GPIO-C*/
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  
}

