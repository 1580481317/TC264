/*
 * display.c
 *
 *  Created on: 2026年1月12日
 *      Author: sun
 */
#include "zf_common_headfile.h"

int8 i;
int sum;
int zero=0;
int8 suo=0;
//显示赛道边线以及循迹线
void draw_line(uint16 x, uint16 y, uint16 width, uint16 height)
{
    uint8 i;
    uint16 point_x;
    uint16 point_y;
    for(i=MT9V03X_1_H-3;i>5;i--)
    {
        point_y = y + (uint16)((uint32)i * height / MT9V03X_1_H);

        point_x = x + (uint16)((uint32)image_left[i] * width / MT9V03X_1_W);
        ips200_draw_point(point_x, point_y, RGB565_GREEN);

        point_x = x + (uint16)((uint32)image_right[i] * width / MT9V03X_1_W);
        ips200_draw_point(point_x, point_y, RGB565_YELLOW);

        point_x = x + (uint16)((uint32)image_mid[i] * width / MT9V03X_1_W);
        ips200_draw_point(point_x, point_y, RGB565_RED);

//        ips200_draw_point(xishu*Limit(1,MT9V03X_1_W-2,image_left[i]+1) ,xishu*i,RGB565_GREEN);
//        ips200_draw_point(xishu*Limit(1,MT9V03X_1_W-2,image_right[i]+1),xishu*i,RGB565_YELLOW);
    }
}

//零点飘逸误差
uint8 Caiyang(void)
{
    for(i=0;i<10;i++)
    {
        imu660rb_get_gyro();
        sum+=imu660rb_gyro_z;
        system_delay_ms(80);
    }
    return (int)sum/10;
}






