#ifndef __SWG_H__
#define __SWG_H__

#include "../configuration.h"

#define FREQUENCY_MAX     100000

void swg_init(float frequency);
void swg_set_frequency(float frequency);
void swg_sleep();
void swg_wakeup();

#endif  // __SWG_H__