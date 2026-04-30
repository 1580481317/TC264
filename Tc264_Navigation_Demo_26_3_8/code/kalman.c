#include "zf_common_headfile.h"

#define delta_T     0.0025f
//0.0025f
float I_ex, I_ey, I_ez;
quater_param_t Q_info = {1, 0, 0};
euler_param_t eulerAngle;
icm_param_t icm_data;
gyro_param_t GyroOffset;
bool GyroOffset_init = 1;

//float param_Kp = 0.05; //四元数
//float param_Ki = 0.2;  //0.001
float angle_Z = 0,angle_R = 0,angle_P = 0;


float fast_sqrt(float num)
{
    float halfx = 0.5f * num;
    float y = num;
    long i = *(long *)&y;
    i = 0x5f375a86 - (i >> 1);

    y = *(float *)&i;
    y = y * (1.5f - (halfx * y * y));
    y = y * (1.5f - (halfx * y * y));
    return y;
}



First_Complement first_complement;

float Pitch_Max = 0;
float Pitch_Min = 0;
float Roll_Max = 0;
float Roll_Min = 0;
float Max_Delta_Pitch = 0;
float Max_Delta_Roll = 0;
extKalman_t Kalman1;
extKalman_t Kalman2;

//滤波
void First_complement()
{
    float k = 0.85;
    static float Pitch_Temp = 0;
    static float Roll_Temp = 0;
    static float Delta_Roll = 0;
    static char Pitch_Flag = 0;
    static char Roll_Flag = 0;

    Pitch_Temp = (atan(icm_data.acc_y / icm_data.acc_z)) * 180 / 3.1415926535f;
    Roll_Temp = (atan(icm_data.acc_x / icm_data.acc_z)) * 180 / 3.1415926535f;

    Pitch_Temp = Pitch_Temp + icm_data.gyro_x * 0.005f;
    Roll_Temp = Roll_Temp + icm_data.gyro_y * 0.005f;

    first_complement.angle.pitch = k * Pitch_Temp + (1 - k) * (first_complement.angle.pitch + icm_data.gyro_x * 0.005f);
    first_complement.angle.roll = k * Roll_Temp + (1 - k) * (first_complement.angle.roll + icm_data.gyro_y * 0.005f);

    if (Delta_Roll < 0.1 && Delta_Roll > -0.1)
    {
        first_complement.angle.roll = first_complement.angle.roll - Delta_Roll;
    }

    first_complement.angle.roll = KalmanFilter(&Kalman1, first_complement.angle.roll);
    first_complement.angle.pitch = KalmanFilter(&Kalman2, first_complement.angle.pitch);

    if (Pitch_Flag == 0)
    {
        Pitch_Min = first_complement.angle.roll;
        Pitch_Max = first_complement.angle.roll;

        Pitch_Flag = 1;
    }

    if (Roll_Flag == 0)
    {
        Roll_Min = first_complement.angle.pitch;
        Roll_Max = first_complement.angle.pitch;

        Roll_Flag = 1;
    }

    if (My_abs(Pitch_Min) > My_abs(first_complement.angle.roll))
    {
        Pitch_Min = My_abs(first_complement.angle.roll);
    }
    else if (My_abs(Pitch_Max) < My_abs(first_complement.angle.roll))
    {
        Pitch_Max = My_abs(first_complement.angle.roll);
    }

    if (My_abs(Roll_Min) > My_abs(first_complement.angle.pitch))
    {
        Roll_Min = My_abs(first_complement.angle.pitch);
    }
    else if (My_abs(Roll_Max) < My_abs(first_complement.angle.pitch))
    {
        Roll_Max = My_abs(first_complement.angle.pitch);
    }
    Max_Delta_Pitch = Pitch_Max - Pitch_Min;
    Max_Delta_Roll = Roll_Max - Roll_Min;
}

//绝对值
float My_abs(float x)
{
    return x < 0 ? -x : x;
}

//kalman
void KalmanCreate(extKalman_t *p, float T_Q, float T_R)
{
    p->X_last = (float)0;
    p->P_last = 0;
    p->Q = T_Q;
    p->R = T_R;
    p->A = 1;
    p->B = 0;
    p->H = 1;
    p->X_mid = p->X_last;
}


float KalmanFilter(extKalman_t *p, float dat)
{
    p->X_mid = p->A * p->X_last;
    p->P_mid = p->A * p->P_last + p->Q;
    p->kg = p->P_mid / (p->P_mid + p->R);
    p->X_now = p->X_mid + p->kg * (dat - p->X_mid);
    p->P_now = (1 - p->kg) * p->P_mid;
    p->P_last = p->P_now;
    p->X_last = p->X_now;
    return p->X_now;
}


