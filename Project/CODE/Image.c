#include "Image.h"
#include "stdlib.h"
#include "SEEKFREE_FONT.h"
#include "SEEKFREE_18TFT.h"
#include "zf_gpio.h"
#include "openart.h"
#include "init.h"
#include "Motor.h"
#include "Flash.h"
#include "zf_systick.h"

uint16 image_gain;  //ͼ������
uint16 exposure_time;//����ͷ�ع�ʱ��
uint16 image_LRoffset;//ͼ������ƫ����
float gama;
uint16 CONTRAST_THRESHOLD;
uint16 END_TH;     //ɨ����ֹ�лҶȱ仯��ֵ
uint16 TARGET_TIME;//ˮ�����ʱ��
uint16 ANIMAL_TIME;//����ͣ��ʱ��
uint16 RIGHT_REC_START; //rightart�ҿ���ʼֵ
uint16 RIGHT_REC_END;   //rightart�ҿ���ֵֹ
uint16 LEFT_REC_START;  //leftart�ҿ���ʼֵ
uint16 LEFT_REC_END;    //leftart�ҿ���ֵֹ
uint16 FORK_STOP_DIS;//����ɲ��λ��
uint16 SEEK_NUM_SPEED;//Ѱ�������ٶ�
uint16 GARAGE_ERR1;     //��������ƫ��
uint16 GARAGE_ERR2;
uint16 GARAGE_ERR3;
uint16 OUT_ERR;
int8 lap_cnt;   //��¼�ܳ�Ȧ��
uint16 RUN_DIR;       //�������򣬺ͽ���ʱ���ⷽ����ͬ,1Ϊ��0Ϊ��
int8 fork_num = 0;  //��¼�ڼ�����������
int8 cir_num;       //��¼�ڼ�����������
int8 AprilTag = 0;  //ʶ��Apriltag��־���������κ�������
uint16 CIR_SPEED1;  //��һ�������ٶ�
uint16 CIR_SPEED2;  //�ڶ��������ٶ�
uint16 CIR_SPEED3;  //�����������ٶ�
uint16 CIR_SPEED4;  //���ĸ������ٶ�
uint16 CIR_NUM;     //��������
uint16 FIND_REC_V; //����ˮ���ҿ�
uint16 OUT_SPEED;   //�����ٶ�

camera_status camera;


uint8 GammaTable[256]={0};//�ñ�

uint8 image[MT9V03X_CSI_H][MT9V03X_CSI_W];  //����֮���ͼ��
int16 xy_image[WORLD_H][WORLD_W];   //��͸��֮���ͼ��

int16 Lline[MT9V03X_CSI_H];     //����
int16 Rline[MT9V03X_CSI_H];    //����
int16 Mline[MT9V03X_CSI_H];      //����
int16 WLline[WORLD_H];
int16 WRline[WORLD_H];
int16 WMline[WORLD_H];

float AvgGray[MT9V03X_CSI_H];   //ͼ��ÿ��ƽ���Ҷ�

int16 start_point[MT9V03X_CSI_H];   //ɨ����ʼ��
int16 last_start_point;             //��һ��
uint8 end_line = 4;         //ɨ����ֹ��

boder_status left_boder;
boder_status right_boder;

image_status image_s;
pic_info_s pic_info;
target_info_s target_info;//���ʱһЩ������Ϣ����������¼��Ϣ

const uint8 R_Lline[120]={
5,	7,	9,	11,	13,	15,	17,	19,	21,	22,
23,	25,	26,	27,	28,	30,	30,	32,	34,	35,
36,	37,	39,	40,	42,	42,	44,	45,	46,	46,
48,	50,	52,	53,	54,	55,	56,	58,	58,	60,
62,	62,	64,	65,	66,	68,	69,	70,	72,	73,
74,	76,	77,	78,	80,	81,	82,	84,	85,	86,
88,	89,	90,	92,	93,	94,	96,	96,	98,	99,
100,102,102,104,105,106,108,109,110,111,
112,113,115,115,117,117,119,119,121,122,
124,124,126,126,128,128,130,130,132,133,
134,135,136,137,138,139,140,141,142,143,
144,146,147,149,150,152,153,154,155,156,
};
//const uint8 R_Lline[120]={
//9,10,11,12,13,14,15,16,17,18,
//19,20,21,	22,	24,	25,	27,	28,	29,	31,
//33,	35,	36,	38,	40,	41,	43,	45,	47,	48,
//50,	52,	53,	54,	56,	58,	59,	60,	62,	64,
//66,	68,	70,	71,	72,	74,	76,	77,	79,	81,
//83,	84,	86,	88,	90,	92,	94,	95,	96,	98,
//100,101,103,105,107,108,109,111,113,114,
//115,116,118,120,121,122,124,126,128,129,
//130,131,132,133,135,136,137,138,139,140,
//141,142,143,144,146,147,148,149,150,150,
//159,159,159,159,159,159,159,159,159,159,
//159,159,159,159,159,159,159,159,159,159,
//};

const float Correction_array[120] = {

4.32,4.15,4.00,3.86,3.72,3.48,3.48,3.27,3.18,3.09,
3.275,3.250,3.225,3.200,3.175,3.150,3.125,3.100,3.075,3.050,


3.025,2.950,2.925,2.900,2.875,2.850,2.825,2.800,2.775,2.750,
//    2.975,2.950,2.925,2.900,2.875,2.850,2.825,2.800,2.775,2.750,
2.725,2.700,2.675,2.650,2.625,2.600,2.575,2.550,2.525,2.500,
//    3.265,3.230,3.195,3.160,3.125,3.090,3.055,3.020,2.985,2.950,
//    2.915,2.880,2.845,2.810,2.775,2.600,2.575,2.550,2.525,2.500,
2.475,2.450,2.425,2.400,2.375,2.350,2.325,2.300,2.275,2.250,
2.225,2.200,2.175,2.150,2.125,2.100,2.075,2.050,2.025,2.000,
1.975,1.950,1.925,1.900,1.875,1.850,1.825,1.800,1.775,1.750,
1.725,1.700,1.675,1.650,1.625,1.600,1.575,1.550,1.525,1.500,
1.475,1.450,1.425,1.400,1.375,1.350,1.325,1.300,1.275,1.250,
1.225,1.200,1.175,1.150,1.125,1.100,1.075,1.050,1.025,1.000,

1.06,1.06,1.06,1.05,1.04,1.04,1.04,1.04,1.04,1.03,
1.02,1.02,1.02,1.02,1.02,1.02,1.00,1.00,1.00,1.00
};

void sobel()
{
  uint32 Sy = 0;
  uint32 Gx = 0,Gy = 0; //ˮƽ,��ֱ���������
  int16 Sx = 0;
  //��100��ɨ����ʼ��
  if(last_start_point)
    start_point[START_ROW-1] = last_start_point;
  else
    start_point[START_ROW-1] = M_INIT;
  
  if(start_point[START_ROW-1]<M_INIT-5)
    start_point[START_ROW-1] = M_INIT-5;
  else if(start_point[START_ROW-1]>M_INIT+5)
    start_point[START_ROW-1] = M_INIT+5;
  
  int i = START_ROW-1;
  //��100�����Ե��ȡ
  for(int j = start_point[START_ROW-1]-2; j > 0; j--)
  {
    Sx = image[START_ROW-1][j+2] - image[START_ROW-1][j];
    if(Sx > 0)
    {
      Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
      Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
      Sy = Gx + Gy;
      if(Sy > CONTRAST_THRESHOLD)
      {
        Lline[i] = j;
        break;
      }
    }
  }
  if(Lline[i] <= 1) Lline[i]=0;
  
  //�ײ��ұ�Ե��ȡ
  for(int j = start_point[START_ROW-1]+2; j < R_INIT; j++)
  {
    Sx = image[START_ROW-1][j-2] - image[START_ROW-1][j];
    if(Sx > 0)
    {
      Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
      Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
      Sy = Gx + Gy;
      if(Sy > CONTRAST_THRESHOLD)
      {
        Rline[i] = j;
        break;
      }
    }
  }
  if(Rline[i] >= R_INIT-1) Rline[i]=R_INIT;
  
//  for(int j = Lline[START_ROW-1]; j <= Rline[START_ROW-1]; j++)
//     AvgGray[START_ROW-1] += image[START_ROW-1][j];
//  AvgGray[START_ROW-1] = AvgGray[START_ROW-1]/(float)(Rline[START_ROW-1]-Lline[START_ROW-1]);
  
  //�����в��ñ�����һ�е�ɨ����㣬����ɨ�߼���
  for(int i = START_ROW-2; i >= end_line; i--)
  {
    start_point[i] = (Lline[i+1] + Rline[i+1])/2;
//    int temp = start_point[i] - start_point[i+1];
//    if(temp >= 10)
//      start_point[i] = start_point[i+1];
//    else if(temp >= 6 && temp <= 10)
//      start_point[i] = start_point[i+1]+3;
//    else if(temp <= -10)
//      start_point[i] = start_point[i+1];
//    else if(temp <= -6 && temp>= -10)
//      start_point[i] = start_point[i+1]-3;
//    if(temp <= -5|| temp >=5)
//    {
//        //���Ϊ���ߣ��ұ�����
//        if(Lline[i+1] == L_INIT && Rline[i+1]!=R_INIT)
//        {
////            start_point[i] = Rline[i+1] -R_Lline[i]/2;
//            start_point[i] = GetImage_Xsite((int16)(GetTure_Xsite(Rline[i+1],i+1)- TRACK_WIDTH/2),i+1);
//        }
//        //�ұ�Ϊ���ߣ��������
//        else if(Lline[i+1] != L_INIT && Rline[i+1] == R_INIT)
//        {
////            start_point[i] = Lline[i+1] +R_Lline[i]/2;
//            start_point[i] = GetImage_Xsite((int16)(GetTure_Xsite(Lline[i+1],i+1)+TRACK_WIDTH/2),i+1);
//        }
//        //�������
//        else
//            start_point[i]=start_point[i+1];
//        if(start_point[i]>=R_INIT)
//          start_point[i] = R_INIT-20;
//        else if(start_point[i]<L_INIT)
//          start_point[i] = L_INIT+20;
//    }
    
    //���Ե��ȡ
    for(int j = start_point[i]-1; j > 0; j--)
    {
      Sx = image[i][j+1] - image[i][j-1]; 
      if(Sx > 0)
      {
        Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
        Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
        Sy = Gx + Gy;
        if(Sy > CONTRAST_THRESHOLD)
        {
          Lline[i] = j;
          break;
        }
      }
    }
    if(Lline[i] <= 1)   Lline[i] = 0;
    
    for(int j = start_point[i]+1; j < R_INIT; j++)
    {
      Sx = image[i][j-1] - image[i][j+1];
      if(Sx > 0)
      {
        Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
        Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
        Sy = Gx + Gy;
        if(Sy > CONTRAST_THRESHOLD)
        {
          Rline[i] = j;
          break;
        }
      }
    }
    if(Rline[i] >= R_INIT-1) Rline[i]=R_INIT;
    
//    if(i <= 55)
//    {
//      for(int j = Lline[i]; j <= Rline[i]; j++)
//         AvgGray[i] += image[i][j];
//      AvgGray[i] = AvgGray[i]/(float)(Rline[i]-Lline[i]);
//      if((AvgGray[i]- AvgGray[i+2]) < -END_TH)
//      {
//        image_s.endline = i;
//        break;
//      }
//    }
  }

//  if(image_s.endline <end_line)
//      image_s.endline = end_line;
  last_start_point = start_point[START_ROW-2];
}

void get_endline()
{
  int i;
  for(i = 50; i > end_line; i--)
  {
    AvgGray[i]=0;
    for(int j = Lline[i]; j<=Rline[i]; j++)
      AvgGray[i] += image[i][j];
    AvgGray[i] = AvgGray[i]/(float)(Rline[i]-Lline[i]);
    if((AvgGray[i]- AvgGray[i+2]) < -END_TH)
    {
      image_s.endline = i;
      break;
    }
  }
  image_s.endline = i;
  
  int16 foresight;
  switch(pic_info.elem_type)
  {
  case COMMON:  //��ͨ������70cmǰհ
    {
      //������������endlineȡǰհ
      foresight = GetImage_Ysite(FORESIGHT_COMMON);
      image_s.endline = image_s.endline < foresight ? foresight:image_s.endline;
    }break;
  case FORK:
    {
      foresight = GetImage_Ysite(FORESIGHT_FORK);
      if(pic_info.fork_e == FORK_IN)//�������������Ҫ���ǣ�ֱ�Ӽ���endline
        image_s.endline = image_s.endline < foresight ? foresight:image_s.endline;
      else
        image_s.endline = foresight;
    }break;
  case CROSS:
    {
      foresight = GetImage_Ysite(FORESIGHT_CROSS);
      image_s.endline = foresight;
    }break;
  case CIRCLE:
    {
      foresight = GetImage_Ysite(FORESIGHT_CIRCLE);
      if(pic_info.circle_e == LCIRCLE_IN || pic_info.circle_e == RCIRCLE_IN)
        image_s.endline = image_s.endline < foresight ? foresight:image_s.endline;
      else if(pic_info.circle_e == LEFT_BEFORE || pic_info.circle_e == RIGHT_BEFORE)
        image_s.endline = GetImage_Ysite(FORESIGHT_CIRCLE_BEFORE);
      else
        image_s.endline = foresight;
    }break;
  default:
    {
      foresight = GetImage_Ysite(FORESIGHT_COMMON);
      image_s.endline = image_s.endline < foresight ? foresight:image_s.endline;
    }
  }
}
void image_midline_get()
{
  for(uint8 i = START_ROW - 1; i>image_s.endline; i--)
  {
    //���޸ģ������Ҫ����������������
//    if(Lline[i]>=5 && Rline[i] <= R_INIT-5)//���Ҿ��޶���     
      Mline[i] = (Rline[i] + Lline[i])/2;
//    else if(Lline[i] >= M_INIT-R_Lline[i]/2 && Rline[i]>R_INIT-10 && Rline[i]<=R_INIT)//������Ч��������
//      Mline[i] = Lline[i] + R_Lline[i+12]/2;
//    else if(Rline[i]<M_INIT+R_Lline[i]/2 && Lline[i]>=L_INIT && Lline[i]<10)//������Ч��������Ч
//      Mline[i] = Rline[i] - R_Lline[i+12]/2;
//    else
//      Mline[i] = (Rline[i] + Lline[i])/2;
//    if(Mline[i]<L_INIT)
//      Mline[i] = L_INIT;
//    else if(Mline[i]>R_INIT)
//      Mline[i] = R_INIT;
  }
  
//  for(uint8 i = START_ROW; i>image_s.endline;i--)
//    image_s.error += (Mline[i]-M_INIT)*Correction_array[i];
    
  for(uint8 i = START_ROW-1; i>image_s.endline;i--)
    image_s.error += (GetTure_Xsite(Lline[i],i)+GetTure_Xsite(Rline[i],i));
  image_s.error = image_s.error/(START_ROW-image_s.endline);
  
  //����������ʱ��ƫ���С��
  if(AprilTag == 1 && pic_info.apriltag_e == APRILTAG)
    image_s.error = 0;
  
  if(pic_info.garage_e == ZABRA && lap_cnt == 2)
  {
    if(RUN_DIR == 1)
    {
      if(left_boder.breakpoint)
      {
        //��������
        int cnt1 = 0;
        for(int16 i = START_ROW-3;i>left_boder.breakpoint+4;i--)
        {
          if(Lline[i-3] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 5)
            cnt1++;
        }
        if(cnt1<1)
        {
          if(left_boder.breakpoint>=30 && left_boder.breakpoint<45)
            image_s.error = -GARAGE_ERR1;
          else if((left_boder.breakpoint>=45 && left_boder.breakpoint<60))
            image_s.error = -GARAGE_ERR2;
        }
        else
          image_s.error = -GARAGE_ERR3;
      }
      else if(!left_boder.breakpoint)
        image_s.error = -GARAGE_ERR3;
    }
    else if(RUN_DIR == 0)
    {
      if(right_boder.breakpoint)
      {
        //��������
        int cnt2 = 0;
        for(int16 i = START_ROW-3;i > right_boder.breakpoint+4;i--)
        {
          if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 5)
            cnt2++;
        }
        if(cnt2<1)
        {
          if((right_boder.breakpoint>=30 && right_boder.breakpoint<45))
            image_s.error = GARAGE_ERR1;
          else if((right_boder.breakpoint>=45 && right_boder.breakpoint<60))
            image_s.error = GARAGE_ERR2;
        }
        else
          image_s.error = GARAGE_ERR3;
      }
      else if(!right_boder.breakpoint)
        image_s.error = GARAGE_ERR3;
    }
  }
  else if(pic_info.garage_e == STOP_CAR && lap_cnt == 2)
  {
      motor.stop_flag = 1;
  }
  
  if(pic_info.garage_e == OUT)
  {
    //�󷢳�
    if(RUN_DIR == 1)
      image_s.error = -OUT_ERR;
    //�ҷ���
    else if(RUN_DIR == 0)
      image_s.error = OUT_ERR;
  }
}

//build gama table
void BuildGamaTable(float coefficient)// called in initialization
{
  float f;
  for(int i = 0;i < 256; i++)
  {
    f = (i + 0.5)/256;  //normalized
    f = (float)pow(f, coefficient);
    GammaTable[i] = (uint8)(f*256 - 0.5);   //denormalized
  }
}

