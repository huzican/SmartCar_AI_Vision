#include "Flash.h"
#include "zf_flash.h"
#include "include.h"
#include "zf_systick.h"
#include "Motor.h"

//FLASH数据表(存放需要写入Flash的参数)
FLASH_WRITE_TYPE     FLASH_DATA[50] = {0};

//FLASH转换表(将参数转成FLASH_WRITE_TYPE) float类型乘以1000
FLASH_WRITE_TYPE     FLASH_CONVERT[50] = {  1    ,1    ,1    ,1    ,1    ,             //0-4
                                            1000 ,1000 ,1000 ,1000 ,1000 ,             //5-9
                                            1000 ,1000 ,1 ,1000 ,1  ,             //10-14
                                            1    ,1    ,1    ,1    ,1    ,             //15-19
                                            1    ,1    ,1    ,1    ,1 ,             //20-24
                                            1    ,1    ,1    ,1    ,1    ,             //25-29
                                            1    ,1    ,1    ,1    ,1    ,             //30-34

                                            1 ,1 ,1 ,1 ,1 ,             //35-39
                                            1000 ,1000 ,1000 ,1000 ,1000 ,};           //40-44

void MY_FLASH(char cmd[],uint8 num)
{
    FLASH_WRITE_TYPE temp = 0;
    if(!strcmp(cmd,"RA"))//全体读出
    {
        for(uint8 i = 0; i < FLASH_MAX; i++)
        {
//            temp = flash_read(FLASH_sector_num,i,FLASH_WRITE_TYPE);
//            FLASH_DATA[i] = temp;
          uint8 page_num_temp = i % FLASH_SECTOR_PAGE_NUM;
          uint8 sector_num_temp = (uint8)(i / FLASH_SECTOR_PAGE_NUM);
          flash_read_page(FLASH_sector_num - sector_num_temp, page_num_temp, &FLASH_DATA[i], 1);
        }
        DATA_SET();//刷新数据
    }
    else if(!strcmp(cmd,"WA"))//全体写入
    {
      DATA_RES();//获取数据
      flash_erase_sector(FLASH_sector_num);
      flash_erase_sector(FLASH_sector_num-1);
      flash_erase_sector(FLASH_sector_num-2);
      for(uint8 i = 0; i < FLASH_MAX; i++)
      {
        uint8 page_num_temp = i % FLASH_SECTOR_PAGE_NUM;
        uint8 sector_num_temp = (uint8)(i / FLASH_SECTOR_PAGE_NUM);
        temp = FLASH_DATA[i];
        flash_page_program(FLASH_sector_num - sector_num_temp,page_num_temp, &temp, 1);//flash_write(255-i,0,temp);
      }
    }
    else if(!strcmp(cmd,"RO"))//单独读取
    {
//        temp = flash_read(FLASH_sector_num,num,FLASH_WRITE_TYPE);
//        FLASH_DATA[num] = temp;
      uint8 page_num_temp = num % FLASH_SECTOR_PAGE_NUM;
      uint8 sector_num_temp = (uint8)(num / FLASH_SECTOR_PAGE_NUM);
      flash_read_page(FLASH_sector_num - sector_num_temp, page_num_temp, &FLASH_DATA[num],1);
    }
    else if(!strcmp(cmd,"WO"))//单独写入
    {
      DATA_RES();
      flash_erase_sector(FLASH_sector_num);
      flash_erase_sector(FLASH_sector_num-1);
      flash_erase_sector(FLASH_sector_num-2);
      for(uint8 i = 0; i < FLASH_MAX; i++)
      {
        uint8 page_num_temp = i % FLASH_SECTOR_PAGE_NUM;
        uint8 sector_num_temp = (uint8)(i / FLASH_SECTOR_PAGE_NUM);
        temp = FLASH_DATA[i];
        flash_page_program(FLASH_sector_num - sector_num_temp,page_num_temp, &temp, 1);//flash_write(255-i,0,temp);
      }
    }
    return;
}