float Daty_Z = 0;
float Daty_X = 0;
float Daty_Y = 0;

void IMU_YAW_integral()
{
    if (icm_data.gyro_z < 0.015 && icm_data.gyro_z > -0.015)
    {
        Daty_Z -= 0;
    }
    else
    {
        IMU_Handle_180();
        IMU_Handle_360();
        IMU_Handle_0();
    }

    if (icm_data.gyro_x < 0.015 && icm_data.gyro_x > -0.015)
    {
        Daty_X -= 0;
    }
    if (icm_data.gyro_y < 0.015 && icm_data.gyro_y > -0.015)
    {
        Daty_Y -= 0;
    }
}

void IMU_Handle_180()
{
    Daty_Z -= RAD_TO_ANGLE(icm_data.gyro_z * 0.005);

    if ((Daty_Z > 0 && Daty_Z <= 180) || (Daty_Z < 0 && Daty_Z >= (-180)))
    {
        Daty_Z = +Daty_Z;
    }
    else if (Daty_Z > 180 && Daty_Z <= 360)
    {
        Daty_Z -= 360;
    }
    else if (Daty_Z < (-180) && Daty_Z >= (-360))
    {
        Daty_Z += 360;
    }
}

char Round = 0;
float yaw_limit_360 = 0;

void IMU_Handle_360()
{
    if (Round == 0)
    {
        yaw_limit_360 -= RAD_TO_ANGLE(icm_data.gyro_z * 0.005);
        if (yaw_limit_360 > 360)
        {
            yaw_limit_360 = 360;
            Round = 1;
        }
        else if (yaw_limit_360 < -360)
        {
            yaw_limit_360 = -360;
            Round = 1;
        }
    }
    if (Round == 1)
    {
        if (yaw_limit_360 <= 360 && yaw_limit_360 >= -360)
        {
            yaw_limit_360 += RAD_TO_ANGLE(icm_data.gyro_z * 0.005);
            if (yaw_limit_360 > 360)
            {
                yaw_limit_360 = 360;
                Round = 0;
            }
        }
        else if (yaw_limit_360 >= -360)
        {
            yaw_limit_360 -= RAD_TO_ANGLE(icm_data.gyro_z * 0.005);
            if (yaw_limit_360 < -360)
            {

                yaw_limit_360 = -360;
                Round = 0;
            }
        }
    }
}

float yaw_total = 0;

void IMU_Handle_0()
{
    yaw_total += RAD_TO_ANGLE(icm_data.gyro_z * 0.005);
}



//零票初始化
void gyroOffset_init(void)
{
    GyroOffset.Xdata = 0;
    GyroOffset.Ydata = 0;
    GyroOffset.Zdata = 0;
    for (uint16_t i = 0; i < 1000; ++i) {
    imu963ra_get_gyro();
    imu963ra_get_acc();

//    imu660ra_get_acc();
//    imu660ra_get_gyro();
    GyroOffset.Xdata += imu963ra_gyro_x;
    GyroOffset.Ydata += imu963ra_gyro_y;
    GyroOffset.Zdata += imu963ra_gyro_z;

//    GyroOffset.Xdata += imu660ra_gyro_x;
//    GyroOffset.Ydata += imu660ra_gyro_y;
//    GyroOffset.Zdata += imu660ra_gyro_z;
    system_delay_ms(5);
    }
    GyroOffset.Xdata /= 1000;
    GyroOffset.Ydata /= 1000;
    GyroOffset.Zdata /= 1000;
    eulerAngle.Dirchange=0;
    GyroOffset_init = 0;
}


#define alpha           0.4f

//获得角度
void ICM_getValues() {

    if(GyroOffset_init == 1)
    {

        gyroOffset_init();
    }

//    imu660ra_get_gyro();
//    imu660ra_get_acc();

      imu963ra_get_gyro();
      imu963ra_get_acc();

      icm_data.acc_x = (((float)imu963ra_acc_x) * 0.3f) + icm_data.acc_x * 0.7f;
      icm_data.acc_y = (((float)imu963ra_acc_y) * 0.3f) + icm_data.acc_y * 0.7f;
      icm_data.acc_z = (((float)imu963ra_acc_z) * 0.3f) + icm_data.acc_z * 0.7f;



      icm_data.gyro_x = ((float) imu963ra_gyro_x - GyroOffset.Xdata) * PI / 180 / 14.3f;
      icm_data.gyro_y = ((float) imu963ra_gyro_y - GyroOffset.Ydata) * PI / 180 / 14.3f;
      icm_data.gyro_z = ((float) imu963ra_gyro_z - GyroOffset.Zdata) * PI / 180 / 14.3f;
//    icm_data.acc_x = (((float)imu660ra_acc_x) * 0.3f) + icm_data.acc_x * 0.7f;
//    icm_data.acc_y = (((float)imu660ra_acc_y) * 0.3f) + icm_data.acc_y * 0.7f;
//    icm_data.acc_z = (((float)imu660ra_acc_z) * 0.3f) + icm_data.acc_z * 0.7f;
//
//
//
//    icm_data.gyro_x = ((float) imu660ra_gyro_x - GyroOffset.Xdata) * PI / 180 / 16.4f;
//    icm_data.gyro_y = ((float) imu660ra_gyro_y - GyroOffset.Ydata) * PI / 180 / 16.4f;
//    icm_data.gyro_z = ((float) imu660ra_gyro_z - GyroOffset.Zdata) * PI / 180 / 16.4f;

}

