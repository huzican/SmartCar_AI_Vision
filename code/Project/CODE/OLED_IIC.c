#include "headfile.h"
#include "OLED_IIC.h"


#define    SDA_                 C20
#define    SCL_                 D12
#define    SCL__high            gpio_set (SCL_, 1);
#define    SCL__low             gpio_set (SCL_, 0);
#define    SCL__OUT             gpio_init(SCL_, GPO, 1, GPIO_PIN_CONFIG|OD_EN);     //  ����SCL_PinΪ�������,����˫��I/O���л�Ϊ���
#define    SDA__high            gpio_set (SDA_, 1);
#define    SDA__low             gpio_set (SDA_, 0);
#define    SDA__OUT             gpio_init(SDA_, GPO, 1, GPIO_PIN_CONFIG|OD_EN);     //  ����SCL_PinΪ�������,����˫��I/O���л�Ϊ���

void delay()
{
//  systick_delay_us(5);
  simiic_delay();
}
void IIC_Start()
{
   SDA__OUT;
//   SCL__high;
//   delay();
//   SDA__high;
//   delay();
//   SDA__low;
//   delay();
//   SCL__low;
	SDA__high;
	SCL__high;
	simiic_delay();
	SDA__low;
	simiic_delay();
	SCL__low;
}

void IIC_Stop()
{
   SDA__OUT;
//   SCL__low;
//   delay();
//   SDA__low;
//   delay();
//   SCL__high;
//   delay();
//   SDA__high;
	SDA__low;
	SCL__low;
	simiic_delay();
	SCL__high;
	simiic_delay();
	SDA__high;
	simiic_delay();

}

// ͨ��I2C����дһ���ֽ�
void Write_IIC_Byte(unsigned char IIC_Byte)
{
    unsigned char i;
    SCL__low;
    for(i=0;i<8;i++)
    {
        if(IIC_Byte & 0x80)
                {SDA__high;}
        else
                {SDA__low;}
        SCL__high;
//        simiic_delay();
        SCL__low;
        IIC_Byte<<=1;
    }
    SDA__high;
    SCL__high;
    simiic_delay();
    SCL__low;
}

//OLEDд����
void OLED_Wrdat(unsigned char IIC_Data)
{
    IIC_Start();
    Write_IIC_Byte(0x78);
    Write_IIC_Byte(0x40);           //write data
    Write_IIC_Byte(IIC_Data);
    IIC_Stop();
}

//OLEDд����
void OLED_Wrcmd(unsigned char IIC_Command)
{
    IIC_Start();
    Write_IIC_Byte(0x78);            //Slave address,SA0=0
    Write_IIC_Byte(0x00);           //write command
    Write_IIC_Byte(IIC_Command);
    IIC_Stop();
}

//OLED ��������
void OLED_Set_POS(unsigned char x, unsigned char y) 
{ 
    OLED_Wrcmd(0xb0+y);
    OLED_Wrcmd(((x&0xf0)>>4)|0x10);
    OLED_Wrcmd((x&0x0f)|0x01);
} 

//OLEDȫ��
void OLED_fill(unsigned char bmp_dat) 
{
    unsigned char y,x;
    for(y=0;y<8;y++)
    {
        OLED_Wrcmd(0xb0+y);
        OLED_Wrcmd(0x01);
        OLED_Wrcmd(0x10);
        for(x=0;x<X_WIDTH;x++)
        OLED_Wrdat(bmp_dat);
    }
}

//OLED��λ
void OLED_CLS(void)
{
    unsigned char y,x;
    for(y=0;y<8;y++)
    {
        OLED_Wrcmd(0xb0+y);
        OLED_Wrcmd(0x01);
        OLED_Wrcmd(0x10);
        for(x=0;x<X_WIDTH;x++)
        OLED_Wrdat(0);
    }
}

