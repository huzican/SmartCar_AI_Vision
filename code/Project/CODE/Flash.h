#ifndef _FLASH_H
#define _FLASH_H
#include "common.h"

#define   FLASH_MAX    37      //FLASH_DATA数据存储数据数目 注意，一个扇区只能储存1024个数据
#define   FLASH_sector_num    1023       //flash储存的扇区，一个扇区可以存储1024的数据
#define   FLASH_FLAG   1
typedef uint32 FLASH_WRITE_TYPE;       //flash_write函数写入的数据类型

extern FLASH_WRITE_TYPE     FLASH_DATA[50];
extern FLASH_WRITE_TYPE     FLASH_CONVERT[50];
void DATA_SET();
void DATA_RES();
void MY_FLASH(char cmd[],uint8 num);
void parameter_init();
void dataf_set(float temp, uint8 num);
void Flash_handle();

#endif