void ImageGamaCorrect()
{
  for(int row = 0; row < MT9V03X_CSI_H; row++)
    for(int col = 0; col < MT9V03X_CSI_W; col++)
    {
      image[row][col] = GammaTable[mt9v03x_csi_image[row][col]];
    }
}
void edge()
{
  ImageGamaCorrect();
  sobel();
}

void edge_show()
{
//  int row = GetImage_Ysite(21+(int16)right_boder.breakpoint_dis);
  //���ұ�Ե��ʾ
  for(uint8 i = 10; i < START_ROW-1; i++)
  {
//    lcd_drawpoint(Lline[i], i, PURPLE);
//    lcd_drawpoint(Rline[i], i, GREEN);

//    LCD_Fill(track_cross_l[i],i,track_cross_l[i],i,YELLOW);
//    LCD_Fill(track_cross_r[i],i,track_cross_r[i],i,RED);
    
//    LCD_Fill(track_l[i],i,track_l[i],i,RED);
//    LCD_Fill(track_r[i],i,track_r[i],i,PURPLE);
    
    LCD_Fill(Lline[i],i,Lline[i],i,PURPLE);
    LCD_Fill(Rline[i],i,Rline[i],i,GREEN);
    
    LCD_Fill(Mline[i],i,Mline[i],i,YELLOW);
//    LCD_Fill(start_point[i],i,start_point[i],i,RED);
  }
  for(int i = 0; i<MT9V03X_CSI_W; i++)
  {
     LCD_Fill(i,image_s.endline,i,image_s.endline,RED);
     LCD_Fill(i,START_ROW,i,START_ROW,RED);
//     LCD_Fill(i,row,i,row,YELLOW);
  }
  
  for(int i = 0; i<MT9V03X_CSI_H; i++)
    LCD_Fill(79,i,79,i,RED);
}

void track_mark_show()
{
//  //������ʾ
//  for(int i = 0;i < left_boder.lost_count; i++)
//  {
//    lcd_drawpoint(left_boder.lost_head[i], L_INIT, BLUE);
//    lcd_drawpoint(left_boder.lost_head[i]+1, L_INIT, BLUE);
//        lcd_drawpoint(left_boder.lost_head[i], L_INIT+1, BLUE);
//    lcd_drawpoint(left_boder.lost_head[i]+1, L_INIT+1, BLUE);
//  }
  
}

void image_show()
{
  uint16 temp = 0,color = 0;
  lcd_set_region(0,0,MT9V03X_CSI_W-1,MT9V03X_CSI_H-1);
  for(int i = 0; i < MT9V03X_CSI_H; i++)
  {
    for(int j = 0; j < MT9V03X_CSI_W; j++)
    {
      if(i == left_boder.breakpoint && j>=Lline[i]-5 && j<=Lline[i]+5)//��յ�
        lcd_writedata_16bit(BLACK);
      else if(i==right_boder.breakpoint && j>=Rline[i]-5 && j<=Rline[i]+5)//�ҹյ�
        lcd_writedata_16bit(BROWN);
      else if(i == left_boder.breakpoint_up && j>=Lline[i]-5 && j<=Lline[i]+5)//���Ϲյ�
        lcd_writedata_16bit(RED);
      else if(i==right_boder.breakpoint_up && j>=Rline[i]-5 && j<=Rline[i]+5)//���Ϲյ�
        lcd_writedata_16bit(YELLOW);
      else if(i==right_boder.lost_tail[0] &&(j>140&&j<160)) //�Ҷ���
          lcd_writedata_16bit(PINK);
      else if(i==right_boder.lost_tail[1] &&(j>140&&j<160))
          lcd_writedata_16bit(PINK);
      else if(i==right_boder.lost_head[0] &&(j>140&&j<160))
          lcd_writedata_16bit(PURPLE);
      else if(i==right_boder.lost_head[1] &&(j>140&&j<160))
          lcd_writedata_16bit(PURPLE);
      else if(i==left_boder.lost_tail[0] &&(j>=0&&j<20))//����
          lcd_writedata_16bit(PINK);
      else if(i==left_boder.lost_tail[1] &&(j>=0&&j<20))
          lcd_writedata_16bit(PINK);
      else if(i==left_boder.lost_head[0] &&(j>=0&&j<20))
          lcd_writedata_16bit(PURPLE);
      else if(i==left_boder.lost_head[1] &&(j>=0&&j<20))
          lcd_writedata_16bit(PURPLE);
      else
      {
        temp = image[i][j];
        color=(0x001f&((temp)>>3))<<11;
        color=color|(((0x003f)&((temp)>>2))<<5);
        color=color|(0x001f&((temp)>>3));
        lcd_writedata_16bit(color);
      }
    }
  }
//  lcd_showint16(0,0,pic_info.elem_type);
//  lcd_showint16(1,1,pic_info.circle_e);
}
void line_init()
{
  for(int i = 0;i < MT9V03X_CSI_H;i++)
  {
    Lline[i] = L_INIT;
    Rline[i] = R_INIT;
    Mline[i] = M_INIT;
  }
}
void image_hanldle()
{
  line_init();
  edge();
  get_endline();

  if(image_s.show_flag)//��tft��ʾ
    GetWorldImage();
  GetWorldEdge();
  
  /**Ѱ�ҹյ�**/
  find_breakpoint(&left_boder, Lline,image_s.endline,'l');
  find_breakpoint(&right_boder, Rline,image_s.endline,'r');
  
  /**�յ㵽��ʵ�ʾ���**/
  if(left_boder.breakpoint)
    left_boder.breakpoint_dis = GetTure_Ysite(left_boder.breakpoint);
  else
    left_boder.breakpoint_dis = 0;
  if(right_boder.breakpoint)
    right_boder.breakpoint_dis = GetTure_Ysite(right_boder.breakpoint);
  else
    right_boder.breakpoint_dis = 0;
  
 /**Ѱ���Ϲյ�**/
  find_breakpoint_up(&left_boder, Lline,image_s.endline,'l');
  find_breakpoint_up(&right_boder, Rline,image_s.endline,'r');
  
  /**�Ϲյ㵽��ʵ�ʾ���**/
  if(left_boder.breakpoint_up)
    left_boder.breakpoint_up_dis = GetTure_Ysite(left_boder.breakpoint_up);
  else
    left_boder.breakpoint_up_dis = 0;
  if(right_boder.breakpoint_up)
    right_boder.breakpoint_up_dis = GetTure_Ysite(right_boder.breakpoint_up);
  else
    right_boder.breakpoint_up_dis = 0;
  
  /**Ѱ�Ҷ���**/
  find_lost_line(Lline, &left_boder, 'l');
  find_lost_line(Rline, &right_boder, 'r');

  /**�����ж�**/
  garage_judge(&left_boder, &right_boder, &pic_info);
  garage_out(&left_boder, &right_boder, &pic_info);
  
  /**ʮ���ж�**/
  cross_judge(&left_boder, &right_boder, &pic_info);
  
  /**���һ����ж�**/
  circle_judge_r(&left_boder, &right_boder, &pic_info);
  circle_judge_l(&left_boder, &right_boder, &pic_info);
  
  /**����·�ж�**/
  fork_judge(&left_boder, &right_boder, &pic_info);
  
  /**���һ�������**/
  circle_handle_r(pic_info.circle_e,&left_boder, &right_boder);
  circle_handle_l(pic_info.circle_e,&left_boder, &right_boder);
    
  /**����·����**/
  fork_handle(pic_info.fork_e, pic_info.fork_dir, &left_boder, &right_boder);
  
  /**ʮ�ִ���**/
  cross_handle(pic_info.cross_e, pic_info.cross_dir, &left_boder, &right_boder);
  
  /**���⴦��**/
  garage_handle(pic_info.garage_e,&left_boder, &right_boder);
  
  /**Apriltagʶ����**/
  if(pic_info.fork_e != FORK_BEFORE && pic_info.fork_e != JUDGE_NUMBER)
    apritag_judge(&left_boder, &right_boder, &pic_info);
  
  image_midline_get();
}

////����б��
//void first_derivative(boder_status *boder, int16 *line)
//{
//  for(uint8 i = START_ROW-1; i > end_line; i--)
//    boder->first_deriva[i] = (line[i+1] - line[i-1])/2.0;
//}
//void first_derivative_true(boder_status *boder, int16 *line)
//{
//  for(uint8 i = START_ROW-1; i > end_line; i--)
//  {
//    boder->first_deriva_true[i] = (line[i+1] - line[i-1])/2.0;
//    if(fabs(boder->first_deriva_true[i])>100)
//      boder->first_deriva_true[i] = 0;
//  }
//}

//Ѱ�ҹյ�
void find_breakpoint(boder_status *boder, int16 *line, int16 end, char flag)
{
  int8 breakpoint_th = 3;
  uint8 count1 = 0,count2 = 0;
  boder->breakpoint = 0;
  
  //��յ�
  if(flag == 'l')
  {
    for(uint8 i = START_ROW-2; i > end+4; i--)
    {
      if(line[i]-line[i-3] <= 0)
      {
        if(count2 != 0)
          count1 = 0;
        count1++;
        count2 = 0;
      }
      else if((!count2 && line[i]-line[i-3] > breakpoint_th)|| count2)
      {
        count2++;
        if(count1 > 2 && count2 >2)
        {
          boder->breakpoint = i+1;
          break;
        }
      }
    }
  }
  else if(flag == 'r')
  {
    for(uint8 i = START_ROW-2; i > end+4; i--)
    {
      if(line[i] - line[i-3] >= 0)
      {
        if(count2)
          count1 = 0;
        count1++;
        count2 = 0;
      }
      else if((!count2 && line[i]-line[i-3] < -breakpoint_th)||count2)
      {
        count2++;
        if(count1 >2 && count2 > 2)
        {
          boder->breakpoint = i+1;
          break;
        }
      }
    }
  }
}

void find_breakpoint_up(boder_status *boder, int16 *line, int16 end, char flag)
{
  int8 breakpoint_th = 5;
  uint8 count1 = 0,count2 = 0;
  boder->breakpoint_up = 0;
  
  //��յ�
  if(flag == 'l')
  {
    for(uint8 i = START_ROW-5-2; i > end+4; i--)
    {
      if(line[i]-line[i-3] <= -breakpoint_th)
      {
        if(count2)
          count1 = 0;
        count1++;
        count2 = 0;
      }
      else if((!count2 && line[i]-line[i-3] > -10)&&line[i]-line[i-3]<=0|| count2)
      {
        count2++;
        if(count1 > 2 && count2 >=3 &&((line[i]-line[i+3])-(line[i-3]-line[i]))>2)
        {
          boder->breakpoint_up = i+count2-1;
          break;
        }
      }
    }
  }
  else if(flag == 'r')
  {
    for(uint8 i = START_ROW-5-2; i > end+4; i--)
    {
      if(line[i] - line[i-3] >= breakpoint_th)
      {
        if(count2)
          count1 = 0;
        count1++;
        count2 = 0;
      }
      else if((!count2 && line[i]-line[i-3] <= 10)&&line[i]-line[i-3]>=0||count2)
      {
        count2++;
        if(count1 >2 && count2 >= 2&&(line[i+3]-line[i])-(line[i]-line[i-3])>2)
        {
          boder->breakpoint_up = i+count2-1;
          break;
        }
      }
    }
  }
}

//�Ҷ���
void find_lost_line(int16 *line, boder_status *boder, char flag)
{
  int8 count,loop,total,lost_th = 9;
  
  count = loop = total = 0;
  boder->lost_head[0] = START_ROW;
  boder->lost_tail[0] = START_ROW;
  boder->lost_head[1] = START_ROW;
  boder->lost_tail[1] = START_ROW;
  
  if(flag == 'l')
  {
    for(int i = START_ROW-1; i > image_s.endline; i--)
    {       //�Ѿ����˽�����           �ҵ��˶���ͷ��            û�ҵ�����β��           ���ߵ�һ��ڶ���
      if(i==image_s.endline+1 && boder->lost_head[count]!=START_ROW && boder->lost_tail[count]==START_ROW && count<=1)
      {     
        if(boder->lost_head[count]-i >= lost_th)
        {
          boder->lost_tail[count] = i;
          count++;
        }
        else
          boder->lost_head[count] = START_ROW;
        break;
      }
      if(line[i] <= 0)
      {
        loop++; //ÿ�ζ�����������
        total++;//����������
        if(boder->lost_head[count] == START_ROW)
          boder->lost_head[count]=i;
      }
      else if(loop)
      {
        if(boder->lost_head[count]!=START_ROW)
          boder->lost_tail[count] = i; //��ͷ������β��
        count++;    //����һ�ζ���
        if(loop < lost_th)  //����̫�̣�����
        {
          count--;
          boder->lost_head[count] = START_ROW;
          boder->lost_tail[count] = START_ROW;
        }
        if(count >=2)
          break;
        loop = 0;
      }
    }
  }
  else if(flag == 'r')
  {
    for(int i = START_ROW-1; i > image_s.endline; i--)
    {       //�Ѿ����˽�����           �ҵ��˶���ͷ��            û�ҵ�����β��           ���ߵ�һ��ڶ���
      if(i==image_s.endline+1 && boder->lost_head[count]!=START_ROW && boder->lost_tail[count]==START_ROW && count<=1)
      {     
        if(boder->lost_head[count]-i >= lost_th)
        {
          boder->lost_tail[count] = i;
          count++;
        }
        else
          boder->lost_head[count] = START_ROW;
        break;
      }
      if(line[i] >= R_INIT)
      {
        loop++; //ÿ�ζ�����������
        total++;//����������
        if(boder->lost_head[count]==START_ROW)
          boder->lost_head[count]=i;
      }
      else if(loop)
      {
        if(boder->lost_head[count]!=START_ROW)
          boder->lost_tail[count] = i; //��ͷ������β��
        count++;    //����һ�ζ���
        if(loop < lost_th)  //����̫�̣�����
        {
          count--;
          boder->lost_head[count] = START_ROW;
          boder->lost_tail[count] = START_ROW;
        }
        if(count >=2)
          break;
        loop = 0;
      }
    }
  }
  
  boder->lost_count = count;//������������� 0��1 or 2
  boder->lost_total = total;//���ж���������ܶ�������
  if(count == 1)
  {
    boder->lost_head[1] = START_ROW;
    boder->lost_tail[1] = START_ROW;
  }
}

int a1,b1;

