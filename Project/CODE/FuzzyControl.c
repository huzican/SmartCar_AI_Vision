#include "FuzzyControl.h"

const int16 QMAX = 50 ;       //赛道实测最大偏差-10
const int16 QMIN = 2 ;       //小于PMIN的值将被当成      赛道越简单，速度越快，否则越是复杂就越慢
const int16 RMAX = 120;       //这个值表示图像后RMAX行空的多少，空的越多，速度越慢
const int16 RMIN = 5;       // 前瞻
const int16 TMAX = 98;      //两个参数都统一到0~100的值（归一化）
const int16 TMIN = 2 ;
const int16 OMAX = 100;     //语言值的满幅值


              // 0   1   2    3   4   5   6   统一到最大值； 最小值；
//int16 DYY[7] = {2, 10, 15, 20, 30, 40, 50};    //偏差
////int16 PYY[7] = {15, 17, 20, 25, 30, 35, 40};    //前瞻
//int16 PYY[7] = {5, 25, 40, 50, 70, 100, 120};    //舵机输出
int16 DYY[7] = { 10, 20, 30, 50, 65, 80, 98 };    //偏差
int16 PYY[7] = { 10, 20, 30, 50, 65, 80, 98 };    //前瞻

int16 UYY1[7] = {0, 70, 120, 190, 250, 320 ,400};//0, 70, 120, 190, 250, 320 ,400
int16 UYY2[7] = {0, 50, 100, 140, 160, 200 ,250};//旅游档差速


//float UYY_P1[7] ={1.05, 1.08, 1.11, 1.13, 1.15, 1.16 ,1.17};
////0.82, 0.81, 0.80, 0.76, 0.72, 0.70, 0.68, 0.70, 0.72, 0.76, 0.80, 0.81 ,0.82
////0.83, 0.81, 0.80, 0.79, 0.75, 0.70, 0.65, 0.70, 0.75, 0.79, 0.80, 0.81 ,0.83
////0.85, 0.83, 0.81, 0.80, 0.72, 0.68, 0.65, 0.68, 0.72, 0.80, 0.81, 0.83 ,0.85
//float UYY_D1[7] ={1.0, 1.0, 1.2, 2.4, 2.8, 3.2 ,3.6};

uint16 rule_Casu[7][7] =
{
//  //    /*P*/                //  0,  1,  2,  3,  4,  5,  6 /*D*/
//  /* 0 */                   {  0,  0,  0,  0,  1,  1,  1,} , //U语言数组里面的下标
//  /* 1 */                   {  1,  1,  1,  2,  2,  2,  3,} ,
//  /* 2 */                   {  2,  2,  2,  3,  3,  3,  4,} ,
//  /* 3 */                   {  3,  3,  3,  3,  4,  4,  5,} ,
//  /* 4 */                   {  3,  3,  4,  4,  5,  5,  5,} ,
//  /* 5 */                   {  4,  4,  4,  5,  5,  6,  6,} ,
//  /* 6 */                   {  4,  5,  5,  6,  6,  6,  6,} ,
//    /*P*/                //  0,  1,  2,  3,  4,  5,  6 /*D*/
  /* 0 */                   {  0,  1,  2,  3,  4,  5,  6,} , //U语言数组里面的下标
  /* 1 */                   {  1,  1,  2,  3,  4,  5,  6,} ,
  /* 2 */                   {  2,  2,  2,  3,  4,  5,  6,} ,
  /* 3 */                   {  3,  3,  3,  3,  4,  5,  6,} ,
  /* 4 */                   {  4,  4,  4,  4,  4,  5,  6,} ,
  /* 5 */                   {  5,  5,  5,  5,  5,  5,  6,} ,
  /* 6 */                   {  6,  6,  6,  6,  6,  6,  6,} ,
 
};

int16 *UYY=UYY1;  //这样可以选择不用的U语言数组
//float *UYY_P=UYY_P1;
//float *UYY_D=UYY_D1;

