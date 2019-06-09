#ifndef HTU21D_H_INCLUDED
#define HTU21D_H_INCLUDED

#define HTU21D_WRITE   0x80
#define HTU21D_READ    0x81

#define HTU21D_TEMP_MEASURE     0xE3    //NO HOLD MASTER MODE!!!
#define HTU21D_HUMIDITY_MEASURE 0xE5


HAL_StatusTypeDef htu21d_write_reg(I2C_HandleTypeDef *hi2c,uint8_t reg,uint8_t data);
uint8_t htu21d_read_reg(I2C_HandleTypeDef *hi2c,uint8_t reg);

//A data pointernek egy három elemű byte-tömbre kell mutatnia
HAL_StatusTypeDef htu21d_read_data(I2C_HandleTypeDef *hi2c,uint8_t com, uint8_t *data);

HAL_StatusTypeDef htu21d_read_temp(I2C_HandleTypeDef *hi2c, uint16_t *temp);
HAL_StatusTypeDef htu21d_read_humidity(I2C_HandleTypeDef *hi2c, uint16_t *humidity);

float htu21d_calc_temp(uint16_t temp);
float htu21d_calc_humidity(uint16_t humidity);

HAL_StatusTypeDef htu21d_init(I2C_HandleTypeDef *hi2c);

#endif // HTU21D_H_INCLUDED
