#include "stm32f4xx_hal.h"
#include "camera.h"

void clear_vector(char *tomb)
{
    uint8_t i = 0;

    for(;i<CAM_COMMAND_SIZE;i++)
    {
        *tomb=0;
        tomb++;
    }
}

void vector_to_vector(char *to, char *from)
{
    uint8_t i = 0;
    for(;i<CAM_COMMAND_SIZE;i)
    {
        *to = *from;
        to++;
        from++;
    }
}

void cam_init(struct cam *camera, UART_HandleTypeDef *cam_huart, UART_HandleTypeDef *cam_huart_debug)
{
    camera->cam_huart = cam_huart;
    camera->cam_huart_debug = cam_huart_debug;

    camera->command=0x00;
    camera->answer=0x00;
}

CAM_StatusTypeDef cam_check_answer(struct cam *camera)
{
    if(!camera->command || !camera->answer)
        return CAM_ERROR;

    (camera->command)++;
    (camera->answer)++;

    if(*(camera->answer) != 0x0e)
        return CAM_WRONG_ANSWER;

    (camera->answer)++;

    if(*(camera->command) != *(camera->answer))
        return CAM_WRONG_ANSWER;

    return CAM_OK;
}


CAM_StatusTypeDef cam_send_data(struct cam *camera, uint32_t timeout)
{
    if(HAL_OK!=HAL_UART_Transmit(camera->cam_huart,camera->command,CAM_COMMAND_SIZE,timeout))
        return CAM_ERROR;

    return CAM_OK;
}

CAM_StatusTypeDef cam_receive_data(struct cam *camera, uint32_t timeout)
{
    if(HAL_OK!=HAL_UART_Receive(camera->cam_huart,camera->answer,CAM_COMMAND_SIZE,timeout))
        return CAM_ERROR;

    return CAM_OK;
}


CAM_StatusTypeDef cam_send_command(struct cam *camera)
{

    if(HAL_OK!=HAL_UART_Transmit(camera->cam_huart,camera->command,CAM_COMMAND_SIZE,CAM_TIMEOUT))
        return CAM_ERROR;

    if(HAL_OK!=HAL_UART_Receive(camera->cam_huart,camera->answer,CAM_COMMAND_SIZE,CAM_TIMEOUT))
        return CAM_ERROR;

#ifdef CAM_DEBUG
        HAL_UART_Transmit(camera->cam_huart_debug,camera->answer,CAM_COMMAND_SIZE,CAM_TIMEOUT);
#endif // DEBUG

    return cam_check_answer(camera);
}

CAM_StatusTypeDef send_data(struct cam *camera, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6)
{
    camera->command = (uint8_t []){c1,c2,c3,c4,c5,c6};

    return cam_send_data(camera,CAM_TIMEOUT);
}

CAM_StatusTypeDef send_command(struct cam *camera, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6)
{
    camera->command = (uint8_t []){c1,c2,c3,c4,c5,c6};

    return cam_send_command(camera);
}

/**
    The process of the Synchronisation.

          MCU                     CAM
    AA 0D 00 00 00 00   ->
                            TIMEOUT/WRONG ACK
    AA 0D 00 00 00 00   ->
                            TIMEOUT/WRONG ACK

          ...               (MAX 60 time)

    AA 0D 00 00 00 00   ->
                            AA 0E 0D 00 00 00
                            AA 0D 00 00 00 00
    AA 0E 0D 00 00 00   ->


M:  Ez a felújított folyamatábra, mert az előző
    verzióban hiba volt hogy volt hogy megérkezett
    a jel, csak esetlegesen rossz baudrate-el, ezért
    rossz üzenet érkezett, de a rendszer azt hitte
    hogy minden rendben van, és elfogadta.

*/

CAM_StatusTypeDef cam_synchronisation(struct cam *camera)
{

    uint8_t i = 0;
    uint8_t sync[] = {CAM_START,CAM_SYNC,0x00,0x00,0x00,0x00};
    uint8_t ack[] = {CAM_START,CAM_ACK,CAM_SYNC,0x00,0x00,0x00};
    uint8_t answer[6];
    clear_vector(&answer);



    camera->command = &sync;  //az itt lévő adatot küldjük
    camera->answer = &answer; //ide várjuk a választ


     while(i<60)
     {

        HAL_Delay(5);

        if(CAM_ERROR==cam_send_data(camera,CAM_TIMEOUT))
            return CAM_ERROR;

        cam_receive_data(camera,CAM_TIMEOUT_MIN); //Itt nem kell kilépni, mert nem baj ha timeout

        if(answer[1]==0x0e && answer[2]==0x0d)
            break;

        i++;
     }

     if(i>=60)
        return CAM_ERROR;

    if(CAM_ERROR==cam_receive_data(camera,CAM_TIMEOUT))
        return CAM_ERROR;

#ifdef CAM_DEBUG
        HAL_UART_Transmit(camera->cam_huart_debug,&i,1,CAM_TIMEOUT); //sikerült jel, elküldi hogy hányadszorra válaszolt
        HAL_UART_Transmit(camera->cam_huart_debug,camera->answer,CAM_COMMAND_SIZE,CAM_TIMEOUT);
#endif // DEBUG

     camera->command = &ack;

     if(CAM_ERROR==cam_send_data(camera,CAM_TIMEOUT))
        return CAM_ERROR;

     return CAM_OK;
}


