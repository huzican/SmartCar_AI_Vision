#include "Motor.h"
#include "zf_pwm.h"
#include "Pid.h"
#include "Image.h"
#include "FuzzyControl.h"
#include "Servo.h"

float LEFT_KP = 21.3;
float LEFT_KI = 0.72;
float LEFT_KD = 0;
float RIGHT_KP = 21.3;
float RIGHT_KI = 0.72;
float RIGHT_KD = 0;

uint16 SPEED_ERR;
uint16 GARAGE_ERR;

motor_status motor;

pid_inc_s pid_inc_l;
pid_inc_s pid_inc_r;

pid_pos_s pid_pos_l;
pid_pos_s pid_pos_r;

void motor_init()
{
  pwm_init(LEFT_GO_PWMCH,MOTOR_FRE,0);
  pwm_init(LEFT_BACK_PWMCH,MOTOR_FRE,0);
  
  pwm_init(RIGHT_GO_PWMCH,MOTOR_FRE,INIT_DUTY);
  pwm_init(RIGHT_BACK_PWMCH,MOTOR_FRE,0);
  motor.stop_flag = 1;
}

//void SpeedSet()
//{
//  motor.speed_set = 2000;
//  motor.speed_error = 0;
//  
//  motor.speed_set_left = motor.speed_set + motor.speed_error;
//  motor.speed_set_right = motor.speed_set - motor.speed_error;
//}



//增量式PI控制
void MotorCtrl_IncPI()
{
  pid_inc_l.kp = LEFT_KP;
  pid_inc_l.ki = LEFT_KI;
  pid_inc_r.kp = RIGHT_KP;
  pid_inc_r.ki = RIGHT_KI;
  
//  motor.speed_error = FuzzySet_Casu(abs((int16)image_s.error), abs(servo_pid.output))/500;

//  if(motor.stop_flag)
//  {
//    motor.speed_set_left = 0;
//    motor.speed_set_right = 0;
//  }
//  else
//  {
    motor.speed_set_left = motor.speed_set;
    motor.speed_set_right = motor.speed_set;
   
  
    if(image_s.error>15)//舵机往右边打角
    {
      //车库加偏差
      if(pic_info.garage_e == ZABRA && lap_cnt == 2)
        motor.speed_error = FuzzySet_Casu(abs((int16)image_s.error), abs(servo_pid.output))/(GARAGE_ERR*1.0);
      else
        motor.speed_error = FuzzySet_Casu(abs((int16)image_s.error), abs(servo_pid.output))/(SPEED_ERR*1.0);
      motor.speed_set_right=(int16)(motor.speed_set_right*(1-0.33*motor.speed_error));
      motor.speed_set_left=(int16)(motor.speed_set_left*(1+0.17*motor.speed_error));
    }
    else if(image_s.error<-15)//舵机往左边打角
    {
      if(pic_info.garage_e == ZABRA && lap_cnt == 2)
        motor.speed_error = FuzzySet_Casu(abs((int16)image_s.error), abs(servo_pid.output))/(GARAGE_ERR*1.0);
      else 
        motor.speed_error = FuzzySet_Casu(abs((int16)image_s.error), abs(servo_pid.output))/(SPEED_ERR*1.0);
      motor.speed_set_right=(int16)(motor.speed_set_right*(1+0.17*motor.speed_error));
      motor.speed_set_left=(int16)(motor.speed_set_left*(1-0.33*motor.speed_error));
    }
//  }
//  PidInc_control(&pid_inc_l, motor.speed_set_left, motor.speed_act_left);
//  PidInc_control(&pid_inc_r, motor.speed_set_right, motor.speed_act_right);
  if(!motor.stop_flag)
  {
    PidInc_control(&pid_inc_l, motor.speed_set_left, motor.speed_act_left);
    PidInc_control(&pid_inc_r, motor.speed_set_right, motor.speed_act_right);
  }
  else
  {
    PidInc_control(&pid_inc_l, 0, motor.speed_act_left);
    PidInc_control(&pid_inc_r, 0, motor.speed_act_right);
  }
  if(pid_inc_l.output > 0)
  {
    if(pid_inc_l.output > PWM_MAX)
      pid_inc_l.output = PWM_MAX;
    pwm_duty(LEFT_GO_PWMCH, (uint32)(pid_inc_l.output));
    pwm_duty(LEFT_BACK_PWMCH, 0);
  }
  else
  {
    if(pid_inc_l.output < PWM_MIN)
      pid_inc_l.output = PWM_MIN;
    pwm_duty(LEFT_GO_PWMCH, 0);
    pwm_duty(LEFT_BACK_PWMCH, (uint32)(-pid_inc_l.output));
  }
  
  if(pid_inc_r.output > 0)
  {
    if(pid_inc_r.output > PWM_MAX)
      pid_inc_r.output = PWM_MAX;
    pwm_duty(RIGHT_GO_PWMCH, (uint32)(pid_inc_r.output));
    pwm_duty(RIGHT_BACK_PWMCH, 0);
  }
  else
  {
    if(pid_inc_r.output < PWM_MIN)
      pid_inc_r.output = PWM_MIN;
    pwm_duty(RIGHT_GO_PWMCH, 0);
    pwm_duty(RIGHT_BACK_PWMCH, (uint32)(-pid_inc_r.output));
  }
}

//位置式PI控制
void MotorCtrl_PosPI()
{
//  motor.speed_set = 180;
//  motor.speed_error = 0;
//  
//  motor.speed_set_left = motor.speed_set + motor.speed_error;
//  motor.speed_set_right = motor.speed_set - motor.speed_error;
//  
//  PidPos_control(&pid_pos_l, motor.speed_set_left, motor.speed_act_left);
//  PidPos_control(&pid_pos_r, motor.speed_set_right, motor.speed_act_right);
//  
//  if(pid_pos_l.output > 0)
//  {
//    if(pid_pos_l.output > PWM_MAX)
//      pid_pos_l.output = PWM_MAX;
//    pwm_duty(LEFT_GO_PWMCH, (uint32)(pid_pos_l.output));
//    pwm_duty(LEFT_BACK_PWMCH, 0);
//  }
//  else
//  {
//    if(pid_pos_l.output < PWM_MIN)
//      pid_pos_l.output = PWM_MIN;
//    pwm_duty(LEFT_GO_PWMCH, 0);
//    pwm_duty(LEFT_BACK_PWMCH, (uint32)(-pid_pos_l.output));
//  }
//  
//  if(pid_pos_r.output > 0)
//  {
//    if(pid_pos_r.output > PWM_MAX)
//      pid_pos_r.output = PWM_MAX;
//    pwm_duty(RIGHT_GO_PWMCH, (uint32)(pid_pos_r.output));
//    pwm_duty(RIGHT_BACK_PWMCH, 0);
//  }
//  else
//  {
//    if(pid_pos_r.output < PWM_MIN)
//      pid_pos_r.output = PWM_MIN;
//    pwm_duty(RIGHT_GO_PWMCH, 0);
//    pwm_duty(RIGHT_BACK_PWMCH, (uint32)(-pid_pos_r.output));
//  }
}

