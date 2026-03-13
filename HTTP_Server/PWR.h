
#ifndef __PWR_H
#define __PWR_H

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_pwr.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_eth.h"
/* Exported types ------------------------------------------------------------*/


#define LAN8742A_PHY_ADDRESS  0
/* Exported constants --------------------------------------------------------*/
#if !defined (SLEEP_MODE) && !defined (STOP_MODE) && !defined (STOP_UNDERDRIVE_MODE) && !defined (STANDBY_MODE)\
 && !defined (STANDBY_RTC_MODE) && !defined (STANDBY_RTC_BKPSRAM_MODE)
/* Uncomment the corresponding line to select the STM32F4xx Low Power mode */
//#define SLEEP_MODE
 #define STOP_MODE 
 //#define STOP_UNDERDRIVE_MODE 
 //#define STANDBY_MODE 
//#define STANDBY_RTC_MODE 
 //#define STANDBY_RTC_BKPSRAM_MODE 
#endif

#if !defined (SLEEP_MODE) && !defined (STOP_MODE) && !defined (STOP_UNDERDRIVE_MODE) && !defined (STANDBY_MODE)\
 && !defined (STANDBY_RTC_MODE) && !defined (STANDBY_RTC_BKPSRAM_MODE)
 #error "Please select first the target STM32F4xx Low Power mode to be measured (in stm32f4xx_lp_modes.h file)"
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SleepMode_Measure(void);
void StopMode_Measure(void);
//void StopUnderDriveMode_Measure(void);
//void StandbyMode_Measure(void);
//void StandbyRTCMode_Measure(void);
//void StandbyRTCBKPSRAMMode_Measure(void);
void ETH_PhyEnterPowerDownMode(void);
void ETH_PhyExitFromPowerDownMode(void);


#endif
