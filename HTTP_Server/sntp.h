#ifndef __SNTP_H
#define __SNTP_H


#include <time.h>
#include "RTC.h"
#include "rl_net_lib.h"
#include <stdio.h>


extern struct tm t_SNTP;

void Init_sntp(void);
void init_Boton (void);

#endif

