#ifndef _HX710_H
#define _HX710_H

#include "tim.h"

//HX710_DT PA11
//HX710_SCK PA12

void Get_Maopi(void);
unsigned int Get_Weight(void);
unsigned long HX710_Read(void);

#endif


