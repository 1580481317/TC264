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
// *************************** 例程硬件连接说明 ***************************
// 接入总钻风灰度数字摄像头 对应主板摄像头接口 请注意线序
//      模块管脚            单片机管脚
//      TXD                 查看 zf_device_mt9v03x.h 中 MT9V03X_COF_UART_TX 宏定义
//      RXD                 查看 zf_device_mt9v03x.h 中 MT9V03X_COF_UART_RX 宏定义
//      PCLK                查看 zf_device_mt9v03x.h 中 MT9V03X_PCLK_PIN 宏定义
//      VSY                 查看 zf_device_mt9v03x.h 中 MT9V03X_VSYNC_PIN 宏定义
//      D0-D7               查看 zf_device_mt9v03x.h 中 MT9V03X_DATA_PIN 宏定义 从该定义开始的连续八个引脚
//      GND                 核心板电源地 GND
//      3V3                 核心板 3V3 电源
// 接入2寸IPS模块
// *************************** 例程硬件连接说明 ***************************
//      模块管脚            单片机管脚
//      BL                  查看 zf_device_ips200_parallel8.h 中 IPS200_BL_PIN 宏定义  默认 P15_3
//      CS                  查看 zf_device_ips200_parallel8.h 中 IPS200_CS_PIN 宏定义  默认 P15_5
//      RST                 查看 zf_device_ips200_parallel8.h 中 IPS200_RST_PIN 宏定义 默认 P15_1
//      RS                  查看 zf_device_ips200_parallel8.h 中 IPS200_RS_PIN 宏定义  默认 P15_0
//      WR                  查看 zf_device_ips200_parallel8.h 中 IPS200_WR_PIN 宏定义  默认 P15_2
//      RD                  查看 zf_device_ips200_parallel8.h 中 IPS200_RD_PIN 宏定义  默认 P15_4
//      D0-D7               查看 zf_device_ips200_parallel8.h 中 IPS200_Dx_PIN 宏定义  默认 P11_9/P11_10/P11_11/P11_12/P13_0/P13_1/P13_2/P13_3
//      GND                 核心板电源地 GND
//      3V3                 核心板 3V3 电源



// *************************** 例程测试说明 ***************************
// 1.核心板烧录完成本例程 将核心板插在主板上 插到底
// 2.摄像头接在主板的摄像头接口 注意线序2寸IPS模块插入主板屏幕接口
// 3.主板上电 或者核心板链接完毕后上电 核心板按下复位按键
// 4.屏幕会显示初始化信息然后显示摄像头图像
// 如果发现现象与说明严重不符 请参照本文件最下方 例程常见问题说明 进行排查
// **************************** 代码区域 ****************************

//3.14

#define PREVIEW_X       (0)
#define PREVIEW_Y       (0)
#define PREVIEW_W       (220)
#define PREVIEW_H       (139)
#define PANEL_X         (226)
#define PANEL_TOP_Y     (2)
#define DATA_TOP_Y      (148)

static void show_i32_with_label(uint16 label_x, uint16 y, const char label[], uint16 value_x, int32 value, uint8 digits)
{
    ips200_show_string(label_x, y, label);
    ips200_show_int(value_x, y, value, digits);
}

