#include "stm32f4xx_hal.h"

//0b10100000 -> A0 A1 földre kötve
#define MICROCHIP_24lc1025_WRITE_ADDRESS 0xA0

HAL_StatusTypeDef microchip_24lc1025_write_byte(I2C_HandleTypeDef *hi2c, uint16_t address, uint8_t data_byte);
HAL_StatusTypeDef microchip_24lc1025_write_data(I2C_HandleTypeDef *hi2c, uint16_t start_address, uint8_t *data, uint8_t length);

HAL_StatusTypeDef microchip_24lc1025_read_byte(I2C_HandleTypeDef *hi2c, uint16_t address, uint8_t *byte);
HAL_StatusTypeDef microchip_24lc1025_read_data(I2C_HandleTypeDef *hi2c, uint16_t start_address, uint8_t *data, uint8_t length);
