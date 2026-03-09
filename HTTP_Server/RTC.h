#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"

#include <time.h>
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif


#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

//estructuras 
extern RTC_HandleTypeDef RtcHandle;
extern RTC_TimeTypeDef RTC_Time;
extern RTC_DateTypeDef RTC_Date;
extern RTC_AlarmTypeDef alarma;



//funciones

extern void Init_LSE (void);
extern void RTC_Init(void);
extern void RTC_fecha(uint8_t day, uint8_t mth, uint8_t year, uint8_t wday);
extern void RTC_Show(uint8_t *showtime, uint8_t *showdate);
extern void Alarma (void);

extern void RTC_hora(uint8_t hh, uint8_t mm, uint8_t ss);

#endif /* __RTC_H */
