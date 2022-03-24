#include "openart.h"
#include "zf_uart.h"
#include "SEEKFREE_18TFT.h"

FrontArtData_s front_art_data;
LR_ArtData_s left_art_data;
LR_ArtData_s right_art_data;

lpuart_handle_t     front_art_lpuartHandle;
lpuart_transfer_t   front_art_receivexfer; 
lpuart_handle_t     left_art_lpuartHandle;
lpuart_transfer_t   left_art_receivexfer; 
lpuart_handle_t     right_art_lpuartHandle;
lpuart_transfer_t   right_art_receivexfer; 

void openart_init()
{
  uart_init (FRONT_ART_UART, FRONT_ART_BAUD,FRONT_ART_TX,FRONT_ART_RX);
  uart_init (LEFT_ART_UART, LEFT_ART_BAUD,LEFT_ART_TX,LEFT_ART_RX);
  uart_init (RIGHT_ART_UART, RIGHT_ART_BAUD,RIGHT_ART_TX,RIGHT_ART_RX);
  
  NVIC_SetPriority(LPUART4_IRQn, 4);
  NVIC_SetPriority(LPUART6_IRQn, 4);
  NVIC_SetPriority(LPUART8_IRQn, 4);

  uart_set_handle(FRONT_ART_UART,&front_art_lpuartHandle,front_art_IRQHandle,NULL,0,front_art_receivexfer.data,1);
  uart_set_handle(LEFT_ART_UART,&left_art_lpuartHandle,left_art_IRQHandle,NULL,0,left_art_receivexfer.data,1);
  uart_set_handle(RIGHT_ART_UART,&right_art_lpuartHandle,right_art_IRQHandle,NULL,0,right_art_receivexfer.data,1);

  uart_rx_irq(FRONT_ART_UART,1);
  uart_rx_irq(LEFT_ART_UART,1);
  uart_rx_irq(RIGHT_ART_UART,1);
}

void front_art_IRQHandle()
{
  uint8 openart_data;
  uart_getchar(FRONT_ART_UART, &openart_data);
  static int8 data_buffer_f[8];
  static int8 data_cnt_f = 0;
  static int8 data_state_f = 0;
  if(data_state_f==0&&openart_data==OPENART_DATA_HEAD)
  {
    data_state_f=1;
    data_buffer_f[0] = OPENART_DATA_HEAD;
  }
  else if(data_state_f==1&&openart_data==OPENART_DATA_HEAD)
  {
    data_state_f=2;
    data_buffer_f[1] = OPENART_DATA_HEAD;
    data_cnt_f = 1;

  }
  else if(data_state_f==2)
  {
    data_buffer_f[++data_cnt_f]=openart_data;
    if(data_cnt_f >= 7)
    {
      data_state_f = 0;
      front_data_info(data_buffer_f,&front_art_data);
    }
  }
  else
    data_state_f = 0;  
}

void left_art_IRQHandle()
{
  uint8 openart_data;
  uart_getchar(LEFT_ART_UART, &openart_data);
  static int8 data_buffer_l[8];
  static int8 data_cnt_l = 0;
  static int8 data_state_l = 0;
  if(data_state_l==0&&openart_data==OPENART_DATA_HEAD)
  {
    data_state_l=1;
    data_buffer_l[0] = OPENART_DATA_HEAD;
  }
  else if(data_state_l==1&&openart_data==OPENART_DATA_HEAD)
  {
    data_state_l=2;
    data_buffer_l[1] = OPENART_DATA_HEAD;
    data_cnt_l = 1;

  }
  else if(data_state_l==2)
  {
    data_buffer_l[++data_cnt_l]=openart_data;
    if(data_cnt_l >= 7)
    {
      data_state_l = 0;
      LR_data_info(data_buffer_l,&left_art_data);
    }
  }
  else
    data_state_l = 0;  
}

void right_art_IRQHandle()
{
  uint8 openart_data;
  uart_getchar(RIGHT_ART_UART, &openart_data);
  static int8 data_buffer_r[8];
  static int8 data_cnt_r = 0;
  static int8 data_state_r = 0;
  if(data_state_r==0&&openart_data==OPENART_DATA_HEAD)
  {
    data_state_r=1;
    data_buffer_r[0] = OPENART_DATA_HEAD;
  }
  else if(data_state_r==1&&openart_data==OPENART_DATA_HEAD)
  {
    data_state_r=2;
    data_buffer_r[1] = OPENART_DATA_HEAD;
    data_cnt_r = 1;

  }
  else if(data_state_r==2)
  {
    data_buffer_r[++data_cnt_r]=openart_data;
    if(data_cnt_r >= 7)
    {
      data_state_r = 0;
      LR_data_info(data_buffer_r,&right_art_data);
    }
  }
  else
    data_state_r = 0;
}

