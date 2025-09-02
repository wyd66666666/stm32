#ifndef __COMMON_H__
#define __COMMON_H__	 

#include "usart.h"		 
//#include "malloc.h"
#include "string.h"    
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� ������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
	

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void atk_8266_init(void);

uint8_t atk_8266_mode_cofig(uint8_t netpro);
void atk_8266_at_response(uint8_t mode);
uint8_t* atk_8266_check_cmd(uint8_t *str);
uint8_t atk_8266_apsta_check(void);
uint8_t atk_8266_send_data(uint8_t *data,uint8_t *ack,uint16_t waittime);
uint8_t atk_8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime);
uint8_t atk_8266_quit_trans(void);
uint8_t atk_8266_consta_check(void);
void atk_8266_load_keyboard(uint16_t x,uint16_t y);
void atk_8266_key_staset(uint16_t x,uint16_t y,uint8_t keyx,uint8_t sta);
uint8_t atk_8266_get_keynum(uint16_t x,uint16_t y);
void atk_8266_get_wanip(uint8_t* ipbuf);
void atk_8266_get_ip(uint8_t x,uint8_t y);
void atk_8266_msg_show(uint16_t x,uint16_t y,uint16_t wanip);
void atk_8266_wificonf_show(uint16_t x,uint16_t y,uint8_t* rmd,uint8_t* ssid,uint8_t* encryption,uint8_t* password);
uint8_t atk_8266_netpro_sel(uint16_t x,uint16_t y,uint8_t* name);
void atk_8266_mtest_ui(uint16_t x,uint16_t y);

uint8_t atk_8266_ip_set(uint8_t* title,uint8_t* mode,uint8_t* port,uint8_t* ip);
void atk_8266_test(void);



uint8_t atk_8266_apsta_test(void);	//WIFI AP+STAģʽ����
uint8_t atk_8266_wifista_test(void);	//WIFI STA����
uint8_t atk_8266_wifiap_test(void);	//WIFI AP����

//�û����ò���
extern const uint8_t* portnum;			//���Ӷ˿�
 
extern const uint8_t* wifista_ssid;		//WIFI STA SSID
extern const uint8_t* wifista_encryption;//WIFI STA ���ܷ�ʽ
extern const uint8_t* wifista_password; 	//WIFI STA ����

extern const uint8_t* wifiap_ssid;		//WIFI AP SSID
extern const uint8_t* wifiap_encryption;	//WIFI AP ���ܷ�ʽ
extern const uint8_t* wifiap_password; 	//WIFI AP ����

extern uint8_t* kbd_fn_tbl[2];
extern const uint8_t* ATK_ESP8266_CWMODE_TBL[3];
extern const uint8_t* ATK_ESP8266_WORKMODE_TBL[3];
extern const uint8_t* ATK_ESP8266_ECN_TBL[5];
#endif





