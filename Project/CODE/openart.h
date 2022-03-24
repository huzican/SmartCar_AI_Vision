#ifndef _OPENART_H
#define _OPENART_H

#include "common.h"

#define OPENART_DATA_HEAD   0x49

#define LASER_RIGHT_PIN     B1      //右边激光引脚
#define LASER_LEFT_PIN      B0      //左边激光引脚

#define laser_right_on      gpio_set(LASER_RIGHT_PIN, 1);
#define laser_right_off     gpio_set(LASER_RIGHT_PIN, 0);
#define laser_left_on       gpio_set(LASER_LEFT_PIN, 1);
#define laser_left_off      gpio_set(LASER_LEFT_PIN, 0);

#define RGB2GRAY_PUTCHAR    'B'   //彩色图切灰度图
#define GRAY2RGB_PUTCHAR    'A'   //灰度图切彩色图

#define FRONT_ART_UART  USART_4           //front openart
#define FRONT_ART_BAUD  115200
#define FRONT_ART_TX    UART4_TX_C16
#define FRONT_ART_RX    UART4_RX_C17

#define LEFT_ART_UART  USART_6           //left openart
#define LEFT_ART_BAUD  115200
#define LEFT_ART_TX    UART6_TX_B2
#define LEFT_ART_RX    UART6_RX_B3

#define RIGHT_ART_UART  USART_8           //right openart
#define RIGHT_ART_BAUD  115200
#define RIGHT_ART_TX    UART8_TX_D16
#define RIGHT_ART_RX    UART8_RX_D17

typedef struct
{
  //type:  number, AprilTag
  //        1        2
  int8 type; 
  //number[0,1,2,3,4,5,6,7,8,9]
  //Apriltag[0,1,2,3,4,5,6,7,8,9]
  int8 categroy;
  float probability;
}FrontArtData_s;

typedef struct
{
  int8 image_type;//灰度0，彩色1
  //animal_fruit[cat,dog,horse,pig,cattle,apple,orange,banana,durian,grape]
  int8 categroy;//动物水果 0~4动物,>5水果
  int16 rec_x;   //矩形框中心位置
  int16 rec_width;//矩形框宽度
}LR_ArtData_s;

extern FrontArtData_s front_art_data;
extern LR_ArtData_s left_art_data;
extern LR_ArtData_s right_art_data;

void openart_init();
void front_art_IRQHandle();
void left_art_IRQHandle();
void right_art_IRQHandle();
void front_data_info(int8 *data_buf, FrontArtData_s *art_data);   //获取openart传回来的信息
void LR_data_info(int8 *data_buf, LR_ArtData_s *art_data);
void openart_show();

#endif
