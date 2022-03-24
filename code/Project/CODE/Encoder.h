#ifndef _ENCODER_H_
#define _ENCODER_H_


#define QTIME    2

#define LEFT_QTIMER               QTIMER_3
#define LEFT_ENCODER_PHASE_A      QTIMER3_TIMER3_B19
#define LEFT_ENCODER_PHASE_B      QTIMER3_TIMER2_B18

#define RIGHT_QTIMER              QTIMER_3         
#define RIGHT_ENCODER_PHASE_A     QTIMER3_TIMER1_B17
#define RIGHT_ENCODER_PHASE_B    QTIMER3_TIMER0_B16

void Encoder_init();
void speed_get();


#endif