//OLED��ʼ��
void OLED_Init(void)
{
    SCL__OUT;
    SDA__OUT;
    SCL__high;
    SDA__high;
    systick_delay_ms(500);//��ʼ��֮ǰ����ʱ����Ҫ��
    OLED_Wrcmd(0xae);//--turn off oled panel
    OLED_Wrcmd(0x00);//---set low column address
    OLED_Wrcmd(0x10);//---set high column address
    OLED_Wrcmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_Wrcmd(0x81);//--set contrast control register
    OLED_Wrcmd(Brightness_1); // Set SEG Output Current Brightness
    OLED_Wrcmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
    OLED_Wrcmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
    OLED_Wrcmd(0xa6);//--set normal display
    OLED_Wrcmd(0xa8);//--set multiplex ratio(1 to 64)
    OLED_Wrcmd(0x3f);//--1/64 duty
    OLED_Wrcmd(0xd3);//-set display offset  Shift Mapping RAM Counter (0x00~0x3F)
    OLED_Wrcmd(0x00);//-not offset
    OLED_Wrcmd(0xd5);//--set display clock divide ratio/oscillator frequency
    OLED_Wrcmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_Wrcmd(0xd9);//--set pre-charge period
    OLED_Wrcmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_Wrcmd(0xda);//--set com pins hardware configuration
    OLED_Wrcmd(0x12);
    OLED_Wrcmd(0xdb);//--set vcomh
    OLED_Wrcmd(0x40);//Set VCOM Deselect Level
    OLED_Wrcmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_Wrcmd(0x02);//
    OLED_Wrcmd(0x8d);//--set Charge Pump enable/disable
    OLED_Wrcmd(0x14);//--set(0x10) disable
    OLED_Wrcmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
    OLED_Wrcmd(0xa6);// Disable Inverse Display On (0xa6/a7)
    OLED_Wrcmd(0xaf);//--turn on oled panel
    OLED_fill(0x00); //��ʼ����
    OLED_Set_POS(0,0);
//        Draw_BMP_Y(0,0,128,8,BMP0);
//           OLED_P6x8Str_Y(12,1,"inf_up:"); 
//   LED_PrintValueF(64,1,888,4);

} 

//������������ʾ6*8һ���׼ASCII�ַ���  ��ʾ�����꣨x,y����yΪҳ��Χ0��7
void OLED_P6x8Str_Y(unsigned char x, unsigned char y,unsigned char *ch)
{
    unsigned char c=0,i=0,j=0;
    while (ch[j]!='\0')
    {
        c =ch[j]-32;
        if(x>126){x=0;y++;}
        OLED_Set_POS(x,y);
        for(i=0;i<6;i++)
        OLED_Wrdat(oled_6x8[c][i]);
        x+=6;
        j++;
    }
}

//������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7
void Draw_BMP_Y(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char  y1,unsigned char BMP[])
{
    unsigned int j=0;
    unsigned char x,y;

  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
    for(y=y0;y<y1;y++)
    {
        OLED_Set_POS(x0,y);
    for(x=x0;x<x1;x++)
        {
            OLED_Wrdat(BMP[j++]);
        }
    }
}
/**************************���������ʾ************************/
//==============================================================
//��������void LED_P6x8Char(uint8 x,uint8 y,uint8 ch);
//������������ʾһ��6x8��׼ASCII�ַ�
//������xΪ��ʾ�ĺ�����0~122��yΪҳ��Χ0��7��chҪ��ʾ���ַ�
//���أ���
//============================================================== 
void LED_P6x8Char(uint8 x,uint8 y,uint8 ch)
{
     uint8 c=0,i=0;//j=0;

    c =ch-32;
    if(x>122)
    {
        x=0;
        y++;
    }
    OLED_Set_POS(x,y);
    for(i=0;i<6;i++)
    {
        OLED_Wrdat(oled_6x8[c][i]);
    }
}
//==============================================================
//��������  void LED_PrintBMP(uint8 x0,uint8 y0,uint8 x1,uint8 y1,uint8 bmp[]);
//������������ʾBMPͼƬ  ��СΪ(x1-x0+1) �� ((y1-y0+1)*8)
//             BMPͼƬ��ȡҳ��yΪ0��8λ(��λ����)Ϊ��λȡ��(x1-x0+1)��
//            ��ȡҳ��yΪ1��8λ(��λ����)Ϊ��λȡ��(x1-x0+1)���Դ�����
//������x�ķ�ΧΪ0��127��yΪҳ�ķ�Χ0��7
//���أ���
//==============================================================
void LED_PrintBMP(uint8 x0,uint8 y0,uint8 x1,uint8 y1,uint8 bmp[])
{
    uint16 ii=0;
    uint8 x,y;
    for(y=y0;y<=y1;y++)
    {
        OLED_Set_POS(x0,y);
        for(x=x0;x<x1;x++)
        {
            OLED_Wrdat(bmp[ii++]);
        }
    }
}
//==============================================================
//��������  void LED_PrintValue(uchar x, uchar y, char data);
//������������һ��char����ת����3λ��������ʾ
//������x�ķ�ΧΪ0��122��yΪҳ�ķ�Χ0��7��dataΪ��Ҫת����ʾ����ֵ
//���أ���
//==============================================================
void LED_PrintValueC(uint8 x, uint8 y, char data)
{
    uint8 i,j,k;
    if(data < 0)
    {
        LED_P6x8Char(x,y,'-');
        data = - data;
    }
    else
    {
        LED_P6x8Char(x,y,'+');
    }
    i = data/100;
    j = (data%100)/10;
    k = data%10;
    LED_P6x8Char(x+6,y,i+48);
    LED_P6x8Char(x+12,y,j+48);
    LED_P6x8Char(x+18,y,k+48);
}