// 全体操作FLASH_DATA数组  读取
void DATA_SET()
{ 
    image_gain                          = (uint16)((float)(FLASH_DATA[0]/FLASH_CONVERT[0]));
    exposure_time                       = (uint16)((float)(FLASH_DATA[1]/FLASH_CONVERT[1]));
    image_LRoffset                      = (uint16)((float)(FLASH_DATA[2]/FLASH_CONVERT[2]));
    END_TH                              = (uint16)((float)(FLASH_DATA[3]/FLASH_CONVERT[3]));
    CONTRAST_THRESHOLD                  = (uint16)((float)(FLASH_DATA[4]/FLASH_CONVERT[4]));
    gama                                =          (float) FLASH_DATA[5]/FLASH_CONVERT[5];
    LEFT_KP                             =          (float) FLASH_DATA[6]/FLASH_CONVERT[6];
    LEFT_KI                             =          (float) FLASH_DATA[7]/FLASH_CONVERT[7];
    RIGHT_KP                            =          (float) FLASH_DATA[8]/FLASH_CONVERT[8];
    RIGHT_KI                            =          (float) FLASH_DATA[9]/FLASH_CONVERT[9];
    SERVO_KP                            =          (float) FLASH_DATA[10]/FLASH_CONVERT[10];
    SERVO_KD                            =          (float) FLASH_DATA[11]/FLASH_CONVERT[11];
    motor.speed_set                     = (uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]));
    ART_SERVO_KP                        =          (float) FLASH_DATA[13]/FLASH_CONVERT[13];
    ART_SERVO_KD                        =          (float) FLASH_DATA[14]/FLASH_CONVERT[14];
    TARGET_TIME                         = (uint16)((float)(FLASH_DATA[15]/FLASH_CONVERT[15]));
    ANIMAL_TIME                         = (uint16)((float)(FLASH_DATA[16]/FLASH_CONVERT[16]));
    RIGHT_REC_START                     = (uint16)((float)(FLASH_DATA[17]/FLASH_CONVERT[17]));
    RIGHT_REC_END                       = (uint16)((float)(FLASH_DATA[18]/FLASH_CONVERT[18]));
    LEFT_REC_START                      = (uint16)((float)(FLASH_DATA[19]/FLASH_CONVERT[19]));
    LEFT_REC_END                        = (uint16)((float)(FLASH_DATA[20]/FLASH_CONVERT[20]));
    FORK_STOP_DIS                       = (uint16)((float)(FLASH_DATA[21]/FLASH_CONVERT[21]));
    SEEK_NUM_SPEED                      = (uint16)((float)(FLASH_DATA[22]/FLASH_CONVERT[22]));
    RUN_DIR                             = (uint16)((float)(FLASH_DATA[23]/FLASH_CONVERT[23]));
    GARAGE_ERR1                         = (uint16)((float)(FLASH_DATA[24]/FLASH_CONVERT[24]));
    GARAGE_ERR2                         = (uint16)((float)(FLASH_DATA[25]/FLASH_CONVERT[25]));
    GARAGE_ERR3                         = (uint16)((float)(FLASH_DATA[26]/FLASH_CONVERT[26]));
    OUT_ERR                             = (uint16)((float)(FLASH_DATA[27]/FLASH_CONVERT[27]));
    CIR_SPEED1                          = (uint16)((float)(FLASH_DATA[28]/FLASH_CONVERT[28]));
    CIR_SPEED2                          = (uint16)((float)(FLASH_DATA[29]/FLASH_CONVERT[29]));
    CIR_SPEED3                          = (uint16)((float)(FLASH_DATA[30]/FLASH_CONVERT[30]));
    CIR_SPEED4                          = (uint16)((float)(FLASH_DATA[31]/FLASH_CONVERT[31]));
    CIR_NUM                             = (uint16)((float)(FLASH_DATA[32]/FLASH_CONVERT[32]));
    SPEED_ERR                           = (uint16)((float)(FLASH_DATA[33]/FLASH_CONVERT[33]));
    GARAGE_ERR                          = (uint16)((float)(FLASH_DATA[34]/FLASH_CONVERT[34]));
    FIND_REC_V                          = (uint16)((float)(FLASH_DATA[35]/FLASH_CONVERT[35]));
    OUT_SPEED                           = (uint16)((float)(FLASH_DATA[36]/FLASH_CONVERT[36]));
}


