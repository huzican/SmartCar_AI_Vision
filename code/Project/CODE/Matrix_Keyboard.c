#include "Matrix_Keyboard.h"
#include "zf_gpio.h"
#include "include.h"
#include "SEEKFREE_18TFT.h"

//              对应键值：0  1  2  3  4  5  6  7  8  9  .  ok
const uint8 key_value[]={76,71,79,87,70,78,86,69,77,85,68,84};

menu_status_s oled_menu;
parameter_t parameter;
msg_status_s msg_status;
uint16 old_value = 0;
uint16 new_value = 0;
uint16 KeyValue_Buff[5]={0};//按键值
sky  Sky;
float  Value_Last;

int8 show_type = 0;
/*按键分布示意图
    1      2      3
    4      5      6
    7      8      9
    .      0      ok
*/
//typedef enum
//{
//    key1=71,       key2=79,       key3=87,
//    key4=70,       key5=78,       key6=86,
//    key7=69,       key8=77,       key9=85,
//    key_dot=68,    key0=76,       key_ok=84,
//}key_status_e;

/*
 * 有效组合 1234    3.98    35.63   124.4
 * 无效组合 .389
 * 按OK后取值（或者溢出结束输入）
 * 否则按键为选项功能
 */

//对采集的任意五位值处理
int key_get_parm(uint16 *Data, uint8 flag)
{
  uint8 i,j,k=0,d=5;
  uint16 value;
 //*****  对应值：0  1  2  3  4  5  6  7  8  9  .  ok//
  uint8  Value[]={76,71,79,87,70,78,86,69,77,85,68,84};
  if(flag==0||flag==1) return 0;
  for(i=0;i<5;i++)           
    for(j=0;j<12;j++)
      if(Value[j]==Data[i]) 
      {
        Data[i]=j;
        if(Data[i]==11)  {Data[i]=0;k=i;}
      }
  for(i=0;i<5;i++)  if(Data[i]==10)   d=i;     //j位为小数位
  if(d==5)  
  {
    if(k==0)value=0;
    if(k==1)value=Data[0];
    if(k==2)value=Data[0]*10+Data[1];
    if(k==3)value=Data[0]*100+Data[1]*10+Data[2];
    if(k==4)value=Data[0]*1000+Data[1]*100+Data[2]*10+Data[3];
    Value_Last=(float)value;
  }
  else
  {
    if(Data[4]==0) {for(i=d;i<4;i++)   Data[i]=Data[i+1];}
    if(Data[4]!=0) {{for(i=d;i<4;i++)   Data[i]=Data[i+1];} Data[4]=0;}
    value=Data[0]*1000+Data[1]*100+Data[2]*10+Data[3];
    Value_Last=(float)value;
    for(i=4-d;i>0;i--) Value_Last=Value_Last/10.000;
  }
  if(Sky.unm_flag)
  {
    Value_Last=-Value_Last;
    Sky.unm_flag=0;
  }
  return 0;
}

