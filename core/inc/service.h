/*
 * service.h
 *
 *  Created on: Nov 27, 2023
 *      Author: kaushik
 */

#ifndef INC_SERVICE_H_
#define INC_SERVICE_H_


tBleStatus add_service(void);
void APP_UserEvtRx(void *pData);
void send_data(uint8_t *data_buffer, uint8_t no_bytes);

#endif /* INC_SERVICE_H_ */
