//#include "Motor.h"
#include "Pid.h"

//����ʽpid��ʼ��
void PidInc_init(pid_inc_s *pid, float kp, float ki, float kd)      
{
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  
  pid->e_k = 0;
  pid->e_k1 = 0;
  pid->e_k2 = 0;
  
  pid->output = 0;
}

//����ʽpid����
void PidInc_control(pid_inc_s *pid, int16 set, int16 actual)    
{
  pid->e_k2 = pid->e_k1;
  pid->e_k1 = pid->e_k;
  pid->e_k = (set-actual);
  
  pid->output += (int16)(pid->kp*(pid->e_k-pid->e_k1)+pid->ki*pid->e_k+pid->kd*(pid->e_k-2*pid->e_k1+pid->e_k2));
}

//λ��ʽpid��ʼ��
void PidPos_init(pid_pos_s *pid, float kp, float ki, float kd)
{
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
      
  pid->e_k = 0;
  pid->e_k1 = 0;
  pid->integral_ek = 0;
        
  pid->output = 0;
}

//λ��ʽpid����
void PidPos_control(pid_pos_s *pid, int16 set, int16 actual)
{
  pid->e_k1 = pid->e_k;
  pid->e_k = set - actual;
  pid->integral_ek += pid->e_k;
  
  pid->output = (int16)(pid->kp*pid->e_k+pid->ki*pid->integral_ek+pid->kd*(pid->e_k-pid->e_k1));
  
//  if(pid->output > PWM_MAX) 
//    pid->output = PWM_MAX;
//  else if(pid->output < PWM_MIN)
//    pid->output = PWM_MIN;
}