//矩阵键盘中断服务函数
void KeyBoard_IRQhandle()
{
    if(!gpio_get(CH455_IN_PIN))
    {
        CH455_Write(CH455_SYSON);   //重新启动信号
        old_value = new_value;
        new_value = CH455_Read();
//        lcd_showint16(0,0,new_value);
        //OLED菜单控制
        if(oled_menu.set && oled_menu.show == 'T')//键盘输入键值
        {
          if(old_value == key_value[5] ) msg_status.ok_flag=1;
          if(msg_status.ok_flag == 1)
          {
            KeyValue_Buff[msg_status.cnt]=new_value;
            msg_status.cnt++;
            if(old_value == key_value[10] && new_value == key_value[10])//双击.退出输入键值
            {
              oled_menu.set = !oled_menu.set;
              old_value=0;
              msg_status.cnt=0;
              memset(KeyValue_Buff,0,sizeof(KeyValue_Buff));
            }
            if(msg_status.cnt==2&&old_value == key_value[0] && new_value == key_value[0])//双击0开始输入负值
            {
              Sky.unm_flag=1;
              old_value=0;
              msg_status.cnt=0;
              memset(KeyValue_Buff,0,sizeof(KeyValue_Buff));
            }
            if(msg_status.cnt==5||KeyValue_Buff[msg_status.cnt-1]== key_value[11])
            {                                                           //ok
                old_value=0;
                msg_status.cnt=0;
                msg_status.ok_flag=2;   //OK_flag=2表示采集一次值完毕
            }
          }
          key_get_parm(KeyValue_Buff,msg_status.ok_flag);
          if(msg_status.ok_flag==2) memset(KeyValue_Buff,0,sizeof(KeyValue_Buff));
          beep(50);
        }
        else if(new_value == key_value[2])// UP  2
        {
          if(oled_menu.show == 'T')
          {
            if(!oled_menu.set)  oled_menu.step -= 1;
          }
          beep(50);
        }
        else if(new_value == key_value[8])// DOWN  8
        {
          if(oled_menu.show == 'T')
          {
            if(!oled_menu.set)  oled_menu.step += 1;
          }
          beep(50);
        }
        else if(new_value == key_value[4])// LEFT   4
        {
          if(oled_menu.show == 'T')
          {
            if(!oled_menu.set)
            {
              oled_menu.page -= 1;
  //              data_set(Oled.page, 0);
  //              MY_FLASH("WO",0);
            }
            oled_menu.cls = 1;
          }
          beep(50);
        }
        else if(new_value == key_value[6])// RIGHT  6
        {
          if(oled_menu.show == 'T')
          {
            if(!oled_menu.set)
            {
              oled_menu.page += 1;
  
//                data_set(Oled.page, 0);
//                MY_FLASH("WO",0);
            }
            else  oled_menu.sign = !oled_menu.sign;
            oled_menu.cls = 1;
          }
          beep(50);
        }
        else if(new_value == key_value[5])//MID   5
        {
          oled_menu.set = !oled_menu.set;
          beep(50);
        }
      else if(new_value == key_value[10])// 刷新Flash   .
      {
        parameter_init();
        
        MY_FLASH("WA",0);
        DATA_SET();
        oled_menu.cls = 1;
        oled_menu.show = 'T';
        beep(50);
      }
      else if(new_value == key_value[0])// 显示切换   0
      {
        motor.stop_flag = 0;
      }
    }
//    lcd_showint16(3,3,oled_menu.page);
//      lcd_showint16(2,2,count++);
}