void gyro_init(void)
{
     imu963ra_init();
//    imu660ra_init();
//    gyroOffset_init();
}

void gyro_proc(void)
{
    ICM_getValues();
    IMU_YAW_integral();
    First_complement();
}
/*
//四元数解算
void ICM_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az) {
    float halfT = 0.5 * delta_T;
    float vx, vy, vz;
    float ex, ey, ez;
    float q0 = Q_info.q0;
    float q1 = Q_info.q1;
    float q2 = Q_info.q2;
    float q3 = Q_info.q3;
    float q0q0 = q0 * q0;
    float q0q1 = q0 * q1;
    float q0q2 = q0 * q2;
    //float q0q3 = q0 * q3;
    float q1q1 = q1 * q1;
    //float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q3q3 = q3 * q3;
    float delta_2 = 0.17;

    float norm = fast_sqrt(ax * ax + ay * ay + az * az);
    ax = ax * norm;
    ay = ay * norm;
    az = az * norm;

    vx = 2 * (q1q3 - q0q2);
    vy = 2 * (q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3;
    //vz = (q0*q0-0.5f+q3 * q3) * 2;

    ex = ay * vz - az * vy;
    ey = az * vx - ax * vz;
    ez = ax * vy - ay * vx;


    I_ex += halfT * ex;
    I_ey += halfT * ey;
    I_ez += halfT * ez;

    gx = gx + param_Kp * ex + param_Ki * I_ex;
    gy = gy + param_Kp * ey + param_Ki * I_ey;
    gz = gz + param_Kp * ez + param_Ki * I_ez;




    q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
    q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
    q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
    q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;
    delta_2=(2*halfT*gx)*(2*halfT*gx)+(2*halfT*gy)*(2*halfT*gy)+(2*halfT*gz)*(2*halfT*gz);

    q0 = (1-delta_2/8)*q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
    q1 = (1-delta_2/8)*q1 + (q0*gx + q2*gz - q3*gy)*halfT;
    q2 = (1-delta_2/8)*q2 + (q0*gy - q1*gz + q3*gx)*halfT;
    q3 = (1-delta_2/8)*q3 + (q0*gz + q1*gy - q2*gx)*halfT;



    norm = fast_sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    Q_info.q0 = q0 * norm;
    Q_info.q1 = q1 * norm;
    Q_info.q2 = q2 * norm;
    Q_info.q3 = q3 * norm;
}*/


/*//得到航向角
void ICM_getEulerianAngles(void) {
    static uint8 change_f=0;
    ICM_getValues();
    if(icm_data.gyro_z < 0.110 && icm_data.gyro_z > -0.110) {angle_Z-= 0;}
    else{


    ICM_AHRSupdate(icm_data.gyro_x, icm_data.gyro_y, icm_data.gyro_z, icm_data.acc_x, icm_data.acc_y, icm_data.acc_z);  //四元数解算
    float q0 = Q_info.q0;
    float q1 = Q_info.q1;
    float q2 = Q_info.q2;
    float q3 = Q_info.q3;


    eulerAngle.pitch = asin(-2 * q1 * q3 + 2 * q0 * q2) * 180 / PI; // pitch
    eulerAngle.roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 180 / PI; // roll
    eulerAngle.yaw =  atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2 * q2 - 2 * q3 * q3 + 1) * 180/ PI; // yaw

//
    if (eulerAngle.yaw >= 180) { eulerAngle.yaw -= 360; } else if (eulerAngle.yaw <= -180) { eulerAngle.yaw += 360; }

    //偏航角航向处理
    if((eulerAngle.yaw-eulerAngle.last_yaw) < -350) eulerAngle.Dirchange++;
    else if ((eulerAngle.yaw-eulerAngle.last_yaw) > 350) eulerAngle.Dirchange--;

    angle_Z=360*eulerAngle.Dirchange+eulerAngle.yaw;//180*eulerAngle.Dirchange+eulerAngle.yaw
    eulerAngle.last_yaw=eulerAngle.yaw;
    }
}*/