void apritag_judge(boder_status *l, boder_status *r,pic_info_s * pic)
{
  static int8 flag_gray_convert = 0;//���л����Ҷ��ҿ�ı�־
  static int8 laser_on_rising = 0;         //��������������ر�־
  static int8 laser_off_falling = 0;    //�رռ�����½��ر�־
  static int8 laser_status = 0;
  static int8 fruit_status_conver = 0;//���л����Ҷȴ��״̬
  //�Ҷ��ж�Apriltag��ͣ��
  if(AprilTag == 0 && pic->apriltag_e == NO_APRILTAG)
  {
    bool flag1 = ((!l->breakpoint || !r->breakpoint) && !l->lost_count && !r->lost_count);
    if(flag1)
    {
      
      bool flag2 = apriltag_tuzi_judge(START_ROW-5,10,'l',10);
      //��������
      int cnt1 = 0;
      for(int16 i = START_ROW-10;i>15;i--)
      {
        if(track_l[i-3] - track_l[i] < 0 || track_l[i-1] - track_l[i] > 4)
          cnt1++;
      }
      a1 = cnt1;
      bool flag3 = apriltag_tuzi_judge(START_ROW-5,10,'r',10);
      //��������
      int cnt2 = 0;
      for(int16 i = START_ROW-10;i>15;i--)
      {
        if(track_r[i] - track_r[i-3] < 0 || track_r[i] - track_r[i-1] > 4)
          cnt2++;
      }
      b1 = cnt2;
      
      if((flag2 && flag3) && cnt1<1 && cnt2<1)
      {
        //      gpio_toggle(B9);
        beep_on;
        motor.speed_set = 0;
        AprilTag = 1;   //����pic->elem_type�����ܳ���������Ԫ����
        pic->apriltag_e = APRILTAG;
      }
    }
  }
  
  //��ȡApriltag�����ݣ��л����Ҷ��ҿ�ͣ��
  //���δ��ȡ��Apriltag�����ݣ��ٶȸĵ�50����
  if(AprilTag == 1 && pic->apriltag_e == APRILTAG)
  {
    //��ȡ����Apriltag��ֵ��ͣ�����л����Ҷ��ҿ�
    if(front_art_data.type == 2)
    {
      motor.speed_set = 0;
      pic->apriltag_e = GRAY_FIND_REC;//�Ҷ��ҿ�
      beep_off;
      flag_gray_convert = 1;
      if(front_art_data.categroy%2==1)
        pic->apriltag_dir = 'r';//�����ұ���ͼƬ
      else
        pic->apriltag_dir = 'l';//ż�������ͼƬ
    }
    else
    {
      image_s.endline = 60; //����ʱ��ǰհ����
      motor.speed_set = -20;
    }
  }
  
  //�ٶȸ�Ϊ50�����ҿ�
  if(AprilTag == 1 && pic->apriltag_e == GRAY_FIND_REC)
  {
    if(pic->apriltag_dir == 'r')//apriltagΪ�������ұ�����ͷ
    {
      //���л����ҿ�ʱ���Ȱ��ٶȵ�������
      if(flag_gray_convert)
      {
        motor.speed_set =FIND_REC_V;
        flag_gray_convert = 0;
      }
      //�Ѿ�����������ʼ�ҿ�
      else
      {
        //����ҵ��˿�ͣ�������ұ�openart�������л�ʶ�����ͼƬ
        if(IsLimitedScope(right_art_data.rec_x,RIGHT_REC_START,RIGHT_REC_END))
        {
          motor.speed_set = 0;
          pic->apriltag_e = IDENTIFY;
          uart_putchar(RIGHT_ART_UART,GRAY2RGB_PUTCHAR);
//          systick_delay_ms(500);//�ȴ��л��ɹ�
        }
      }
    }
    else if(pic->apriltag_dir == 'l')
    {
      //���л����ҿ�ʱ���Ȱ��ٶȵ�������
      if(flag_gray_convert)
      {
        motor.speed_set =FIND_REC_V;
        flag_gray_convert = 0;
      }
      //�Ѿ�����������ʼ�ҿ�
      else
      {
        //����ҵ��˿�ͣ�������ұ�openart�������л�ʶ�����ͼƬ
        if(IsLimitedScope(left_art_data.rec_x,LEFT_REC_START,LEFT_REC_END))
        {
          motor.speed_set = 0;
          pic->apriltag_e = IDENTIFY;
          uart_putchar(LEFT_ART_UART,GRAY2RGB_PUTCHAR);
//          systick_delay_ms(500);//�ȴ��л��ɹ�
        }
      }
    }
  }
  
  //ʶ������Ƕ��ﻹ��ˮ��
  if(AprilTag == 1 && pic->apriltag_e == IDENTIFY)
  {
    if(pic->apriltag_dir == 'r')
    {
      //�����ˮ������ô�л������״̬,����ͼ���лػҶ�
      if(IsLimitedScope(right_art_data.categroy,5,9)&& right_art_data.image_type == 1 && IsLimitedScope(right_art_data.rec_width,30,60))
      {
        pic->apriltag_e = FRUIT;
        
        //ֱ�Ӵ򿪰���
        laser_right_on;
        laser_status = 1;
        
        uart_putchar(RIGHT_ART_UART,RGB2GRAY_PUTCHAR);
        systick_start();//��ʼ��ʱ
//        systick_delay_ms(1000);//�ȴ��л��ɹ�
        fruit_status_conver = 1;
      }
      //����Ƕ���л���ͣ��3s
      else if(IsLimitedScope(right_art_data.categroy,0,4) && right_art_data.image_type == 1 && IsLimitedScope(right_art_data.rec_width,30,60))
      {
        pic->apriltag_e = ANIMAL;
        uart_putchar(RIGHT_ART_UART,RGB2GRAY_PUTCHAR);
        systick_start();//��ʼ��ʱ
//        systick_delay_ms(1000);//�ȴ��л��ɹ�
      }
    }
    else if(pic->apriltag_dir == 'l')
    {
      //�����ˮ������ô�л������״̬
      if(IsLimitedScope(left_art_data.categroy,5,9)&& left_art_data.image_type == 1 && IsLimitedScope(left_art_data.rec_width,30,60))
      {
        pic->apriltag_e = FRUIT;
        
        //ֱ�Ӵ򿪰���
        laser_left_on;
        laser_status = 1;
//        systick_start();//��ʼ��ʱ
        systick_start();//��ʼ��ʱ
        uart_putchar(LEFT_ART_UART,RGB2GRAY_PUTCHAR);
        
//        systick_delay_ms(1000);//�ȴ��л��ɹ�
        fruit_status_conver = 1;
      }
      //����Ƕ���л�ͣ��3s
      else if(IsLimitedScope(left_art_data.categroy,0,4) && left_art_data.image_type == 1 && IsLimitedScope(left_art_data.rec_width,30,60))
      {
        pic->apriltag_e = ANIMAL;
        systick_start();//��ʼ��ʱ
        uart_putchar(LEFT_ART_UART,RGB2GRAY_PUTCHAR);
//        systick_delay_ms(1000);//�ȴ��л��ɹ�
      }
    }
  }
  
  
  //�����ˮ����ʱ��͹ؼ���
  if(AprilTag == 1 && pic->apriltag_e == FRUIT)
  {
    //�������ұ�
    if(pic->apriltag_dir == 'r')
    {
      if(systick_getval_ms()>TARGET_TIME)//ʱ�䵽��
      {
        laser_right_off;
        laser_status = 0;
        motor.speed_set = (uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]));
        AprilTag = 0;
        pic->apriltag_e = NO_APRILTAG;
      }
    }
    else if(pic->apriltag_dir == 'l')
    {
      if(systick_getval_ms()>TARGET_TIME)
      {
        laser_left_off;
        laser_status = 0;
        motor.speed_set = (uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]));
        AprilTag = 0;
        pic->apriltag_e = NO_APRILTAG;
      }
    }
  }
  
  if(AprilTag == 1 && pic->apriltag_e == ANIMAL)
  {
    if(systick_getval_ms()>ANIMAL_TIME)
    {
      motor.speed_set = (uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]));
      AprilTag = 0;
      pic->apriltag_e = NO_APRILTAG;
    }
  }
}

//����ɨ�߰棬apriltag����ʶ��
//   start��ʼ��     end ������
// flag== l ��ߣ�������Ϊ����ɨ��,�ұ�һ������߰���
// flag== r �ұ�  ������Ϊ����ɨ�ߣ����һ�����ұ߰�
//width ������������һ�ߵľ���
int16 track_l[MT9V03X_CSI_H];
int16 track_r[MT9V03X_CSI_H];
bool AprilAoziAgain(int16 start, int16 end, char flag, int16 wid_th)
{
//  memset(track_l,L_INIT,sizeof(track_l));
//  memset(track_r,R_INIT,sizeof(track_r));
//  if(flag == 'l' || flag == 'L')
//  {
//    //������Ϊ����ɨ��
//    track_again_tuzi(START_ROW-10, 10, 'l', track_l, track_r);
//    for()
//  }
//  else if(flag == 'r' || flag == 'R')
//  {
//    
//  }
  return false;
}

//flag == 'l' ������Ϊ����ɨ�ߣ�flag == 'r' ������Ϊ����ɨ��
void track_again_tuzi(int16 start,int16 end,char flag, int16 *l_line, int16 *r_line)
{
  uint32 Sy = 0,Gx,Gy;
  int i,j;
  int16 Sx = 0;
  if(flag=='r'||flag=='R')
  {
    for(i = start; i > end; i--)
    {//������Ϊ��׼������ɨ��
      if(i == start)
        start_point[i] = Rline[i+1]-5;
      else
        start_point[i] = r_line[i+1] - 5;
      for(j = start_point[i]-1; j > 0; j--)
      {
        Sx = image[i][j+1] - image[i][j-1];        
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            l_line[i] = j;
            break;
          }
        }
      }
      l_line[i] = j;
      if(l_line[i]<=1)l_line[i]=0;
      
      for(j = start_point[i]+1; j<R_INIT;j++)
      {
        Sx = image[i][j-1] - image[i][j+1];
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            r_line[i] = j;
            break;
          }
        } 
      }
      r_line[i] = j;
      if(r_line[i] >= R_INIT-1) r_line[i]=R_INIT;
    }
  }
  if(flag=='l'||flag=='L')
  {
    for(i = start; i > end; i--)
    {//������Ϊ��׼������ɨ��
      //���ұ�ɨ
      if(i == start-1)
       start_point[i] = Lline[i+1]+5;
      else
        start_point[i] = l_line[i+1]+5;
      for(j = start_point[i]+1; j < R_INIT; j++)
      {
        Sx = image[i][j-1] - image[i][j+1];
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            r_line[i] = j;
            break;
            
          }
        }
      }
      r_line[i] = j;
      if(r_line[i]>=R_INIT-1)r_line[i]=R_INIT;
      
      for(j = start_point[i]-1; j > 0; j--)
      {
        Sx = image[i][j+1] - image[i][j-1]; 
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            l_line[i] = j;
            break;
          }
        }
      }
      l_line[i] = j;
      if(l_line[i] <= 1)   l_line[i] = 0;
    }
  }
}
//int8 count = 0;
//Apriltagɨ��ͻȻ�Ƿ���������,�ͳ������������͹�������Ȳ�ͬ
bool apriltag_tuzi_judge(int16 start, int16 end, char flag, int16 wid_th)
{
  memset(track_l,L_INIT,sizeof(track_l));
  Memset(track_r,0,MT9V03X_CSI_H,R_INIT);
  int cnt = 0;
//  count = 0;
  if(flag == 'l'|| flag == 'L')
  {
    //������Ϊ��������ɨ��
    track_again_tuzi(start, end, 'l', track_l, track_r);
    int16 tuzi_start = 0;
    int16 tuzi_end = 0;
    for(int16 i = start; i>end; i--)
    {
      //�Ұ������
      if(tuzi_start == 0)
      {
         //��-��>8 -> ˵�������氼
        if(GetTure_Xsite(track_r[i],i)-GetTure_Xsite(track_r[i-1],i-1)>wid_th)
        {
          //����ȥ��λ�ÿ��С��Apriltag�Ŀ��
          if(GetTure_Xsite(track_r[i-1],i-1)-GetTure_Xsite(track_l[i-1],i-1)<20)
          {
            tuzi_start = i;
          }
        }
      }
      //�Ѿ��ҵ�������㣬���յ�
      else if(tuzi_start!=0)
      {
        //��¼Apriltag���¾���
        if((GetTure_Xsite(track_r[i],i)-GetTure_Xsite(track_l[i],i))<20)
          cnt++;
        
        //��-��>8 -> ˵���������
        if(GetTure_Xsite(track_r[i-1],i-1)-GetTure_Xsite(track_r[i],i)>wid_th)
        {
          //�ع鵽���������
          if(GetTure_Xsite(track_r[i-3],i-3)-GetTure_Xsite(track_l[i-3],i-3)>35)
          {
            tuzi_end = i;
            if(FloatIsLimitedScope(GetTure_Ysite(tuzi_end)-GetTure_Ysite(tuzi_start),10,25))
            {
              if(cnt>8)
                return true;
            }
          }
        }
      }
    }
//    return false;
  }
  else if(flag == 'r' || flag == 'R')
  {
    //������Ϊ��׼ɨ�ߣ���߳��ְ���ȥ
    track_again_tuzi(start, end, 'r', track_l, track_r);
    
    int16 tuzi_start = 0;
    int16 tuzi_end = 0;
    for(int16 i = start; i>end; i--)
    {
      //�Ұ������
      if(tuzi_start == 0)
      {
         //��-��>8 -> ˵�������氼
        if(GetTure_Xsite(track_l[i-1],i-1)-GetTure_Xsite(track_l[i],i)>wid_th)
        {
          //����ȥ��λ�ÿ��С��Apriltag�Ŀ��
          if(GetTure_Xsite(track_r[i-1],i-1)-GetTure_Xsite(track_l[i-1],i-1)<20)
          {
            tuzi_start = i;
          }
        }
      }
      //�Ѿ��ҵ�������㣬���յ�
      else if(tuzi_start!=0)
      {
        //��¼Apriltag���¾���
        if((GetTure_Xsite(track_r[i],i)-GetTure_Xsite(track_l[i],i))<25)
          cnt++;
        
        //��-��>8 -> ˵���������
        if(GetTure_Xsite(track_l[i],i)-GetTure_Xsite(track_l[i-1],i-1)>wid_th)
        {
          //�ع鵽���������
          if(GetTure_Xsite(track_r[i-3],i-3)-GetTure_Xsite(track_l[i-3],i-3)>35)
          {
            tuzi_end = i;
            if(FloatIsLimitedScope(GetTure_Ysite(tuzi_end)-GetTure_Ysite(tuzi_start),10,25))
            {
              if(cnt>8)
                return true;
            }
          }
        }
      }
    }
//    return false;
  }
  
  return false;
}

void zebra_judge(boder_status *l, boder_status *r, pic_info_s * pic)
{
  pic->zebra_flag = 0;
  
  bool flag1 = (l->breakpoint>20 && l->breakpoint<70   //��յ��������
//     && !r->breakpoint && !r->breakpoint_up
     && l->lost_total>10 && IsLimitedScope((l->breakpoint - l->lost_head[0]),0,10) //��߶��ߣ��յ������Ŷ���
     && !r->lost_count//�ұ�û�ж���
     && FloatIsLimitedScope(GetTure_Ysite(l->lost_tail[0])-GetTure_Ysite(l->lost_head[0]),8,25)//��߿����յ㶪�߳���
       );
  bool flag2 = (IsLimitedScope(r->breakpoint,20,70)     //�ҹյ��������
//        && !l->breakpoint && !l->breakpoint_up
        && r->lost_total>10 && IsLimitedScope((r->breakpoint - r->lost_head[0]),0,10)
        && !l->lost_count
        && FloatIsLimitedScope(GetTure_Ysite(r->lost_tail[0])-GetTure_Ysite(r->lost_head[0]),8,25)//��߿����յ㶪�߳���
          );
  if((flag1 && !flag2) || (flag2 && !flag1))    //�����������������жϰ�����
  {
    uint32 Sy = 0,Gx,Gy;    
    int16 Sx = 0;
    int16 i;
    int16 j = M_INIT;
    int16 l_line=L_INIT,r_line = R_INIT;
    
    if(l->breakpoint)
      i = GetImage_Ysite(21+(int16)l->breakpoint_dis);//�󳵿⣬����յ�ǰ21cm������ɨ��һ��
    else if(r->breakpoint)
      i = GetImage_Ysite(21+(int16)r->breakpoint_dis);//�ҳ��⣬���ҹյ�ǰ21cm������ɨ��һ��
    
    for(j = M_INIT;j>0;j--)//������ȡ
    {
      Sx = image[i][j+1] - image[i][j-1];        
      if(Sx > 0)
      {
        Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
        Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
        Sy = Gx + Gy;
        if(Sy > CONTRAST_THRESHOLD)
        {
          l_line = j;
          break;
        }
      }
    }
    for(j = M_INIT;j<R_INIT;j++)//������ȡ
    {
      Sx = image[i][j-1] - image[i][j+1];
      if(Sx > 0)
      {
        Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
        Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
        Sy = Gx + Gy;
        if(Sy > CONTRAST_THRESHOLD)
        {
          r_line = j;
          break;
        }
      } 
    }
    float wid = GetTure_Xsite(r_line,i) - GetTure_Xsite(l_line,i);
    if(IsLimitedScope(r_line,M_INIT,R_INIT-1) && IsLimitedScope(l_line,L_INIT+1,M_INIT))
    {
      if((wid)<20)    //���С����ֵ
        pic->zebra_flag = 1;
      if(flag1)
        pic->garage_dir = 'l';  //���ⷽ��
      else if(flag2)
        pic->garage_dir = 'r';
    }
  }
}

void garage_out(boder_status *l, boder_status *r, pic_info_s * pic)
{
  //�ǵý�lap_cnt = 1
  //�ҷ���
  if(pic->garage_e == OUT && pic->elem_type == GARAGE && RUN_DIR == 0)
  {
    //�������
    int cnt1 = 0;
    for(int16 i = START_ROW-3;i > 25;i--)
    {
      if(Lline[i-2] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 4)
        cnt1++;
    }
    //��������
    int cnt2 = 0;
    for(int16 i = START_ROW-3;i > START_ROW-10;i--)
    {
      if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 4)
        cnt2++;
    }
    if(cnt1<1 && ((r->lost_head[0]==64 && r->lost_total>20)||(cnt2<1)))
    {
      lap_cnt = 1;
      pic_info.elem_type = COMMON;
      pic_info.garage_e = NO_GARAGE;
      motor.speed_set = (uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]));
    }
    
  }
  //�󷢳�
  else if(pic->garage_e == OUT && pic->elem_type == GARAGE && RUN_DIR == 1)
  {
    //��������
    int cnt1 = 0;
    for(int16 i = START_ROW-3;i > 25;i--)
    {
      if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 4)
        cnt1++;
    }
    //�������
    int cnt2 = 0;
    for(int16 i = START_ROW-3;i > START_ROW-10;i--)
    {
      if(Lline[i-2] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 4)
        cnt2++;
    }
    if(cnt1<1 && ((l->lost_head[0]==64 && l->lost_total>20)||(cnt2<1)))
    {
      lap_cnt = 1;
      pic_info.elem_type = COMMON;
      pic_info.garage_e = NO_GARAGE;
      motor.speed_set = (uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]));
    }
  }
}

