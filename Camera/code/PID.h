/*
 * PID.h
 *
 *  Created on: 2026年1月12日
 *      Author: sun
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

typedef struct  //速度增量式PID结构体
{
    float kp;
    float ki;
    float kd;
    float error;
    float last_error;
    float output;
} Speed_Pid;

typedef struct  //转角值计算PID
{
    float kp;
    float kp2;
    float kd;
    float gkd;
    float last_error;
    float ki;
} Steer_Pid;

typedef struct  //位置式PID
{
    float kp;
    float ki;
    float kd;
    float error;
    float last_error;
    float output;
} Angle_Pid;

extern Speed_Pid l_speed_pid;
extern Speed_Pid r_speed_pid;
extern Steer_Pid steer_pid;


float limit(float value, float max, float min);
float angle_pid_calc(float target, float input, Angle_Pid *pid);        //位置环
float steer_pid_calc(float image_error, float gyro, Steer_Pid *pid);    //转角值PID
float speed_pid_calc(int target, int input, Speed_Pid *pid);            //增量式速度环
void pid_param_init(void);

#endif /* CODE_PID_H_ */
