/*
 * service.c
 *
 *  Created on: Nov 27, 2023
 *      Author: kaushik
 */

//#include "gatt_db.h"
#include "bluenrg1_types.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_gatt_aci.h"
#include "service.h"
#include <stdio.h>
#include <stdlib.h>
#include "app_blenrg.h"
#include "bluenrg_conf.h"
#include "bluenrg1_types.h"
#include "bluenrg1_gap.h"
#include "main.h"

const uint8_t HEALTH_SERVICE_UUID[16] 	= {0x68,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0x91};
const uint8_t BPM_CHAR_UUID[16]  		= {0x68,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0x91};
const uint8_t WEIGHT_CHAR_UUID[16]  	= {0x68,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0x91};

const uint8_t SERVICE_UUID[16]  		= {0x68,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0x91};
const uint8_t CHAR_UUID_RX[16]  		= {0x68,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0x91};
const uint8_t CHAR_UUID_TX[16]  		= {0x68,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0x91};


const uint8_t WEATHER_SERVICE_UUID[16] = {0x67,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x00,0xf2,0x73,0xd9};
const uint8_t TEMP_CHAR_UUID[16]  		= {0x67,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x01,0xf2,0x73,0xd9};
const uint8_t HUM_CHAR_UUID[16]  		= {0x67,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x02,0xf2,0x73,0xd9};


uint16_t health_service_handle, bpm_char_handle,weight_char_handle;

uint16_t comm_service_handle, tx_char_handle,rx_char_handle;

uint16_t weather_service_handle, temp_char_handle,hum_char_handle;


uint8_t  connected=0;
uint8_t  set_connectable=1;
uint16_t connection_handle=0;
uint8_t  notification_enabled=0;

uint8_t rcv_data[20];


tBleStatus add_service(void)
	{

	tBleStatus ret;
	Service_UUID_t health_service_uuid,weather_service_uuid, service_uuid;
	Char_UUID_t bpm_char_uuid,weight_char_uuid,temp_char_uuid,hum_char_uuid, tx_char_uuid, rx_char_uuid;


	BLUENRG_memcpy(health_service_uuid.Service_UUID_128,HEALTH_SERVICE_UUID,16);
	BLUENRG_memcpy(weather_service_uuid.Service_UUID_128,WEATHER_SERVICE_UUID,16);
	BLUENRG_memcpy(service_uuid.Service_UUID_128,SERVICE_UUID,16);

	BLUENRG_memcpy(temp_char_uuid.Char_UUID_128,TEMP_CHAR_UUID,16);
	BLUENRG_memcpy(hum_char_uuid.Char_UUID_128,HUM_CHAR_UUID,16);
	BLUENRG_memcpy(bpm_char_uuid.Char_UUID_128,BPM_CHAR_UUID,16);
	BLUENRG_memcpy(weight_char_uuid.Char_UUID_128,WEIGHT_CHAR_UUID,16);
	BLUENRG_memcpy(tx_char_uuid.Char_UUID_128,CHAR_UUID_TX,16);
	BLUENRG_memcpy(rx_char_uuid.Char_UUID_128,CHAR_UUID_RX,16);


	aci_gatt_add_service(UUID_TYPE_128, &health_service_uuid, PRIMARY_SERVICE, 7, &health_service_handle);
	aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 7, &comm_service_handle);
	aci_gatt_add_service(UUID_TYPE_128, &weather_service_uuid, PRIMARY_SERVICE, 7, &weather_service_handle);


	aci_gatt_add_char(health_service_handle,UUID_TYPE_128,&bpm_char_uuid,2,CHAR_PROP_READ,ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHOR_READ,GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,0,0,&bpm_char_handle);
	aci_gatt_add_char(health_service_handle,UUID_TYPE_128,&weight_char_uuid,2,CHAR_PROP_READ,ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHOR_READ,GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,0,0,&weight_char_handle);

	ret = aci_gatt_add_char(comm_service_handle,UUID_TYPE_128,&tx_char_uuid,20,CHAR_PROP_NOTIFY,ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHOR_READ,0,0,1,&tx_char_handle);
	ret = aci_gatt_add_char(comm_service_handle,UUID_TYPE_128,&rx_char_uuid,20,CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP ,ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHOR_READ,
			GATT_NOTIFY_ATTRIBUTE_WRITE,0,1,&rx_char_handle);

	aci_gatt_add_char(weather_service_handle,UUID_TYPE_128,&temp_char_uuid,2,CHAR_PROP_READ,ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHOR_READ ,GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,16,1,&temp_char_handle);
	aci_gatt_add_char(weather_service_handle,UUID_TYPE_128,&hum_char_uuid,2,CHAR_PROP_READ,ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHOR_READ ,GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,16,1,&hum_char_handle);

	return ret;
	}