// 全体操作FLASH_DATA数组  刷新
void DATA_RES()
{
    FLASH_DATA[0]  = (FLASH_WRITE_TYPE)(image_gain          *FLASH_CONVERT[0]);
    FLASH_DATA[1]  = (FLASH_WRITE_TYPE)(exposure_time       *FLASH_CONVERT[1]);
    FLASH_DATA[2]  = (FLASH_WRITE_TYPE)(image_LRoffset      *FLASH_CONVERT[2]);
    FLASH_DATA[3]  = (FLASH_WRITE_TYPE)(END_TH              *FLASH_CONVERT[3]);
    FLASH_DATA[4]  = (FLASH_WRITE_TYPE)(CONTRAST_THRESHOLD  *FLASH_CONVERT[4]);
    FLASH_DATA[5]  = (FLASH_WRITE_TYPE)(gama                *FLASH_CONVERT[5]);
    FLASH_DATA[6]  = (FLASH_WRITE_TYPE)(LEFT_KP             *FLASH_CONVERT[6]);
    FLASH_DATA[7]  = (FLASH_WRITE_TYPE)(LEFT_KI             *FLASH_CONVERT[7]);
    FLASH_DATA[8]  = (FLASH_WRITE_TYPE)(RIGHT_KP            *FLASH_CONVERT[8]);
    FLASH_DATA[9] = (FLASH_WRITE_TYPE)(RIGHT_KI             *FLASH_CONVERT[9]);
    FLASH_DATA[10] = (FLASH_WRITE_TYPE)(SERVO_KP            *FLASH_CONVERT[10]);
    FLASH_DATA[11] = (FLASH_WRITE_TYPE)(SERVO_KD            *FLASH_CONVERT[11]);
    FLASH_DATA[12] = (FLASH_WRITE_TYPE)(motor.speed_set     *FLASH_CONVERT[12]);
    FLASH_DATA[13] = (FLASH_WRITE_TYPE)(ART_SERVO_KP        *FLASH_CONVERT[13]);
    FLASH_DATA[14] = (FLASH_WRITE_TYPE)(ART_SERVO_KD        *FLASH_CONVERT[14]);
    FLASH_DATA[15] = (FLASH_WRITE_TYPE)(TARGET_TIME         *FLASH_CONVERT[15]);
    FLASH_DATA[16] = (FLASH_WRITE_TYPE)(ANIMAL_TIME         *FLASH_CONVERT[16]);
    FLASH_DATA[17] = (FLASH_WRITE_TYPE)(RIGHT_REC_START     *FLASH_CONVERT[17]);
    FLASH_DATA[18] = (FLASH_WRITE_TYPE)(RIGHT_REC_END       *FLASH_CONVERT[18]);
    FLASH_DATA[19] = (FLASH_WRITE_TYPE)(LEFT_REC_START      *FLASH_CONVERT[19]);
    FLASH_DATA[20] = (FLASH_WRITE_TYPE)(LEFT_REC_END        *FLASH_CONVERT[20]);
    FLASH_DATA[21] = (FLASH_WRITE_TYPE)(FORK_STOP_DIS       *FLASH_CONVERT[21]);
    FLASH_DATA[22] = (FLASH_WRITE_TYPE)(SEEK_NUM_SPEED      *FLASH_CONVERT[22]);
    FLASH_DATA[23] = (FLASH_WRITE_TYPE)(RUN_DIR             *FLASH_CONVERT[23]);
    FLASH_DATA[24] = (FLASH_WRITE_TYPE)(GARAGE_ERR1         *FLASH_CONVERT[24]);
    FLASH_DATA[25] = (FLASH_WRITE_TYPE)(GARAGE_ERR2         *FLASH_CONVERT[25]);
    FLASH_DATA[26] = (FLASH_WRITE_TYPE)(GARAGE_ERR3         *FLASH_CONVERT[26]);
    FLASH_DATA[27] = (FLASH_WRITE_TYPE)(OUT_ERR             *FLASH_CONVERT[27]);
    FLASH_DATA[28] = (FLASH_WRITE_TYPE)(CIR_SPEED1          *FLASH_CONVERT[28]);
    FLASH_DATA[29] = (FLASH_WRITE_TYPE)(CIR_SPEED2          *FLASH_CONVERT[29]);
    FLASH_DATA[30] = (FLASH_WRITE_TYPE)(CIR_SPEED3          *FLASH_CONVERT[30]);
    FLASH_DATA[31] = (FLASH_WRITE_TYPE)(CIR_SPEED4          *FLASH_CONVERT[31]);
    FLASH_DATA[32] = (FLASH_WRITE_TYPE)(CIR_NUM             *FLASH_CONVERT[32]);
    FLASH_DATA[33] = (FLASH_WRITE_TYPE)(SPEED_ERR           *FLASH_CONVERT[33]);
    FLASH_DATA[34] = (FLASH_WRITE_TYPE)(GARAGE_ERR          *FLASH_CONVERT[34]);
    FLASH_DATA[35] = (FLASH_WRITE_TYPE)(FIND_REC_V          *FLASH_CONVERT[35]);
    FLASH_DATA[36] = (FLASH_WRITE_TYPE)(OUT_SPEED           *FLASH_CONVERT[36]);
}

