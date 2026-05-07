/*
 * PID.c
 *
 *  Created on: 2026年1月12日
 *      Author: sun
 */
#include "zf_common_headfile.h"

Speed_Pid l_speed_pid;
Speed_Pid r_speed_pid;
Steer_Pid steer_pid;
//Angle_Pid angle_pid;

float speed_pwm_out = 0;
float steer_pwm_out = 0;
float angle_pwm_out = 0;

void pid_param_init(void)
{
    l_speed_pid.kp = 3.5;
    l_speed_pid.ki = 0.35;
    l_speed_pid.output=0;
    r_speed_pid.kp =3.5;
    r_speed_pid.ki = 0.35;
    r_speed_pid.output=0;

    steer_pid.kp  = 1.6;
    steer_pid.kp2 = 0;
    steer_pid.kd  = 0;
    steer_pid.gkd = -0.26;
}

float speed_pid_calc(int target, int input, Speed_Pid *pid)
{
    // 计算误差
    pid->error = (float)(target - input);

    // 增量式PI公式
    pid->output += pid->ki * pid->error +                       // 积分项
                   pid->kp * (pid->error - pid->last_error);    // 比例项

    // 更新上次误差
    pid->last_error = pid->error;

    // 输出限幅
    pid->output = limit(pid->output, pwm_duty_max, pwm_duty_min);

    return pid->output;
}

float steer_pid_calc(float image_error, float gyro, Steer_Pid *pid)
{
    float steer_value;
//    static float ki_total=0;
//    ki_total+=image_error;
    // 双PD公式
    steer_value = (image_error * pid->kp) +                         // 比例项
                  (image_error * fabsf(image_error) * pid->kp2) +   // 非线性项
                  ((image_error - pid->last_error) * pid->kd) +     // 微分项
//                  (ki_total*pid->ki)+                               //积分项
                  (gyro * pid->gkd) ;                          // 陀螺仪

    // 更新上次误差
    pid->last_error = image_error;

    // 输出限幅
    steer_value = limit(steer_value, 3000.0, -3000.0);
//    ips200_show_int(0,260,(image_error * pid->kp) * pid->kp,3);
//    ips200_show_int(40,260,ki_total,3);
//    ips200_show_int(80,260,(ki_total*pid->ki),3);

    return steer_value;
}

float angle_pid_calc(float target, float input, Angle_Pid *pid)
{
    pid->error = (target - input) * 50;

    // 位置式PD公式
    pid->output = pid->kp * pid->error +
                  pid->kd * (pid->error - pid->last_error);

    // 更新上次误差
    pid->last_error = pid->error;

    // 输出限幅
    pid->output = limit(pid->output, pwm_duty_max, pwm_duty_min);

    return pid->output;
}

float limit(float value, float max, float min)
{
    if (value > max) return max;
    if (value < min) return min;
    return value;
}