//==============================================================
//��������  void LED_PrintValueI(uchar x, uchar y, int data);
//������������һ��int����ת����5λ��������ʾ
//������x�ķ�ΧΪ0��120��yΪҳ�ķ�Χ0��7��dataΪ��Ҫת����ʾ����ֵ
//���أ���
//==============================================================
void LED_PrintValueI(uint8 x, uint8 y, int data)
{
    uint8 i,j,k,l,m;
    if(data < 0)
    {
        LED_P6x8Char(x,y,'-');
        data = - data;
    }
    else
    {
        LED_P6x8Char(x,y,'+');
    }

    l  = data/10000;
    m= (data%10000)/1000;
    i = (data%1000)/100;
    j = (data%100)/10;
    k = data%10;
    LED_P6x8Char(x+6,y,l+48);
    LED_P6x8Char(x+12,y,m+48);
    LED_P6x8Char(x+18,y,i+48);
    LED_P6x8Char(x+24,y,j+48);
    LED_P6x8Char(x+30,y,k+48);
}


/////  ��һ��int����ת����2λ��������ʾ    ___ZLJ    //////
void LED_PrintValueI2(uint8 x, uint8 y, int data)
{
    uint8 i,j;
    if(data < 0)
    {
        LED_P6x8Char(x,y,'-');
        data = - data;
    }
    else
    {
        LED_P6x8Char(x,y,'+');
    }

    i = data/10;
    j = data%10;
    LED_P6x8Char(x+6,y,i+48);
    LED_P6x8Char(x+12,y,j+48);
}

/////  ��һ��int����ת����4λ�з�����������ʾ    ___ZLJ    //////
void LED_PrintsignValueI4(uint8 x, uint8 y, int data)
{
    uint8 i,j,k,h;
    if(data<0)
        {
            data = -data;
            LED_P6x8Char(x,y,'-');
        }
         else
            LED_P6x8Char(x,y,'+');
        h = data/1000;
    k = (data%1000)/100;
    i = (data%100)/10;
    j = data%10;
        LED_P6x8Char(x+6,y,h+48);
    LED_P6x8Char(x+12,y,k+48);
    LED_P6x8Char(x+18,y,i+48);
        LED_P6x8Char(x+24,y,j+48);
}

