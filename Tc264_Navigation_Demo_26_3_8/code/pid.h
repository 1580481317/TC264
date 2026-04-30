/*
 * pid.h
 *
 *  Created on: 2026Äê3ÔÂ2ÈÕ
 *      Author: 29378
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_


extern float place_errorplace;
extern float place_last_errorplace;
extern float place_outplace;
extern float place_last_outplace;
extern float place_kpplace;
extern float place_kdplace;

extern float base_l_speed;
extern float l_speed_error;
extern float l_speed_last_error;
extern float l_speed_out;
extern float l_speed_last_out;
extern float l_speed_ki;
extern float l_speed_kp;

extern float base_r_speed;
extern float r_speed_error;
extern float r_speed_last_error;
extern float r_speed_out;
extern float r_speed_last_out;
extern float r_speed_ki;
extern float r_speed_kp;

void l_speed_pid(float aim_speed,float now_speed);
void r_speed_pid(float aim_speed,float now_speed);




#endif /* CODE_PID_H_ */