void front_data_info(int8 *data_buf, FrontArtData_s *art_data)
{
  int8 check;
  
  check = data_buf[2] + data_buf[3] + data_buf[4] + data_buf[5] +data_buf[6];
  
  if(data_buf[7] == (check & 0xff))
  {
    art_data->type = *(data_buf+2);   //apritag or number
    art_data->categroy = *(data_buf+3);
    art_data->probability = data_buf[4] + data_buf[5]*0.1 + data_buf[6]*0.01;
  }
}

void LR_data_info(int8 *data_buf, LR_ArtData_s *art_data)
{
  int8 check;
  
  check = data_buf[2] + data_buf[3] + data_buf[4] + data_buf[5] +data_buf[6];
  
  if(data_buf[7] == (check & 0xff))
  {
    
    //如果是灰度图片
    //            2         3        4        5       6
    //数据格式 image_type   x_       x_ge     w      w_ge
    if(data_buf[2] == 0) //灰度0
    {
      art_data->image_type = *(data_buf+2);  
      art_data->rec_x = 10*data_buf[3]+data_buf[4];
      art_data->rec_width = 10*data_buf[5] + data_buf[6];
    }
    //如果是彩色图片
    //  2           3           4       5           6
    //image_type    categroy   x_       x_ge        w
    else if(data_buf[2] == 1)//彩色1
    {
      art_data->image_type = *(data_buf+2);
      art_data->categroy = data_buf[3];
      art_data->rec_x = 10*data_buf[4]+data_buf[5];
      art_data->rec_width = data_buf[6];
    }
    else if(data_buf[2] == 10)
    {
      art_data->image_type = *(data_buf+2);
      art_data->categroy = 0;
      art_data->rec_x = 0;
      art_data->rec_width = 0;
    }
  }
}

void openart_show()
{
//#define FRONT_ART_SHOW  1
//#define RIGHT_ART_SHOW  0
//#define LEFT_ART_SHOW  0
//  if(FRONT_ART_SHOW)
//  {
//    lcd_showstr(0,0,"front_openart:");
//    if(front_art_data.type == 2)//AprilTag
//      lcd_showstr(1,1,"AprilTag:");
//    else if(front_art_data.type == 1)
//      lcd_showstr(1,1,"number:  ");
//    lcd_showint8(2,2,front_art_data.categroy);
//  }
//  //[cat,dog,horse,pig,cattle,apple,orange,banana,durian,grape]
//  if(RIGHT_ART_SHOW)
//  {
//    lcd_showstr(0,0,"right_openart:");
//    if(right_art_data.probability>0.5)
//    {
//      switch(right_art_data.categroy)
//      {
//      case 0: 
//        lcd_showstr(1,1,"cat");break;
//      case 1:
//        lcd_showstr(1,1,"dog   ");break;
//      case 2: 
//        lcd_showstr(1,1,"horse ");break;
//      case 3:
//        lcd_showstr(1,1,"pig   ");break;
//      case 4: 
//        lcd_showstr(1,1,"cattle");break;
//      case 5:
//        lcd_showstr(1,1,"apple ");break;
//      case 6: 
//        lcd_showstr(1,1,"orange");break;
//      case 7:
//        lcd_showstr(1,1,"banana");break;
//      case 8: 
//        lcd_showstr(1,1,"durian");break;
//      case 9:
//        lcd_showstr(1,1,"grape ");break;
//      }
//    }
//    lcd_showfloat(2,2,right_art_data.probability,1,2);
//  }
//  
//  if(LEFT_ART_SHOW)
//  {
//    lcd_showstr(0,0,"left_openart:");
//    if(left_art_data.probability>0.5)
//    {
//      switch(left_art_data.categroy)
//      {
//      case 0: 
//        lcd_showstr(1,1,"cat");break;
//      case 1:
//        lcd_showstr(1,1,"dog   ");break;
//      case 2: 
//        lcd_showstr(1,1,"horse ");break;
//      case 3:
//        lcd_showstr(1,1,"pig   ");break;
//      case 4: 
//        lcd_showstr(1,1,"cattle");break;
//      case 5:
//        lcd_showstr(1,1,"apple ");break;
//      case 6: 
//        lcd_showstr(1,1,"orange");break;
//      case 7:
//        lcd_showstr(1,1,"banana");break;
//      case 8: 
//        lcd_showstr(1,1,"durian");break;
//      case 9:
//        lcd_showstr(1,1,"grape ");break;
//      }
//    }
//    lcd_showfloat(2,2,left_art_data.probability,1,2);
//  }
}