CAM_StatusTypeDef set_jpeg(struct cam *camera,uint8_t jpeg_resolution)
{
    uint8_t answer[6];
    clear_vector(&answer);
    camera->answer = &answer;


    if(CAM_OK!=send_command(camera,CAM_START,CAM_INITIAL,0x00,0x07,0x01,jpeg_resolution))
        return CAM_ERROR;

    if(CAM_OK!=send_command(camera,CAM_START,CAM_PACKET_SIZE,0x08,0x40,0x00,0x00)) //Set packet size -> 64 byte
        return CAM_ERROR;

    return CAM_OK;
}




uint16_t get_size(uint8_t *answer)
{
    if(!answer)
        return 0x00;

    uint16_t size = 0;
    answer+=3; //Három elemet előre lépünk
    size = *answer;
    answer++;
    size |= (*answer)<<8;

    return size;
}


CAM_StatusTypeDef get_jpeg(struct cam *camera, uint8_t *img)
{
    uint16_t size = 0x0000;
    uint16_t packets_size = 0;
    uint16_t num = 0;
    uint16_t number = 0;

    uint16_t id = 0;
    uint16_t data_size = 0;
    uint16_t verify_code = 0;
    uint8_t byte = 0;
    uint8_t ack[6];

    cam_synchronisation(camera); //Hogyha alszik felébredjen

    send_data(camera,CAM_START,CAM_GET_PIC,0x05,0x00,0x00,0x00); //Get pic
    HAL_UART_Receive(camera->cam_huart,&ack,6,CAM_TIMEOUT); //RECEIVE ACK

    HAL_UART_Receive(camera->cam_huart,&ack,6,CAM_TIMEOUT);  //RECEIVE LENGHT


    size = ack[3] | (ack[4]<<8);

    packets_size = size/58; //8 BITES -> MÁR 16 MERT ELCSESZTED!!
    packets_size+=1;


    //MÉRET ELKÜLDÉSE HOGY A FOGADÓ OLDAL TUDJA HOGY MENNYI LESZ!
    //HAL_UART_Transmit(camera->cam_huart_debug,&size,2,CAM_TIMEOUT);
    //Most helyette kimentjük img-nek -> első lesz az alsó byte
    *img = ack[3];
    img++;
    *img = ack[4];
    img++;

    while(num<packets_size)
    {
        //packet_size-hoz minden körben hozzáadjuk a csomagméretet
        if(CAM_OK != send_data(camera,CAM_START,CAM_ACK,0x00,0x00,num,(num>>8))) //Megfelelő ACK elküldése
            return CAM_ERROR;

        if(HAL_OK != HAL_UART_Receive(camera->cam_huart,&id,2,CAM_TIMEOUT))
            return CAM_ERROR;

        if(HAL_OK != HAL_UART_Receive(camera->cam_huart,&data_size,2,CAM_TIMEOUT)) //Mennyi bytot kell várni az adat mezőben
            return CAM_ERROR;

        while(data_size>0)
        {
            if(HAL_OK !=  HAL_UART_Receive(camera->cam_huart,&byte,1,CAM_TIMEOUT))
                return CAM_ERROR;

            //Ezt kell menteni vagy ilyesmi, most csak továbbküldöm
            //HAL_UART_Transmit(camera->cam_huart_debug,&byte,1,CAM_TIMEOUT);
            *img = byte;

            img++;
            number++;
            data_size--;
        }

        if(HAL_OK != HAL_UART_Receive(camera->cam_huart,&verify_code,2,CAM_TIMEOUT))
            return CAM_ERROR;


        num++; //nem csak a ciklus hanem az ACK miatt is!
    }

    //Utolsó, "lezárás" elküldése
    if(HAL_OK != send_data(camera,CAM_START,CAM_ACK,0x00,0x00,num,(num>>8)))
        return CAM_ERROR;

    return CAM_OK;
}