void garage_judge(boder_status *l, boder_status *r, pic_info_s * pic)
{
  pic->zebra_flag = 0;
  if(pic->garage_e == NO_GARAGE && pic->elem_type == COMMON)
  {
    //�󷢳����󳵿��ж�
    if(RUN_DIR == 1)
    {
      if(IsLimitedScope(l->breakpoint,40,65)//�յ�λ������
         && Lline[l->breakpoint+2]-Lline[l->breakpoint-4]>15 //�յ��Ϸ�����
         && Lline[l->breakpoint+2]-Lline[l->breakpoint-5]>20
         && Lline[l->breakpoint+2]-Lline[l->breakpoint-6]>35
         && Lline[l->breakpoint+2]-Lline[l->breakpoint-7]>40
         && !r->lost_count  //�ұ��޶���
                   )
      {
        //�󷢳�ʱ���ߴ�20�е�15����ֱ�ߣ���ֹ��������
        int cnt3 = 0;
        for(int16 i = 20;i>=15;i--)
        {
          if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 3)
            cnt3++;
        }
        //��������
        int cnt1 = 0;
        for(int16 i = START_ROW-3;i > l->breakpoint+4;i--)
        {
          if(Lline[i-2] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 4)
            cnt1++;
        }
        //��������
        int cnt2 = 0;
        for(int16 i = START_ROW-3;i > l->breakpoint+4;i--)
        {
          if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 4)
            cnt2++;
        }
        if(cnt1<1 && cnt2<1 && cnt3<1)
        {
          uint32 Sy = 0,Gx,Gy;    
          int16 Sx = 0;
          int16 i;
          int16 j = M_INIT;
          int16 l_line=L_INIT,r_line = R_INIT;
          i = GetImage_Ysite(21+(int16)l->breakpoint_dis);
          for(j = M_INIT;j>0;j--)//������ȡ
          {
            Sx = image[i][j+1] - image[i][j-1];        
            if(Sx > 0)
            {
              Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
              Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
              Sy = Gx + Gy;
              if(Sy > CONTRAST_THRESHOLD)
              {
                l_line = j;
                break;
              }
            }
          }
          for(j = M_INIT;j<R_INIT;j++)//������ȡ
          {
            Sx = image[i][j-1] - image[i][j+1];
            if(Sx > 0)
            {
              Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
              Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
              Sy = Gx + Gy;
              if(Sy > CONTRAST_THRESHOLD)
              {
                r_line = j;
                break;
              }
            } 
          }
          float wid = GetTure_Xsite(r_line,i) - GetTure_Xsite(l_line,i);
          if(wid<10)
          {
            int break_point = 0;
            for(uint8 i = START_ROW-5; i > 10; i--)
            {
              float delta;
              delta = Rline[i]-Rline[i+1];
              if(delta>8)
              {
                break_point = i;
                break;
              }
            }
            if(IsLimitedScope(l->breakpoint-break_point, 0,25) && break_point)
            {
              pic->zebra_flag = 1;
//              if(lap_cnt == 2)//�ڶ�Ȧ���״̬
//              {
                pic->garage_e = ZABRA;
                pic->elem_type = GARAGE;
                pic->garage_dir = 'l';
                beep_on;
                gpio_toggle(B9);
//              }
            }
          }
        }
      }
    }
    //�ҷ������ҳ����ж�
    else if(RUN_DIR == 0)
    {
      if(IsLimitedScope(r->breakpoint,40,65)//�յ�λ������
         && Rline[r->breakpoint-4]-Rline[r->breakpoint+2]>15 //�յ��Ϸ�����
         && Rline[r->breakpoint-5]-Rline[r->breakpoint+2]>20
         && Rline[r->breakpoint-6]-Rline[r->breakpoint+2]>35
         && Rline[r->breakpoint-7]-Rline[r->breakpoint+2]>40
         && !l->lost_count  //����޶���
                   )
      {
        //�ҷ���ʱ�����������20�е�15������
        int cnt3 = 0;
        for(int16 i = 20;i>=15;i--)
        {
          if(Lline[i-2] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 3)
            cnt3++;
        }
        //��������
        int cnt1 = 0;
        for(int16 i = START_ROW-3;i>r->breakpoint+4;i--)
        {
          if(Lline[i-2] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 4)
            cnt1++;
        }
        //��������
        int cnt2 = 0;
        for(int16 i = START_ROW-3;i>r->breakpoint+4;i--)
        {
          if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 4)
            cnt2++;
        }
        if(cnt1<1 && cnt2<1 && cnt3<1)
        {
          uint32 Sy = 0,Gx,Gy;    
          int16 Sx = 0;
          int16 i;
          int16 j = M_INIT;
          int16 l_line=L_INIT,r_line = R_INIT;
          i = GetImage_Ysite(21+(int16)r->breakpoint_dis);
          for(j = M_INIT;j>0;j--)//������ȡ
          {
            Sx = image[i][j+1] - image[i][j-1];        
            if(Sx > 0)
            {
              Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
              Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
              Sy = Gx + Gy;
              if(Sy > CONTRAST_THRESHOLD)
              {
                l_line = j;
                break;
              }
            }
          }
          for(j = M_INIT;j<R_INIT;j++)//������ȡ
          {
            Sx = image[i][j-1] - image[i][j+1];
            if(Sx > 0)
            {
              Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
              Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
              Sy = Gx + Gy;
              if(Sy > CONTRAST_THRESHOLD)
              {
                r_line = j;
                break;
              }
            } 
          }
          float wid = GetTure_Xsite(r_line,i) - GetTure_Xsite(l_line,i);
          if(wid<10)
          {
            int break_point = 0;
            for(uint8 i = START_ROW-5; i > 10; i--)
            {
              float delta;
              delta = Lline[i]-Lline[i+1];
              if(delta>8)
              {
                break_point = i;
                break;
              }
            }
            if(IsLimitedScope(r->breakpoint-break_point, 0,25) && break_point)
            {
              pic->zebra_flag = 1;  //������״̬��һȦֱ���ߣ��ڶ�Ȧ���
//              if(lap_cnt == 2)//�ڶ�Ȧ���״̬
//              {
                pic->garage_e = ZABRA;
                pic->elem_type = GARAGE;
                pic->garage_dir = 'r';
                beep_on;
                gpio_toggle(B9);
//              }
            }
          }
        }
      }
    }
  }

  //�ڶ�Ȧ���
  if(pic->garage_e == ZABRA && pic->elem_type == GARAGE && lap_cnt == 2)
  {
    //�󷢳�
    if(RUN_DIR == 1)
    {
      //��������
      int cnt2 = 0;
      for(int16 i = START_ROW-3;i > START_ROW-10;i--)
      {
        if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 5)
          cnt2++;
      }
      //��������
      int cnt1 = 0;
      for(int16 i = START_ROW-3;i > 40;i--)
      {
        if(Lline[i-2] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 20)
          cnt1++;
      }
      
      float wid_down = GetTure_Xsite(Rline[START_ROW-2],START_ROW-2) - GetTure_Xsite(Lline[START_ROW-2],START_ROW-2);
      float wid_up = GetTure_Xsite(Rline[image_s.endline+4],image_s.endline+4) - GetTure_Xsite(Lline[image_s.endline+4],image_s.endline+4);
      if(cnt1<1 && cnt2<1 && wid_down<60 && wid_up<55  && !l->breakpoint && !r->breakpoint)
      {
        pic->garage_e = STOP_CAR;
        pic->elem_type = GARAGE;
        beep_off;
      }
    }
    //�ҷ���
    else if(RUN_DIR == 0)
    {
      //��������
      int cnt1 = 0;
      for(int16 i = START_ROW-3;i > START_ROW-10;i--)
      {
        if(Lline[i-2] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 5)
          cnt1++;
      }
      //��������
      int cnt2 = 0;
      for(int16 i = START_ROW-3;i > 40;i--)
      {
        if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 20)
          cnt2++;
      }
      float wid_down = GetTure_Xsite(Rline[START_ROW-2],START_ROW-2) - GetTure_Xsite(Lline[START_ROW-2],START_ROW-2);
      float wid_up = GetTure_Xsite(Rline[image_s.endline+4],image_s.endline+4) - GetTure_Xsite(Lline[image_s.endline+4],image_s.endline+4);
      if(cnt1<1 && cnt2<1 && wid_down<60 && wid_up<55  && !l->breakpoint && !r->breakpoint)
      {
        pic->garage_e = STOP_CAR;
        pic->elem_type = GARAGE;
        beep_off;
      }
    }
  }
  
  //��һȦ�����־λ
  else if(pic->garage_e == ZABRA && pic->elem_type == GARAGE && lap_cnt == 1)
  {
    //�󷢳�
//    if(RUN_DIR == 1)
//    {
      if(r->breakpoint<20 && l->breakpoint<20)
      {
        //���ߴ���ʼ�е�28������ֱ��
        
        //��������
        int cnt1 = 0;
        int cnt2 = 0;
        for(int16 i = START_ROW-3;i>28;i--)
        {
          if(Lline[i-2] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 3)
            cnt1++;
          if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 3)
            cnt2++;
        }
        if(cnt1<1 && cnt2<1)
        {
          pic->garage_e = NO_GARAGE;
          pic->elem_type = COMMON;
          beep_off;
          lap_cnt = 2;
        }
      }
//    }
//    //�ҷ���
//    else if(RUN_DIR == 0)
//    {
//      
//    }
  }
}

void garage_handle(garage_status_e flag, boder_status *l, boder_status *r)
{
  //��һȦֱ��
  if(flag == ZABRA && lap_cnt == 1)
  {
    //�󷢳�
    if(RUN_DIR == 1)
    {
      track_again_fork(START_ROW-2,15,'r');
      true_pingyi(START_ROW-2,15,'l');
    }
    else if(RUN_DIR == 0)
    {
      track_again_fork(START_ROW-2,15,'l');
      true_pingyi(START_ROW-2,15,'r');
    }
  }
  if(flag == OUT)
  {
    motor.speed_set = OUT_SPEED;
  }
}
void cross_judge(boder_status *l, boder_status *r, pic_info_s * pic)
{
  if(pic->cross_e == NOCROSS && pic->elem_type == COMMON)
  {   
    if(IsLimitedScope(l->breakpoint,25,60) && IsLimitedScope(r->breakpoint,25,60))
    {
      //��������
      int cnt1 = 0;
      for(int16 i = START_ROW-3;i > l->breakpoint+4;i--)
      {
        if(Lline[i-2] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 4)
          cnt1++;
      }
      //��������
      int cnt2 = 0;
      for(int16 i = START_ROW-3;i > r->breakpoint+4;i--)
      {
        if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 4)
          cnt2++;
      }
      
      bool delta_r1 = Rline[r->breakpoint-3]-Rline[r->breakpoint+2]>10;//�ҹյ��Ϸ�����
      bool delta_r2 = Rline[r->breakpoint-4]-Rline[r->breakpoint+2]>10;
      bool delta_r3 = Rline[r->breakpoint-5]-Rline[r->breakpoint+2]>15;
      bool delta_r4 = Rline[r->breakpoint-6]-Rline[r->breakpoint+2]>20;
      
      bool delta_l1 = Lline[l->breakpoint+2]-Lline[l->breakpoint-3]>10;//��յ��Ϸ�����
      bool delta_l2 = Lline[l->breakpoint+2]-Lline[l->breakpoint-4]>10;
      bool delta_l3 = Lline[l->breakpoint+2]-Lline[l->breakpoint-5]>15;
      bool delta_l4 = Lline[l->breakpoint+2]-Lline[l->breakpoint-6]>20;
      
      bool flag_r = (delta_r1 && delta_r2 && delta_r3 && delta_r4);
      bool flag_l = (delta_l1 && delta_l2 && delta_l3 && delta_l4);
      
      if(cnt1<=1 && cnt2<=1 && (flag_r||(r->lost_count && r->lost_total>10 && IsLimitedScope(r->breakpoint-r->lost_head[0],0,10)))
         && (flag_l||(l->lost_count && l->lost_total>10 && IsLimitedScope(l->breakpoint-l->lost_head[0],0,10))))
      {
        //�ҹյ�ƫ��
        if(r->breakpoint <= l->breakpoint)
          track_again_cross(START_ROW-5,6,'l');
        else if(r->breakpoint > l->breakpoint)
          track_again_cross(START_ROW-5,6,'r');
        
        find_breakpoint_up(&left_boder, track_cross_l,10,'l');
        find_breakpoint_up(&right_boder, track_cross_r,10,'r');
        if(IsLimitedScope(r->breakpoint_up,15,35) && IsLimitedScope(l->breakpoint_up,15,35))
        {
          //���Ϲյ�������������
          int cnt_l = 0;
          for(int16 i = l->breakpoint_up-3;i > l->breakpoint_up-8;i--)
          {
            if(track_cross_l[i-2] - track_cross_l[i] < 0 || track_cross_l[i-1] - track_cross_l[i] > 4)
              cnt_l++;
          }
          //���Ϲյ�������������
          int cnt_r = 0;
          for(int16 i = r->breakpoint_up-3;i > r->breakpoint_up-8;i--)
          {
            if(track_cross_r[i] - track_cross_r[i-2] < 0 || track_cross_r[i] - track_cross_r[i-1] > 4)
              cnt_r++;
          }
          if(cnt_l<1 && cnt_r<1)
          {
            beep_on;
//            gpio_toggle(B9);
            pic->elem_type = CROSS;
            pic->cross_e = CROSS_BEFORE;
          }     
        }
      } 
    }
  }
  if(pic->cross_e == CROSS_BEFORE && pic->elem_type == CROSS)
  {
    if((!l->breakpoint) && !r->breakpoint && l->lost_total>10 && r->lost_total>10)
    {
      pic->elem_type = CROSS;
      pic->cross_e = CROSS_IN;
//      gpio_toggle(B9);
    }
  }

  if(pic->cross_e == CROSS_IN && pic->elem_type == CROSS)
  {
    if(l->lost_total<3 && r->lost_total<3)
    {
      //��������
      int cnt1 = 0;
      for(int16 i = START_ROW-3;i > START_ROW-23;i--)
      {
        if(Lline[i-2] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 4)
          cnt1++;
      }
      //��������
      int cnt2 = 0;
      for(int16 i = START_ROW-3;i > START_ROW-23;i--)
      {
        if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 4)
          cnt2++;
      }
      if(cnt1<1 && cnt2<1)
      {
        pic->elem_type = COMMON;
        pic->cross_e = NOCROSS;
        beep_off;
      }
    }
  }
}

void cross_handle(cross_status_e flag, char dir, boder_status *l, boder_status *r)
{
  if(flag == CROSS_BEFORE)
  {
    //�ҹյ�ƫ��
    if((r->breakpoint <= l->breakpoint && l->breakpoint && r->breakpoint) ||(r->breakpoint && !l->breakpoint))
      track_again_cross(START_ROW-5,6,'l');
    else if((r->breakpoint > l->breakpoint && r->breakpoint && l->breakpoint)||(l->breakpoint && !r->breakpoint))
      track_again_cross(START_ROW-5,6,'r');
    
    find_breakpoint_up(&left_boder, track_cross_l,10,'l');
    find_breakpoint_up(&right_boder, track_cross_r,10,'r');
    
    if(r->breakpoint && l->breakpoint)
    {
      //�Ϲյ���¹յ㶼��,���۲죬�йյ�����Ϲյ�
      if(r->breakpoint_up && l->breakpoint_up && abs(r->breakpoint-l->breakpoint)>6)
      {
        matchline(l->breakpoint+2,track_cross_l[l->breakpoint+2],l->breakpoint_up-4,track_cross_l[l->breakpoint_up-4],Lline);
        matchline(r->breakpoint+2,track_cross_r[r->breakpoint+2],r->breakpoint_up-4,track_cross_r[r->breakpoint_up-4],Rline);
        for(int i = l->breakpoint_up-4;i>10;i--)
          Lline[i] = track_cross_l[i];
        for(int i = r->breakpoint_up-4;i>10;i--)
          Rline[i] = track_cross_r[i];
      }
    }
    else if(r->breakpoint && !l->breakpoint)
    {
      matchline(r->breakpoint+2,track_cross_r[r->breakpoint+2],r->breakpoint_up-4,track_cross_r[r->breakpoint_up-4],Rline);
      for(int i = r->breakpoint_up-4;i>10;i--)
        Rline[i] = track_cross_r[i];
      true_pingyi(START_ROW-1, 15, 'l');
    }
    else if(!r->breakpoint && l->breakpoint)
    {
      matchline(l->breakpoint+2,track_cross_l[l->breakpoint+2],l->breakpoint_up-4,track_cross_l[l->breakpoint_up-4],Lline);
      for(int i = l->breakpoint_up-4;i>10;i--)
        Lline[i] = track_cross_l[i];
      true_pingyi(START_ROW-1, 15, 'r');
    }
    else
    {}
  }
  if(flag == CROSS_IN)
  {
    if(abs(r->lost_total-l->lost_total)<10)
    {
      if(r->lost_count && l->lost_count)
      {
        if(r->lost_tail[0] >= l->lost_tail[0])
        {
          track_again_CrossIn(r->lost_tail[0],15,'r');
        }
        else if(r->lost_tail[0] < l->lost_tail[0])
        {
          track_again_CrossIn(l->lost_tail[0],15,'l');
        }
      }
      else if(r->lost_count && !l->lost_count)
      {
        track_again_CrossIn(r->lost_tail[0]+5,15,'r');
      }
      else if(!r->lost_count && l->lost_count)
      {
        track_again_CrossIn(l->lost_tail[0]+5,15,'l');
      }
    }
    
    image_s.endline = 40;
    
//    promatchline(21,14,START_ROW-1,20,Lline);
//    promatchline(21,14,START_ROW-1,20,Rline);
  }
}

