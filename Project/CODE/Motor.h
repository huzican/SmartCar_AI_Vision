#ifndef _MOTOR_H
#define _MOTOR_H

#include "common.h"
#include "Pid.h"

#define INIT_DUTY 0                     //电机初始化频率
#define PWM_PRECISON    PWM_DUTY_MAX     //PWM精度(在fsl_pwm.h中修改)
#define MOTOR_FRE       13000

//速度限幅
#define PWM_MAX   9999
#define PWM_MIN   -9999

//左电机PWM通道
#define LEFT_GO_PWMCH           PWM2_MODULE3_CHB_D3  
#define LEFT_BACK_PWMCH         PWM2_MODULE3_CHA_D2  

//右电机PWM通道
#define RIGHT_GO_PWMCH          PWM1_MODULE3_CHA_D0
#define RIGHT_BACK_PWMCH        PWM1_MODULE3_CHB_D1

//#define LEFT_KP     21.3
//#define LEFT_KI     0.72
//#define LEFT_KD     0
//#define RIGHT_KP    21.3
//#define RIGHT_KI    0.72
//#define RIGHT_KD    0
extern float LEFT_KP ;
extern float LEFT_KI ;
extern float LEFT_KD ;
extern float RIGHT_KP;
extern float RIGHT_KI;
extern float RIGHT_KD;

extern uint16 SPEED_ERR;
extern uint16 GARAGE_ERR;

typedef struct
{
  int16 speed_act_left;         //左边实际速度       
  int16 speed_set_left;         //左边设定速度   
  int16 speed_left_save[3];     //实际速度缓存   

  int16 speed_act_right;               
  int16 speed_set_right;           
  int16 speed_right_save[3];
  
  int16 left_pwm;   //左轮的PWM输出
  int16 right_pwm;  //右轮的PWM输出

  int16 speed_set;               //速度设定  
  float speed_error;             //差速设定 
  int8 stop_flag;                //停车标志  

}motor_status;
extern motor_status motor;
extern pid_inc_s pid_inc_l;
extern pid_inc_s pid_inc_r;
extern pid_pos_s pid_pos_l;
extern pid_pos_s pid_pos_r;

void motor_init();
void MotorCtrl_PosPI();
void MotorCtrl_IncPI();



#endif
