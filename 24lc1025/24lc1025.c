#include "stm32f4xx_hal.h"

#include "24lc1025.h"


HAL_StatusTypeDef microchip_24lc1025_write_byte(I2C_HandleTypeDef *hi2c, uint16_t address, uint8_t data_byte)
{
    return HAL_I2C_Master_Transmit(hi2c,MICROCHIP_24lc1025_WRITE_ADDRESS,(uint8_t []){ (uint8_t) (address>>8), (uint8_t)address, data_byte},3,100000);
}

HAL_StatusTypeDef microchip_24lc1025_write_data(I2C_HandleTypeDef *hi2c, uint16_t start_address, uint8_t *data, uint8_t length)
{
    return HAL_I2C_Mem_Write(hi2c,MICROCHIP_24lc1025_WRITE_ADDRESS,start_address,2,data,length,100000);
}


HAL_StatusTypeDef microchip_24lc1025_read_byte(I2C_HandleTypeDef *hi2c, uint16_t address, uint8_t *byte)
{
    return HAL_I2C_Mem_Read(hi2c,MICROCHIP_24lc1025_WRITE_ADDRESS,address,2,byte,1,100000);
}

HAL_StatusTypeDef microchip_24lc1025_read_data(I2C_HandleTypeDef *hi2c, uint16_t start_address, uint8_t *data, uint8_t length)
{

    return HAL_I2C_Mem_Read(hi2c,MICROCHIP_24lc1025_WRITE_ADDRESS,start_address,2,data,length,100000);
}
