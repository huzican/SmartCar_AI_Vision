#include "Servo.h"
#include "zf_pwm.h"
#include "Motor.h"
#include "image.h"
#include "openart.h"

#define TARGET_MID 70 //�������λ��

float SERVO_KP;
float SERVO_KI;
float SERVO_KD;

//��̨�������
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


//��������̨����
//ǰ�� �Ѿ�ʶ��ˮ����������
void art_servo_control()
{
  int16 art_servo_pwm = ART_SERVO_MID;
  
  art_servo_pid.kp = ART_SERVO_KP;
  art_servo_pid.kd = ART_SERVO_KD;
  
  //����,����������ұ�
  target_info.apriltag_dir = 'r';
  target_info.taget_type = IS_FRUIT;
  
  if(target_info.taget_type == IS_FRUIT)//�����ˮ��
  {
    //�������ұ�
    if(target_info.apriltag_dir == 'r')
    {
      //�Ҷ��ҿ򣬶�ȡright_art����,���ҿ����������
      if(right_art_data.image_type == 0 && right_art_data.rec_width>20 && right_art_data.rec_width< 65)
      {
        //ͼ���б�����ľ���ʵ�����ĵ�ƫ���+��-����ʱ��̨����ת-��ת+
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
        //����������
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