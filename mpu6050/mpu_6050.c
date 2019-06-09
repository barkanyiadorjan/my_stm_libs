#include "stm32f4xx_hal.h"

#include "mpu_6050.h"

HAL_StatusTypeDef mpu6050_write_reg(I2C_HandleTypeDef *hi2c,uint8_t reg,uint8_t data)
{
    return HAL_I2C_Master_Transmit(hi2c,MPU6050_WRITE,(uint8_t []){reg, data},2,100000);
}


uint8_t mpu6050_read_reg(I2C_HandleTypeDef *hi2c,uint8_t reg)
{
    uint8_t buffer = 0;

    HAL_I2C_Master_Transmit(hi2c,MPU6050_WRITE,&reg,1,100000);
    HAL_I2C_Master_Receive(hi2c,MPU6050_READ,&buffer,1,100000);

    return buffer;
}

uint16_t mpu6050_read_measure(I2C_HandleTypeDef *hi2c, uint8_t h_reg,uint8_t l_reg)
{
    return ((mpu6050_read_reg(hi2c,h_reg) << 8) | mpu6050_read_reg(hi2c,l_reg));
}

void mpu6050_bypass_mode(I2C_HandleTypeDef *hi2c,uint8_t on_off)
{
    if(!on_off) //Kikapcs
    {
        mpu6050_write_reg(hi2c,0x37,0x00); //i2c bypass mód kikapcsolása
        mpu6050_write_reg(hi2c,0x6A,0x20); //i2c master mód bekapcsolása
    }
    else //Bekapcs
    {
        mpu6050_write_reg(hi2c,0x6A,0x00); //i2c master mód kikapcsolása
        mpu6050_write_reg(hi2c,0x37,0x02); //i2c bypass mód bekapcsolása
    }
}

void mpu6050_3party_sensor_init(I2C_HandleTypeDef *hi2c, uint8_t sensor7bitAddress)
{
    mpu6050_write_reg(hi2c, 0x25,sensor7bitAddress | 0x80); //cím beállítása olvasás módban
    mpu6050_write_reg(hi2c, 0x26, 0x03); //slave 0 kezdőregiszter 0x03 (x tengely)
    mpu6050_write_reg(hi2c, 0x27, 6 | 0x80); //Slave 0 átviteli méret = 6 engedélyezése
    mpu6050_write_reg(hi2c, 0x67, 1); //Slave 0 Delay engedélyezése
}


HAL_StatusTypeDef mpu6050_init(I2C_HandleTypeDef *hi2c)
{
    if(mpu6050_write_reg(hi2c,0x6B,0))   //Wake up sensor
        HAL_ERROR;

    if(mpu6050_write_reg(hi2c,0x23,0))   //FIFO disable
        HAL_ERROR;

    if(mpu6050_write_reg(hi2c,0x19,0))   //Sample Rate Div off
        HAL_ERROR;

    if(mpu6050_write_reg(hi2c,0x1C,0x08))   //Acc +-4g
        HAL_ERROR;

    if(mpu6050_write_reg(hi2c,0x1B,0x08))   //Gyr +-500°/s
        HAL_ERROR;

    if(mpu6050_write_reg(hi2c,0x1A,0))   //Bandwidth
        HAL_ERROR;

    return HAL_OK;
}


//0x68 -at kell adnia -> Ascii 'h'
uint8_t mpu6050_identification(I2C_HandleTypeDef *hi2c)
{
    return mpu6050_read_reg(hi2c,MPU6050_WHO_AM_I);
}

