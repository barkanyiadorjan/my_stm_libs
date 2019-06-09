#include "stm32f4xx_hal.h"
#include "htu21d.h"

HAL_StatusTypeDef htu21d_write_reg(I2C_HandleTypeDef *hi2c,uint8_t reg,uint8_t data)
{
   return HAL_I2C_Master_Transmit(hi2c,HTU21D_WRITE,(uint8_t []){reg, data},2,100000);
}

uint8_t htu21d_read_reg(I2C_HandleTypeDef *hi2c,uint8_t reg)
{
    uint8_t buffer = 0;

    HAL_I2C_Mem_Read(hi2c,HTU21D_WRITE,reg,1,&buffer,1,100000);

    return buffer;
}


HAL_StatusTypeDef htu21d_read_data(I2C_HandleTypeDef *hi2c,uint8_t com, uint8_t *data)
{
    return HAL_I2C_Mem_Read(hi2c,HTU21D_WRITE,com,3,data,1,100000);
}


HAL_StatusTypeDef htu21d_read_temp(I2C_HandleTypeDef *hi2c, uint16_t *temp)
{
    uint8_t measure[3];

    if(htu21d_read_data(hi2c,HTU21D_TEMP_MEASURE,&measure))
        return HAL_ERROR;

    *temp = (uint16_t)measure[1] | (uint16_t)(measure[2]<<6);

    return HAL_OK;
}

HAL_StatusTypeDef htu21d_read_humidity(I2C_HandleTypeDef *hi2c, uint16_t *humidity)
{
    uint8_t measure[3];

    if(htu21d_read_data(hi2c,HTU21D_HUMIDITY_MEASURE,&measure))
        return HAL_ERROR;

    *humidity = (uint16_t)measure[1] | (uint16_t)(measure[2]<<6);

    return HAL_OK;
}

float htu21d_calc_temp(uint16_t temp)
{
    return (float) (172.72*(float)temp/65536 -46.85);
}

float htu21d_calc_humidity(uint16_t humidity)
{
   return (float) (125*(float)humidity/65536 -6);
}

HAL_StatusTypeDef htu21d_init(I2C_HandleTypeDef *hi2c)
{
    if(htu21d_write_reg(hi2c,0xE6,0x02)) //Resolution RH 12 bit Temp 14 bit
        return HAL_ERROR;

    return HAL_OK;
}