void dataf_set(float temp, uint8 num)
{
    FLASH_DATA[num] = (FLASH_WRITE_TYPE)(temp*FLASH_CONVERT[num]);
}

void parameter_init()   //参数刷新
{
  image_gain = 5;
  exposure_time =200;
  image_LRoffset = 10;
  END_TH = 20;
  CONTRAST_THRESHOLD = 120;
  gama = 0.45;
  LEFT_KP = 30.45;
  LEFT_KI = 5.0;
  LEFT_KD = 0;
  RIGHT_KP = 28.45;
  RIGHT_KI = 4.5;
  RIGHT_KD = 0;
  SERVO_KP = 1.1;
  SERVO_KI = 0   ;
  SERVO_KD = 2.5;
  ART_SERVO_KP = 3.05;
  ART_SERVO_KD = 0;
  motor.speed_set = 280;
  TARGET_TIME = 800;
  ANIMAL_TIME = 300;
  RIGHT_REC_START = 66; 
  RIGHT_REC_END = 88;   
  LEFT_REC_START = 70;  
  LEFT_REC_END = 84; 
  
  FORK_STOP_DIS = 40;
  SEEK_NUM_SPEED = 40;
  
  GARAGE_ERR1 = 15;
  GARAGE_ERR2 = 30;
  GARAGE_ERR3 = 50;
  
  OUT_ERR = 40;
  RUN_DIR = 0;
  
  CIR_SPEED1 =290;
  CIR_SPEED2 =290;
  CIR_SPEED3 =290;
  CIR_SPEED4 =290;
  CIR_NUM    = 1;
  
  SPEED_ERR = 310;
  GARAGE_ERR = 300;
  FIND_REC_V = 10;
  
  OUT_SPEED = 250;
    
  pic_info.circle_e = NOCICLE;
  pic_info.cross_e = NOCROSS;
  pic_info.fork_e = NOFORK;
  
  oled_menu.page = 1;
  oled_menu.step = 1;
  oled_menu.set = 0;
  oled_menu.up = 0;
  oled_menu.down = 0;
  oled_menu.sign = 0;
  oled_menu.res = 0;
  oled_menu.cls = 0;
  oled_menu.show = 'F';
}
void Flash_handle()
{
    systick_delay_ms(200);

    if(FLASH_FLAG)  MY_FLASH("RA",0);
    else            MY_FLASH("WA",0);
}
