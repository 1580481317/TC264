/*
 * Kalman.h
 *
 *  Created on: 2023骞�10鏈�1鏃�
 *      Author: Monst
 */
#ifndef __KALMAN_H_
#define __KALMAN_H_

typedef struct {
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float acc_x;
    float acc_y;
    float acc_z;
} icm_param_t;


typedef struct {
    float q0;
    float q1;
    float q2;
    float q3;
} quater_param_t;


typedef struct {
    float pitch;    //赂漏脩枚陆脟
    float roll;     //脝芦潞陆陆脟
    float yaw;      //路颅鹿枚陆脟
    float last_yaw;
    int8 Dirchange;
} euler_param_t;


typedef struct {
    float Xdata;
    float Ydata;
    float Zdata;
} gyro_param_t;

typedef struct Angle
{
    float pitch_temp;
    float roll_temp;
    float pitch;
    float roll;
    float yaw;
} Angle;


typedef struct First_Complement
{
    Angle angle;
} First_Complement;


typedef struct extKalman_t
{
    float X_last;
    float X_mid;
    float X_now;
    float P_mid;
    float P_now;
    float P_last;
    float kg;
    float A;
    float B;
    float Q;
    float R;
    float H;
} extKalman_t;


extern float yaw_total;
extern extKalman_t Kalman1;
extern extKalman_t Kalman2;
extern float Daty_Z;
extern float Daty_X;
extern float Daty_Y;
extern First_Complement first_complement;
extern icm_param_t icm_data;
extern euler_param_t eulerAngle;
extern float angle_Z,angle_R,angle_P;
extern bool GyroOffset_init;


float fast_sqrt(float num);
void First_complement(void);
float My_abs(float x);
void KalmanCreate(extKalman_t *p, float T_Q, float T_R);
float KalmanFilter(extKalman_t *p, float dat);
void IMU_YAW_integral(void);
void IMU_Handle_180(void);
void IMU_Handle_360(void);
void IMU_Handle_0(void);
void gyroOffset_init(void);
void ICM_getValues();
void gyro_init(void);
void gyro_proc(void);

//void ICM_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az);

//void ICM_getEulerianAngles(void);







void IMU_Handle_0(void);


#endif /* CODE_KALMAN_H_ */
