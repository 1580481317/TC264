/*
 * Motor.h
 *
 *  Created on: 2025年3月9日
 *      Author: Monst
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_


#define ENCODER_1                       (TIM4_ENCODER)
#define ENCODER_1_A                     (TIM4_ENCODER_CH1_P02_8)
#define ENCODER_1_B                     (TIM4_ENCODER_CH2_P00_9)

#define ENCODER_3                       (TIM6_ENCODER)
#define ENCODER_3_A                     (TIM6_ENCODER_CH1_P20_3)
#define ENCODER_3_B                     (TIM6_ENCODER_CH2_P20_0)  //DIR 对应的引脚
typedef struct{
        int Right_encoder;
        int Left_encoder;
}MotorInf;
extern MotorInf Mf;

extern float total1,total2;

//变量声明
#define L_PWM        ATOM3_CH5_P11_10
#define L_DIR       P11_9
#define R_PWM        ATOM2_CH7_P11_12
#define R_DIR       P11_11

/*#define L_DIR    P11_10
#define L_PWM    ATOM3_CH4_P11_9

#define R_DIR    P11_12
#define R_PWM    ATOM3_CH6_P11_11*/


#define MAX_DRIVE_DUTY   1600
#define MIN_DRIVE_DUTY  -1600
extern int16 L_SPEED, R_SPEED;
//全局变量
extern int16 L_SPEED,R_SPEED;

void Encoder(void);
void EncoderInit(void);


void DRV8701E_init(void);
void DRV8701E_DOUBLE_MOTOR_CRL(int32 L_SPEED,int32 R_SPEED);


#endif /* CODE_MOTOR_H_ */
