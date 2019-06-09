#include "stm32f4xx_hal.h"
#include "bmp180.h"


void bmp180_write_reg(I2C_HandleTypeDef *hi2c,uint8_t reg,uint8_t data)
{
    uint8_t buff[] = {reg,data};
    HAL_I2C_Master_Transmit(hi2c,BMP180_WRITE,&buff,2,100000);
}

uint8_t bmp180_read_reg(I2C_HandleTypeDef *hi2c,uint8_t reg)
{
    uint8_t buffer = 0;

    HAL_I2C_Master_Transmit(hi2c,BMP180_WRITE,&reg,1,100000);
    HAL_I2C_Master_Receive(hi2c,BMP180_READ,&buffer,1,100000);

    return buffer;
}

uint16_t bmp180_read_16bit_reg(I2C_HandleTypeDef *hi2c,uint8_t reg_H, uint8_t reg_L)
{
    uint16_t buffer = 0;

    buffer = (bmp180_read_reg(hi2c,reg_H)<<8);
    buffer |= bmp180_read_reg(hi2c,reg_L);

    return buffer;
}



/*
    0x55 öt kell visszaadnia, nagy 'U' betü
*/
uint8_t bmp180_identification(I2C_HandleTypeDef *hi2c)
{
    return bmp180_read_reg(hi2c,0xd0);
}




void bmp180_read_calcregisters(I2C_HandleTypeDef *hi2c, struct bmp180_DATA_FORM *data)
{
    data->AC1 = bmp180_read_16bit_reg(hi2c,0xAA,0xAB);
    data->AC2 = bmp180_read_16bit_reg(hi2c,0xAC,0xAD);
    data->AC3 = bmp180_read_16bit_reg(hi2c,0xAE,0xAF);
    data->AC4 = bmp180_read_16bit_reg(hi2c,0xB0,0xB1);
    data->AC5 = bmp180_read_16bit_reg(hi2c,0xB2,0xB3);
    data->AC6 = bmp180_read_16bit_reg(hi2c,0xB4,0xB5);

    data->B1 = bmp180_read_16bit_reg(hi2c,0xB6,0xB7);
    data->B2 = bmp180_read_16bit_reg(hi2c,0xB8,0xB9);

    data->MB = bmp180_read_16bit_reg(hi2c,0xBA,0xBB);
    data->MC = bmp180_read_16bit_reg(hi2c,0xBC,0xBD);
    data->MD = bmp180_read_16bit_reg(hi2c,0xBE,0xBF);
}

void bmp180_read_utemp(I2C_HandleTypeDef *hi2c, struct bmp180_DATA_FORM *data)
{
    bmp180_write_reg(hi2c,0xF4,0x2E);
    HAL_Delay(5);

    data->UT = bmp180_read_16bit_reg(hi2c,UT_REGISTER_H,UT_REGISTER_L);
}


void bmp180_read_upressure(I2C_HandleTypeDef *hi2c, struct bmp180_DATA_FORM *data)
{
    bmp180_write_reg(hi2c,0xF4,0x34);
    HAL_Delay(5);

    uint8_t up_MSB = bmp180_read_reg(hi2c,0xF6);
    uint8_t up_LSB = bmp180_read_reg(hi2c,0xF7);
    uint8_t up_XLSB = bmp180_read_reg(hi2c,0xF8);

    data->UP = ((up_MSB<<16) + (up_LSB<<8) + up_XLSB)>>8;
}


void bmp180_calculate_true_temp(struct bmp180_DATA_FORM *data)
{
    data->X1 = ((data->UT - data->AC6)*data->AC5)/32768;
    data->X2 = (data->MC *2048) /(data->X1 + data->MD);
    data->B5 = data->X1 + data->X2;
    data->T = (data->B5 + 8)/16;
}

void bmp180_calculate_true_pressure(struct bmp180_DATA_FORM *data)
{
    data->B6 = data->B5-4000;

    data->X1 = (data->B2*(data->B6*data->B6/4096))/2048;
    data->X2 = data->AC2 * data->B6/2048;
    data->X3 = data->X1 + data->X2;

    data->B3 = ((data->AC1*4)+6+data->X3)/4;

    data->X1 = (data->AC3 * data->B6)/8192;
   // data->X2 = (data->B1 * ((data->B6 * data->B6/4096)))/65536;
    data->X2 = (data->B1 * (data->B6 *data->B6/4096))/65536;


    data->X3 = (data->X1 + data-> X2 + 2)/4;

    data->B4 = (data->AC4 * (uint32_t)(data->X3+32768))/32768;
    data->B7 = (uint32_t)(data->UP - data->B3) * 50000;

    if(data->B7 < 0x80000000)
        data->P = (data->B7*2)/data->B4;
    else
        data->P = (data->B7/data->B4)*2;

    data->X1 = (data->P / 256) * (data->P /256);
    data->X1 = (data->X1*3038)/65536;
    data->X2 = (-7357 * data->P)/65536;

    data->P = data->P + ((data->X1 + data->X2 + 3791)/16);
}


void bmp180_read_calc_all(I2C_HandleTypeDef *hi2c, struct bmp180_DATA_FORM *data)
{
    bmp180_read_calcregisters(hi2c,data);
    bmp180_read_utemp(hi2c,data);
    bmp180_read_upressure(hi2c,data);
    bmp180_calculate_true_temp(data);
    bmp180_calculate_true_pressure(data);
}



/*
    Ezzel a függvénnyel az adatlapon leírt
    példaértékekkel tesztelhető a számolási
    algoritmus helyessége (önteszt, hogy jól
    programoztam-e le) -> 150 -et kell kapjunk!
*/
int32_t bmp180_true_temp_calc_test()
{
    struct bmp180_DATA_FORM data;
    data.AC1 = 408;
    data.AC2 = -72;
    data.AC3 = -14383;
    data.AC4 = 32741;
    data.AC5 = 32757;
    data.AC6 = 23153;

    data.B1 = 6190;
    data.B2 = 4;

    data.MB = -32768;
    data.MC = -8711;
    data.MD = 2868;

    data.UT = 27898;

    bmp180_calculate_true_temp(&data);

    return data.T;
}


/*
Kapni kell:69964 hPa -t
Todo: valamiért 69962 hPa-t ad!
*/
int32_t bmp180_true_pressure_calc_test()
{
    struct bmp180_DATA_FORM data;
    data.AC1 = 408;
    data.AC2 = -72;
    data.AC3 = -14383;
    data.AC4 = 32741;
    data.AC5 = 32757;
    data.AC6 = 23153;

    data.B1 = 6190;
    data.B2 = 4;
    data.B5 = 2399;

    data.MB = -32768;
    data.MC = -8711;
    data.MD = 2868;

    data.UP = 23843;

    bmp180_calculate_true_pressure(&data);


    return data.P;
}

