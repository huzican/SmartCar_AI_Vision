#ifndef _SERVO_H
#define _SERVO_H

#include "Pid.h"

#define SERVO_FRE           50  //3010模拟舵机频率设为50hz
#define SERVO_PWM_PIN       PWM4_MODULE3_CHA_C31
#define ART_SERVO_FRE       50
#define ART_SERVO_PWM_PIN   PWM4_MODULE2_CHA_C30

#define ART_SERVO_MID           750
#define ART_SERVO_LEFT_MAX      200
#define ART_SERVO_RIGHT_MAX     1200

#define SERVO_MID           665
#define SERVO_LEFT_MAX      595
#define SERVO_RIGHT_MAX     735

//#define SERVO_KP    3.05
//#define SERVO_KI    0
//#define SERVO_KD    6.35

extern float SERVO_KP;
extern float SERVO_KI;
extern float SERVO_KD;

extern float ART_SERVO_KP;
extern float ART_SERVO_KI;
extern float ART_SERVO_KD;

//180
//#define SERVO_KP    3.42
//#define SERVO_KI    0
//#define SERVO_KD    6.35

extern pid_pos_s servo_pid;
extern pid_pos_s art_servo_pid;

void servo_init();
void servo_control();
void art_servo_control();

#endif