int16 track_cross_l[MT9V03X_CSI_H];
int16 track_cross_r[MT9V03X_CSI_H];
void track_again_cross(int16 start,int16 end,char flag)
{
  memset(track_cross_l,L_INIT,sizeof(track_cross_l));
  Memset(track_cross_r,0,MT9V03X_CSI_H,R_INIT);
  
  uint32 Sy = 0,Gx,Gy;
  int i,j;
  int16 Sx = 0;
  if(flag=='r'||flag=='R')
  {
    for(i = start; i > end; i--)
    {//������Ϊ��׼������ɨ��
      if(i == start)
        start_point[i] = Rline[i+1]-R_Lline[i+1]/2;
      else
        start_point[i] = track_cross_r[i+1] - R_Lline[i+1]/2;
      for(j = start_point[i]-1; j > 0; j--)
      {
        Sx = image[i][j+1] - image[i][j-1];        
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            track_cross_l[i] = j;
            break;
          }
        }
      }
      track_cross_l[i] = j;
      if(track_cross_l[i]<=1)   track_cross_l[i]=0;
      
      for(j = start_point[i]+1; j<R_INIT;j++)
      {
        Sx = image[i][j-1] - image[i][j+1];
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            track_cross_r[i] = j;
            break;
          }
        } 
      }
      track_cross_r[i] = j;
      if(track_cross_r[i] >= R_INIT-1) track_cross_r[i]=R_INIT;
    }
  }
  if(flag=='l'||flag=='L')
  {
    for(i = start; i > end; i--)
    {//������Ϊ��׼������ɨ��
      //���ұ�ɨ
      if(i == start-1)
        start_point[i] = Lline[i+1]+R_Lline[i+1]/2;
      else
        start_point[i] = track_cross_l[i+1]+R_Lline[i+1]/2;
      for(j = start_point[i]+1; j < R_INIT; j++)
      {
        Sx = image[i][j-1] - image[i][j+1];
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            track_cross_r[i] = j;
            break;
            
          }
        }
      }
      track_cross_r[i] = j;
      if(track_cross_r[i]>=R_INIT-1)    track_cross_r[i]=R_INIT;
      
      for(j = start_point[i]-1; j > 0; j--)
      {
        Sx = image[i][j+1] - image[i][j-1]; 
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            track_cross_l[i] = j;
            break;
          }
        }
      }
      track_cross_l[i] = j;
      if(track_cross_l[i] <= 1)   track_cross_l[i] = 0;
    }
  }
}

void circle_judge_r(boder_status *l, boder_status *r, pic_info_s * pic)
{
  if(pic->circle_e == NOCICLE && pic->elem_type == COMMON)
  {
    float widthL_up = GetTure_Xsite(Lline[r->breakpoint+3],r->breakpoint+3);
    float widthR_up = GetTure_Xsite(Rline[r->breakpoint+3],r->breakpoint+3);
    float widthL_down = GetTure_Xsite(Lline[r->breakpoint+5],r->breakpoint+5);
    float widthR_down = GetTure_Xsite(Rline[r->breakpoint+5],r->breakpoint+5);
    float width_up = widthR_up-widthL_up;
    float width_down = widthR_down -widthL_down;
    if(!l->lost_count && l->breakpoint ==0 && r->breakpoint_up == 0 && r->breakpoint    //����޶��ߣ��޹յ㣬�ұ��йյ�
       && IsLimitedScope(r->breakpoint, 30,60)  //�ҹյ��ڷ�Χ��
//       && IsLimitedScope((r->breakpoint - r->lost_head[0]),0,6) && r->lost_total>10     //�ұ߹յ����Ŷ���,С�������������
       && r->lost_count>0 
       && Rline[r->breakpoint-4]-Rline[r->breakpoint]>25 && Rline[r->breakpoint-5]-Rline[r->breakpoint]>25//�ҹյ����淢ɢ
       && Rline[r->breakpoint-6]-Rline[r->breakpoint]>25 && Rline[r->breakpoint-7]-Rline[r->breakpoint]>25
       && width_up<45 && width_down<45  //�յ����������������
       && !pic->zebra_flag              //��ֹ�������гɻ���
         ) 
    {
//      float deriva1 = (GetTure_Xsite(Rline[r->breakpoint+1],r->breakpoint+1)- GetTure_Xsite(Rline[r->breakpoint+4], r->breakpoint+4))/(GetTure_Ysite(r->breakpoint+1)-GetTure_Ysite(r->breakpoint+4));
//      float deriva2 = (GetTure_Xsite(Rline[r->breakpoint+2],r->breakpoint+2)- GetTure_Xsite(Rline[r->breakpoint+5], r->breakpoint+5))/(GetTure_Ysite(r->breakpoint+2)-GetTure_Ysite(r->breakpoint+5));
      
      //�ҹյ�������ֱ��
      int cnt = 0;
      for(int16 i = r->breakpoint+8; i<START_ROW-5; i++)
      {
        float deriva = (GetTure_Xsite(Lline[i],i)- GetTure_Xsite(Lline[i+3], i+3))/(GetTure_Ysite(i)-GetTure_Ysite(i+3));
        if(fabs(deriva)>0.4)
          cnt++;
      }
      //�����ֱ��
      int cnt2 = 0;
      for(int16 i = START_ROW-10;i>image_s.endline+5;i--)
      {
        float deriva = (GetTure_Xsite(Lline[i],i)- GetTure_Xsite(Lline[i+3], i+3))/(GetTure_Ysite(i)-GetTure_Ysite(i+3));
        if(fabs(deriva)>0.4)
          cnt2++;
      }
      
      //��������
      int cnt3 = 0;
      for(int16 i = START_ROW-5;i>15;i--)
      {
        if(Lline[i-3] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 3)
          cnt3++;
      }
      
      if(cnt < 5 && cnt2 < 5 && cnt3<1)//�ҹյ�������ֱ��
      {
        pic_info.elem_type = CIRCLE;
        pic_info.circle_e = RIGHT;
        beep_on;
        
        switch(cir_num % CIR_NUM)
        {
        case 0:
          motor.speed_set = CIR_SPEED1;
          break;
        case 1:
          motor.speed_set = CIR_SPEED2;
          break;
        case 2:
          motor.speed_set = CIR_SPEED3;
          break;
        case 3:
          motor.speed_set = CIR_SPEED4;
          break;
        }
        cir_num++;
//        gpio_toggle(B9);
      }
    }
  }

  if(pic->elem_type == CIRCLE && pic->circle_e == RIGHT)
  {
    if(r->lost_total>15 && !r->breakpoint)//�ұ��޹յ㣬ֻʣ����
    {
      int16 cnt = 0;
      for(int16 i = r->lost_head[0]-2;i>r->lost_tail[0]+2;i--)
      {
        float deriva = (GetTure_Xsite(Lline[i],i)- GetTure_Xsite(Lline[i+3], i+3))/(GetTure_Ysite(i)-GetTure_Ysite(i+3));
        if(fabs(deriva)>0.45)
          cnt++;
      }
      if(cnt<4)//�����ֱ��
      {
        pic->elem_type = CIRCLE;
        pic->circle_e = RIGHT_BEFORE;
//          gpio_toggle(B9);
      }
    }
  }

  if(pic->elem_type == CIRCLE && pic->circle_e == RIGHT_BEFORE)
  {
    track_again_huandao(START_ROW-3,10,'r');//������Ϊ��׼������ɨ��
    find_breakpoint_up(&right_boder, Rline,15,'r');//Ѱ���Ϲյ�
    
    int cnt = 0;
    for(int16 i = START_ROW-20;i>image_s.endline;i--)//�����ֱ��
    {
      float deriva = (GetTure_Xsite(Lline[i],i)- GetTure_Xsite(Lline[i+3], i+3))/(GetTure_Ysite(i)-GetTure_Ysite(i+3));
      if(fabs(deriva)>0.5)
        cnt++;
    }
    if(IsLimitedScope(r->breakpoint_up, 30,55)&& Rline[r->breakpoint_up]>M_INIT //���Ϲյ����������
       && IsLimitedScope(abs(r->lost_tail[0] - r->breakpoint_up),0,10)&&r->lost_count==1   //�Ҷ���β���������Ϲյ�
       && cnt<10    //��߶���ֱ��
       )
    {
      float widthL_up = GetTure_Xsite(Lline[r->breakpoint_up-3],r->breakpoint_up-3);
      float widthR_up = GetTure_Xsite(Rline[r->breakpoint_up-3],r->breakpoint_up-3);
      float width_up = widthR_up - widthL_up;//���Ϲյ����������������
      float widthL_down = GetTure_Xsite(Lline[r->breakpoint_up+3],r->breakpoint_up+3);
      float widthR_down = GetTure_Xsite(Rline[r->breakpoint_up+3],r->breakpoint_up+3);
      float width_down = widthR_down - widthL_down;//���Ϲյ�������Ⱥܴ�
      if(width_up<50 && width_down>60)//���Ϲյ���������������������泬��
      {
//        gpio_toggle(B9);
        pic->elem_type = CIRCLE;
        pic->circle_e = RIGHT_IN;
      }
    }
  }

  if(pic->elem_type == CIRCLE && pic->circle_e == RIGHT_IN)
  {
    track_again_huandao(START_ROW-3,10,'r');    //������Ϊ��׼������ɨ��
    find_breakpoint_up(&right_boder,Rline,image_s.endline,'r');
    float widthL = GetTure_Xsite(Lline[START_ROW-5],START_ROW-5);
    float widthR = GetTure_Xsite(Rline[START_ROW-5],START_ROW-5);
    float width = widthR - widthL;  //��ʼ����һ�������������
    if(!l->lost_count && width < 60 && !r->breakpoint_up //����޶��ߣ���������������Ϲյ���ʧ
       && huandao_judge_continuity(START_ROW-2,image_s.endline+5,Rline,'r')//����ɨ�ߺ���������
         )
    {
      
      pic->elem_type = CIRCLE;
      pic->circle_e = RCIRCLE_IN;
//      gpio_toggle(B9);
    }
  }
  
  if(pic->elem_type == CIRCLE && pic->circle_e == RCIRCLE_IN)
  {
    if(IsLimitedScope(l->breakpoint,35,60) && Lline[l->breakpoint]>0 && Lline[l->breakpoint]<M_INIT+30//���ֹյ㣬������
//       && l->lost_total>10 && IsLimitedScope((l->breakpoint - l->lost_head[0]),0,15)
       && Lline[l->breakpoint]-Lline[l->breakpoint-5]>20 && Lline[l->breakpoint]-Lline[l->breakpoint-6]>25//��յ����淢ɢ
       && Lline[l->breakpoint]-Lline[l->breakpoint-7]>25 && Lline[l->breakpoint]-Lline[l->breakpoint-8]>30
       )
    {
      float widthL_up = GetTure_Xsite(Lline[l->breakpoint-3],l->breakpoint-3);
      float widthR_up = GetTure_Xsite(Rline[l->breakpoint-3],l->breakpoint-3);
      float widthL_down = GetTure_Xsite(Lline[l->breakpoint+5],l->breakpoint+5);
      float widthR_down = GetTure_Xsite(Rline[l->breakpoint+5],l->breakpoint+5);
      if(widthR_up-widthL_up>43 && widthR_down-widthL_down<65)
      {
//        gpio_toggle(B9);
        pic->elem_type = CIRCLE;
        pic->circle_e = RIGHT_OUT;
      }
    }
  }
  
// 
  if(pic->elem_type == CIRCLE && pic->circle_e == RIGHT_OUT)
  {
//    float widthL= GetTure_Xsite(Lline[START_ROW-5],START_ROW-5);
//    float widthR= GetTure_Xsite(Rline[START_ROW-5],START_ROW-5);
//    float width = widthR - widthL;
//    float widthL_up = GetTure_Xsite(Lline[START_ROW-15],START_ROW-15);
//    float widthR_up = GetTure_Xsite(Rline[START_ROW-15],START_ROW-15);
//    float width_up = widthR_up - widthL_up;
//    && width<60 && width_up<80
    if(l->breakpoint==0   //�յ���ʧ
       && !l->lost_count //������
//       && (r->lost_total<4 || (r->lost_total>=4&&r->lost_head[0]<35))       
         )
    {
      int cnt = 0;
      for(int16 i = START_ROW-5;i>15;i--)
      {
        int16 deriva = Lline[i]- Lline[i+2];
        if(deriva>3)
          cnt++;
      }
      if(cnt<5)//�����ֱ��
      {
//        gpio_toggle(B9);
        pic->elem_type = CIRCLE;
        pic->circle_e = RIGHT_END;
        motor.speed_set = (uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]));
      }
    }
  }
  
  if(pic->elem_type == CIRCLE && pic->circle_e == RIGHT_END)
  {
    track_again_huandao(60,10,'r');//������Ϊ��׼������ɨ��
    find_breakpoint_up(&right_boder,Rline,15,'r');//���Ϲյ�
    float widthL= GetTure_Xsite(Lline[START_ROW-5],START_ROW-5);
    float widthR= GetTure_Xsite(Rline[START_ROW-5],START_ROW-5);
    float width = widthR - widthL;
    if(r->breakpoint_up==0 && r->breakpoint==0 && width<45 //�ұ��޹յ㣬�����������
       && (r->lost_total<3 ||(r->lost_total>=3 &&r->lost_total<10 && r->lost_head[0]<40))//�ұ��޶��ߣ����߳�����Ϊʮ��
         )
    {
//      gpio_toggle(B9);
      pic->elem_type = COMMON;
      pic->circle_e = NOCICLE;
      beep_off;
    }
  }
}
void circle_handle_r(circle_status_e flag, boder_status *l, boder_status *r)
{
  if(flag == RIGHT)
  {
    true_pingyi(START_ROW, image_s.endline,'r');
//    line_pingyi(START_ROW,image_s.endline,8,'r');
  }
  if(flag == RIGHT_BEFORE)
  {
    if(r->breakpoint_up)//������Ϲյ㣬�����Ϲյ��ǶԵ�
    {
      if(Rline[r->breakpoint_up+2]-Rline[r->breakpoint_up-1]>35     //�Ϲյ����治����
         && Rline[r->breakpoint_up+3]-Rline[r->breakpoint_up-1]>35
         && Rline[r->breakpoint_up+4]-Rline[r->breakpoint_up-1]>40
         && Rline[r->breakpoint_up+5]-Rline[r->breakpoint_up-1]>40
////         && r->lost_head[0] != START_ROW-1                  //����ͷ��������ɨ����ʼ��
        )
      {
        Arc_matchlineLY(Lline[START_ROW-1],START_ROW-1,Rline[r->breakpoint_up],image_s.endline,Lline,'o');
        track_again_fork(START_ROW-2,image_s.endline,'r');
//        true_pingyi(START_ROW, image_s.endline,'r');
      }
      else
      {
        true_pingyi(START_ROW, image_s.endline,'r');
      }
    }
    else
    {
      true_pingyi(START_ROW, image_s.endline,'r');
//      line_pingyi(START_ROW,image_s.endline,10,'r');
    }
//    true_pingyi(START_ROW, image_s.endline,'r');
//    line_pingyi(START_ROW,image_s.endline,8,'r');
  }
  if(flag == RIGHT_IN)
  {
    track_again_huandao(START_ROW-1,image_s.endline,'l');//������Ϊ��׼��ɨ��
    true_pingyi(START_ROW, image_s.endline, 'l');
//    if(!r->breakpoint_up)//������Ϲյ���
//      line_pingyi(START_ROW,image_s.endline,8,'l');
//    else if(IsLimitedScope(r->breakpoint_up,25,40))
//      line_pingyi(START_ROW,image_s.endline,6,'l');
//    else
//      line_pingyi(START_ROW,image_s.endline,10,'l');
  }
  if(flag == RCIRCLE_IN)
  {
//    matchline_pingyi(START_ROW,image_s.endline,'r');
////    track_again_huandao(80,image_s.endline,'r');
    if(l->breakpoint)
    {
      for(int i = l->breakpoint-2;i>image_s.endline+2;i--)
      {
        Lline[i] = Lline[i+1]+2;
        if(Lline[i]>R_INIT)
          Lline[i] = R_INIT;
        if(Rline[i]<Rline[i+1])
        {
          Rline[i] = Rline[i+1]+1;
          if(Rline[i]>R_INIT)
            Rline[i] = R_INIT;
        }
      }
      line_pingyi(START_ROW,image_s.endline,10,'r');
    }
    else
      line_pingyi(START_ROW,image_s.endline,12,'r');
  }
  if(flag == RIGHT_OUT)
  {
    int cnt = 0;
    for(int16 i = START_ROW-5;i>15;i--)
    {
      if(Lline[i-3] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 3)
        cnt++;
    }
    if(!l->lost_count && cnt<1)//���û�ж��߲��������������
    {
      true_pingyi(START_ROW, image_s.endline, 'r');
      line_pingyi(START_ROW,image_s.endline,4,'r');
    }
    else
    {
      track_again_huandao(START_ROW-10,image_s.endline,'l');//������Ϊ��׼��ɨ��
      if(l->breakpoint)
      {
        for(int i = l->breakpoint-2;i>image_s.endline+2;i--)
        {
          Lline[i] = Lline[i+1]+2;
          if(Lline[i]>R_INIT)
            Lline[i] = R_INIT;
          if(Rline[i]<Rline[i+1])
          {
            Rline[i] = Rline[i+1]+1;
            if(Rline[i]>R_INIT)
              Rline[i] = R_INIT;
          }
        }
      }
      
      true_pingyi(START_ROW, image_s.endline, 'l');
      
      if(l->breakpoint>25 && l->breakpoint<30)
        line_pingyi(START_ROW,image_s.endline,6,'r');
      else
        line_pingyi(START_ROW,image_s.endline,4,'r');
    }
  }
  if(flag == RIGHT_END)
  {
    true_pingyi(START_ROW, image_s.endline, 'r');
  }
}

