#include "Servo.h"
#include "zf_pwm.h"
#include "Motor.h"
#include "image.h"
#include "openart.h"

#define TARGET_MID 70 //标靶中心位置

float SERVO_KP;
float SERVO_KI;
float SERVO_KD;

//云台舵机参数
float ART_SERVO_KP;
float ART_SERVO_KI;
float ART_SERVO_KD;

pid_pos_s art_servo_pid;
pid_pos_s servo_pid;

void servo_init()
{
  pwm_init(SERVO_PWM_PIN, SERVO_FRE, SERVO_MID);
  pwm_init(ART_SERVO_PWM_PIN, ART_SERVO_FRE, 660);
//  PidPos_init(&servo_pid , SERVO_KP, SERVO_KI, SERVO_KD);
}


//激光打靶云台控制
//前提 已经识别到水果，看到框
void art_servo_control()
{
  int16 art_servo_pwm = ART_SERVO_MID;
  
  art_servo_pid.kp = ART_SERVO_KP;
  art_servo_pid.kd = ART_SERVO_KD;
  
  //测试,假设靶子在右边
  target_info.apriltag_dir = 'r';
  target_info.taget_type = IS_FRUIT;
  
  if(target_info.taget_type == IS_FRUIT)//如果是水果
  {
    //靶子在右边
    if(target_info.apriltag_dir == 'r')
    {
      //灰度找框，读取right_art数据,并且框的数据正常
      if(right_art_data.image_type == 0 && right_art_data.rec_width>20 && right_art_data.rec_width< 65)
      {
        //图像中标靶中心距离实际中心的偏差，右+左-，此时云台是右转-左转+
        target_info.target_error = (right_art_data.rec_x - TARGET_MID);
        PidPos_control(&art_servo_pid, 0, (int16)target_info.target_error);
        art_servo_pwm = ART_SERVO_MID + art_servo_pid.output;
        if(art_servo_pwm > ART_SERVO_RIGHT_MAX)
          art_servo_pwm = ART_SERVO_RIGHT_MAX;
        else if(art_servo_pwm < ART_SERVO_LEFT_MAX)
          art_servo_pwm = ART_SERVO_LEFT_MAX;
        pwm_duty(ART_SERVO_PWM_PIN, art_servo_pwm);
      }
      else
      {
        //不正常就爬
      }
    }
  }
}

void servo_control()
{
  int16 servo_pwm;
  
  servo_pid.kp = SERVO_KP;
  servo_pid.kd = SERVO_KD;
  
  PidPos_control(&servo_pid, 0, (int16)image_s.error);
  servo_pwm = -servo_pid.output + SERVO_MID;
  
  if(servo_pwm > SERVO_RIGHT_MAX)
    servo_pwm = SERVO_RIGHT_MAX;
  else if(servo_pwm < SERVO_LEFT_MAX)
    servo_pwm = SERVO_LEFT_MAX;
  
  pwm_duty(SERVO_PWM_PIN, servo_pwm);
}