#ifndef BMP180_H_INCLUDED
#define BMP180_H_INCLUDED

#define BMP180_WRITE 0xEE
#define BMP180_READ 0xEF

/*
    REGISTERS
*/
#define UT_REGISTER_H   0xF6
#define UT_REGISTER_L   0xF7


struct bmp180_DATA_FORM
{
    /*
        Ezek mind 1-1 2x8 bites regiszter értékek lesznek
    */
    int16_t AC1;
    int16_t AC2;
    int16_t AC3;

    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;

    int16_t B1;
    int16_t B2;
    int16_t MB;
    int16_t MC;
    int16_t MD;

    /*
        Számítási segédváltozók
    */

    int32_t X1;
    int32_t X2;
    int32_t X3;
    int32_t B3;
    uint32_t B4;
    int32_t B5;
    int32_t B6;
    uint32_t B7;

    /*
        Ezek lesznek a számolt eredmények
    */

    int32_t UT;    //Nem kompenzált hőmérséklet érték
    int32_t UP;    //Nem kompenzált nyomás érték
    int32_t T;    //Kompenzált hőmérséklet
    int32_t P;    //Kompenzált nyomás
};

/*
Függvények
*/
void bmp180_write_reg(I2C_HandleTypeDef *hi2c,uint8_t reg,uint8_t data);
uint8_t bmp180_read_reg(I2C_HandleTypeDef *hi2c,uint8_t reg);
uint16_t bmp180_read_16bit_reg(I2C_HandleTypeDef *hi2c,uint8_t reg_H, uint8_t reg_L);



void bmp180_read_calcregisters(I2C_HandleTypeDef *hi2c, struct bmp180_DATA_FORM *data);
void bmp180_read_utemp(I2C_HandleTypeDef *hi2c, struct bmp180_DATA_FORM *data);
void bmp180_read_upressure(I2C_HandleTypeDef *hi2c, struct bmp180_DATA_FORM *data);

void bmp180_calculate_true_temp(struct bmp180_DATA_FORM *data);
void bmp180_calculate_true_pressure(struct bmp180_DATA_FORM *data);

/*
    A működéshez elegendő ezt az egy függvényt meghívni, ha sebesség kritikus
    a rendszer, akkor ajánlott szétszedni, mert ez mindig meghívja a read_calc_reg fgv. is
*/
void bmp180_read_calc_all(I2C_HandleTypeDef *hi2c, struct bmp180_DATA_FORM *data);

/*
Teszt függvények
*/
uint8_t bmp180_identification(I2C_HandleTypeDef *hi2c);
int32_t bmp180_true_temp_calc_test();
int32_t bmp180_true_pressure_calc_test();

#endif // BMP180_H_INCLUDED