//���ж�
void circle_judge_l(boder_status *l, boder_status *r, pic_info_s * pic)
{
  if(pic->circle_e == NOCICLE && pic->elem_type == COMMON)
  {
    float widthL_up = GetTure_Xsite(Lline[l->breakpoint+3],l->breakpoint+3);
    float widthR_up = GetTure_Xsite(Rline[l->breakpoint+3],l->breakpoint+3);
    float widthL_down = GetTure_Xsite(Lline[l->breakpoint+5],l->breakpoint+5);
    float widthR_down = GetTure_Xsite(Rline[l->breakpoint+5],l->breakpoint+5);
    float width_up = widthR_up-widthL_up;
    float width_down = widthR_down -widthL_down;
    if(!r->lost_count && r->breakpoint ==0 && l->breakpoint_up == 0 && l->breakpoint    //�ұ�Ϊ�޶��ߣ���߹յ�
       &&IsLimitedScope(l->breakpoint,30,60) //��յ㷶Χ
       && l->lost_count>0
       && Lline[l->breakpoint]-Lline[l->breakpoint-4]>25 && Lline[l->breakpoint]-Lline[l->breakpoint-5]>25//�ҹյ����淢ɢ
       && Lline[l->breakpoint]-Lline[l->breakpoint-6]>25 && Lline[l->breakpoint]-Lline[l->breakpoint-7]>25
       && width_up<45 && width_down<45  //�յ����������������
       && !pic->zebra_flag              //��ֹ�������гɻ���
         ) 
    {
      //�յ������б��
//      float deriva1 = (GetTure_Xsite(Lline[l->breakpoint+1],l->breakpoint+1)- GetTure_Xsite(Lline[l->breakpoint+4], l->breakpoint+4))/(GetTure_Ysite(l->breakpoint+1)-GetTure_Ysite(l->breakpoint+4));
//      float deriva2 = (GetTure_Xsite(Lline[l->breakpoint+2],l->breakpoint+2)- GetTure_Xsite(Lline[l->breakpoint+5], l->breakpoint+5))/(GetTure_Ysite(l->breakpoint+2)-GetTure_Ysite(l->breakpoint+5));
      
      int cnt = 0;
      for(int16 i = l->breakpoint+8; i<START_ROW-5; i++)
      {
        float deriva = (GetTure_Xsite(Rline[i],i)- GetTure_Xsite(Rline[i+3], i+3))/(GetTure_Ysite(i)-GetTure_Ysite(i+3));
        if(fabs(deriva)>0.40)
          cnt++;
      }
      int cnt2 = 0;
      for(int16 i = START_ROW-10;i>image_s.endline+5;i--)
      {
        float deriva = (GetTure_Xsite(Rline[i],i)- GetTure_Xsite(Rline[i+3], i+3))/(GetTure_Ysite(i)-GetTure_Ysite(i+3));
        if(fabs(deriva)>0.40)
          cnt2++;
      }
      
      //��������
      int cnt3 = 0;
      for(int16 i = START_ROW-5;i>15;i--)
      {
        if(Rline[i] - Rline[i-3] < 0 || Rline[i] - Rline[i-1] > 3)
          cnt3++;
      }
      
      if(cnt < 5 && cnt2 < 5 && cnt3<1)//����Ϊֱ�ߣ���յ�������ֱ��
      {
        pic_info.elem_type = CIRCLE;
        pic_info.circle_e = LEFT;
        beep_on;
        
        switch(cir_num % CIR_NUM)
        {
        case 0:
          motor.speed_set = CIR_SPEED1;
          break;
        case 1:
          motor.speed_set = CIR_SPEED2;
          break;
        case 2:
          motor.speed_set = CIR_SPEED3;
          break;
        case 3:
          motor.speed_set = CIR_SPEED4;
          break;
        }
        cir_num++;
        beep_on;
//        gpio_toggle(B9);
      }
    }
  }

  if(pic->elem_type == CIRCLE && pic->circle_e == LEFT)
  {
    if(l->lost_total>15 && !l->breakpoint)//����޹յ㣬ֻʣ����
    {
      int16 cnt = 0;
      for(int16 i = l->lost_head[0]-2;i>l->lost_tail[0]+2;i--)
      {
        float deriva = (GetTure_Xsite(Rline[i],i)- GetTure_Xsite(Rline[i+3], i+3))/(GetTure_Ysite(i)-GetTure_Ysite(i+3));
        if(fabs(deriva)>0.45)
          cnt++;
      }
      if(cnt<4)//�ұ���ֱ��
      {
        pic->elem_type = CIRCLE;
        pic->circle_e = LEFT_BEFORE;
//          gpio_toggle(B9);
      }
    }
  }
  if(pic->elem_type == CIRCLE && pic->circle_e == LEFT_BEFORE)
  {
    track_again_huandao(START_ROW-3,10,'l');//������Ϊ��׼������ɨ��
    find_breakpoint_up(&left_boder, Lline,15,'l');//Ѱ�����Ϲյ�
    
    int cnt = 0;
    for(int16 i = START_ROW-20;i>image_s.endline;i--)//�����ֱ��
    {
      float deriva = (GetTure_Xsite(Rline[i],i)- GetTure_Xsite(Rline[i+3], i+3))/(GetTure_Ysite(i)-GetTure_Ysite(i+3));
      if(fabs(deriva)>0.5)
        cnt++;
    }
    if(IsLimitedScope(l->breakpoint_up,30,55) && Lline[l->breakpoint_up]<M_INIT //���Ϲյ����������
       && IsLimitedScope(abs(l->lost_tail[0] - l->breakpoint_up),0,10) && l->lost_count==1   //�Ҷ���β���������Ϲյ�
       && cnt<10    //��߶���ֱ��
         )
    {
      float widthL_up = GetTure_Xsite(Lline[l->breakpoint_up-3],l->breakpoint_up-3);
      float widthR_up = GetTure_Xsite(Rline[l->breakpoint_up-3],l->breakpoint_up-3);
      float width_up = widthR_up - widthL_up;//���Ϲյ����������������
      float widthL_down = GetTure_Xsite(Lline[l->breakpoint_up+3],l->breakpoint_up+3);
      float widthR_down = GetTure_Xsite(Rline[l->breakpoint_up+3],l->breakpoint_up+3);
      float width_down = widthR_down - widthL_down;//���Ϲյ�������Ⱥܴ�
      if(width_up<50 && width_down>60)
      {
        //      gpio_toggle(B9);
        pic->elem_type = CIRCLE;
        pic->circle_e = LEFT_IN;
      }
    }
  }
  if(pic->elem_type == CIRCLE && pic->circle_e == LEFT_IN)
  {
    track_again_huandao(START_ROW-3,10,'l');    //������Ϊ��׼������ɨ��
    find_breakpoint_up(&left_boder,Lline,image_s.endline,'l');
    float widthL = GetTure_Xsite(Lline[START_ROW-5],START_ROW-5);
    float widthR = GetTure_Xsite(Rline[START_ROW-5],START_ROW-5);
    float width = widthR - widthL;  //��ʼ����һ�������������
    
    if(!r->lost_count && width < 60 && !l->breakpoint_up//�ұ��޶��ߣ���������������Ϲյ���ʧ
       && huandao_judge_continuity(START_ROW-2,image_s.endline+5,Lline,'l')//����ɨ�ߺ���������
         )
    {
      
      pic->elem_type = CIRCLE;
      pic->circle_e = LCIRCLE_IN;
      //        gpio_toggle(B9);
      
    }
  }
  if(pic->elem_type == CIRCLE && pic->circle_e == LCIRCLE_IN)
  {
    if(IsLimitedScope(r->breakpoint,35,60)&& Rline[r->breakpoint]>M_INIT-30 && Rline[r->breakpoint]<R_INIT//���ֹյ㣬������
//       && r->lost_total>10 && IsLimitedScope((r->breakpoint - r->lost_head[0]),0,15)
       && Rline[r->breakpoint-5]-Rline[r->breakpoint]>20 && Rline[r->breakpoint-6]-Rline[r->breakpoint]>25//��յ����淢ɢ
       && Rline[r->breakpoint-7]-Rline[r->breakpoint]>25 && Rline[r->breakpoint-8]-Rline[r->breakpoint]>30
       )
    {
      float widthL_up = GetTure_Xsite(Lline[r->breakpoint-3],r->breakpoint-3);
      float widthR_up = GetTure_Xsite(Rline[r->breakpoint-3],r->breakpoint-3);
      float widthL_down = GetTure_Xsite(Lline[r->breakpoint+5],r->breakpoint+5);
      float widthR_down = GetTure_Xsite(Rline[r->breakpoint+5],r->breakpoint+5);
      if(widthR_up-widthL_up>43 && widthR_down-widthL_down<65)
      {
//        gpio_toggle(B9);
        pic->elem_type = CIRCLE;
        pic->circle_e = LEFT_OUT;
      }
    }
  }
  if(pic->elem_type == CIRCLE && pic->circle_e == LEFT_OUT)
  {
//    float widthL= GetTure_Xsite(Lline[START_ROW-5],START_ROW-5);
//    float widthR= GetTure_Xsite(Rline[START_ROW-5],START_ROW-5);
//    float width = widthR - widthL;
//    float widthL_up = GetTure_Xsite(Lline[START_ROW-15],START_ROW-15);
//    float widthR_up = GetTure_Xsite(Rline[START_ROW-15],START_ROW-15);
//    float width_up = widthR_up - widthL_up;
    if(r->breakpoint==0   //�յ���ʧ
       && !r->lost_count //�Ҷ�����
//       && (l->lost_total<4 || (l->lost_total>=4 && l->lost_head[0]<35))       
         )
    {
      int cnt = 0;
      for(int16 i = START_ROW-5;i>15;i--)
      {
        float deriva = Rline[i+2]- Rline[i];
        if(deriva>3)
          cnt++;
      }
      if(cnt<5)//�ұ���ֱ��
      {
//        gpio_toggle(B9);
        pic->elem_type = CIRCLE;
        pic->circle_e = LEFT_END;
        
      }
    }
  }
  if(pic->elem_type == CIRCLE && pic->circle_e == LEFT_END)
  {
    track_again_huandao(60,10,'l');//������Ϊ��׼������ɨ��
    find_breakpoint_up(&left_boder,Lline,15,'l');//���Ϲյ�
    float widthL= GetTure_Xsite(Lline[START_ROW-5],START_ROW-5);
    float widthR= GetTure_Xsite(Rline[START_ROW-5],START_ROW-5);
    float width = widthR - widthL;
    if(l->breakpoint_up==0 && l->breakpoint==0 && width<45 //�ұ��޹յ㣬�����������
       && (l->lost_total<3 ||(l->lost_total>3 &&l->lost_total<10 && l->lost_head[0]<40))//����޶��ߣ����߳�����Ϊʮ��
         )
    {
//      gpio_toggle(B9);
      pic->elem_type = COMMON;
      pic->circle_e = NOCICLE;
      motor.speed_set = (uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]));
      beep_off;
    }
  }
}

//�󻷴���
void circle_handle_l(circle_status_e flag, boder_status *l, boder_status *r)
{
  if(flag == LEFT)
  {
    true_pingyi(START_ROW, image_s.endline,'l');
//    line_pingyi(START_ROW,image_s.endline,8,'l');
  }
  if(flag == LEFT_BEFORE)
  {
    if(l->breakpoint_up)
    {
      if(Lline[l->breakpoint_up-1]-Lline[l->breakpoint_up+2]>35     //�Ϲյ����治����
         && Lline[l->breakpoint_up-1]-Lline[l->breakpoint_up+3]>35
         && Lline[l->breakpoint_up-1]-Lline[l->breakpoint_up+4]>40
         && Lline[l->breakpoint_up-1]-Lline[l->breakpoint_up+5]>40
               )
      {
        Arc_matchlineLY(Rline[START_ROW-1],START_ROW-1,Lline[l->breakpoint_up],image_s.endline,Rline,'o');
        track_again_fork(START_ROW-2,image_s.endline,'l');
      }
      else
      {
        true_pingyi(START_ROW, image_s.endline,'l');
      }
    }
    else
    {
      true_pingyi(START_ROW, image_s.endline,'l');
    }
  }
  if(flag == LEFT_IN)
  {
    track_again_huandao(START_ROW-1,image_s.endline,'r');//������Ϊ��׼��ɨ��
    true_pingyi(START_ROW, image_s.endline, 'r');
//    if(!l->breakpoint_up)//������Ϲյ���
//      line_pingyi(START_ROW,image_s.endline,8,'r');
//    else if(IsLimitedScope(l->breakpoint_up,25,40))
//      line_pingyi(START_ROW,image_s.endline,6,'r');
//    else
//      line_pingyi(START_ROW,image_s.endline,10,'r');
  }
  if(flag == LCIRCLE_IN)
  {
    if(r->breakpoint)
    {
      for(int i = r->breakpoint-2;i>image_s.endline+2;i--)
      {
        Rline[i] = Rline[i+1]-2;
        if(Rline[i]<L_INIT)
          Rline[i] = L_INIT;
        if(Lline[i]>Lline[i+1])
        {
          Lline[i] = Lline[i+1]-1;
          if(Lline[i]<L_INIT)
            Lline[i] = L_INIT;
        }
      }
      line_pingyi(START_ROW,image_s.endline,10,'l');
    }
    else
      line_pingyi(START_ROW,image_s.endline,12,'l');
//    matchline_pingyi(START_ROW,image_s.endline,'r');
////    track_again_huandao(80,image_s.endline,'r');
//    if(l->breakpoint)
//    {
//      for(int i = l->breakpoint-2;i>image_s.endline+2;i--)
//      {
//        Lline[i] = Lline[i+1]+2;
//        if(Lline[i]>R_INIT)
//          Lline[i] = R_INIT;
//        if(Rline[i]<Rline[i+1])
//        {
//          Rline[i] = Rline[i+1]+1;
//          if(Rline[i]>R_INIT)
//            Rline[i] = R_INIT;
//        }
//      }
//    }
//    line_pingyi(START_ROW,image_s.endline,10,'r');
  }
  if(flag == LEFT_OUT)
  {
    int cnt = 0;
    for(int16 i = START_ROW-5;i>15;i--)
    {
      if(Rline[i] - Rline[i-3] < 0 || Rline[i] - Rline[i-1] > 3)
        cnt++;
    }
    if(!r->lost_count && cnt<1)
    {
      true_pingyi(START_ROW, image_s.endline, 'l');
      line_pingyi(START_ROW,image_s.endline,4,'l');
    }
    else
    {
      track_again_huandao(START_ROW-10,image_s.endline,'r');
      if(r->breakpoint)
      {
        for(int i = r->breakpoint-2;i>image_s.endline+2;i--)
        {
          Rline[i] = Rline[i+1]-2;
          if(Rline[i]<L_INIT)
            Rline[i] = L_INIT;
          if(Lline[i]>Lline[i+1])
          {
            Lline[i] = Lline[i+1]-1;
            if(Lline[i]<L_INIT)
              Lline[i] = L_INIT;
          }
        }
      }
      true_pingyi(START_ROW, image_s.endline, 'r');
      
      if(r->breakpoint>25 && r->breakpoint<30)
        line_pingyi(START_ROW,image_s.endline,6,'l');
      else
        line_pingyi(START_ROW,image_s.endline,4,'l');
    }
  }
  if(flag == LEFT_END)
  {
    true_pingyi(START_ROW, image_s.endline, 'l');
  }
}


