/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 		i2c-lcd.h
	*
	*	作者： 		刘知海 SimonLiu
	*
	*	日期： 		2018-4-22
	*
	*	版本： 		V1.0
	*
	*	说明： 		基于麒麟座官方例程修改，原作者张继瑞
	*
	*	修改记录：
	************************************************************
	************************************************************
	************************************************************
**/

#ifndef _SHT20_H_
#define _SHT20_H_


/*SHT20 设备操作相关宏定义，详见手册*/
#define SHT20_ADDRESS  0X40
#define SHT20_Write_Add 0x80
#define SHT20_Read_Add	0x81
#define SHT20_Measurement_RH_HM  0XE5
#define SHT20_Measurement_T_HM  0XE3
#define SHT20_Measurement_RH_NHM  0XF5  //非主机模式地址
#define SHT20_Measurement_T_NHM  0XF3   //非主机模式地址
#define SHT20_READ_REG  0XE7
#define SHT20_WRITE_REG  0XE6
#define SHT20_SOFT_RESET  0XFE


typedef struct
{

	float tempreture;
	float humidity;

} SHT20_INFO;

extern SHT20_INFO sht20_info;


void SHT20_GetValue(void);


#endif
