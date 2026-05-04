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
#define xishu 1
//显示赛道边线以及循迹线
void draw_line(void)
{
    uint8 i;
    for(i=MT9V03X_1_H-3;i>5;i--)
    {
        ips200_draw_point(xishu*image_left[i] ,xishu*i,RGB565_GREEN);
        ips200_draw_point(xishu*image_right[i],xishu*i,RGB565_YELLOW);
        ips200_draw_point(xishu*image_mid[i]  ,xishu*i,RGB565_RED);

//        ips200_draw_point(xishu*Limit(1,MT9V03X_1_W-2,image_left[i]+1) ,xishu*i,RGB565_GREEN);
//        ips200_draw_point(xishu*Limit(1,MT9V03X_1_W-2,image_right[i]+1),xishu*i,RGB565_YELLOW);
        if(count==3 && suo==0)
        {
            ips200_full(RGB565_RED);
            suo=0;
        }
        if(count==4 && suo==0)
          {
              ips200_full(RGB565_BLUE);
              suo=0;
          }
        if(count==7)
        {
            ips200_full(RGB565_GREEN);
        }
        if(count==5)
        {
            ips200_full(RGB565_PINK);
        }
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






