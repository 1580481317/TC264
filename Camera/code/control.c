/*
 * control.c
 *
 *  Created on: 2026年1月12日
 *      Author: sun
 */
#include "zf_common_headfile.h"

#define speed_base     30            //基础速度PWM占空比15%

float steer_out=0;
float left_speed_target=speed_base;
float right_speed_target=speed_base;
float left_speed=0;
float right_speed=0;
float Yaw=0;                             //偏航角
int8 run=1;                              //可以跑的标志位，用于跟

void control_core(int16 mid_err,uint8 zero)
{
    if(flag==1 || flag==2){steer_pid.kp  = 2.6;}
    else{steer_pid.kp  = 1.8;}
        steer_out=steer_pid_calc((float)mid_err,imu660rb_gyro_transition((imu660rb_gyro_z-zero)),&steer_pid);              //计算转角值，存入steer_out中
        left_speed_target=speed_base+steer_out;                             //左轮目标速度=基础速度加转角值
        right_speed_target=speed_base-steer_out;                            //右轮目标速度=基础速度减转角值
        if(left_speed_target<0)    {left_speed_target=0;}
        if(right_speed_target<0)   {right_speed_target=0;}


        left_speed=speed_pid_calc(left_speed_target,motor_encoder_left.encoder_speed,&l_speed_pid);         //增量式速度环算出左轮速度
        right_speed=speed_pid_calc(right_speed_target,motor_encoder_right.encoder_speed,&r_speed_pid);      //增量式速度环算出右轮速度
        if(left_speed<0)    {left_speed=0;}
        if(right_speed<0)   {right_speed=0;}
        if(suo==1)
        {
            MotorL_SetSpeed(0);
            MotorR_SetSpeed(0);
        }
        else
        {
            MotorL_SetSpeed(left_speed);
            MotorR_SetSpeed(right_speed);
        }
}











