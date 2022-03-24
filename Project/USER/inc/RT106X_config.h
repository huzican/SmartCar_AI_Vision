/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		IAR 8.3 or MDK 5.28
 * @Target core		NXP RT1064DVL6A
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-04-30
 ********************************************************************************************************************/

#ifndef _RT106X_config_h
#define _RT106X_config_h

//-------------------------------------------------------------------------------------------------------------------
//  当前版本号
//  如果此处版本号与version.txt文件版本不一致，以version.txt文件为准
//-------------------------------------------------------------------------------------------------------------------
#define SEEKFREE_RT106X_VERSION   0X100




//-------------------------------------------------------------------------------------------------------------------
//  PRINTF_ENABLE 为 0               不使用printf串口打印函数及不输出调试信息，则不会初始化相应串口
//  PRINTF_ENABLE 为 1               使用printf串口打印函数及输出调试信息，则会初始化相应串口，这里定义为1之后，无需自己手动初始化串口
//-------------------------------------------------------------------------------------------------------------------
#define PRINTF_ENABLE               0


#if(1 == PRINTF_ENABLE)

//-------------------------------------------------------------------------------------------------------------------
//printf串口打印函数所使用的串口、串口波特率、串口引脚宏定义
//-------------------------------------------------------------------------------------------------------------------
#define DEBUG_UART                  USART_1
#define DEBUG_UART_BAUD             115200
#define DEBUG_UART_TX_PIN           UART1_TX_B12
#define DEBUG_UART_RX_PIN           UART1_RX_B13

//-------------------------------------------------------------------------------------------------------------------
//  DEBUG_INFO 为 0                  不输出断言信息
//  DEBUG_INFO 为 1                  输出断言信息，断言信息包含错误所在文件及所在行  需要PRINTF宏定义为1才能有效输出
//-------------------------------------------------------------------------------------------------------------------
#define DEBUG_PRINTF                1

#endif





#endif
