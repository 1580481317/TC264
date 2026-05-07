/*
 * motor.h
 *
 *  Created on: 2026年1月12日
 *      Author: sun
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_common_headfile.h"

#define MotorL_pwm      ATOM2_CH5_P11_10
#define MotorL_turn     P11_9

#define MotorR_pwm      ATOM2_CH7_P11_12
#define MotorR_turn     P11_11

typedef struct motor1
{
        int targrt_speed;
        int duty;
        int encoder_speed;
        int encoder_raw;
        int32 total_encoder;
}motor1;

extern motor1 motor_encoder_left;
extern motor1 motor_encoder_right;

void Motor_Init(void);
void MotorL_SetSpeed(int pwm);      //左电机速度设置（最大10000）
void MotorR_SetSpeed(int pwm);      //右电机速度设置（最大10000）
void Encoder_Init(void);
void Encoder_Data_Get(void);
#endif /* CODE_MOTOR_H_ */
