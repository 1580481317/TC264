/*
 * key.c
 *
 *  Created on: 2026年2月21日
 *      Author: 29378
 */


#include "zf_common_headfile.h"

//按键
uint8 key1_state = 1;
uint8 key2_state = 1;
uint8 key3_state = 1;
uint8 key4_state = 1;

uint8 key1_state_last = 0 ;
uint8 key2_state_last = 0 ;
uint8 key3_state_last = 0 ;
uint8 key4_state_last = 0 ;

uint8 key1_flag;
uint8 key2_flag;
uint8 key3_flag;
uint8 key4_flag;

void Key_init()
{
    gpio_init(KEY1,GPI,1,GPI_PULL_UP);
    gpio_init(KEY2,GPI,1,GPI_PULL_UP);
    gpio_init(KEY3,GPI,1,GPI_PULL_UP);
    gpio_init(KEY4,GPI,1,GPI_PULL_UP);
}
//消抖
void Key_scan()
{
    //保存当前按键状态
    key1_state_last=key1_state;
    key2_state_last=key2_state;
    key3_state_last=key3_state;
    key4_state_last=key4_state;

    //更新当前按键状态
    key1_state = gpio_get_level(KEY1);
    key2_state = gpio_get_level(KEY2);
    key3_state = gpio_get_level(KEY3);
    key4_state = gpio_get_level(KEY4);

    //比较状态
    //比较状态
    if(key1_state && !key1_state_last) {key1_flag = 1;}
    if(key2_state && !key2_state_last) {key2_flag = 1;}
    if(key3_state && !key3_state_last) {key3_flag = 1;}
    if(key4_state && !key4_state_last) {key4_flag = 1;}

}

void key_work(void)
{

    if(key1_flag&&!key2_flag&&!key3_flag&&!key4_flag)
            {
                key1_flag = 0;
                N.End_f=1;
                gpio_set_level(LED1, 0);
                gpio_set_level(LED2, 1);
                gpio_set_level(LED3, 1);
                gpio_set_level(LED4, 1);
            }

            if(key2_flag&&!key1_flag&&!key3_flag&&!key4_flag)
            {
                key2_flag = 0;
                N.Nag_SystemRun_Index=1;
                gpio_set_level(LED1, 1);
                gpio_set_level(LED2, 0);
                gpio_set_level(LED3, 1);
                gpio_set_level(LED4, 1);
            }

            if(key3_flag&&!key2_flag&&!key1_flag&&!key4_flag)
            {
                key3_flag = 0;
                N.Nag_SystemRun_Index=2;
                N.Flash_page_index = Nag_Start_Page;   // 从起始页开始
                N.Flash_read_f = 0;                     // 强制重新读取
                N.size = 0;
                N.Save_count = 0;
                N.Mileage_All = 0;
                N.Nag_Stop_f = 0;
                gpio_set_level(LED1, 1);
                gpio_set_level(LED2, 1);
                gpio_set_level(LED3, 0);
                gpio_set_level(LED4, 1);

            }

            if(key4_flag&&!key2_flag&&!key3_flag&&!key1_flag)
                    {
                key4_flag = 0;
                N.Nag_SystemRun_Index=2;
                gpio_set_level(LED1, 1);
                gpio_set_level(LED2, 1);
                gpio_set_level(LED3, 1);
                gpio_set_level(LED4, 0);

                    }
}

void Buzzer_init()
{
    gpio_init(BEEP,GPO,0,GPO_PUSH_PULL);
}

void Buzzer_check(int TIME)
{
    gpio_set_level(BEEP, 1);//响
    system_delay_ms(TIME);
    gpio_set_level(BEEP, 0);
}

// LED

void LED_init()
{
    gpio_init(LED1, GPO, 1, GPO_PUSH_PULL);          // 初始化 LED1 输出 默认高电平 推挽输出模式
    gpio_init(LED2, GPO, 1, GPO_PUSH_PULL);         // 初始化 LED2 输出 默认高电平 推挽输出模式
    gpio_init(LED3, GPO, 1, GPO_PUSH_PULL);          // 初始化 LED3 输出 默认高电平 推挽输出模式
    gpio_init(LED4, GPO, 1, GPO_PUSH_PULL);         // 初始化 LED4 输出 默认高电平 推挽输出模式

}


void Encoder_text(void)//电机测试
{

       static int32 L_SPEED=0,R_SPEED=0;

       Key_scan();

          if(key1_flag)//左电机+500
             {
                 key1_flag=0;
                 L_SPEED+=500;
             }
          if(key2_flag)//左电机-500
             {
                 key2_flag=0;
                 L_SPEED-=500;
             }
          if(key3_flag)//右电机+500
             {
                 key3_flag=0;
                 R_SPEED+=500;
             }
          if(key4_flag)//右电机-500
             {
                 key4_flag=0;
                 R_SPEED-=500;
             }


          ips_show_string(8*0,16*1, "Encoder_text");

          ips_show_string(8*0,16*3, "L:");          ips_show_int(8*7,  16*3,L_SPEED, 5);
          ips_show_string(8*0,16*4, "L_A:");        ips_show_int(8*7,  16*4,Mf.Left_encoder,5);

          ips_show_string(8*0,16*6, "R:");          ips_show_int(8*7,  16*6,R_SPEED, 5);
          ips_show_string(8*0,16*7, "R_A:");        ips_show_int(8*7,  16*7,Mf.Right_encoder, 5);

          DRV8701E_DOUBLE_MOTOR_CRL(L_SPEED,R_SPEED);
}
