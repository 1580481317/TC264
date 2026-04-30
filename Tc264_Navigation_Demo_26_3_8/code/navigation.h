/*
 * nagivation.h
 *
 *  Created on: 2024年10月16日
 *      Author: Monst
 */

#ifndef _NAVIGATION_H_
#define _NAVIGATION_H_

//*********************用户设置区域****************************//
#define MaxSize 1020    //flash存储的最大页面

//参数范围 <0 - 11>
#define Nag_End_Page 2      //flash中止页面
#define Nag_Start_Page 11   //flah起始页面

#define Nag_Set_mileage 265 //里程计
#define Nag_Prev 200    //前瞻
#define Nag_Yaw yaw_total //陀螺仪读取出来的偏航角

#define L_Mileage Mf.Left_encoder   //左轮编码器
#define R_Mileage Mf.Right_encoder//右轮编码器
//********************************************************//

typedef struct{
       float Final_Out; //最终输出
       float Mileage_All;   //里程计数
       float Angle_Run; //读取的偏航角
       bool Nag_Stop_f; //惯导中止flag
       uint8 Flash_read_f;//惯导读取flag
       uint16 size; //惯导数组索引通用计数
       uint16 Save_count;
       uint16 Save_index;//保存的flag
       uint8 End_f;//中止flag
       //与flash相关的
       uint8 Flash_page_index;//flash页面索引
       uint8 Flash_Save_Page_Index;//flash保存页码索引
       uint8 Nag_SystemRun_Index;   //惯导执行索引
       //暂时未开发部分
       int Prev_mile[Nag_Prev]; //前瞻
}Nag;

extern Nag N;   //整个变量的结构体，方便开发和移植

void Nag_Run(); //偏航角复现总函数
void Run_Nag_GPS();//偏航角复现

void Run_Nag_Save(void);    //偏航角读取函数
void Nag_Read();    //偏航角读取总函数
//****************************//
void Init_Nag(void);    //这个是参数初始化与flash的缓冲区初始化，请放到函数开始。
void Nag_System(void);  //这个是惯性导航最后的包装函数，请放到中断中。
#endif /* CODE__NAVIGATION_H_ */