void fork_judge(boder_status *l, boder_status *r, pic_info_s * pic)
{
  if(pic->fork_e == NOFORK && pic->elem_type == COMMON)
  {
    if(l->breakpoint>25 && l->breakpoint<60
       &&r->breakpoint>25 && r->breakpoint<60)
    {
      bool b_2,b_4,b_6,b_8;//�յ�������յ㴦��ֱ���밴120��˥��
      bool b_ad_2,b_ad_4,b_ad_6,b_ad_8;//�յ�������յ㴦����Ӧ����С
      int16 l_break_dec = GetImage_Ysite((int16)l->breakpoint_dis+5);//��յ�����5cm��������λ��,120��
      int16 r_break_dec = GetImage_Ysite((int16)r->breakpoint_dis+5);//�ҹյ�����5cm��������λ��
      int16 l_break_add = GetImage_Ysite((int16)l->breakpoint_dis-5);//��յ�����5cm��������λ�ã�ֱ��
      int16 r_break_add = GetImage_Ysite((int16)r->breakpoint_dis-5);//�ҹյ�����5cm��������λ��

      b_2 = IsLimitedScope((Lline[l_break_dec]-Lline[l_break_dec-2]),2,14)
        && IsLimitedScope((Rline[r_break_dec-2]-Rline[r_break_dec]),2,14);
      b_4 = IsLimitedScope((Lline[l_break_dec]-Lline[l_break_dec-4]),4,22)
        && IsLimitedScope((Rline[r_break_dec-4]-Rline[r_break_dec]),4,22);
      b_6 = IsLimitedScope((Lline[l_break_dec]-Lline[l_break_dec-6]),6,30)
        && IsLimitedScope((Rline[r_break_dec-6]-Rline[r_break_dec]),6,30);
      b_8 = IsLimitedScope((Lline[l_break_dec]-Lline[l_break_dec-8]),9,35)
        && IsLimitedScope((Rline[r_break_dec-8]-Rline[r_break_dec]),9,35);
      
      b_ad_2 = IsLimitedScope((Lline[l_break_add]-Lline[l_break_add+2]),-2,2)
        && IsLimitedScope((Rline[r_break_add+2]-Rline[r_break_add]),-2,2);
      b_ad_4 = IsLimitedScope((Lline[l_break_add]-Lline[l_break_add+4]),-2,3)
        && IsLimitedScope((Rline[r_break_add+4]-Rline[r_break_add]),-2,3);
      b_ad_6 = IsLimitedScope((Lline[l_break_add]-Lline[l_break_add+6]),-1,5)
        && IsLimitedScope((Rline[r_break_add+6]-Rline[r_break_add]),-1,5);
      b_ad_8 = IsLimitedScope((Lline[l_break_add]-Lline[l_break_add+8]),-1,7)
        && IsLimitedScope((Rline[r_break_add+8]-Rline[r_break_add]),-1,7);
      
      if(b_2 && b_4 && b_6 && b_8 && b_ad_2 && b_ad_4 && b_ad_6 && b_ad_8)
      {
        fork_num++;
        //������ǵ�һ�����������Ǿ�ֱ�ӿ�����Ѿ�ʶ������
        if(fork_num>1)
        {
          pic->elem_type = FORK;
          pic->fork_e = JUDGE_NUMBER;
//          motor.speed_set=(uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]));
          
          //ÿ�����������л�����
          if(pic->fork_dir == 'r')
            pic->fork_dir = 'l';
          else if(pic->fork_dir == 'l')
            pic->fork_dir = 'r';
        }
        //��һ����������,�л���ʶ������״̬
        else if(fork_num == 1)
        {
          pic->elem_type = FORK;
          pic->fork_e = FORK_BEFORE;
        }
      }
    }
  }
  if(pic->fork_e == FORK_BEFORE && pic->elem_type == FORK)
  {
    if(front_art_data.type == 1)    //ʶ��������
    {
      pic->elem_type = FORK;
      pic->fork_e = JUDGE_NUMBER;
      beep_on;
      if(front_art_data.categroy%2) //����Ϊ�������ұ�
        pic->fork_dir = 'r';
      else
        pic->fork_dir = 'l';
    }
  }
  if(pic->fork_e == JUDGE_NUMBER && pic->elem_type == FORK)
  {
    float widthL = GetTure_Xsite(Lline[image_s.endline],image_s.endline);
    float widthR = GetTure_Xsite(Rline[image_s.endline],image_s.endline);
    float width = widthR - widthL;
    float width_startL = GetTure_Xsite(Lline[START_ROW-2],START_ROW-2);
    float width_startR = GetTure_Xsite(Rline[START_ROW-2],START_ROW-2);
    float width_start = width_startR - width_startL;

    if(pic->fork_dir == 'l')
    {
      if(r->lost_count == 0 && l->lost_count == 0 
         &&(l->breakpoint==0 || r->breakpoint==0)
         && width < 50 && width_start<50 && width_start>40//�����������   
         )      
      {
        pic->elem_type = FORK;
        pic->fork_e = FORK_IN;
  //      beep_on;
      }
    }
    else if(pic->fork_dir == 'r')
    {
      if(l->lost_count == 0 && r->lost_count == 0
         &&(l->breakpoint==0 || r->breakpoint==0)
         && width < 50 && width_start<50 && width_start>40//�����������    
            )
      {
        pic->elem_type = FORK;
        pic->fork_e = FORK_IN;
  //      beep_on;
      }
    }
  }
  if(pic->fork_e == FORK_IN && pic->elem_type == FORK)
  {
    if(IsLimitedScope(l->breakpoint,45,65) && IsLimitedScope(r->breakpoint,45,65))
    {
      bool b_2,b_4,b_6,b_8;//�յ�������յ㴦��ֱ���밴120��˥��
      bool b_ad_2,b_ad_4,b_ad_6,b_ad_8;//�յ�������յ㴦����Ӧ����С
      int16 l_break_dec = GetImage_Ysite((int16)l->breakpoint_dis+5);//��յ�����5cm��������λ��,120��
      int16 r_break_dec = GetImage_Ysite((int16)r->breakpoint_dis+5);//�ҹյ�����5cm��������λ��
      int16 l_break_add = GetImage_Ysite((int16)l->breakpoint_dis-5);//��յ�����5cm��������λ�ã�ֱ��
      int16 r_break_add = GetImage_Ysite((int16)r->breakpoint_dis-5);//�ҹյ�����5cm��������λ��
      
      b_2 = IsLimitedScope((Lline[l_break_dec]-Lline[l_break_dec-2]),2,14)
        || IsLimitedScope((Rline[r_break_dec-2]-Rline[r_break_dec]),2,14);
      b_4 = IsLimitedScope((Lline[l_break_dec]-Lline[l_break_dec-4]),4,22)
        || IsLimitedScope((Rline[r_break_dec-4]-Rline[r_break_dec]),4,22);
      b_6 = IsLimitedScope((Lline[l_break_dec]-Lline[l_break_dec-6]),6,30)
        || IsLimitedScope((Rline[r_break_dec-6]-Rline[r_break_dec]),6,30);
      b_8 = IsLimitedScope((Lline[l_break_dec]-Lline[l_break_dec-8]),9,35)
        || IsLimitedScope((Rline[r_break_dec-8]-Rline[r_break_dec]),9,35);
      
      b_ad_2 = IsLimitedScope((Lline[l_break_add]-Lline[l_break_add+2]),-2,2)
        || IsLimitedScope((Rline[r_break_add+2]-Rline[r_break_add]),-2,2);
      b_ad_4 = IsLimitedScope((Lline[l_break_add]-Lline[l_break_add+4]),-2,3)
        || IsLimitedScope((Rline[r_break_add+4]-Rline[r_break_add]),-2,3);
      b_ad_6 = IsLimitedScope((Lline[l_break_add]-Lline[l_break_add+6]),-1,5)
        || IsLimitedScope((Rline[r_break_add+6]-Rline[r_break_add]),-1,5);
      b_ad_8 = IsLimitedScope((Lline[l_break_add]-Lline[l_break_add+8]),-1,7)
        || IsLimitedScope((Rline[r_break_add+8]-Rline[r_break_add]),-1,7);
      
      if(b_2 && b_4 && b_6 && b_8 && b_ad_2 && b_ad_4 && b_ad_6 && b_ad_8)
      {
        pic->elem_type = FORK;
        pic->fork_e = FORK_END;
        //        beep_off;
      }
    }
  }
  
  if(pic->fork_e == FORK_END && pic->elem_type == FORK)
  {
    //    float widthL = GetTure_Xsite(Lline[image_s.endline],image_s.endline);
    //    float widthR = GetTure_Xsite(Rline[image_s.endline],image_s.endline);
    //    float width = widthR - widthL;
    float width_startL = GetTure_Xsite(Lline[START_ROW-2],START_ROW-2);
    float width_startR = GetTure_Xsite(Rline[START_ROW-2],START_ROW-2);
    float width_start = width_startR - width_startL;
    //    if(pic->fork_dir == 'l')
    //    {
    if(((!r->lost_count && !l->lost_count)||(r->lost_head[0]<45||l->lost_head[0]<45))//���ǵ�������Ϊʮ��
       //       &&(l->breakpoint==0 || r->breakpoint==0)
       && width_start < 50//��ֹ�������������   
         )      
    {
      //��������
      int cnt1 = 0;
      for(int16 i = START_ROW-3;i > 40;i--)
      {
        if(Lline[i-2] - Lline[i] < 0 || Lline[i-1] - Lline[i] > 4)
          cnt1++;
      }
      //��������
      int cnt2 = 0;
      for(int16 i = START_ROW-3;i > 40;i--)
      {
        if(Rline[i] - Rline[i-2] < 0 || Rline[i] - Rline[i-1] > 4)
          cnt2++;
      }
      if(cnt1<1 && cnt2<1)
      {
        pic->elem_type = COMMON;
        pic->fork_e = NOFORK;
        beep_off;
      }
    }
//    }
//    else if(pic->fork_dir == 'r')
//    {
//      if(((r->lost_total<=4 && l->lost_total<=4)||(r->lost_head[0]<45||l->lost_head[0]<45))
////         &&(l->breakpoint==0 || r->breakpoint==0)
//         && width < 46 && width_start < 50//��ֹ�������������    
//            )
//      {
//        pic->elem_type = COMMON;
//        pic->fork_e = NOFORK;
////        pic->fork_dir = 'n';
//        beep_off;
//      }
//    }
  }
  
}

void fork_handle(fork_status_e flag, char dir, boder_status *l, boder_status *r)
{
  if(flag == FORK_BEFORE)
  {
    track_again_fork(START_ROW-1,10,'l');
    track_again_fork(START_ROW-1,10,'r');
    
    if(l->breakpoint>FORK_STOP_DIS||r->breakpoint>FORK_STOP_DIS)
      motor.speed_set = 0;
    else if(!l->breakpoint && !r->breakpoint)
      motor.speed_set = SEEK_NUM_SPEED;
  }
  if(flag == JUDGE_NUMBER)
  {
//    motor.stop_flag = 1;
    if(dir == 'r')
    {
      track_again_fork(START_ROW-5,10,'r');//������Ϊ��׼ɨ����
      for(int i = START_ROW-1;i>START_ROW-10;i--)
      {
        if(Rline[i]<M_INIT+40)
          Rline[i] = R_INIT;
      }
      true_pingyi(START_ROW, 10,'l');//����ֱ�Ӳ�������ƽ��
      line_pingyi(START_ROW,10,8,'l');

    }
    else if(dir == 'l')
    {
      track_again_fork(START_ROW-5,10,'l');//������Ϊ��׼ɨ����
      for(int i = START_ROW-1;i>START_ROW-10;i--)
      {
        if(Lline[i]>M_INIT-40)
          Lline[i] = L_INIT;
      }
      true_pingyi(START_ROW, 10,'r');//����ֱ�Ӳ�������ƽ��
      line_pingyi(START_ROW,10,8,'r');
    }
//    motor.speed_set=(uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]));
    if(fork_num == 1)
      motor.speed_set=(uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]/2));
  }
  if(flag == FORK_IN)
  {
    if(!AprilTag)
      motor.speed_set=(uint16)((float)(FLASH_DATA[12]/FLASH_CONVERT[12]));
  }
  
  if(flag == FORK_END)
  {
    if(dir == 'r')
    {
      track_again_fork(START_ROW-5,10,'r');//������Ϊ��׼ɨ����
      true_pingyi(START_ROW, 10,'l');//����ֱ�Ӳ�������ƽ��
    }
    else if(dir == 'l')
    {
      track_again_fork(START_ROW-5,10,'l');//������Ϊ��׼ɨ����
      true_pingyi(START_ROW, 10,'r');//����ֱ�Ӳ�������ƽ��
    }
  }
}


//�����ж�����������'r'�һ�������ͻȻ�������ߣ�'l'�󻷵���������ͻȻ��������
bool huandao_judge_continuity(int8 start, int8 end, int16* line,char flag)
{
  int count=0;
  for(int i = start;i >= end; i--)
  {
    if(flag == 'r')
    {
      if(line[i] - line[i-2] > 20)
        count++;
    }
    else if(flag == 'l')
    {
      if(line[i-2] - line[i] > 20)
        count++;
    }
  }
  if(count > 1) return false;
  else          return true;
}

bool IsLimitedScope(int16 value, int16 low, int16 high)
{
  if(value >=low && value <=high)
    return true;
  else
    return false;
}

bool FloatIsLimitedScope(float value, float low, float high)
{
  if(value >=low && value <=high)
    return true;
  else
    return false;
}

float slope_range(int16 start, int16 end, float *line)
{
  float max, min;
  
  max = -999;
  min = 999;
  
  if(end < end_line)
    end = end_line;
  for(int i = start; i >= end; i--)
  {
    if(line[i] > max)
      max = line[i];
    if(line[i] < min)
      min = line[i];
  }
  return (max - min);
}

int deviate_mid(char flag,int16 end)
{
  int row,count=0,start = 50,step=5;
  if(flag == 'l'||flag == 'L')
  {
    for(row = start ; row > end ; row--)
    {
      if(Lline[row] > M_INIT-step)
        count++;
    }
  }
  if(flag == 'r'||flag == 'R')
  {
    for(row = start ; row > end ; row--)
    {
      if(Rline[row] < M_INIT+step)
        count++;
    }
  }
  if(count > start - end)
      return 0;
  else
      return 1;
}

void track_again_CrossIn(int16 start, int16 end, char flag)
{
  uint32 Sy = 0,Gx,Gy;
  int i,j;
  int16 Sx = 0;
  if(flag=='r'||flag=='R')
  {
    for(i = start - 2; i > end; i--)
    {

      start_point[i] = Rline[i+1] - R_Lline[i+1]/2;
      if(start_point[i]>R_INIT)
        start_point[i]=R_INIT-1;
      else if(start_point[i]<L_INIT)
        start_point[i]=L_INIT+1;
      for(j = start_point[i]-1; j > 0; j--)
      {
        Sx = image[i][j+1] - image[i][j-1];        
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            Lline[i] = j;
            break;
          }
        }
      }
      Lline[i] = j;
      if(Lline[i]<=1)Lline[i]=0;
      
      for(j = start_point[i]+1; j<R_INIT;j++)
      {
        Sx = image[i][j-1] - image[i][j+1];
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            Rline[i] = j;
            break;
          }
        } 
      }
      Rline[i] = j;
      if(Rline[i] >= R_INIT-1) Rline[i]=R_INIT;
    }
  }
  if(flag=='l'||flag=='L')
  {
    for(i = start - 2; i > end; i--)
    {//����ʼɨ������
      start_point[i] = Lline[i+1]+R_Lline[i+1]/2;
      if(start_point[i]>R_INIT)
        start_point[i]=R_INIT-1;
      else if(start_point[i]<L_INIT)
        start_point[i]=L_INIT+1;
      
      for(j = start_point[i]+1; j < R_INIT; j++)
      {
        Sx = image[i][j-1] - image[i][j+1];
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            Rline[i] = j;
            break;
            
          }
        }
      }
      Rline[i] = j;
      if(Rline[i]>=R_INIT-1)Rline[i]=R_INIT;
      
      for(j = start_point[i]-1; j > 0; j--)
      {
        Sx = image[i][j+1] - image[i][j-1]; 
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            Lline[i] = j;
            break;
          }
        }
      }
      Lline[i] = j;
      if(Lline[i] <= 1)   Lline[i] = 0;
    }
  }
}

//��������ɨ�ߣ�������Ϊ��׼ɨ���ߣ�������Ϊ��׼ɨ����
void track_again_fork(int16 start, int16 end, char flag)
{
  uint32 Sy = 0,Gx,Gy;
  int i,j;
  int16 Sx = 0;
  if(flag=='r'||flag=='R')
  {
    for(i = start-1; i > end; i--)
    {//������Ϊ��׼��ɨ����
      
      start_point[i] = Rline[i+1]-5;
      if(start_point[i]<M_INIT-20)
        start_point[i] = 139;
      
      for(j = start_point[i]+1; j<R_INIT;j++)
      {
        Sx = image[i][j-1] - image[i][j+1];
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            Rline[i] = j;
            break;
          }
        } 
      }
      Rline[i] = j;
      if(Rline[i] >= R_INIT-1) Rline[i]=R_INIT;
    }
  }
  if(flag=='l'||flag=='L')
  {
    for(i = start-1; i > end; i--)
    {//������Ϊ��׼ɨ����
      
      start_point[i] = Lline[i+1]+5;
      if(start_point[i]>M_INIT-20)
        start_point[i] = 20;
      
      for(j = start_point[i]-1; j > 0; j--)
      {
        Sx = image[i][j+1] - image[i][j-1]; 
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            Lline[i] = j;
            break;
          }
        }
      }
      Lline[i] = j;
      if(Lline[i] <= 1)   Lline[i] = 0;
    }
  }
}
//��������ɨ��
void track_again_huandao(int16 start,int16 end,char flag)
{
  uint32 Sy = 0,Gx,Gy;
  int i,j;
  int16 Sx = 0;
  if(flag=='l'||flag=='L')
  {
    for(i = start - 2; i > end; i--)
    {//�󻷵�,������Ϊ��׼������ɨ��

      start_point[i] = Rline[i+1] - R_Lline[i+1]/8;
      if(start_point[i]>R_INIT)
        start_point[i]=R_INIT-1;
      else if(start_point[i]<L_INIT)
        start_point[i]=L_INIT+1;
      for(j = start_point[i]-1; j > 0; j--)
      {
        Sx = image[i][j+1] - image[i][j-1];        
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            Lline[i] = j;
            break;
          }
        }
      }
      Lline[i] = j;
      if(Lline[i]<=1)Lline[i]=0;
      
      for(j = start_point[i]+1; j<R_INIT;j++)
      {
        Sx = image[i][j-1] - image[i][j+1];
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            Rline[i] = j;
            break;
          }
        } 
      }
      Rline[i] = j;
      if(Rline[i] >= R_INIT-1) Rline[i]=R_INIT;
    }
  }
  if(flag=='r'||flag=='R')
  {
    for(i = start - 2; i > end; i--)
    {//����ʼɨ������
      start_point[i] = Lline[i+1]+R_Lline[i+1]/8;
      if(start_point[i]>R_INIT)
        start_point[i]=R_INIT-1;
      else if(start_point[i]<L_INIT)
        start_point[i]=L_INIT+1;
      
      for(j = start_point[i]+1; j < R_INIT; j++)
      {
        Sx = image[i][j-1] - image[i][j+1];
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            Rline[i] = j;
            break;
            
          }
        }
      }
      Rline[i] = j;
      if(Rline[i]>=R_INIT-1)Rline[i]=R_INIT;
      
      for(j = start_point[i]-1; j > 0; j--)
      {
        Sx = image[i][j+1] - image[i][j-1]; 
        if(Sx > 0)
        {
          Gx = abs(image[i-1][j-1]+2*image[i][j-1]+image[i+1][j-1]-image[i-1][j+1]-2*image[i][j+1]-image[i+1][j+1]);
          Gy = abs(image[i-1][j-1]+2*image[i-1][j]+image[i-1][j+1]-image[i+1][j-1]-2*image[i+1][j]-image[i+1][j+1]);
          Sy = Gx + Gy;
          if(Sy > CONTRAST_THRESHOLD)
          {
            Lline[i] = j;
            break;
          }
        }
      }
      Lline[i] = j;
      if(Lline[i] <= 1)   Lline[i] = 0;
    }
  }
}

