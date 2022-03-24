#ifndef _FLASH_H
#define _FLASH_H
#include "common.h"

#define   FLASH_MAX    37      //FLASH_DATA���ݴ洢������Ŀ ע�⣬һ������ֻ�ܴ���1024������
#define   FLASH_sector_num    1023       //flash�����������һ���������Դ洢1024������
#define   FLASH_FLAG   1
typedef uint32 FLASH_WRITE_TYPE;       //flash_write����д�����������

extern FLASH_WRITE_TYPE     FLASH_DATA[50];
extern FLASH_WRITE_TYPE     FLASH_CONVERT[50];
void DATA_SET();
void DATA_RES();
void MY_FLASH(char cmd[],uint8 num);
void parameter_init();
void dataf_set(float temp, uint8 num);
void Flash_handle();

#endif
