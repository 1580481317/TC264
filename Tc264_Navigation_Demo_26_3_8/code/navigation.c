/*
 * nagivation.c
 *
 *  Created on: 2024年10月16日
 *      Author: Monst
 *
 *
 */


#include "zf_common_headfile.h"
#include "navigation.h"

Nag N;
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     读取偏航角的线程函数
// 参数说明     读取偏航角的线程函数，通过切换N.End_f来切换线程
// 返回参数     void
// 使用示例     用户无需调用
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Nag_Read()
{
        switch(N.End_f)
        {
            case 0:Run_Nag_Save();  //默认执行函数
                break;
            case 1:;
                    flash_Nag_Write();  //写入最后一页，保证falsh存储满
                    N.End_f++;
                    break;
            case 2:Buzzer_check(500);   //蜂鸣器确认执行
                    N.End_f++;  //结束线程
                    break;
        }
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     用于生成偏差计算
// 参数说明     N.Final_Out为最终生成的偏差大小
// 返回参数     void
// 使用示例     用户无需调用
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Nag_Run()
{
  if(N.Nag_Stop_f){
    base_speed = 0;
    left_speed = 0;
    right_speed = 0;
    N.Final_Out=0;

    return;
  }
    Run_Nag_GPS();  //偏航角读取复现
    
    N.Final_Out=N.Angle_Run-Nag_Yaw;//负数向右转，正数向左转
    
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     偏航角存入
// 参数说明     将读取的YAW存储到flash中存储
// 返回参数     void
// 使用示例     用户无需调用
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Run_Nag_Save(void)
{
    N.Mileage_All+=(R_Mileage+L_Mileage)/2.0;//历程计读取，左右编码器，使用浮点数的话误差能保留下来
    if(N.size > MaxSize)//当大于这页有的flash大小的时候，写入一次，防止重复写入
    {
        flash_Nag_Write();
        N.size=0;   //索引重置为0从下一个缓冲区开始读取
        N.Flash_page_index--;   //flash页面索引减小
        zf_assert(N.Flash_page_index > Nag_End_Page);//防止越界报错
    }

    if(N.Mileage_All >= Nag_Set_mileage)    //大于你的设定值的时候
    {
       int32 Save=(int32)(Nag_Yaw*100); //读取的偏航角放大100倍，避免使用Float类型来存储
       flash_union_buffer[N.size++].int32_type = Save;  //将偏航角写入缓冲区

       N.Save_index++;


       if(N.Mileage_All > 0) N.Mileage_All -= Nag_Set_mileage;//重置历程计数字//保存到flash
       else N.Mileage_All += Nag_Set_mileage;//倒车
    }

}
// 偏航角复现
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     偏航角复现
// 参数说明     读取flash中存储的YAW
// 返回参数     void
// 使用示例     用户无需调用
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Run_Nag_GPS()
{
    
    uint8 offset=0;//计数偏移
    N.Mileage_All+=(R_Mileage+L_Mileage)/2.0;//历程计读取，左右编码器，使用浮点数的话误差能保留下来
    if(N.Flash_read_f == 0)//同样是每次翻页只读取一次，防止反复读取
    {
        N.size=offset;
        flash_Nag_Read();   //flash读取
        N.Flash_read_f=1;

    }
    
    if(N.size >= MaxSize)    //当大于设定的flsh大小的时候
    {
        N.Flash_read_f=0;   //读取flash重置
        N.Flash_page_index--;   //页面减少
        N.size = 0;
        zf_assert(N.Flash_page_index > Nag_End_Page);
    }


    if(N.Mileage_All >= Nag_Set_mileage)
    {
        //偏差计算
      N.Save_count++;
    if(N.Save_count > N.Save_index-2)
    {
        N.Nag_Stop_f++;
        
        return;
    }
       
       N.Angle_Run = (flash_union_buffer[N.size++].int32_type / 100.0f);
       
       if(N.Mileage_All > 0) N.Mileage_All -= Nag_Set_mileage;//重置历程计数字//保存到flash
       else N.Mileage_All += Nag_Set_mileage;   //倒车
    }


}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     惯导参数初始化
// 返回参数     void
// 使用示例     放入程序执行开始
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Init_Nag(void)
{
    memset(&N, 0, sizeof(N));
    N.Flash_page_index=Nag_Start_Page;
    flash_buffer_clear();
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     惯性导航执行函数
// 参数说明     index           索引
// 参数说明     type            类型值
// 返回参数     void
// 使用示例     放入中断中
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Nag_System(void){
    //卫保护
    if(!N.Nag_SystemRun_Index || N.Nag_Stop_f )  return;

    switch(N.Nag_SystemRun_Index)
    {
        case 1 : Nag_Read();    //1是读取
            break;
        case 2: Nag_Run(); //2是复现
            break;

    }
}