//void aci_gap_pass_key_req_event(uint16_t Connection_Handle)
//{
//tBleStatus ret;
//uint32_t pin;
///*Generate a random pin with an user specific function */
//pin = 1234;
//ret= aci_gap_pass_key_resp(Connection_Handle,pin);
//	if (ret != BLE_STATUS_SUCCESS) printf("Failure.\n");
//}


void receive_data(uint8_t *data_buffer, uint8_t no_bytes)
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);

	for(int i=0; i<no_bytes; i++)
	{
		rcv_data[i] = data_buffer[i];
	}
}

void send_data(uint8_t *data_buffer, uint8_t no_bytes)
{
	tBleStatus ret;

	ret=aci_gatt_update_char_value(comm_service_handle, tx_char_handle, 0, no_bytes, data_buffer);
	if(ret !=BLE_STATUS_SUCCESS)
		{
			printf("aci_gatt_update_char_value  failed \r\n");
		}


}


void update_bpm(int16_t new_data)
	{
	tBleStatus ret;
	/*Update characteristics value*/
	ret=aci_gatt_update_char_value(health_service_handle, bpm_char_handle, 0, 2, (uint8_t *)&new_data);
	if(ret !=BLE_STATUS_SUCCESS)
	{
		printf("bpm chatacteristics update failed  failed \r\n");
	}
}

void update_weight(int16_t new_data)
	{
	tBleStatus ret;
	/*Update characteristics value*/
	ret=aci_gatt_update_char_value(health_service_handle, weight_char_handle, 0, 2, (uint8_t *)&new_data);
	if(ret !=BLE_STATUS_SUCCESS){printf("weight chatacteristics update failed  failed \r\n");}

	}

void update_temp(int16_t new_data)
	{
	tBleStatus ret;
	/*Update characteristics value*/
	ret=aci_gatt_update_char_value(weather_service_handle, temp_char_handle, 0, 2, (uint8_t *)&new_data);
	if(ret !=BLE_STATUS_SUCCESS){printf("temp chatacteristics update failed  failed \r\n");}

	}

void update_hum(int16_t new_data)
	{
	tBleStatus ret;
	/*Update characteristics value*/
	ret=aci_gatt_update_char_value(weather_service_handle, hum_char_handle, 0, 2, (uint8_t *)&new_data);
	if(ret !=BLE_STATUS_SUCCESS){printf("hum chatacteristics update failed  failed \r\n");}

	}

int bpm =85;
int weight=90;
int temp=20;
int hum=80;

void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
	if(handle==(rx_char_handle+1))
		{
			printf("RX Comm request\r\n");
			receive_data(att_data, data_length);
		}
	/*for notification , config data is {0x00, 0x01}.
	 * In this case handle for attribute of TX is offset by 2 from char handle	 */
	else if(handle == (tx_char_handle+2))
		{
			if(att_data[0] == 0x01)
			{
				notification_enabled = 1;
			}
		}
}

