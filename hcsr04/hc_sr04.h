#ifndef HC_SR04_H_INCLUDED
#define HC_SR04_H_INCLUDED

/**
You must defined htim -> 1 count := 1us !!!! or won't work!
*/
float hc_sr04_measure(TIM_HandleTypeDef *htim, GPIO_TypeDef *TRIG_PORT, uint16_t TRIG_PIN, GPIO_TypeDef *ECHO_PORT, uint16_t ECHO_PIN);

#endif // HC_SR04_H_INCLUDED
