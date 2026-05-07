/*
 * image.h
 *
 *  Created on: 2026年1月12日
 *      Author: sun
 */

#ifndef CODE_IMAGE_H_
#define CODE_IMAGE_H_

#include "zf_common_headfile.h"


uint8 Otsu(uint8 image[MT9V03X_1_H][MT9V03X_1_W]);
uint8 Get_yuzhi(uint8 image[MT9V03X_1_H][MT9V03X_1_W]);
void Find_jidian(uint8 image[MT9V03X_1_H][MT9V03X_1_W]);
void Find_bianxian(uint8 image[MT9V03X_1_H][MT9V03X_1_W]);
uint8 Limit(uint8 a,uint8 b,uint8 c);
uint8 Find_line(void);
uint8 otsuThreshold(uint8 *image);
void Three_road(uint8 image[MT9V03X_1_H][MT9V03X_1_W]);
void Turn_or_not_turn(void);
int16 Get_image_err(uint8 mid_line_already);    //车身偏右是负数，偏左是正数
void Turn_time(void);
uint8 Find_road(uint8 image[MT9V03X_1_H][MT9V03X_1_W], uint8 y);
uint8 Check_Black_Gap(uint8 image[MT9V03X_1_H][MT9V03X_1_W], uint8 row, uint8 threshold);
uint8 Find_above_road(uint8 image[MT9V03X_1_H][MT9V03X_1_W], uint8 x);
uint8 find_track_bottom_at_col(uint8 col, uint8 (*bin_image)[MT9V03X_1_W]);
void change_line(uint8 x1, uint8 y1, uint8 x2, uint8 y2);
// ----- 新增：软件靶心中点零偏补偿 -----
extern int target_center;
extern int turn_cooldown;
extern uint8 image_sample[MT9V03X_1_H][MT9V03X_1_W];
extern uint8 image_temp[MT9V03X_1_H][MT9V03X_1_W];
extern uint8 image_already[MT9V03X_1_H][MT9V03X_1_W];      //二值化之后的数组
extern uint8 left_jidian,right_jidian;
extern uint8 image_left[MT9V03X_1_H];                    //左赛道边线
extern uint8 image_right[MT9V03X_1_H];                         //右赛道边线
extern uint8 image_mid[MT9V03X_1_H];                           //循迹线
extern uint8 mid_line_already;                                                     //最后输出的中线值
extern uint8 mid_last_line;                                                        //上一次的中线值
extern uint8 image_display[MT9V03X_1_H][MT9V03X_1_W];
extern uint32 time;
extern uint8 have_left_turn;
extern uint8 have_right_turn;
extern uint8 have_road;
extern uint8 Turn_road[];
extern uint8 turn;
extern int16 mid_err;
extern uint8 left_duandian;
extern uint8 right_duandian;
extern uint8 test1;
extern uint8 test2;
extern uint8 test3;
extern uint8 threshold_far;
extern uint8 threshold_mid;
extern uint8 threshold_near;
extern uint8 scarch_left_show;
extern uint8 scarch_right_show;
extern uint8 count,count2;
extern uint8 flag;
#endif /* CODE_IMAGE_H_ */
