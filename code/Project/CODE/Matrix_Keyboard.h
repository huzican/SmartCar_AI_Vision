#ifndef CODE_MATRIX_KEYBOARD_H_
#define CODE_MATRIX_KEYBOARD_H_

#include "common.h"

#define CH455_IN_PIN D13
#define CH455_INT_ON      gpio_get(CH455_IN_PIN)
#define PAGE_MAX 5
#define STEP_MAX 7
#define  OLED_HEAD    12

#define  TFT_HEAD            14
#define  TFT_START_ROW       0
#define  TFT_STEP_MAX        7 + TFT_START_ROW
#define  TFT_PAGE_MAX        14
#define  N1                  85
#define  N2                  120

typedef struct
{
    uint16 cnt;
    uint16 page;
    uint16 dot;
    uint16 ok_flag;
}msg_status_s;
extern msg_status_s msg_status;

typedef struct
{
    int8 step;  //行数
    int8 set;   //选中
    int8 page;  //页
    int8 up;    //加信号
    int8 down;   //减信号
    int8 sign;  //镜像
    int8 cls;   //清屏
    int8 res;   //复位
    char show;  //开关
}menu_status_s;
extern menu_status_s oled_menu;

typedef struct  //显示参数
{
    int16 a;
    int16 b;
    int16 c;
    int16 d;
}parameter_t;
extern parameter_t parameter;

typedef struct  //暂时放这，之后放存数据文件
{   
  //标识
  int8 tft_flag;
  int8 change_flag;
  int8 unm_flag;
}sky;
extern sky  Sky;

extern uint16 old_value;
extern uint16 new_value;
extern uint16 KeyValue_Buff[5];
extern float  Value_Last;
extern int8 show_type;

int key_get_parm(uint16 *Data, uint8 flag);
void KeyBoard_IRQhandle();
void tft_Show(void);
void TFT_CLS();

#endif /* CODE_MATRIX_KEYBOARD_H_ */
