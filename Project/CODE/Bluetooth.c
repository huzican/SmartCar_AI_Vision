#include "Bluetooth.h"
#include "zf_uart.h"
#include "Motor.h"
#include "SEEKFREE_18TFT.h"
#include "Image.h"
#include "openart.h"

int16 var[8];
lpuart_handle_t     zigbee_lpuartHandle;
lpuart_transfer_t   zigbee_receivexfer; 
void vcan_sendware(void *wareaddr, uint32 waresize)
{
#define CMD_WARE     3
    uint8 cmdf[2] = {CMD_WARE, ~CMD_WARE};    //串口调试 使用的前命令
    uint8 cmdr[2] = {~CMD_WARE, CMD_WARE};    //串口调试 使用的后命令

    uart_putbuff(BULETOOTH_UART, cmdf, sizeof(cmdf));    //先发送前命令
    uart_putbuff(BULETOOTH_UART, (uint8 *)wareaddr, waresize);    //发送数据
    uart_putbuff(BULETOOTH_UART, cmdr, sizeof(cmdr));    //发送后命令
}

void zigbee_init(void)
{
    uart_init(ZIGBEE_UART, ZIGBEE_BAUD, ZIGBEE_TX, ZIGBEE_RX);
    NVIC_SetPriority(LPUART1_IRQn, 2);
    uart_set_handle(ZIGBEE_UART,&zigbee_lpuartHandle,zigbee_IRQHandle,NULL,0,zigbee_receivexfer.data,1);
    uart_rx_irq(ZIGBEE_UART,1);
}

void bluetooth_init(void)
{
    uart_init(BULETOOTH_UART, BULETOOTH_BAUD, BULETOOTH_TX,BLUETOOTH_RX);
}

void zigbee_IRQHandle()
{
  uint8 zigbee_data;
  uart_getchar(ZIGBEE_UART, &zigbee_data);

  if(zigbee_data == 1)
  {
    if(!motor.stop_flag)
      motor.stop_flag = 1;
    else
      motor.stop_flag = 0;
  }
  else if(zigbee_data == 2)
  {
    if(!image_s.show_flag)
      image_s.show_flag = 1;
    else
      image_s.show_flag = 0;
  }
//  if(zigbee_data == 3)
//  {
//    image_s.show_flag = 0;
//  }
//  else if(zigbee_data == 4)
//  {
//    image_s.show_flag = 1;
//  }
}