void matchline_pingyi(int16 start,int16 end,char flag)
{//ƽ��������Ȳ���
  int i=0;
  if(start<=end)  return;
  if(flag=='l'||flag=='L')//������
  {
    for(i=start;i>=end;i--)
    {
      Lline[i]=Rline[i]-R_Lline[i];
      if(Lline[i] <= 0) Lline[i] = 0;
    }
  }
  else if(flag=='r'||flag=='R')
  {
    for(i=start;i>=end;i--)
    {
      Rline[i]=Lline[i]+R_Lline[i];
      if(Rline[i] >= R_INIT) Rline[i] = R_INIT;
    }
  }
}
void true_pingyi(int16 start,int16 end,char flag)//͸�Ӳ���
{
  int i=0;
  if(start<=end)  return;
  if(flag=='l'||flag=='L')//������
  {
    for(i=start;i>=end;i--)
    {
      Lline[i] = GetImage_Xsite((int16)(GetTure_Xsite(Rline[i],i)- TRACK_WIDTH),i);
    }
  }
  else if(flag=='r'||flag=='R')
  {
    for(i=start;i>=end;i--)
    {
      Rline[i]=GetImage_Xsite((int16)(GetTure_Xsite(Lline[i],i)+TRACK_WIDTH),i);
    }
  }
}

void line_pingyi(int16 start, int16 end,float times, char flag)
{
  int i=0;
  if(start<=end)  return;
  if(flag=='l'||flag=='L')
  {
    for(i=start;i>=end;i--)
    {
      Lline[i]=Lline[i]+(int16)(R_Lline[i]/times);
      if(Lline[i] >= R_INIT) Lline[i] = R_INIT-5;
    }
  }
  else if(flag=='r'||flag=='R')
  {
    for(i=start;i>=end;i--)
    {
      Rline[i]=Rline[i]-(int16)(R_Lline[i]/times);
      if(Rline[i] < L_INIT) Rline[i] = L_INIT+5;
    }
  }
}

void extend_track(uint8 start, uint8 end, int16* line)//�Կ�ʼ��б��������������
{
    int8 row;
    float det;
    if(start<=end)  return ;
    det=(*(line+start)-*(line+start+3))/3.0;
    //б��Ϊ0,�����Ƕ��߲���
    if(!det&&(*(line+start)!=L_INIT&&*(line+start)!=R_INIT))    return;
    for(row=start-1;row>=end;row--)
    {
        *(line+row)=*(line+start-2)+(int)((start-1-row)*det);
        if(line[row]>R_INIT)   line[row] = R_INIT;
        if(line[row]<L_INIT)   line[row] = L_INIT;
    }
}

void ExtendTheTrack_down(uint8 start, uint8 end, int16* line)//�Կ�ʼ��б��������������
{
    int8 row;
    float det;
    if(start>=end)  return ;
    det=(*(line+start-3)-*(line+start))/3.0;
    //б��Ϊ0,�����Ƕ��߲���
    if(!det&&(*(line+start)!=L_INIT && *(line+start)!=R_INIT))    return;
    for(row=start-1;row<=end;row++)
    {
        *(line+row+1)=*(line+row)-(int)det;
        if(line[row]>R_INIT)   line[row] = R_INIT;
        if(line[row]<L_INIT)   line[row] = L_INIT;
    }
}

void Arc_matchlineLY(int16 col1, int16 row1, int16 col2, int16 row2, int16* line, char d)//����Բ������(ԭ����ΪY��)
{
//    if(row1<=row2||col1>=col2)  return;
    row1 = row1 > START_ROW-1 ? START_ROW-1 : row1;
    row2 = row2 < 1 ? 1 : row2;
    int16 x0 = col2-col1, y0 = row1-row2;
    int16 sign = 1;
    if(x0 < 0)  sign = -1;
    float R = (float)(((x0)*(x0) + (y0)*(y0))/(y0<<1));
    float R2 = R*R;
    float loop;
    for(int16 i = row1;i >= row2;i--)
    {
        if(d == 'U')
        {
            loop = R2 -(R-row1+i)*(R-row1+i);
            line[i] = (int16)(col1 + sign * sqrt(loop));
        }
        else
        {
            loop = R2 -(R+row2-i)*(R+row2-i);
            line[i] = (int16)(col2 - sign * sqrt(loop));
        }
        if(line[i] < 0)  line[i] = 0;
        else if(line[i] > R_INIT)  line[i] = R_INIT;
    }
}

float GetTure_Ysite(uint8 y) //��ͼ����У������ʵ������ͷ���Ӿ���
{
    static float temp,a,b;
    a=(camera.Height *camera.s1/camera.Fy* y -(camera.Height *camera.s1*camera.Cy /camera.Fy) - camera.Height *camera.c1);
    b=(-camera.c1 /camera.Fy*y+ (camera.c1* camera.Cy /camera.Fy) - camera.s1);
    temp=a/b;
    return temp;
}
                       //��   ��
float GetTure_Xsite(float x,uint8 y)//��ͼ������꣬�õ������ߵ�ʵ�ʾ���
{
    static float temp,c,d;
    c=(-camera.Height/camera.Fx*x+camera.Height*camera.Cx/camera.Fx);
    d=(-camera.c1 /camera.Fy*y+ (camera.c1* camera.Cy /camera.Fy) - camera.s1);
    temp=c/d;
    return temp;
}


int16 GetImage_Ysite(int16 Y)//����ʵ�ľ�����ͼ���ϵ���
{
    static float e,f;
    static int16 temp;
    e=(camera.c1 *camera.Cy/camera.Fy-camera.s1)*Y+(camera.Height *camera.s1*camera.Cy /camera.Fy) + camera.Height *camera.c1;
    f=(camera.c1 /camera.Fy*Y+ camera.Height* camera.s1 /camera.Fy);
    temp=(int16)(e/f);
    if(temp<0)return 0;
    else if(temp>=119)return 119;
    else return temp;
}
int16 GetImage_Xsite(int16 X,int16 y)//����ʵ�����꣬��ͼ������
{
    static float a,b;
    static int16 temp;
    a = (-camera.c1 /camera.Fy*y+ (camera.c1* camera.Cy /camera.Fy) - camera.s1);
    b = camera.Fx/camera.Height;    
    temp=(int16)(camera.Cx-a*b*X);
    if(temp<0)return 0;
    else if(temp>=159)return 159;
    else return temp;
}

void GetWorldLine()
{
    int xi=0,yi=0,i=0; 
    static int16 start,end;
    uint8 start_flag=0,end_flag=0; 
    memset(WMline,WORLD_W,sizeof(WMline));
      /*********��͸��������********/
     for(i=119 ;i>camera.ENDdistance;i--)//����
    {
         yi=(int16)(WORLD_H-1-GetTure_Ysite(i));
         xi=(int16)(WORLD_W/2-0.5+(GetTure_Xsite(Mline[i],i)));
         if(yi<0||xi<0||xi>=WORLD_W)break;
         else if(yi<WORLD_H)WMline[yi]=xi;
    }
    for(i=WORLD_H-1 ;i>camera.ENDdistance;i--)//���߲�ֵ����
    {
        if(WMline[i]==WORLD_W && !start_flag)
        { 
            start=i+1;
            start_flag=1;
        }
        else if(start_flag && WMline[i] !=WORLD_W)
        {
            end=i+1;
            end_flag=1;
        }
        if(start_flag && end_flag )
        {
            start_flag=0;end_flag=0;
            if(start==WORLD_H)matchline(start,WMline[end-1],end,WMline[end-1],WMline);
            else matchline(start,WMline[start+1],end,WMline[end-1],WMline);
        }
    }
}

//������͸��
void GetWorldEdge()
{
    static int xi=0,yi=0, i=0; 
    static int16 start,end;
    uint8 start_flag=0,end_flag=0; 
    memset(WLline,WORLD_W,sizeof(WLline));
    Memset(WRline,0,WORLD_H,R_INIT);
    /*********��͸������********/
    for(i=119 ;i>camera.ENDdistance;i--)//����
    {
         yi=(int16)(WORLD_H-1-GetTure_Ysite(i));
         xi=(int16)(WORLD_W/2-1+(GetTure_Xsite(Lline[i],i)));
         if(yi<0||xi<0||xi>=WORLD_W)break;
         else if(yi<WORLD_H)WLline[yi]=xi;
    }
    start_flag=0,end_flag=0; 
    for(i=WORLD_H-1 ;i>camera.ENDdistance;i--)//���߲�ֵ����
    {
        if(WLline[i]==WORLD_W && !start_flag)
        { 
            start=i+1;
            start_flag=1;
        }
        else if(start_flag && WLline[i] !=WORLD_W)
        {
            end=i+1;
            end_flag=1;
        }
        if(start_flag && end_flag )
        {
            start_flag=0;end_flag=0;
            if(start==WORLD_H)matchline(start,WLline[end-1],end,WLline[end-1],WLline);
            else matchline(start,WLline[start+1],end,WLline[end-1],WLline);
        }
    }
    /*********��͸������********/
    for(i=119 ;i>camera.ENDdistance;i--)//����
    {
         yi=(int16)(WORLD_H-1-GetTure_Ysite(i));
         xi=(int16)(WORLD_W/2-0.5+(GetTure_Xsite(Rline[i],i)));
         if(yi<0||xi<0||xi>=WORLD_W)break;
         else if(yi<WORLD_H)WRline[yi]=xi;
    }
    start_flag=0,end_flag=0; 
    for(i=WORLD_H-1 ;i>camera.ENDdistance;i--)//���߲�ֵ����
    {
        if(WRline[i]==WORLD_W && !start_flag)
        { 
            start=i+1;
            start_flag=1;
        }
        else if(start_flag && WRline[i] !=WORLD_W)
        {
            end=i+1;
            end_flag=1;
        }
        if(start_flag && end_flag )
        {
            start_flag=0;end_flag=0;
            if(start==WORLD_H)matchline(start,WRline[end-1],end,WRline[end-1],WRline);
            else matchline(start,WRline[start+1],end,WRline[end-1],WRline);
        }
    }
    
//    /*********��͸������********/
//    for(i=119 ;i>camera.ENDdistance;i--)//����
//    {
//         yi=(int16)(WORLD_H-1-GetTure_Ysite(i));
//         xi=(int16)(WORLD_W/2-0.5+(GetTure_Xsite(Mline[i],i)));
//         if(yi<0||xi<0||xi>=WORLD_W)break;
//         else if(yi<WORLD_H)WMline[yi]=xi;
//    }
//    start_flag=0,end_flag=0; 
//    for(i=WORLD_H-1 ;i>camera.ENDdistance;i--)//���߲�ֵ����
//    {
//        if(WMline[i]==WORLD_W && !start_flag)
//        { 
//            start=i+1;
//            start_flag=1;
//        }
//        else if(start_flag && WMline[i] !=WORLD_W)
//        {
//            end=i+1;
//            end_flag=1;
//        }
//        if(start_flag && end_flag )
//        {
//            start_flag=0;end_flag=0;
//            if(start==WORLD_H)matchline(start,WMline[end-1],end,WMline[end-1],WMline);
//            else matchline(start,WMline[start+1],end,WMline[end-1],WMline);
//        }
//    }
}

//��͸��ȫͼ
void GetWorldImage()
{
    static int xi=0,yi=0, i=0,j=0; 
//    static int16 start,end;
//    uint8 start_flag=0,end_flag=0; 
    memset(WLline,0,sizeof(WLline));
    Memset(WRline,0,WORLD_H,R_INIT);

     /**********��͸��ȫͼ***********/
   for(i= 0;i<WORLD_H;i++)
   {
     for(j=0;j<WORLD_W;j++)
     {
       xy_image[i][j]=0;
     }
   }
   for(i=119 ;i>camera.ENDdistance;i--)
   {
     yi=(int16)(WORLD_H-1-GetTure_Ysite(i));
     for(j=0;j<MT9V03X_CSI_W;j++)
     {
       xi=(int16)(WORLD_W/2-0.5+(GetTure_Xsite(j,i)));
       if(yi>=0&&yi<WORLD_H&&xi>=0&&xi<WORLD_W)
       {
         xy_image[yi][xi]=image[i][j];
       }
     }
   }
}

void promatchline(int16 start,int16 end,int16 matstart,int16 matend,int16 * line)
{
    float b,k;
    int16 i,divide;
    divide=line[start]-line[end];
    if(divide!=0)
    {
        k=(start-end)*1.0/divide;
        b=( start-k*line[start] )*1.0;
        for(i=matstart;i>=matend;i--)
        {
            line[i]= (int16)((i-b)/k);
            if(line[i]<0)
              line[i]=0;
            else if(line[i]>159)
              line[i] = 159;
        }
    }
    
    
//  int8 row;
//  float det;
//  if(start<=end) return ;
//  det = (*(line+start-4) - *(line+start))/4.0;
//  //    det = det / 3;
//  if(!det && (*(line+start)!=0 || *(line+start)!=159))    return ;
//  for(row = end;row <= start;row++)
//    *(line+row) = *(line+start-2) - (int)((start-1-row)*det);
}


void matchline(int16 start,int16 startp,int16 end,int16 endp,int16 * line)
{
//    if(start == LOAD || end == LOAD)  return; 
    start = start > MT9V03X_CSI_H-1 ? MT9V03X_CSI_H-1 : start;
    end = end < 1 ? 1 : end;
    float a0,a1;
    int16 i,divide;
    divide = startp-endp;
     if(divide != 0)
     {
        a1 = (start-end)*1.0/divide;
        a0 = (start-a1*startp)*1.0;
         if(a1 != 0)
         {
            for(i = start-1;i >= end;i --)
                line[i] = (int16)((i-a0)/a1);
         }
     }
     else
     {
        for(i = start-1;i >= end;i --)
            line[i] = startp;
     }
}

void camera_para_init()
{
  camera.ZeroAngle=0;
  camera.FaceAngle=50.8;
  
//  camera.AxasLength=4.3;
//  camera.InterLength=13.3;
//  camera.Wheelradius=3.2;
  
  camera.Fx=61.4789;
  camera.Fy=61.7377;
  camera.Cx=78.6861;
  camera.Cy=62.3490;
  
  camera.Height=23.5;
  camera.c1 = cos(camera.FaceAngle*PI/180);
  camera.s1 = sin(camera.FaceAngle*PI/180);
  camera.vpy =(camera.Cy*camera.c1 - camera.Fy*camera.s1)/camera.c1;
  camera.ENDdistance=(int16)(camera.vpy<0?0:camera.vpy);
}

void world_image_show()
{
  uint16 temp = 0,color = 0;
  lcd_set_region(0,0,WORLD_W-1,WORLD_H-1);
  for(int i = 0; i < WORLD_H; i++)
  {
    for(int j = 0; j < WORLD_W; j++)
    {

      if(j == WRline[i])
        lcd_writedata_16bit(PURPLE);
      else if(j == WLline[i])
        lcd_writedata_16bit(GREEN);
      else
      {
        temp = xy_image[i][j];
        color=(0x001f&((temp)>>3))<<11;
        color=color|(((0x003f)&((temp)>>2))<<5);
        color=color|(0x001f&((temp)>>3));
        lcd_writedata_16bit(color);
      }
    }
  }
}
void Memset(int16 *arr, int16 start, int16 end, int16 val)
{
  for(int i = start;i < end;i++)
    arr[i] = val;
}