/////  ��һ��int����ת����3λ��������ʾ    ___ZLJ    //////
void LED_PrintValueI3(uint8 x, uint8 y, int data)
{
    uint8 i,j,k;


    k = data/100;
    i = (data%100)/10;
    j = data%10;
        LED_P6x8Char(x,y,k+48);
    LED_P6x8Char(x+6,y,i+48);
    LED_P6x8Char(x+12,y,j+48);
}
/////  ��һ��int����ת����4λ��������ʾ    ___ZLJ    //////
void LED_PrintValueI4(uint8 x, uint8 y, int data)
{
    uint8 i,j,k,h;

        
    h = data/1000;
    k = (data%1000)/100;
    j = ((data%100)%100)/10;
        i = data%10;
        LED_P6x8Char(x,y,h+48);
    LED_P6x8Char(x+6,y,k+48);
    LED_P6x8Char(x+12,y,j+48);
        LED_P6x8Char(x+18,y,i+48);
}
/////  ��һ��float����ת����4λ�������Ž�����ʾ       //////
void LED_PrintsignValueF4(uint8 x, uint8 y, float data)
{
    uint8 i,j,k,h;
    if(data < 0.0000001)
        {
          data = - data;
          LED_P6x8Char(x, y,'-');
        }
        else
          LED_P6x8Char(x, y,'+');
        
    h = (int)data/1000;
    k = (((int)data)%1000)/100;
    j = ((((int)data)%100)%100)/10;
        i = ((int)data)%10;
        LED_P6x8Char(x+6,y,h+48);
    LED_P6x8Char(x+12,y,k+48);
    LED_P6x8Char(x+18,y,j+48);
        LED_P6x8Char(x+24,y,i+48);
}
/////  ��һ��int����ת����5λ��������ʾ    ___ZLJ    //////
void LED_PrintValueI5(uint8 x, uint8 y, int data)
{
    uint8 i,j,k,h,l;

        
    l = data/10000;
    h = (data%10000)/1000;
    k = ((data%10000)%1000)/100;
        j = (((data%10000)%1000)%100)/10;
        i = data%10;
        LED_P6x8Char(x,y,l+48);
    LED_P6x8Char(x+6,y,h+48);
    LED_P6x8Char(x+12,y,k+48);
        LED_P6x8Char(x+18,y,j+48);
        LED_P6x8Char(x+24,y,i+48);
}        
 void LED_PrintValueFP(uint8 x, uint8 y, uint16 data, uint8 num)
 {
    uint8 m,i,j,k;
    LED_P6x8Char(x, y, '.');
    m= data/1000;
    i = (data%1000)/100;
    j = (data%100)/10;
    k = data%10;
    switch(num)
    {
        case 1:     LED_P6x8Char(x+6,y,k+48);
                break;
        case 2:     LED_P6x8Char(x+6,y,j+48);
                LED_P6x8Char(x+12,y,k+48);
                break;
        case 3:         LED_P6x8Char(x+6,y,i+48);
                LED_P6x8Char(x+12,y,j+48);
                LED_P6x8Char(x+18,y,k+48);
                break;
        case 4:     LED_P6x8Char(x+6,y,m+48);
                LED_P6x8Char(x+12,y,i+48);
                LED_P6x8Char(x+18,y,j+48);
                LED_P6x8Char(x+24,y,k+48);
                break;
    }
 }

//==============================================================
//��������   void LED_PrintValueF(uchar x, uchar y, float data, uchar num);
//������������һ��float����ת������������5λ��С���ͷ��ŵ����ݲ�������ʾ
//������x�ķ�ΧΪ0��62��yΪҳ�ķ�Χ0��7��dataΪ��Ҫת����ʾ����ֵ�����������λ5λ  num��ʾ������С��λ0~4
//���أ���
//Bug�޸ģ�Tearow
//==============================================================
 void LED_PrintValueF(uint8 x, uint8 y, float data, uint8 num)
 {
    uint16 l,m,i,j,k;  //��ǧ��ʮ��
    uint16 databiti = 6; //����λ��
    uint16 tempdataui = 0;
    int tempdataii = (int)data; //��������
    long int tempdatalp = (long int)((data - (int)data)*10000); //ȡС��λ��4λ
    //����������ʾ   �������ѱ�ע��
    if(data < -0.0000001)  LED_P6x8Char(x, y,'-');
    else LED_P6x8Char(x, y,'+');
    if(tempdataii < 0)tempdataii = - tempdataii;  //ȥ���������ָ���
    tempdataui = (uint16)tempdataii;
    l  = tempdataui/10000;
    m= (tempdataui%10000)/1000;
    i = (tempdataui%1000)/100;
    j = (tempdataui%100)/10;
    k = tempdataui%10;
    if (l != 0)  //��λ
    {
        LED_P6x8Char(x+6,y,l+48);
        LED_P6x8Char(x+12,y,m+48);
        LED_P6x8Char(x+18,y,i+48);
        LED_P6x8Char(x+24,y,j+48);
        LED_P6x8Char(x+30,y,k+48);
    }
    else if(m != 0) //��λ
    {
        databiti = 5;
        LED_P6x8Char(x+6,y,m+48);
        LED_P6x8Char(x+12,y,i+48);
        LED_P6x8Char(x+18,y,j+48);
        LED_P6x8Char(x+24,y,k+48);
    }
    else if(i != 0) //��λ
    {
        databiti = 4;
        LED_P6x8Char(x+6,y,i+48);
        LED_P6x8Char(x+12,y,j+48);
        LED_P6x8Char(x+18,y,k+48);
    }
    else if(j != 0) //��λ
    {
            databiti = 3;
        LED_P6x8Char(x+6,y,j+48);
        LED_P6x8Char(x+12,y,k+48);
    }
    else
    {
        databiti = 2;
        LED_P6x8Char(x+6,y,k+48);
    }
    if(tempdatalp < 0)tempdatalp = - tempdatalp;    //ȥ��С�����ָ���
    switch(num)
    {
        case 0: break;
        case 1:  LED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp / 1000),num);break;
        case 2:  LED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp / 100),num);break;
        case 3:  LED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp / 10),num);break;
        case 4:  LED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp),num);break;
    }
 }
