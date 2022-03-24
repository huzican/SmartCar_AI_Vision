#ifndef _INIT_H
#define _INIT_H

#include "common.h"

#define    BEEP         B11
#define    beep_on      gpio_set(BEEP, 1);
#define    beep_off     gpio_set(BEEP, 0);

#define KEY1 C25
#define KEY2 C26

#define SW1 D4
#define SW2 D27

#define BM1_LEFT    gpio_get(SW1)
#define BM1_RIGHT  !gpio_get(SW1)
#define BM2_LEFT    gpio_get(SW2)
#define BM2_RIGHT  !gpio_get(SW2)

void beep_init();
void beep(uint16 time);
void key_init();
void data_init();
void BM_init();


#endif
