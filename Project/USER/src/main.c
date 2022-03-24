#include "headfile.h"
#include "include.h"


void main()
{
  DisableGlobalIRQ();
  board_init();//初始化MPU时钟
  
  BM_init();
  data_init();
  motor_init();
  gpio_init(LASER_RIGHT_PIN, GPO, 0, GPIO_PIN_CONFIG);
  gpio_init(LASER_LEFT_PIN, GPO, 0, GPIO_PIN_CONFIG);
  servo_init();
  camera_para_init();//逆透视用
  flash_init();
  parameter_init();
  Flash_handle();
  MT9V03X_CFG_CSI[7][1] = image_gain;//图像增益
  MT9V03X_CFG_CSI[1][1] = exposure_time;//曝光时间
  MT9V03X_CFG_CSI[5][1] = image_LRoffset;
  mt9v03x_csi_init();
  beep_init();
  lcd_init();
//  OLED_Init();
  CH455_Init();
  gpio_interrupt_init(CH455_IN_PIN,FALLING,GPIO_INT_CONFIG);
  NVIC_SetPriority(GPIO3_Combined_0_15_IRQn,5);         //设置中断优先级 范围0-15 越小优先级越高
  PidInc_init(&pid_inc_l, LEFT_KP, LEFT_KI, LEFT_KD);
  PidInc_init(&pid_inc_r, RIGHT_KP, RIGHT_KI, RIGHT_KD);
  openart_init();
  BuildGamaTable(gama);

  key_init();

  pit_init();
  Encoder_init();
  pit_interrupt_ms(PIT_CH2, QTIME);
  NVIC_SetPriority(PIT_IRQn,3);
  zigbee_init();
//  bluetooth_init();
  gpio_init(B9, GPO, 1, GPIO_PIN_CONFIG);
  
  
  EnableGlobalIRQ(0);
  
  while(true)
  {
//    memset(track_l,L_INIT,sizeof(track_l));
//    Memset(track_r,0,MT9V03X_CSI_H,R_INIT);
    image_hanldle();
    servo_control();
    if(image_s.show_flag && motor.stop_flag)
    {
      tft_Show();
    } 
    
    if(mt9v03x_csi_finish_flag)
    {
      mt9v03x_csi_finish_flag = 0;  
    }
    
  }
}