/*
 void LED_PrintValueF(uint8 x, uint8 y, float data, uint8 num)
 {
    uint8 l,m,i,j,k;  //��ǧ��ʮ��
    uint8 databiti = 6; //����λ��
    uint8 tempdataui = 0;
    int tempdataii = (int)data; //��������
    long int tempdatalp = (long int)((data - (int)data)*10000); //ȡС��λ��4λ
    //����������ʾ   �������ѱ�ע��
 // if(data < 0.0000001)  LED_P6x8Char(x, y,'-');
 // else LED_P6x8Char(x, y,'+');
    if(tempdataii < 0)tempdataii = - tempdataii;  //ȥ���������ָ���
    tempdataui = tempdataii;
    l  = tempdataui/10000;
    m= (tempdataui%10000)/1000;
    i = (tempdataui%1000)/100;
    j = (tempdataui%100)/10;
    k = tempdataui%10;
    if (l != 0)  //��λ
    {
        LED_P6x8Char(x+6,y,l+48);
        LED_P6x8Char(x+12,y,m+48);
        LED_P6x8Char(x+18,y,i+48);
        LED_P6x8Char(x+24,y,j+48);
        LED_P6x8Char(x+30,y,k+48);
    }
    else if(m != 0) //��λ
    {
        databiti = 5;
        LED_P6x8Char(x+6,y,m+48);
        LED_P6x8Char(x+12,y,i+48);
        LED_P6x8Char(x+18,y,j+48);
        LED_P6x8Char(x+24,y,k+48);
    }
    else if(i != 0) //��λ
    {
        databiti = 4;
        LED_P6x8Char(x+6,y,i+48);
        LED_P6x8Char(x+12,y,j+48);
        LED_P6x8Char(x+18,y,k+48);
    }
    else if(j != 0) //��λ
    {
            databiti = 3;
        LED_P6x8Char(x+6,y,j+48);
        LED_P6x8Char(x+12,y,k+48);
    }
    else
    {
        databiti = 2;
        LED_P6x8Char(x+6,y,k+48);
    }
    if(tempdatalp < 0)tempdatalp = - tempdatalp;    //ȥ��С�����ָ���
    switch(num)
    {
        case 0: break;
        case 1:  LED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp / 1000),num);break;
        case 2:  LED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp / 100),num);break;
        case 3:  LED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp / 10),num);break;
        case 4:  LED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp),num);break;
    }
 }
*/
 void LED_Cursor(uint8 cursor_column, uint8 cursor_row)
 {
    if(cursor_row != 0) //������
    {
        if(cursor_column == 1) OLED_Set_POS(0, cursor_row + 2);
        else  OLED_Set_POS(80 + (cursor_column - 2)*6, cursor_row + 2);
        OLED_Wrdat(0xFF);
        OLED_Wrdat(0xFF);
        OLED_Wrdat(0xFF);
        OLED_Wrdat(0xFF);
        OLED_Wrdat(0xFF);
        OLED_Wrdat(0xFF);
    }
 }
