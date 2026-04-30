/*
 * key.h
 *
 *  Created on: 2026年2月21日
 *      Author: 29378
 */

#ifndef CODE_COMMON_PERIPHERALS_H_
#define CODE_COMMON_PERIPHERALS_H_

#define KEY1                    P20_6
#define KEY2                    P20_7
#define KEY3                    P11_2
#define KEY4                    P11_3

#define LED1                    (P20_9)
#define LED2                    (P20_8)
#define LED3                    (P21_5)
#define LED4                    (P21_4)

#define BEEP                    (P33_10)

//IPS114宏定义     240*114
#define ips_show_string                  ips114_show_string
#define ips_show_int                     ips114_show_int
#define ips_show_uint                    ips114_show_uint
#define ips_show_float                   ips114_show_float
#define ips_show_chinese                 ips114_show_chinese
#define ips_clear                        ips114_clear
#define ips_init                         ips114_init
#define ips_show_rgb565_image            ips114_show_rgb565_image

//IPS200宏定义     240*320
//#define ips_show_string                  ips200_show_string
//#define ips_show_int                     ips200_show_int
//#define ips_show_uint                    ips200_show_uint
//#define ips_show_float                   ips200_show_float
//#define ips_show_chinese                 ips200_show_chinese
//#define ips_clear                        ips200_clear
//#define ips_init                         ips200_init
//#define ips_show_rgb565_image            ips200_show_rgb565_image

//extern uint8 key1_flag;
//extern uint8 key2_flag;
//extern uint8 key3_flag;
//extern uint8 key4_flag;


void Key_init(void);
void Key_scan(void);
void key_work(void);

void Buzzer_init(void);
void Buzzer_check(int TIME);

void LED_init(void);
void Encoder_text(void);

//void DRV8701E_DOUBLE_MOTOR_CRL(int32 L_SPEED,int32 R_SPEED);














#endif /* CODE_COMMON_PERIPHERALS_H_ */
