/*
 * Motor.h
 *
 *  Created on: 2025年3月9日
 *      Author: Monst
 */
#include "zf_common_headfile.h"



MotorInf Mf;
float total1,total2;
int encodertest=0;
void Encoder(){
    float temp1,temp2;
    temp1=-encoder_get_count(ENCODER_1);
    Mf.Left_encoder = temp1 ;                             // 获取编码器计数
    total1 += Mf.Right_encoder ;
    encoder_clear_count(ENCODER_1);                                             // 清空编码器计数



    temp2=-encoder_get_count(ENCODER_3);
    Mf.Right_encoder=-temp2 ;                               // 获取编码器计数
    total2 += Mf.Left_encoder ;
    encoder_clear_count(ENCODER_3);                                             // 清空编码器计数

    encodertest=(temp1+temp2)/2;
}

void EncoderInit(){
    encoder_dir_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B);
    encoder_dir_init(ENCODER_3, ENCODER_3_A, ENCODER_3_B);
}

//电击（有刷）
void DRV8701E_init()
{
    gpio_init(L_DIR,GPO,1,GPO_PUSH_PULL);
    pwm_init(L_PWM,17000,0);                        // 函数简介     PWM 初始化

    gpio_init(R_DIR,GPO,1,GPO_PUSH_PULL);
    pwm_init(R_PWM,17000,0);
}

//开环驱动函数
void DRV8701E_DOUBLE_MOTOR_CRL(int32 L_SPEED,int32 R_SPEED)
{
    L_SPEED=L_SPEED>MAX_DRIVE_DUTY?MAX_DRIVE_DUTY:(L_SPEED<MIN_DRIVE_DUTY)?MIN_DRIVE_DUTY:L_SPEED;
    R_SPEED=R_SPEED>MAX_DRIVE_DUTY?MAX_DRIVE_DUTY:(R_SPEED<MIN_DRIVE_DUTY)?MIN_DRIVE_DUTY:R_SPEED;
    if(L_SPEED > 0)
        {
            gpio_set_level(L_DIR,0);
        }
        else
        {
            L_SPEED *=(-1);
            gpio_set_level(L_DIR,1);
        }

        if(R_SPEED > 0)
        {
            gpio_set_level(R_DIR,0);
        }
        else
        {
            R_SPEED *=(-1);
            gpio_set_level(R_DIR,1);
        }
        pwm_set_duty(L_PWM, L_SPEED);
        pwm_set_duty(R_PWM, R_SPEED);


}


