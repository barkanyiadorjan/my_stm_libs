#include "stm32f4xx_hal.h"
#include "hc_sr04.h"


float hc_sr04_measure(TIM_HandleTypeDef *htim, GPIO_TypeDef *TRIG_PORT, uint16_t TRIG_PIN, GPIO_TypeDef *ECHO_PORT, uint16_t ECHO_PIN)
{
    uint8_t flag = 0;
    uint16_t count = 0;
    float distance = 0;

    while(1)
    {
        if(flag==0)
        {
            HAL_GPIO_WritePin(TRIG_PORT,TRIG_PIN,GPIO_PIN_SET); //Trigger be
            __HAL_TIM_SET_COUNTER(htim,0); //nullába állítjuk a timert
            HAL_TIM_Base_Start(htim);
            flag++;
        }

        if(flag==1 && __HAL_TIM_GET_COUNTER(htim)==10)
        {
            HAL_GPIO_WritePin(TRIG_PORT,TRIG_PIN,GPIO_PIN_RESET); //Trigger ki
            HAL_TIM_Base_Stop(htim);
            flag++;
        }

        if(flag==2 && HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN))
        {
            __HAL_TIM_SET_COUNTER(htim,0); //nullába állítjuk a timert
            HAL_TIM_Base_Start(htim);
            flag++;
        }

        if(flag==3 && !HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN)) //Megvárjuk míg logikai 0-ba visszaáll
        {
            count = __HAL_TIM_GET_COUNTER(htim);
            HAL_TIM_Base_Stop(htim);
            //HAL_UART_Transmit(&huart2,&count,2,100000);
            distance = (float)count *(float)0.0343/(float)2;

            break;
        }
    }

    return (distance);
}
