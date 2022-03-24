#include "Init.h"
#include "zf_systick.h"
#include "zf_gpio.h"
#include "include.h"

void beep_init()
{
  gpio_init(BEEP, GPO, 0, GPIO_PIN_CONFIG);
}

void beep(uint16 time)
{
    beep_on;
    systick_delay_ms(time);
    beep_off;
}

void BM_init()
{
  gpio_init(SW1,GPI,0,GPIO_PIN_CONFIG);
  gpio_init(SW2,GPI,0,GPIO_PIN_CONFIG);
}

void key_init()
{
  gpio_interrupt_init(KEY1,FALLING,GPIO_INT_CONFIG);
  NVIC_SetPriority(GPIO2_Combined_16_31_IRQn,5);
  gpio_interrupt_init(KEY2,FALLING,GPIO_INT_CONFIG);
  NVIC_SetPriority(GPIO2_Combined_16_31_IRQn,5);
}

void data_init()
{
  pic_info.fork_dir = 'n';
  oled_menu.show ='T';
  fork_num = 0;
  cir_num = 0;
  image_s.show_flag = 0;
  
  //����1�����ұ�,����ģʽ
  if(BM1_RIGHT)
  {
    lap_cnt = 0;
    pic_info.garage_e = OUT;
    pic_info.elem_type = GARAGE;
  }
  //����1������ߣ�����ģʽ
  else
  {
    //����2�����ұ��ǵڶ�Ȧ���ԣ��ɵ������
    if(BM2_RIGHT)
      lap_cnt = 2;
    //����2��������ǵ�һȦ���ԣ��ɵ�������
    else if(BM2_LEFT)
      lap_cnt = 1;
    pic_info.garage_e = NO_GARAGE;
    pic_info.elem_type = COMMON;
  }
}