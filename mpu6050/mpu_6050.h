#ifndef MPU_6050_H_INCLUDED
#define MPU_6050_H_INCLUDED

#define MPU6050_WRITE 0xD0
#define MPU6050_READ 0xD1

/*
    Accelerator registers
*/
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

/*
    Gyroscope registers
*/
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

/*
    Tempature registers
*/
#define MPU6050_TEMP_H 0x41
#define MPU6050_TEMP_L 0x42

/*
    WHO AM I
*/
#define MPU6050_WHO_AM_I 0x75

HAL_StatusTypeDef mpu6050_init(I2C_HandleTypeDef *hi2c);

//Írási művelet
HAL_StatusTypeDef mpu6050_write_reg(I2C_HandleTypeDef *hi2c,uint8_t reg,uint8_t data);

//Olvasási műveletek
uint8_t mpu6050_read_reg(I2C_HandleTypeDef *hi2c,uint8_t reg);
uint16_t mpu6050_read_measure(I2C_HandleTypeDef *hi2c, uint8_t h_reg,uint8_t l_reg);

//Egyébb műveletek
void mpu6050_3party_sensor_init(I2C_HandleTypeDef *hi2c, uint8_t sensor7bitAddress);
void mpu6050_bypass_mode(I2C_HandleTypeDef *hi2c,uint8_t on_off);
uint8_t mpu6050_identification(I2C_HandleTypeDef *hi2c);

#endif // MPU_6050_H_INCLUDED
