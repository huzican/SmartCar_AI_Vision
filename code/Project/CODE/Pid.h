#ifndef _PID_H
#define _PID_H
#include "common.h"

typedef struct  //ÔöÁ¿Ê½pid
{
  int16 e_k,e_k1,e_k2;
  float kp,ki,kd;
  int16 output;
}pid_inc_s;

typedef struct
{
  int16 e_k,e_k1;
  int32 integral_ek;
  float kp,ki,kd;
  int16 output;
}pid_pos_s;

void PidInc_init(pid_inc_s *pid, float kp, float ki, float kd);
void PidInc_control(pid_inc_s *pid, int16 set, int16 actual);
void PidPos_init(pid_pos_s *pid, float kp, float ki, float kd);
void PidPos_control(pid_pos_s *pid, int16 set, int16 actual);

#endif
