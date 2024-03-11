/*
 * app_blenrg.c
 *
 *  Created on: Nov 27, 2023
 *      Author: kaushik
 */


#include "app_blenrg.h"

#include "bluenrg_conf.h"
#include "hci.h"
#include "bluenrg1_types.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include "bluenrg1_events.h"
#include "hci_tl.h"
#include "service.h"
#include <main.h>
#include <stdio.h>


#define BDARR_SIZE 6
#define server_addr 6

uint16_t SERVER_BDARR[] = {0x01,0x02,0x03,0x04,0x04,0x05,0x06};
uint8_t btn_state;
uint8_t DIV[2];
uint8_t ER[16];
uint8_t IR[16];

tBleStatus ret;

uint16_t service_handle, dev_name_handle, appearance_char_handle;


tBleStatus authorization_callback(uint16_t handle, uint8_t authorization_enable) {
    /* Check your authorization logic here */
    if (authorization_enable == AUTHORIZATION_REQUIRED) {
        /* Perform authorization based on your criteria */

        /* For simplicity, always authorize in this example */
        return aci_gap_authorization_resp(service_handle, CONNECTION_AUTHORIZED);
    }

    return BLE_STATUS_SUCCESS;  // Always return success if no authorization is required
}

void blenrg_init(void)
{

	uint8_t bdaddr[BDARR_SIZE];
	Char_UUID_t server_uuid;

	const char *name = "BLE_Dev";


	BLUENRG_memcpy(bdaddr, SERVER_BDARR, sizeof(SERVER_BDARR));
	BLUENRG_memcpy(server_uuid.Char_UUID_128,SERVER_BDARR,16);

	hci_init(APP_UserEvtRx, NULL);

	hci_reset();

	HAL_Delay(100);

	ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);

//	/* Configure read root key DIV on BlueNRG, BlueNRG-MS device */
//	ret = aci_hal_write_config_data(CONFIG_DATA_DIV_OFFSET,	CONFIG_DATA_DIV_LEN,(uint8_t *) DIV);
//
//	/* Configure read root key ER on BlueNRG, BlueNRG-MS device */
//	ret = aci_hal_write_config_data(CONFIG_DATA_ER_OFFSET,	CONFIG_DATA_ER_LEN,(uint8_t *) ER);
//
//	/* Configure read root key IR on BlueNRG, BlueNRG-MS device */
//	ret = aci_hal_write_config_data(CONFIG_DATA_IR_OFFSET,CONFIG_DATA_IR_LEN,(uint8_t *) IR);


	ret = aci_gatt_init();
	if(ret != BLE_STATUS_SUCCESS)
		{
			printf("aci_gatt_init : FAILED!! \n\r");
		}


	ret = aci_gap_init(GAP_PERIPHERAL_ROLE, 0, 0x07, &service_handle, &dev_name_handle, &appearance_char_handle);
//	ret = aci_gap_init(GAP_PERIPHERAL_ROLE, 0x02, sizeof (name), &service_handle, &dev_name_handle, &appearance_char_handle);
		if(ret != BLE_STATUS_SUCCESS)
		{
			printf("aci_gap_init : FAILED!! \n\r");
		}

		/*Update characteristics value*/
	ret= aci_gap_set_io_capability(IO_CAP_DISPLAY_ONLY);
		if (ret != BLE_STATUS_SUCCESS)
				{printf("Failure.\n");}


				/*Update characteristics value*/
//	ret = aci_gap_set_authorization_requirement(service_handle, AUTHORIZATION_REQUIRED);
//		if (ret != BLE_STATUS_SUCCESS) {
//		    printf("aci_gap_set_authorization_requirement : FAILED!! \n\r");
//		}

//	ret = aci_gap_authorization_resp(service_handle, CONNECTION_AUTHORIZED);
//		if (ret != BLE_STATUS_SUCCESS) {
//			    printf("aci_gap_set_authorization_requirement : FAILED!! \n\r");
//			}

	ret=aci_gap_set_authentication_requirement(NO_BONDING,/*bonding is	enabled */
						MITM_PROTECTION_REQUIRED,
						SC_IS_SUPPORTED,/*Secure connection	supported	but optional */
						KEYPRESS_IS_SUPPORTED,
						7, /* Min encryption key size */
						16, /* Max encryption	key size */
						USE_FIXED_PIN_FOR_PAIRING, /* fixed pin is not used*/
						123456, /* fixed pin */
						STATIC_RANDOM_ADDR /* Public Identity address type */);
		if (ret != BLE_STATUS_SUCCESS)
					{printf("Failure.\n");}


//	ret = aci_gap_slave_security_req(service_handle);
//		if(ret != BLE_STATUS_SUCCESS){
//					printf("aci_gap_slave_security_req : FAILED!! \n\r");
//				}
//
//	aci_gap_pass_key_req_event(service_handle);
//		if(ret != BLE_STATUS_SUCCESS)
//		{
//			printf("aci_gap_pass_key_req_event : FAILED!! \n\r");
//		}
//	ret = aci_gatt_add_char(service_handle,UUID_TYPE_128,&server_uuid,2,CHAR_PROP_NOTIFY,ATTR_PERMISSION_ENCRY_READ ,GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,16,1,&appearance_char_handle);
//		if(ret != BLE_STATUS_SUCCESS)
//				{
//					printf("aci_gatt_update_char_value : FAILED!! \n\r");
//				}

	ret = aci_gatt_update_char_value(service_handle, dev_name_handle, 0, strlen(name), (uint8_t *)name);
		if(ret != BLE_STATUS_SUCCESS)
			{
				printf("aci_gatt_update_char_value : FAILED!! \n\r");
			}

	ret = aci_gatt_set_security_permission(service_handle,dev_name_handle,ATTR_PERMISSION_AUTHEN_READ |ATTR_PERMISSION_AUTHOR_READ );
		if(ret != BLE_STATUS_SUCCESS)
				{
						printf("aci_gatt_set_security_permission : FAILED!! \n\r");
				}


	ret = aci_gap_clear_security_db();
		if(ret != BLE_STATUS_SUCCESS)
			{
					printf("aci_gap_clear_security_db : FAILED!! \n\r");
			}

	ret = add_service();

	if(ret != BLE_STATUS_SUCCESS)
				{
					printf("add_simple_service : FAILED!! \n\r");
				}


//	ret = hci_le_set_random_address();

}


void blenrg_process(void)
{
	tBleStatus ret;
	uint8_t local_name[] = {AD_TYPE_SHORTENED_LOCAL_NAME, 'B', 'L', 'E', '_', 'D', 'E', 'V'} ;

//	uint8_t ServiceUUID_Scan[31] = {0x11,0x06,0x8a,0x97,0xf7,0xc0,0x85,0x06,0x11,0xe3,0xba,0xa7,0x08,0x00,0x2,0x0c,0x9a,0x66};
//
//	hci_le_set_scan_response_data(31,ServiceUUID_Scan);

	ret = aci_gap_set_discoverable(ADV_IND, 100, 100, PUBLIC_ADDR, NO_WHITE_LIST_USE , sizeof(local_name), local_name, 0, NULL, 0,0);
		if(ret != BLE_STATUS_SUCCESS)
			{
				printf("aci_gap_set_discoverable : FAILED!! \n\r");
			}

//	ret = aci_gap_set_undirected_connectable(0x100,0x200,PUBLIC_ADDR, WHITE_LIST_FOR_ALL);


	hci_user_evt_proc();

	btn_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

	if(btn_state == 0)
	{
		uint8_t data[2] = {'a','b'};
		send_data(data,sizeof(data));

	}

}



