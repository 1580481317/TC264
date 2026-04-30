/*********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          cpu0_main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.9.4
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设





// **************************** 代码区域 ****************************
int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等

    Buzzer_init();
    ips114_init();
    gyro_init();
    EncoderInit();
    DRV8701E_init();
    LED_init() ;
    Key_init();
    Init_Nag();

    pit_ms_init(CCU60_CH0, 5);
    pit_ms_init(CCU60_CH1, 5);
    pit_ms_init(CCU61_CH0, 10);

    Buzzer_check(50);
    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {
//        // 此处编写需要循环执行的代码
        key_work();
//
        if(N.Nag_SystemRun_Index==2)
        {
            DRV8701E_DOUBLE_MOTOR_CRL(left_speed,right_speed);
        }

//        Encoder_text();
//        DRV8701E_DOUBLE_MOTOR_CRL(l_speed_out,r_speed_out);
//
//         ips114_show_string(0,16*1,"M_All");        ips114_show_float(8*6,16*1,N.Mileage_All,4,4);
//         ips114_show_string(0,16*2,"L_e");          ips114_show_int  (8*6,16*2,Mf.Left_encoder ,4);
//         ips114_show_string(0,16*3,"R_e");          ips114_show_int  (8*6,16*3,Mf.Right_encoder,4);
//         ips114_show_string(0,16*4,"A_Run");        ips114_show_float(8*6,16*4,N.Angle_Run,4,4);
//         ips114_show_string(0,16*5,"yaw");          ips114_show_float(8*6,16*5,yaw_total,4,4);
//         ips114_show_string(0,16*6,"l_out");        ips114_show_float(8*6,16*6,l_speed_out,4,4);
//         ips114_show_string(0,16*7,"r_out");        ips114_show_float(8*6,16*7,r_speed_out,4,4);


//         ips200_show_string(0,20*6,"N.End_f"); ips200_show_float(8*sizeof("N.End_f"),20*6,N.End_f,4,4);
//         ips200_show_string(0,20*7,"N.S_Index"); ips200_show_float(8*sizeof("N.S_Index"),20*7,N.Nag_SystemRun_Index,4,4);
//         ips200_show_string(0,20*7,"l_speed_ki");ips200_show_float(8*sizeof("l_speed_ki"),20*7,l_speed_ki,4,4);
//         ips200_show_string(0,20*7,"N.Nag_Stop_f");ips200_show_float(8*sizeof("N.Nag_Stop_f"),20*7,N.Nag_Stop_f,4,4);
//         ips200_show_string(0,20*8,"N.Angle_Run"); ips200_show_float(8*sizeof("N.Angle_Run"),20*8,N.Angle_Run,4,4);
//         ips200_show_string(0,20*9,"total1");ips200_show_float(8*sizeof("total1"),20*9,total1,4,4);
//         ips200_show_string(0,20*10,"total2");ips200_show_float(8*sizeof("total2"),20*10,total2,4,4);
//         ips200_show_string(0,20*11,"left_speed");ips200_show_float(8*sizeof("left_speed"),20*11,left_speed,4,4);
//         ips200_show_string(0,20*12,"right_speed");ips200_show_float(8*sizeof("right_speed"),20*12,right_speed,4,4);
//         ips200_show_string(0,20*13,"N.Final_Out");ips200_show_float(8*sizeof("N.Final_Out"),20*13,N.Final_Out,4,4);
//         ips200_show_string(0,20*14,"l_speed_out");ips200_show_float(8*sizeof("l_speed_out"),20*14,l_speed_out,4,4);
//         ips200_show_string(0,20*14,"l_speed_out");ips200_show_float(8*sizeof("l_speed_out"),20*14,l_speed_out,4,4);
//         ips200_show_string(0,20*15,"Index");ips200_show_float(8*sizeof("Index"),20*15,N.Nag_SystemRun_Index,4,4);
//         ips200_show_string(0,20*15,"l_speed_error");ips200_show_float(8*sizeof("l_speed_error"),20*15,l_speed_error,4,4);

        // 此处编写需要循环执行的代码
    }
}

#pragma section all restore
// **************************** 代码区域 ****************************