void TFT_menu()
{
    if(oled_menu.cls)//在显示之前判断是否需要清屏
        TFT_CLS();
    //选择页数跳出范围
    oled_menu.page = oled_menu.page > TFT_PAGE_MAX ? 1 : (oled_menu.page < 1 ? TFT_PAGE_MAX : oled_menu.page);
    //选择行数跳出范围
    oled_menu.step = oled_menu.step > TFT_STEP_MAX ? 1 : (oled_menu.step < 1 ? TFT_STEP_MAX : oled_menu.step);
    
    
      if(oled_menu.page != TFT_PAGE_MAX)    //最后一页用来看图像
      {
        //更新目前的行数，用'>'作为标识，若选中状态则表示为'>>'
        for(uint8 row = 1; row <= TFT_STEP_MAX; row++)
        {
            if(row == oled_menu.step)
            {
                if(oled_menu.set)
                    lcd_showstr(0,oled_menu.step,">>");
                else
                    lcd_showstr(0,oled_menu.step,">");
            }
            else
                lcd_showstr(0,row,"  ");
        }

      lcd_showint8(80,TFT_START_ROW,oled_menu.page);//显示当前页码
      }

    switch(oled_menu.page)
    {
        case 1:
        {
          lcd_showstr(TFT_HEAD,1,"EDL:");          lcd_showint32(N1,TFT_START_ROW+1,image_s.endline,3);
          lcd_showstr(TFT_HEAD,2,"error:");        lcd_showfloat(N1,TFT_START_ROW+2,image_s.error,4,2);
          lcd_showstr(TFT_HEAD,3,"ServoPWM:");     lcd_showint32(N1,TFT_START_ROW+3,(int)servo_pid.output,3);
          lcd_showstr(TFT_HEAD,4,"SpeedErr:");     lcd_showfloat(N1,TFT_START_ROW+4,motor.speed_error,3,4);
          lcd_showstr(TFT_HEAD,5,"SpeedL:");       lcd_showint32(N1,TFT_START_ROW+5,(int)motor.speed_set_left,3);
          lcd_showstr(TFT_HEAD,6,"SpeedR:");       lcd_showint32(N1,TFT_START_ROW+6,(int)motor.speed_set_right,3);
          lcd_showstr(TFT_HEAD,7,"StopFlag:");     lcd_showint32(N1,TFT_START_ROW+7,(int)motor.stop_flag,3);
        }break;
        case 2:
        {
          lcd_showstr(TFT_HEAD,1,"Llost1:");
          lcd_showint32(N1,1,left_boder.lost_head[0],3);  lcd_showint32(N2,1,left_boder.lost_tail[0],3);
          lcd_showstr(TFT_HEAD,2,"Llost2:");                                
          lcd_showint32(N1,2,left_boder.lost_head[1],3);  lcd_showint32(N2,2,left_boder.lost_tail[1],3);
          lcd_showstr(TFT_HEAD,3,"Rlost1:");                                
          lcd_showint32(N1,3,right_boder.lost_head[0],3); lcd_showint32(N2,3,right_boder.lost_tail[0],3);
          lcd_showstr(TFT_HEAD,4,"Rlost2:");                                 
          lcd_showint32(N1,4,right_boder.lost_head[1],3); lcd_showint32(N2,4,right_boder.lost_tail[1],3);
          lcd_showstr(TFT_HEAD,5,"LostCnt:");                                
          lcd_showint32(N1,5,left_boder.lost_count,3);    lcd_showint32(N2,5,right_boder.lost_count,3);
          lcd_showstr(TFT_HEAD,6,"LostTotal:");                              
          lcd_showint32(N1,6,left_boder.lost_total,3);    lcd_showint32(N2,6,right_boder.lost_total,3);
          lcd_showstr(TFT_HEAD,7,"LapCnt:");              lcd_showint32(N2,7,lap_cnt,3);
        }break;
        case 3:
        {
          uint16 a,b;
          a = 65;b=110;
          lcd_showstr(TFT_HEAD,1,"OpenartData:"); 
          lcd_showstr(TFT_HEAD,2,"F_t:");        lcd_showint32(TFT_HEAD,3,front_art_data.type,2);
          lcd_showstr(a,2,"F_c:");              lcd_showint32(a,3,front_art_data.categroy,2);
          lcd_showstr(b,2,"F_p:");              lcd_showfloat(b,3,front_art_data.probability,2,2);
          
          lcd_showint32(TFT_HEAD,4,left_art_data.image_type,2);                                       
          if(left_art_data.image_type == 1)//彩图
            lcd_showint32(TFT_HEAD,5,left_art_data.categroy,2);
          else if(left_art_data.image_type == 0)//灰度
            lcd_showint32(TFT_HEAD,5,10,2);
          lcd_showstr(a,4,"L_rx:");              lcd_showint32(a,5,left_art_data.rec_x,3);
          lcd_showstr(b,4,"L_wid:");             lcd_showint32(b,5,left_art_data.rec_width,3);
          
          lcd_showint32(TFT_HEAD,6,right_art_data.image_type,2);                                       
          if(right_art_data.image_type == 1)//彩图
            lcd_showint32(TFT_HEAD,7,right_art_data.categroy,2);
          else if(right_art_data.image_type == 0)//灰度
            lcd_showint32(TFT_HEAD,7,10,2);
          lcd_showstr(a,6,"R_rx:");              lcd_showint32(a,7,right_art_data.rec_x,3);
          lcd_showstr(b,6,"R_wid:");             lcd_showint32(b,7,right_art_data.rec_width,3);
        }break;
        case 4:
        {
            if(oled_menu.set && msg_status.ok_flag==2)//参数加减，固定模式，修完立马写入Flash
            {
                Sky.change_flag=1;
                switch(oled_menu.step)
                {

                    case 1:
                        LEFT_KP=(float)Value_Last;
                        dataf_set(LEFT_KP, 6);
                        MY_FLASH("WO",6);
                        break;
                    case 2:
                        LEFT_KI=(float)Value_Last;
                        dataf_set(LEFT_KI, 7);
                        MY_FLASH("WO",7);
                        break;
                    case 3:
                        RIGHT_KP=(float)Value_Last;
                        dataf_set(RIGHT_KP, 8);
                        MY_FLASH("WO",8);
                        break;
                    case 4:
                        RIGHT_KI=(float)Value_Last;
                        dataf_set(RIGHT_KI, 9);
                        MY_FLASH("WO",9);
                        break;
                    case 5:
                        SERVO_KP=Value_Last;
                        dataf_set(SERVO_KP, 10);
                        MY_FLASH("WO",10);
                        break;
                    case 6:
                        SERVO_KD=(float)Value_Last;
                        dataf_set(SERVO_KD, 11);
                        MY_FLASH("WO",11);
                        break;
                    case 7:
                        motor.speed_set=(int16)Value_Last;
                        dataf_set(motor.speed_set, 12);
                        MY_FLASH("WO",12);
                        break;
                }
            }
            lcd_showstr(TFT_HEAD,1,"LEFT_KP:");    lcd_showfloat(N1,1,LEFT_KP,2,3);
                        
            lcd_showstr(TFT_HEAD,2,"LEFT_KI:");    lcd_showfloat(N1,2,LEFT_KI,2,3);
                        
            lcd_showstr(TFT_HEAD,3,"RIGHT_KP:");     lcd_showfloat(N1,3,RIGHT_KP,2,3);
                        
            lcd_showstr(TFT_HEAD,4,"RIGHT_KI:");     lcd_showfloat(N1,4,RIGHT_KI,2,3);
                        
            lcd_showstr(TFT_HEAD,5,"SERVO_KP:");       lcd_showfloat(N1,5,SERVO_KP,2,3);
                        
            lcd_showstr(TFT_HEAD,6,"SERVO_KD:");    lcd_showfloat(N1,6,SERVO_KD,2,3);
                        
            lcd_showstr(TFT_HEAD,7,"SpeedSet:");    lcd_showint16(N1,7,motor.speed_set);

        }break;
        case 5:
        {
          if(oled_menu.set && msg_status.ok_flag == 2)//参数加减，固定模式，修完立马写入Flash
          {
            Sky.change_flag=1;
            switch(oled_menu.step)
            {
              case 1:
                  image_gain = (uint16)Value_Last;
                  dataf_set(image_gain, 0);
                  MY_FLASH("WO",0);
                  break;
              case 2:
                  exposure_time = (uint16)Value_Last;
                  dataf_set(exposure_time, 1);
                  MY_FLASH("WO",1);
                  break;
              case 3:
                  image_LRoffset = (uint16)Value_Last;
                  dataf_set(image_LRoffset, 2);
                  MY_FLASH("WO",2);
                  break;
              case 4:
                  END_TH = (uint16)Value_Last;
                  dataf_set(END_TH, 3);
                  MY_FLASH("WO",3);
                  break;
              case 5:
                  CONTRAST_THRESHOLD = (uint16)Value_Last;
                  dataf_set(CONTRAST_THRESHOLD, 4);
                  MY_FLASH("WO",4);
                  break;
              case 6:
                  gama = (float)Value_Last;
                  dataf_set(gama, 5);
                  MY_FLASH("WO",5);
                  break;
              case 7:
                  FORK_STOP_DIS = (uint16)Value_Last;
                  dataf_set(FORK_STOP_DIS, 21);
                  MY_FLASH("WO",21);
                  break;
            }
          }
          lcd_showstr(TFT_HEAD,1,"gain:");            lcd_showint16(N1,1,image_gain);
                      
          lcd_showstr(TFT_HEAD,2,"time:");            lcd_showint16(N1,2,exposure_time);
                      
          lcd_showstr(TFT_HEAD,3,"ofset:");          lcd_showint16(N1,3,image_LRoffset);
                      
          lcd_showstr(TFT_HEAD,4,"endTH:");          lcd_showint16(N1,4,END_TH);
                      
          lcd_showstr(TFT_HEAD,5,"SHOLD:");          lcd_showint16(N1,5,CONTRAST_THRESHOLD);
                      
          lcd_showstr(TFT_HEAD,6,"gama:");           lcd_showfloat(N1,6,gama,3,3);

          lcd_showstr(TFT_HEAD,7,"FORK_ST:");        lcd_showint16(N1,7,FORK_STOP_DIS);
        }break;
        
        case 6:
        {
          if(oled_menu.set && msg_status.ok_flag == 2)//参数加减，固定模式，修完立马写入Flash
          {
            Sky.change_flag=1;
            switch(oled_menu.step)
            {
              case 1:
                  RUN_DIR = (uint16)Value_Last;
                  dataf_set(RUN_DIR, 23);
                  MY_FLASH("WO",23);
                  break;
              case 2:
                  GARAGE_ERR1 = (uint16)Value_Last;
                  dataf_set(GARAGE_ERR1, 24);
                  MY_FLASH("WO",24);
                  break;
              case 3:
                  GARAGE_ERR2 = (uint16)Value_Last;
                  dataf_set(GARAGE_ERR2, 25);
                  MY_FLASH("WO",25);
                  break;
              case 4:
                  GARAGE_ERR3 = (uint16)Value_Last;
                  dataf_set(GARAGE_ERR3, 26);
                  MY_FLASH("WO",26);
                  break;
              case 5:
                  OUT_ERR = (uint16)Value_Last;
                  dataf_set(OUT_ERR, 27);
                  MY_FLASH("WO",27);
                  break;
              case 6:
                SPEED_ERR = (uint16)Value_Last;
                dataf_set(SPEED_ERR, 33);
                MY_FLASH("WO",33);
                break;
              case 7:
                GARAGE_ERR = (uint16)Value_Last;
                dataf_set(GARAGE_ERR, 34);
                MY_FLASH("WO",34);
                  break;
            }
          }
          lcd_showstr(TFT_HEAD,1,"RunDir:");             lcd_showint16(N1,1,RUN_DIR);
          lcd_showstr(TFT_HEAD,2,"GarErr1:");            lcd_showint16(N1,2,GARAGE_ERR1);
          lcd_showstr(TFT_HEAD,3,"GarErr2:");            lcd_showint16(N1,3,GARAGE_ERR2);
          lcd_showstr(TFT_HEAD,4,"GarErr3:");            lcd_showint16(N1,4,GARAGE_ERR3);
          lcd_showstr(TFT_HEAD,5,"OUT_ERR:");            lcd_showint16(N1,5,OUT_ERR);
          lcd_showstr(TFT_HEAD,6,"SPEDerr:");            lcd_showint16(N1,6,SPEED_ERR);
          lcd_showstr(TFT_HEAD,7,"GAR_ERRv:");            lcd_showint16(N1,7,GARAGE_ERR);
        }break;
        
        case 7:
        {
          if(oled_menu.set && msg_status.ok_flag == 2)//参数加减，固定模式，修完立马写入Flash
          {
            Sky.change_flag=1;
            switch(oled_menu.step)
            {
              case 1:
                  CIR_SPEED1 = (uint16)Value_Last;
                  dataf_set(CIR_SPEED1, 28);
                  MY_FLASH("WO",28);
                  break;
              case 2:
                  CIR_SPEED2 = (uint16)Value_Last;
                  dataf_set(CIR_SPEED2, 29);
                  MY_FLASH("WO",29);
                  break;
              case 3:
                  CIR_SPEED3 = (uint16)Value_Last;
                  dataf_set(CIR_SPEED3, 30);
                  MY_FLASH("WO",30);
                  break;
              case 4:
                  CIR_SPEED4 = (uint16)Value_Last;
                  dataf_set(CIR_SPEED4, 31);
                  MY_FLASH("WO",31);
                  break;
              case 5:
                  CIR_NUM = (uint16)Value_Last;
                  dataf_set(CIR_NUM, 32);
                  MY_FLASH("WO",32);
                  break;
              case 6:
                  FIND_REC_V = (uint16)Value_Last;
                  dataf_set(FIND_REC_V, 35);
                  MY_FLASH("WO",35);
                  break;
              case 7:
                  OUT_SPEED = (uint16)Value_Last;
                  dataf_set(OUT_SPEED, 36);
                  MY_FLASH("WO",36);
                  break;
            }
          }
          lcd_showstr(TFT_HEAD,1,"CIR_V1:");             lcd_showint16(N1,1,CIR_SPEED1);
          lcd_showstr(TFT_HEAD,2,"CIR_V2:");             lcd_showint16(N1,2,CIR_SPEED2);
          lcd_showstr(TFT_HEAD,3,"CIR_V3:");             lcd_showint16(N1,3,CIR_SPEED3);
          lcd_showstr(TFT_HEAD,4,"CIR_V4:");             lcd_showint16(N1,4,CIR_SPEED4);
          lcd_showstr(TFT_HEAD,5,"CirNum:");             lcd_showint16(N1,5,CIR_NUM);
          lcd_showstr(TFT_HEAD,6,"REC_V:");              lcd_showint16(N1,6,FIND_REC_V);
          lcd_showstr(TFT_HEAD,7,"OUT_V:");              lcd_showint16(N1,7,OUT_SPEED);
        }break;

        case 8:
        {
            if(oled_menu.set && msg_status.ok_flag==2)//参数加减，固定模式，修完立马写入Flash
            {
                Sky.change_flag=1;
                switch(oled_menu.step)
                {
                    case 1:
                        TARGET_TIME = (uint16)Value_Last;
                        dataf_set(TARGET_TIME, 15);
                        MY_FLASH("WO",15);
                        break;
                    case 2:
                        ANIMAL_TIME = (uint16)Value_Last;
                        dataf_set(ANIMAL_TIME, 16);
                        MY_FLASH("WO",16);
                        break;
                    case 3:
                        RIGHT_REC_START = (uint16)Value_Last;
                        dataf_set(RIGHT_REC_START, 17);
                        MY_FLASH("WO",17);
                        break;
                    case 4:
                        RIGHT_REC_END = (uint16)Value_Last;
                        dataf_set(RIGHT_REC_END, 18);
                        MY_FLASH("WO",18);
                        break;
                    case 5:
                        LEFT_REC_START = (uint16)Value_Last;
                        dataf_set(LEFT_REC_START, 19);
                        MY_FLASH("WO",19);
                        break;
                    case 6:
                        LEFT_REC_END = (uint16)Value_Last;
                        dataf_set(LEFT_REC_END, 20);
                        MY_FLASH("WO",20);
                        break;
                    case 7:
                        SEEK_NUM_SPEED = (uint16)Value_Last;
                        dataf_set(SEEK_NUM_SPEED, 22);
                        MY_FLASH("WO",22);
                        break;
                }
            }
            lcd_showstr(TFT_HEAD,1,"TAR_T:");          lcd_showint16(N1,1,TARGET_TIME);
            lcd_showstr(TFT_HEAD,2,"ANI_T:");          lcd_showint16(N1,2,ANIMAL_TIME);
            lcd_showstr(TFT_HEAD,3,"rRECs:");          lcd_showint16(N1,3,RIGHT_REC_START);
            lcd_showstr(TFT_HEAD,4,"rRECe:");          lcd_showint16(N1,4,RIGHT_REC_END);
            lcd_showstr(TFT_HEAD,5,"lRECs:");          lcd_showint16(N1,5,LEFT_REC_START);
            lcd_showstr(TFT_HEAD,6,"lRECe:");          lcd_showint16(N1,6,LEFT_REC_END);
            lcd_showstr(TFT_HEAD,7,"NUM_V:");          lcd_showint16(N1,7,SEEK_NUM_SPEED);
            
        }break;
        case 9:
        {
          int a = 70, b = 120;
          
          lcd_showstr(TFT_HEAD,1,"ElemType:");          lcd_showint32(N2,1,pic_info.elem_type,3);
          lcd_showstr(TFT_HEAD,2,"CirStau:");           lcd_showint32(N2,2,pic_info.circle_e,3);
          lcd_showstr(TFT_HEAD,3,"Fork:");              lcd_showint32(N2,3,pic_info.fork_e,3);
          lcd_showstr(TFT_HEAD,4,"ForkDir:");
          if(pic_info.fork_dir == 'r')
            lcd_showstr(N2,4,"r");
          else if(pic_info.fork_dir == 'l')
            lcd_showstr(N2,4,"l");
          else
            lcd_showstr(N2,4," ");
          
          lcd_showstr(TFT_HEAD,5,"Cross:");             lcd_showint32(N1,5,pic_info.cross_e,3);
//          lcd_showstr(TFT_HEAD,6,"CrossDir:");
          if(pic_info.cross_dir == 'r')
            lcd_showstr(N2,5,"r");
          else if(pic_info.cross_dir == 'l')
            lcd_showstr(N2,5,"l");
          else if(pic_info.cross_dir == 'm')
            lcd_showstr(N2,5,"m");
          else
            lcd_showstr(N2,5,"n");
          
          lcd_showstr(TFT_HEAD,6,"ZEBRA:");          lcd_showint32(N2,6,pic_info.zebra_flag,3);
          
          lcd_showstr(TFT_HEAD,7,"GARA:");  
          if(pic_info.garage_dir =='l')
            lcd_showstr(b,7,"l");
          else if(pic_info.garage_dir =='r')
            lcd_showstr(b,7,"r");
          else
            lcd_showstr(b,7," ");
          lcd_showint32(a,7,pic_info.garage_e,1);
//          lcd_showint32(TFT_HEAD,6,a,3);lcd_showint32(N1,6,b,3);lcd_showint32(N2,6,c,3);
//          lcd_showint32(TFT_HEAD,5,d,3);lcd_showint32(N1,5,e,3);lcd_showint32(N2,5,f,3);
  
        }break;
        case 10:
        {
          uint16 a,b;
          a = 70;b=120;
          lcd_showstr(TFT_HEAD,1,"TAG:");          lcd_showint32(N2,1,AprilTag,3);
          lcd_showstr(TFT_HEAD,2,"TYPE:");         lcd_showint32(a,2,pic_info.apriltag_e,3); 
          if(pic_info.apriltag_dir == 'l')
            lcd_showstr(b,2,"l");
          else if(pic_info.apriltag_dir == 'r')
            lcd_showstr(b,2,"r");
          else
            lcd_showstr(b,2," ");
          
          
//           lcd_showint32(a,4,BM1_LEFT,3); 
//            lcd_showint32(a,5,BM2_LEFT,3); 
//          float widthD = GetTure_Xsite(Lline[image_s.endline+4],image_s.endline+4);
//          float widthU = GetTure_Xsite(Rline[image_s.endline+4],image_s.endline+4);
//          float err = widthU - widthD;
//          lcd_showfloat(N1,4,err,4,3);
          
//          lcd_showint32(N2,4,a1,3);
//          lcd_showint32(N2,4,b1,3);
//          int i = START_ROW-5;
//          float deriva = (Lline[i]- Lline[i+1]);
//          lcd_showfloat(N1,4,deriva,2,3);
//          i = START_ROW-10;
//          deriva = (Lline[i]- Lline[i+1]);;
//          lcd_showfloat(N1,5,deriva,2,3);
          lcd_showint32(N1,4,right_boder.breakpoint-right_boder.lost_head[0],3);
          
//         lcd_showint32(N1,4,Rline[right_boder.breakpoint-3]-Rline[right_boder.breakpoint+2],3);
//         lcd_showint32(N1,5,Rline[right_boder.breakpoint-4]-Rline[right_boder.breakpoint+2],3);
//         lcd_showint32(N1,6,Rline[right_boder.breakpoint-5]-Rline[right_boder.breakpoint+2],3);
//         lcd_showint32(N1,7,Rline[right_boder.breakpoint-6]-Rline[right_boder.breakpoint+2],3);

        }break;
        case 11:
        {
          int16 l_break_dec = GetImage_Ysite((int16)(left_boder.breakpoint_dis+5));//左拐点往上5cm处，像素位置,120度
          int16 r_break_dec = GetImage_Ysite((int16)(right_boder.breakpoint_dis+5));//右拐点往上5cm处，像素位置
          lcd_showstr(TFT_HEAD,1,"Inf:");
          lcd_showint32(N1,1,left_boder.breakpoint,3);  lcd_showint32(N2,1,right_boder.breakpoint,3);
          lcd_showstr(TFT_HEAD,2,"InfDec:");
          lcd_showint32(N1,2,l_break_dec,3);  lcd_showint32(N2,2,r_break_dec,3);
          lcd_showstr(TFT_HEAD,3,"b-2:");
          lcd_showint32(N1,3,Lline[l_break_dec]-Lline[l_break_dec-2],3);  
          lcd_showint32(N2,3,Rline[r_break_dec-2]-Rline[r_break_dec],3);
          lcd_showstr(TFT_HEAD,4,"b-4:");
          lcd_showint32(N1,4,Lline[l_break_dec]-Lline[l_break_dec-4],3);  
          lcd_showint32(N2,4,Rline[r_break_dec-4]-Rline[r_break_dec],3);
          lcd_showstr(TFT_HEAD,5,"b-6:");
          lcd_showint32(N1,5,Lline[l_break_dec]-Lline[l_break_dec-6],3);  
          lcd_showint32(N2,5,Rline[r_break_dec-6]-Rline[r_break_dec],3);
          lcd_showstr(TFT_HEAD,6,"b-8:");
          lcd_showint32(N1,6,Lline[l_break_dec]-Lline[l_break_dec-8],3);  
          lcd_showint32(N2,6,Rline[r_break_dec-8]-Rline[r_break_dec],3);
          lcd_showstr(TFT_HEAD,7,"break:");
          lcd_showint32(N1,7,Lline[l_break_dec],3);  lcd_showint32(N2,7,Rline[r_break_dec],3);
        }break;
        case 12:
        {
          int16 l_break_add = GetImage_Ysite((int16)(left_boder.breakpoint_dis-5));//左拐点往下5cm处，像素位置，直道
          int16 r_break_add = GetImage_Ysite((int16)(right_boder.breakpoint_dis-5));//右拐点往下5cm处，像素位置
          lcd_showstr(TFT_HEAD,1,"Inf:");
          lcd_showint32(N1,1,left_boder.breakpoint,3);  lcd_showint32(N2,1,right_boder.breakpoint,3);
          lcd_showstr(TFT_HEAD,2,"InfAdd:");
          lcd_showint32(N1,2,l_break_add,3);  lcd_showint32(N2,2,r_break_add,3);
          lcd_showstr(TFT_HEAD,3,"b+2:");
          lcd_showint32(N1,3,Lline[l_break_add]-Lline[l_break_add+2],3);  
          lcd_showint32(N2,3,Rline[r_break_add+2]-Rline[r_break_add],3);
          lcd_showstr(TFT_HEAD,4,"b+4:");
          lcd_showint32(N1,4,Lline[l_break_add]-Lline[l_break_add+4],3);  
          lcd_showint32(N2,4,Rline[r_break_add+4]-Rline[r_break_add],3);
          lcd_showstr(TFT_HEAD,5,"b+6:");
          lcd_showint32(N1,5,Lline[l_break_add]-Lline[l_break_add+6],3);  
          lcd_showint32(N2,5,Rline[r_break_add+6]-Rline[r_break_add],3);
          lcd_showstr(TFT_HEAD,6,"b+8:");
          lcd_showint32(N1,6,Lline[l_break_add]-Lline[l_break_add+8],3);  
          lcd_showint32(N2,6,Rline[r_break_add+8]-Rline[r_break_add],3);
          lcd_showstr(TFT_HEAD,7,"break:");
          lcd_showint32(N1,7,Lline[l_break_add],3);  lcd_showint32(N2,7,Rline[r_break_add],3);
        }break;
        case 13:
        {
          lcd_showstr(TFT_HEAD,1,"Inf:");
          lcd_showint32(N1,1,left_boder.breakpoint,3);  lcd_showint32(N2,1,right_boder.breakpoint,3);
          lcd_showstr(TFT_HEAD,2,"InfUP:");
          lcd_showint32(N1,2,left_boder.breakpoint_up,3);  lcd_showint32(N2,2,right_boder.breakpoint_up,3);
          lcd_showstr(TFT_HEAD,3,"Llost1:");
          lcd_showint32(N1,3,left_boder.lost_head[0],3);  lcd_showint32(N2,3,left_boder.lost_tail[0],3);
          lcd_showstr(TFT_HEAD,4,"Rlost1:");                                
          lcd_showint32(N1,4,right_boder.lost_head[0],3); lcd_showint32(N2,4,right_boder.lost_tail[0],3);
          lcd_showstr(TFT_HEAD,5,"LostCnt:");                                
          lcd_showint32(N1,5,left_boder.lost_count,3);    lcd_showint32(N2,5,right_boder.lost_count,3);
          lcd_showstr(TFT_HEAD,6,"LostTotal:");                              
          lcd_showint32(N1,6,left_boder.lost_total,3);    lcd_showint32(N2,6,right_boder.lost_total,3);
        }break;
        case 14:
        {
          world_image_show();
        }break;
    }
    
    if(oled_menu.cls)  TFT_CLS();
    if(msg_status.ok_flag==2 && Sky.change_flag==1)
    {
        oled_menu.set = !oled_menu.set;
        msg_status.ok_flag=0;
        Sky.change_flag=0;
    }
    oled_menu.up = 0;
    oled_menu.down = 0;
    oled_menu.cls = 0;
    oled_menu.res = 0;//清除标志位

}

void tft_Show(void)
{
    oled_menu.show ='T';
//    Oled_Menu();
    if(show_type == 0)
      TFT_menu();
    else if(show_type == 1)
    {
      image_show();
      edge_show();
    }
      
}


void TFT_CLS()
{
 	uint8 i,j;
	lcd_set_region(0,0,TFT_X_MAX-1,TFT_Y_MAX-1);
 	for (i=0;i<TFT_Y_MAX;i++)
    	for (j=0;j<TFT_X_MAX;j++)
        	lcd_writedata_16bit(WHITE);
}
