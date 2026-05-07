/*
 * motor.c
 *
 *  Created on: 2026年1月12日
 *      Author: sun
 */
#include "motor.h"

motor1 motor_encoder_left;
motor1 motor_encoder_right;


//电机初始化
void Motor_Init(void)
{
    gpio_init(MotorL_turn, GPO, 1, GPO_PUSH_PULL);
    pwm_init(MotorL_pwm, 17000, 0);

    gpio_init(MotorR_turn, GPO, 1, GPO_PUSH_PULL);
    pwm_init(MotorR_pwm, 17000, 0);

}

//编码器初始化
void Encoder_Init(void)
{
    encoder_dir_init(TIM4_ENCODER, TIM4_ENCODER_CH1_P02_8, TIM4_ENCODER_CH2_P00_9);//左编码器
    encoder_dir_init(TIM6_ENCODER, TIM6_ENCODER_CH1_P20_3, TIM6_ENCODER_CH2_P20_0);//右编码器
}


//左电机设速
void MotorL_SetSpeed(int pwm)
{
    if(pwm>=0)
    {
        gpio_set_level(MotorL_turn, 0);
        pwm_set_duty(MotorL_pwm, pwm);
    }
    else
    {
        gpio_set_level(MotorL_turn, 1);
        pwm_set_duty(MotorL_pwm, -pwm);
    }
}

//右电机设速
void MotorR_SetSpeed(int pwm)
{
    if(pwm>=0)
    {
        gpio_set_level(MotorR_turn, 0);
        pwm_set_duty(MotorR_pwm, pwm);
    }
    else
    {
        gpio_set_level(MotorR_turn, 1);
        pwm_set_duty(MotorR_pwm, -pwm);
    }
}

void Encoder_Data_Get(void)
{
    //左编码
    motor_encoder_left.encoder_raw=-encoder_get_count(TIM4_ENCODER);
    motor_encoder_left.encoder_speed=0.2*motor_encoder_left.encoder_speed+0.8*motor_encoder_left.encoder_raw;
    motor_encoder_left.total_encoder+=motor_encoder_left.encoder_raw;
    encoder_clear_count(TIM4_ENCODER);
    //右编码
    motor_encoder_right.encoder_raw=encoder_get_count(TIM6_ENCODER);
    motor_encoder_right.encoder_speed=0.2*motor_encoder_right.encoder_speed+0.8*motor_encoder_right.encoder_raw;
    motor_encoder_right.total_encoder+=motor_encoder_right.encoder_raw;
    encoder_clear_count(TIM6_ENCODER);
}





