#include "CH455.h"
#include "zf_gpio.h"
#include "zf_systick.h"

#define SDAPin    C20
#define SCLPin    D12

#define high    1
#define low     0

#define     CH455_SCLPin_high           gpio_set (SCLPin, 1)
#define     CH455_SCLPin_low            gpio_set (SCLPin, 0)
#define     CH455_SCLPin_D_OUT          gpio_init (SCLPin, GPO,1, GPIO_PIN_CONFIG|OD_EN)   //  ����SCLPinΪ�������,����˫��I/O���л�Ϊ���
#define     CH455_SDAPin_high           gpio_set (SDAPin, 1)
#define     CH455_SDAPin_low            gpio_set (SDAPin, 0)
#define     CH455_SDAPin_IN             gpio_get(SDAPin)                    //  ��ȡSDAPin�����ƽ
#define     CH455_SDAPin_D_OUT          gpio_init (SDAPin, GPO,1, GPIO_PIN_CONFIG|OD_EN)   // ����SDAPinΪ�������,����˫��I/O���л�Ϊ���
#define     CH455_SDAPin_D_IN           gpio_init (SDAPin, GPI,1,GPIO_PIN_CONFIG|OD_EN)   // ����SDAPinΪ���뷽��,����˫��I/O���л�Ϊ����


void CH455_I2c_Start(void)
{
    CH455_SDAPin_D_OUT;
    CH455_SCLPin_high;
    systick_delay_us(1);
    CH455_SDAPin_high;
    systick_delay_us(1);
    CH455_SDAPin_low;
    systick_delay_us(1);
    CH455_SCLPin_low;
    systick_delay_us(1);
}
void CH455_I2c_Stop(void)
{
    CH455_SDAPin_D_OUT;
    CH455_SCLPin_low;
    systick_delay_us(1);
    CH455_SDAPin_low;
    systick_delay_us(1);
    CH455_SCLPin_high;
    systick_delay_us(1);
    CH455_SDAPin_high;
    systick_delay_us(1);
}
void CH455_I2c_WrByte(unsigned char IIC_Byte) //IIC����һ���ֽ�
{
    unsigned char i;
    CH455_SDAPin_D_OUT;
    for(i = 0; i < 8; i++)
    {
        if(IIC_Byte & 0x80) CH455_SDAPin_high;   //SDAPin=high;
        else  CH455_SDAPin_low;
        systick_delay_us(1);
        CH455_SCLPin_high;
        systick_delay_us(1);
        CH455_SCLPin_low;
        systick_delay_us(1);
        IIC_Byte<<=1;
    }
    systick_delay_us(1);
    CH455_SDAPin_high;
    systick_delay_us(1);
    CH455_SCLPin_high;
    systick_delay_us(1);
    CH455_SCLPin_low;
    systick_delay_us(1);
}

unsigned char  CH455_I2c_RdByte(void)     //��һ���ֽ�����
{
    unsigned char i, bytedata;
    CH455_SDAPin_high;      //����������
    systick_delay_us(1);
    CH455_SDAPin_D_IN;     //����������Ϊ����ģʽ
    systick_delay_us(1);
    CH455_SCLPin_low;
    systick_delay_us(1);
    for(i = 0; i < 8; i++)      //��8λ����
    {
        CH455_SCLPin_high;
        systick_delay_us(1);
        bytedata <<= 1;
        systick_delay_us(1);
        bytedata |= (CH455_SDAPin_IN);
        systick_delay_us(1);
        CH455_SCLPin_low;
        systick_delay_us(1);
    }
    CH455_SDAPin_D_OUT;            //���������û����ģʽ
    CH455_SDAPin_high;
    systick_delay_us(1);
    CH455_SCLPin_high;
    systick_delay_us(1);
    CH455_SCLPin_low;
    systick_delay_us(1);
    return(bytedata);//��������
}

void CH455_Write(uint16 cmd)  //д����
{
    CH455_I2c_Start();               //��������
    CH455_I2c_WrByte(((uint8)(cmd>>7)&CH455_I2C_MASK)|CH455_I2C_ADDR);
    CH455_I2c_WrByte((uint8)cmd);     //��������
    CH455_I2c_Stop();                 //��������
}

uint8 CH455_Read(void)        //��ȡ����
{
    uint8 keycode;
    CH455_I2c_Start();                //��������
    CH455_I2c_WrByte(((uint8)(CH455_GET_KEY>>7)&(CH455_I2C_MASK))|(0x01)|(CH455_I2C_ADDR));//��ȡ����
    keycode=CH455_I2c_RdByte();      //��ȡ����
    CH455_I2c_Stop();                //��������
    return keycode;
}
void CH455_Init(void)
{
    CH455_SCLPin_D_OUT;
    CH455_SDAPin_D_OUT;
    CH455_Write( CH455_SYSOFF );
    CH455_Write( CH455_SYSON );  //���������ź�
    CH455_SDAPin_high;
}
