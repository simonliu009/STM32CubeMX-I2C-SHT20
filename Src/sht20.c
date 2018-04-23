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

//单片机头文件
//#include "stm32f10x.h"
#include "stm32f1xx_hal.h"

//硬件驱动
#include "sht20.h"
#include "i2c.h"
//#include "delay.h"


const int16_t POLYNOMIAL = 0x131;

SHT20_INFO sht20_info;


/*
************************************************************
*	函数名称：	SHT20_reset
*
*	函数功能：	SHT20复位
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void SHT20_reset(void)
{
	
    HAL_I2C_Mem_Write(&hi2c2, SHT20_Write_Add, SHT20_SOFT_RESET, I2C_MEMADD_SIZE_8BIT,(void*)0, 1, 1000);
    HAL_Delay(15);
	
}

/*
************************************************************
*	函数名称：	SHT20_read_user_reg
*
*	函数功能：	SHT20读取用户寄存器
*
*	入口参数：	无
*
*	返回参数：	读取到的用户寄存器的值
*
*	说明：
************************************************************
*/
unsigned char  SHT20_read_user_reg(void)
{
	
    unsigned char val = 0;
	
    HAL_I2C_Mem_Read(&hi2c2, SHT20_Read_Add, SHT20_READ_REG, I2C_MEMADD_SIZE_8BIT,&val, 1, 1000);
	
    return val;
	
}

/*
************************************************************
*	函数名称：	SHT2x_CheckCrc
*
*	函数功能：	检查数据正确性
*
*	入口参数：	data：读取到的数据
*				nbrOfBytes：需要校验的数量
*				checksum：读取到的校对比验值
*
*	返回参数：	校验结果
*
*	说明：		0-成功		1-失败
************************************************************
*/
char SHT2x_CheckCrc(char data[], char nbrOfBytes, char checksum)
{
	
    char crc = 0;
    char bit = 0;
    char byteCtr = 0;
	
    //calculates 8-Bit checksum with given polynomial
    for(byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
    {
        crc ^= (data[byteCtr]);
        for ( bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
            else crc = (crc << 1);
        }
    }
	
    if(crc != checksum)
		return 1;
    else
		return 0;
	
}

/*
************************************************************
*	函数名称：	SHT2x_CalcTemperatureC
*
*	函数功能：	温度计算
*
*	入口参数：	u16sT：读取到的温度原始数据
*
*	返回参数：	计算后的温度数据
*
*	说明：
************************************************************
*/
float SHT2x_CalcTemperatureC(unsigned short u16sT)
{
	
    float temperatureC = 0;            // variable for result

    u16sT &= ~0x0003;           // clear bits [1..0] (status bits)
    //-- calculate temperature [℃] --
    temperatureC = -46.85 + 175.72 / 65536 * (float)u16sT; //T= -46.85 + 175.72 * ST/2^16
	
    return temperatureC;
	
}

/*
************************************************************
*	函数名称：	SHT2x_CalcRH
*
*	函数功能：	湿度计算
*
*	入口参数：	u16sRH：读取到的湿度原始数据
*
*	返回参数：	计算后的湿度数据
*
*	说明：
************************************************************
*/
float SHT2x_CalcRH(unsigned short u16sRH)
{
	
    float humidityRH = 0;              // variable for result
	
    u16sRH &= ~0x0003;          // clear bits [1..0] (status bits)
    //-- calculate relative humidity [%RH] --
    //humidityRH = -6.0 + 125.0/65536 * (float)u16sRH; // RH= -6 + 125 * SRH/2^16
    humidityRH = ((float)u16sRH * 0.00190735) - 6;
	
    return humidityRH;
	
}

/*
************************************************************
*	函数名称：	SHT2x_MeasureHM
*
*	函数功能：	测量温湿度
*
*	入口参数：	cmd：测量温度还是湿度
*				pMeasurand：不为空则保存为ushort值到此地址
*
*	返回参数：	测量结果
*
*	说明：
************************************************************
*/
float SHT2x_MeasureHM(unsigned char cmd, unsigned short *pMeasurand)
{
	
    char  checksum = 0;  //checksum
    char  data[2];    //data array for checksum verification
	unsigned char addr = 0;
    unsigned short tmp = 0;
    float t = 0;
	HAL_I2C_Mem_Read(&hi2c2, SHT20_Read_Add, cmd, I2C_MEMADD_SIZE_8BIT,data,2, 0xFFFF);

	
	SHT2x_CheckCrc(data, 2, checksum);
    tmp = (data[0] << 8) + data[1];
    if(cmd == SHT20_Measurement_T_HM)
    {
        t = SHT2x_CalcTemperatureC(tmp);
    }
    else
    {
        t = SHT2x_CalcRH(tmp);
    }
	
    if(pMeasurand)
    {
        *pMeasurand = (unsigned short)t;
    }
	
    return t;
	
}

/*
************************************************************
*	函数名称：	SHT20_GetValue
*
*	函数功能：	获取温湿度数据
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		温湿度结果保存在SHT20结构体里
************************************************************
*/
void SHT20_GetValue(void)
{
	
	
	sht20_info.tempreture = SHT2x_MeasureHM(SHT20_Measurement_T_HM, (void *)0);
	HAL_Delay(70);
	
	sht20_info.humidity = SHT2x_MeasureHM(SHT20_Measurement_RH_HM, (void *)0);
	HAL_Delay(25);
	
	SHT20_reset();

}