/*  P 代表赛道复杂程度
D 表示前瞻
模糊控制算法通过这两个跟定量以及规则表给出当前应该输出的差速
*/
int16 FuzzySet_Casu(int16 P, int16 D)          //模糊运算引擎，返回速度值
{
   uint16 PF[2];
   uint16 DF[2];
   int16 UL[4] = {0};
   int16 Pn = 0, Dn = 0;
   int16 Un[4];
   float temp1,temp2;
   if(P < QMIN)
     P = QMIN;
   else
     if(P > QMAX)
       P = QMAX;

   P = (int16)((double)(P - QMIN) / (QMAX - QMIN) * (TMAX - TMIN) + TMIN); //归一化到TMIN ~ TMAX

   if(D < RMIN)
     D = RMIN;
   else
     if( D > RMAX)
     D = RMAX;

   D = (int16)((double)(D - RMIN) / (RMAX - RMIN) * (TMAX - TMIN) + TMIN) ;    //归一化到TMIN ~ TMAX

   /*隶属度的确定*/
   /*根据PD的指定语言获得有效的隶属度*/
   if(P > PYY[0] && P < PYY[6])
   {
        if (P <= PYY[1])
        {
          Pn = 1;
          PF[0] = (uint16)(OMAX * ((float)(PYY[1] - P) / (PYY[1] - PYY[0])));
        }
        else if (P <= PYY[2])
        {
          Pn = 2;
          PF[0] = (uint16)(OMAX * ((float)(PYY[2] - P) / (PYY[2] - PYY[1])));
        }
        else if (P <= PYY[3])
        {
          Pn = 3;
          PF[0] = (uint16)(OMAX * ((float)(PYY[3] - P) / (PYY[3] - PYY[2])));
        }
        else if (P <= PYY[4])
        {
          Pn = 4;
          PF[0] = (uint16)(OMAX * ((float)(PYY[4] - P) / (PYY[4] - PYY[3])));
        }
        else if (P <= PYY[5])
        {
          Pn = 5;
          PF[0] = (uint16)(OMAX * ((float)(PYY[5] - P) / (PYY[5] - PYY[4])));
        }
        else if (P <= PYY[6])
        {
          Pn = 6;
          PF[0] = (uint16)(OMAX * ((float)(PYY[6] - P) / (PYY[6] - PYY[5])));
        }
   }
   else if (P <= PYY[0])
   {
        Pn = 1;
        PF[0] = (uint16)(OMAX);
   }
   else if (P >= PYY[6])
   {
        Pn = 6;
        PF[0] = 0;
   }

   PF[1] = (uint16)(OMAX - PF[0]);


      if (D > DYY[0] && D < DYY[6])
      {
        if (D <= DYY[1])
        {
          Dn = 1;
          DF[0] = (uint16)(OMAX * ((float)(DYY[1] - D) / (DYY[1] - DYY[0])));
        }
        else if (D <= DYY[2])
        {
          Dn = 2;
          DF[0] = (uint16)(OMAX * ((float)(DYY[2] - D) / (DYY[2] - DYY[1])));
        }
        else if (D <= DYY[3])
        {
          Dn = 3;
          DF[0] = (uint16)(OMAX * ((float)(DYY[3] - D) / (DYY[3] - DYY[2])));
        }
        else if (D <= DYY[4])
        {
          Dn = 4;
          DF[0] = (uint16)(OMAX * ((float)(DYY[4] - D) / (DYY[4] - DYY[3])));
        }
        else if (D <= DYY[5])
        {
          Dn = 5;
          DF[0] = (uint16)(OMAX * ((float)(DYY[5] - D) / (DYY[5] - DYY[4])));
        }
        else if (D <= DYY[6])
        {
          Dn = 6;
          DF[0] = (uint16)(OMAX * ((float)(DYY[6] - D) / (DYY[6] - DYY[5])));
        }
      }
      else if (D <= DYY[0])
      {
        Dn = 1;
        DF[0] = (uint16)(OMAX);
      }
      else if (D >= DYY[6])
      {
        Dn = 6;
        DF[0] = 0;
      }

      DF[1] = (uint16)(OMAX - DF[0]);

      Un[0] = rule_Casu[Pn - 1][ Dn - 1];
      Un[1] = rule_Casu[Pn][ Dn - 1];
      Un[2] = rule_Casu[Pn - 1][ Dn];
      Un[3] = rule_Casu[Pn][ Dn];
      
//     /*（双线性差值法反模糊）*/
//     temp1=((float)DF[1]/OMAX*(Un[2]-Un[0])+Un[0]);
//     temp2=((float)DF[1]/OMAX*(Un[3]-Un[1])+Un[1]);
//     U=((float)PF[1]/OMAX*(temp2-temp1)+temp1);
//     //U = (int16)(temp1 / temp2);
//      return (int16)U;
      
      //计算R表，隶属函数求交集
	UL[0] = MIN(PF[0],DF[0]);
	UL[1] = MIN(PF[1],DF[0]);
	UL[2] = MIN(PF[0],DF[1]);
	UL[3] = MIN(PF[1],DF[1]);
	/*同隶属函数输出语言值求大*/
	if(Un[0] == Un[1])
	{
		if(UL[0]>UL[1])
			UL[1] = 0;
		else 
			UL[0] = 0;
	}
	if(Un[0] == Un[2])
	{
		if(UL[0]>UL[2])
			UL[2] = 0;
		else 
			UL[0] = 0;		
	}
	if(Un[0] == Un[3])
	{
		if(UL[0]>UL[3])
			UL[3] = 0;
		else 
			UL[0] = 0;		
	}
	if(Un[1] == Un[2])
	{
		if(UL[1]>UL[2])
			UL[2] = 0;
		else 
			UL[1] = 0;		
	}
    if(Un[1] == Un[3])
	{
		if(UL[1]>UL[3])
			UL[3] = 0;
		else 
			UL[1] = 0;		
	}
	if(Un[2] == Un[3])
	{
		if(UL[2]>UL[3])
			UL[3]  = 0;
		else 
			UL[2] = 0;		
	}
	/*重心法(按隶属度加权平均)反模糊*/
    /*Un[]原值为输出隶属函数标号，转换为隶属函数值*/

        
        Un[0] = UYY[Un[0]];
        Un[1] = UYY[Un[1]];
        Un[2] = UYY[Un[2]];
        Un[3] = UYY[Un[3]];
	
    temp1 = UL[0] * Un[0] + UL[1] * Un[1] + UL[2] * Un[2] + UL[3] * Un[3];
	temp2 = UL[0] + UL[1] + UL[2] + UL[3];
	return (int)(temp1 / temp2);  
}