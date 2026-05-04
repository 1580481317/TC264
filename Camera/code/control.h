/*
 * control.h
 *
 *  Created on: 2026年1月12日
 *      Author: sun
 */

#ifndef CODE_CONTROL_H_
#define CODE_CONTROL_H_

#define pwm_duty_max         6000
#define pwm_duty_min         -6000

#define steer_pwm_duty_max   2000
#define steer_pwm_duty_min  -2000


extern float left_speed;
extern float right_speed;
extern float steer_out;
extern float left_speed_target;
extern float right_speed_target;
extern float Yaw;
void control_core(int16 mid_err,uint8 zero);

#endif /* CODE_CONTROL_H_ */