void Read_Request_CB(uint16_t handle)
	{
	if(handle==(bpm_char_handle+1))
		{
		printf("BPM request\r\n");
		update_bpm(bpm);
		bpm++;
		}

	if(handle==(weight_char_handle+1))
			{
			printf("weight request\r\n");
			update_weight(weight);
			weight++;
			}
	if(handle==(temp_char_handle+1))
			{
			printf("temp request\r\n");
			update_temp(temp);
			temp++;
			}
	if(handle==(hum_char_handle+1))
			{
			printf("hum request\r\n");
			update_hum(hum);
			hum++;
			}

	if(connection_handle !=0)
		{
		aci_gatt_allow_read(connection_handle);
		}
}

void aci_gatt_attribute_modified_event(uint16_t Connection_handle, uint16_t Attr_handle, uint16_t Offset, uint16_t Attr_data_length, uint8_t Attr_data[])
{
	Attribute_Modified_CB(Attr_handle, Attr_data_length, Attr_data);
}

void aci_gatt_notification_event(uint16_t Connection_handle, uint16_t Attribute_handle, uint8_t Attribute_value_length, uint8_t Attribute_value[])
{
	if(Attribute_handle == tx_char_handle+2 )
	{
		receive_data(Attribute_value, Attribute_value_length);
	}
}

void aci_gatt_read_permit_req_event(uint16_t Connection_Hanlde,
									uint16_t Atrribute_Handle,
									uint16_t Offset
									)
	{
	Read_Request_CB(Atrribute_Handle);
	}


void hci_le_connection_complete_event
									(uint8_t Status,
									uint16_t Connection_Handle,
									uint8_t Role,
									uint8_t Peer_Address_Type,
									uint8_t Peer_Address[6],
									uint16_t Conn_Interval,
									uint16_t Conn_Latency,
									uint16_t Supervision_Timeout,
									uint8_t Master_Clock_Accuracy
									)

	{
	connected=1;
	connection_handle=Connection_Handle;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	printf("Connected \r\n");
	}

void hci_le_disconnection_complete_event
										(uint8_t Status,
										 uint16_t Connection_Handle,
										 uint8_t Reason
										)
	{
	connected=0;
	set_connectable=1;
	Connection_Handle=0;
	printf("Disconnected \r\n");
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	}

//void hci_le_encrypt(uint8_t Key[16],
//                          uint8_t Plaintext_Data[16],
//                          uint8_t Encrypted_Data[16])
//{
//
//}

void APP_UserEvtRx(void *pData)
	{
	uint32_t i;
	hci_spi_pckt *hci_pckt =(hci_spi_pckt *)pData;
	if(hci_pckt->type==HCI_EVENT_PKT)
		{
		//Get data from packet
		hci_event_pckt *event_pckt =(hci_event_pckt*)hci_pckt->data;
		//process meta data
		if(event_pckt->evt ==EVT_LE_META_EVENT)
			{
			//get meta data
			evt_le_meta_event *evt =(void *)event_pckt->data;
			//process each meta data;
			for (i=0;i<(sizeof(hci_le_meta_events_table))/(sizeof(hci_le_meta_events_table_type));i++)
				{
					if(evt->subevent ==hci_le_meta_events_table[i].evt_code)
					{
						hci_le_meta_events_table[i].process((void *)evt->data);
					}
				}
			}
		//process vendor event
		else if(event_pckt->evt==EVT_VENDOR)
				{
				evt_blue_aci *blue_evt= (void *)event_pckt->data;
				for (i=0;i<(sizeof(hci_vendor_specific_events_table)/sizeof(hci_vendor_specific_events_table_type));i++)
					{
					if(blue_evt->ecode==hci_vendor_specific_events_table[i].evt_code)
						{
						hci_vendor_specific_events_table[i].process((void*)blue_evt->data);

						}

					}
				}
		else
			{
			for (i=0; i<(sizeof(hci_events_table)/sizeof(hci_events_table_type));i++)
					{

					if(event_pckt->evt==hci_events_table[i].evt_code)
						{
						hci_events_table[i].process((void*)event_pckt->data);
						}

					}
			}
		}




	}
