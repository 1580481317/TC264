/*
 * speed.c
 *
 *  Created on: 2026年3月2日
 *      Author: 29378
 */
#include "zf_common_headfile.h"
#include "speed.h"

int32 base_speed = 170;//1500占空比

//取值建议：800~1500（确保轮子能转，且不超速）


// 2. 比例系数：将角度偏差转为速度修正值（调试关键）
// 取值建议：20~50（值越大，转向越灵敏；值越小，转向越平缓）
int32 kp = 25;  //35  30  25  40  30  25 20  50  40
// 3. 计算修正值（仅用你代码的N.Final_Out）
int32 left_speed=0;
int32 right_speed=0;

int32 correction ;
float l_speed_now=0;
float r_speed_now=0;
            // 核心转化：按偏差调整左右轮速度

int32 speed_out_max = MAX_DRIVE_DUTY-300 ;
int32 speed_out_min = MIN_DRIVE_DUTY+300 ;

void speed_out(void)
{
    if (N.Nag_Stop_f)
       {
           base_speed = 0;
           left_speed = 0;
           right_speed = 0;

           return;
       }
    correction = N.Final_Out * kp;

    l_speed_out=l_speed_out>speed_out_max?(float)speed_out_max:(l_speed_out<speed_out_min)?(float)speed_out_min:l_speed_out;
    r_speed_out=r_speed_out>speed_out_max?(float)speed_out_max:(r_speed_out<speed_out_min)?(float)speed_out_min:r_speed_out;
//    left_speed = base_speed - correction;
//    right_speed = base_speed + correction;
//    left_speed = l_speed_out - correction;
        right_speed = r_speed_out - correction;
        left_speed =  l_speed_out + correction;
//        right_speed = r_speed_out;
//    if(N.Nag_SystemRun_Index==2)
//            {
//    DRV8701E_DOUBLE_MOTOR_CRL(left_speed,right_speed);
//    if (N.Nag_Stop_f)
//           {
//               left_speed = 0;
//               right_speed = 0;
//               return;
//           }
//            }
}