static void show_status_panel(void)
{
    ips200_set_font(IPS200_6X8_FONT);
    ips200_set_color(RGB565_BLACK, RGB565_WHITE);

    ips200_draw_line(PREVIEW_W + 2, 0, PREVIEW_W + 2, PREVIEW_H, RGB565_GRAY);
    ips200_draw_line(0, PREVIEW_H + 5, 319, PREVIEW_H + 5, RGB565_GRAY);

    ips200_show_string(PANEL_X, PANEL_TOP_Y, "TRACK");
    show_i32_with_label(PANEL_X, 14, "Mid:", 252, mid_line_already, 3);
    show_i32_with_label(PANEL_X, 26, "Err:", 252, mid_err, 4);
    show_i32_with_label(PANEL_X, 38, "Time:", 258, (int32)time, 8);
    show_i32_with_label(PANEL_X, 50, "Yaw:", 252, (int32)Yaw, 5);
    show_i32_with_label(PANEL_X, 62, "Str:", 252, (int32)steer_out, 5);

    ips200_show_string(PANEL_X, 82, "ROAD");
    show_i32_with_label(PANEL_X, 94, "L:", 244, have_left_turn, 1);
    show_i32_with_label(258, 94, "R:", 276, have_right_turn, 1);
    show_i32_with_label(PANEL_X, 106, "Rd:", 250, have_road, 1);
    show_i32_with_label(258, 106, "Tn:", 282, turn, 1);
    show_i32_with_label(PANEL_X, 118, "CD:", 250, turn_cooldown, 3);

    ips200_show_string(0, DATA_TOP_Y, "MOTOR");
    ips200_show_string(PANEL_X, DATA_TOP_Y, "TEST");
    show_i32_with_label(0, 160, "Steer:", 42, (int32)steer_out, 5);
    show_i32_with_label(108, 160, "Yaw:", 138, (int32)Yaw, 5);
    show_i32_with_label(PANEL_X, 160, "SL:", 258, scarch_left_show, 3);
    show_i32_with_label(0, 174, "Lspd:", 36, (int32)left_speed_target, 5);
    show_i32_with_label(108, 174, "Rspd:", 144, (int32)right_speed_target, 5);
    show_i32_with_label(PANEL_X, 174, "SR:", 258, scarch_right_show, 3);
    //show_i32_with_label(268, 174, "M:", 286, threshold_mid, 3);
    show_i32_with_label(0, 188, "Time:", 36, (int32)time, 8);
    show_i32_with_label(108, 188, "Zero:", 144, zero, 5);
    //show_i32_with_label(PANEL_X, 188, "N:", 244, threshold_near, 3);
    show_i32_with_label(0, 202, "L:", 18, have_left_turn, 1);
    show_i32_with_label(36, 202, "R:", 54, have_right_turn, 1);
    show_i32_with_label(72, 202, "Road:", 108, have_road, 1);
    show_i32_with_label(132, 202, "Turn:", 168, turn, 1);

}

int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等
    Debug_Assistant_Init();

//    ips200_set_dir(3);
//    ips200_init(IPS200_TYPE_SPI);
//    mt9v03x_double_init(mt9v03x_1);
    Motor_Init();                   //电机初始化
    Encoder_Init();                 //编码器初始化
    imu660rb_init();                //陀螺仪初始化
    pid_param_init();               //PID初始化
    zero=Caiyang();

    // =============== 发车按键锁 ===============
    // 初始化 P11_3 为上拉输入
    gpio_init(P11_3, GPI, GPIO_HIGH, GPI_PULL_UP);
    // 死循环阻塞在此，直到 P11_3 被按下（拉低为 0）才往后执行中断初始化
    while(gpio_get_level(P11_3)==1);
    // ==========================================

    pit_ms_init(CCU60_CH1,2);       //获取编码器速度
    pit_ms_init(CCU60_CH0, 10);     //给陀螺仪采样
    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码
        ips200_show_gray_image(PREVIEW_X, PREVIEW_Y, (const uint8 *)image_display, MT9V03X_1_W, MT9V03X_1_H, PREVIEW_W, PREVIEW_H, 0);
        draw_line(PREVIEW_X, PREVIEW_Y, PREVIEW_W, PREVIEW_H);
        show_status_panel();
    }
}

#pragma section all restore
// **************************** 代码区域 ****************************
// *************************** 例程常见问题说明 ***************************
// 遇到问题时请按照以下问题检查列表检查
// 问题1：屏幕不显示
//      如果使用主板测试，主板必须要用电池供电 检查屏幕供电引脚电压
//      检查屏幕是不是插错位置了 检查引脚对应关系
//      如果对应引脚都正确 检查一下是否有引脚波形不对 需要有示波器
//      无法完成波形测试则复制一个GPIO例程将屏幕所有IO初始化为GPIO翻转电平 看看是否受控
// 问题2：显示 reinit 字样
//      检查接线是否正常
//      主板供电是否使用电量充足的电池供电
// 问题2：显示图像杂乱 错位
//      检查摄像头信号线是否有松动
