
#undef CAM_DEBUG
#define CAM_COMMAND_SIZE 6
#define CAM_TIMEOUT 100000
#define CAM_TIMEOUT_MIN 3

#define CAM_START 0x0a
#define CAM_INITIAL 0x01
#define CAM_PACKET_SIZE 0x06
#define CAM_SYNC 0x0d
#define CAM_ACK 0x0e
#define CAM_GET_PIC 0x04


#define CAM_JPEG_160x128 0x03
#define CAM_JPEG_320x240 0x05
#define CAM_JPEG_640x480 0x07

struct cam{
    uint8_t *command;
    uint8_t *answer;

    UART_HandleTypeDef *cam_huart;
    UART_HandleTypeDef *cam_huart_debug;
};


typedef enum {
CAM_OK,
CAM_ERROR,
CAM_WRONG_ANSWER
}CAM_StatusTypeDef;


/**
            ############################################
            #       #        #               #         #
            #  ID   # P_Size #     DATA      # Verify  #
            #       #        #               #         #
            ############################################
              2byte    2byte     P_Size byte    2byte
*/


void cam_init(struct cam *camera, UART_HandleTypeDef *cam_huart, UART_HandleTypeDef *cam_huart_debug);
CAM_StatusTypeDef cam_check_answer(struct cam *camera);


CAM_StatusTypeDef cam_send_data(struct cam *camera, uint32_t timeout);
CAM_StatusTypeDef cam_receive_data(struct cam *camera, uint32_t timeout);
CAM_StatusTypeDef cam_send_command(struct cam *camera);


CAM_StatusTypeDef cam_synchronisation(struct cam *camera);
CAM_StatusTypeDef set_jpeg(struct cam *camera,uint8_t jpeg_resolution);
CAM_StatusTypeDef get_jpeg(struct cam *camera, uint8_t *img);

