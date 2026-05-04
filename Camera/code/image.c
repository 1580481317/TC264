/*
 * image.c
 *
 *  Created on: 2026年1月12日
 *      Author: sun
 */
#include "image.h"

uint8 image_sample[MT9V03X_1_H][MT9V03X_1_W];                                   //采样的数组（复制一份的）
uint8 image_display[MT9V03X_1_H][MT9V03X_1_W];                                  //复制数组
uint8 image_temp[MT9V03X_1_H][MT9V03X_1_W];                                     //滤波后的数组
uint8 image_already[MT9V03X_1_H][MT9V03X_1_W];                                  //二值化之后的数组（用于后面循迹算法）
uint8 image_left[MT9V03X_1_H],image_right[MT9V03X_1_H],image_mid[MT9V03X_1_H];    //左右中赛道线
uint8 left_jidian,right_jidian;                                             //左基点，右基点
uint8 mid_line_already=MT9V03X_1_W/2;                                                     //最后输出的中线值
uint8 mid_last_line=MT9V03X_1_W/2;                                                        //上一次的中线值
uint32 time;
uint8 have_left_turn=0;
uint8 have_right_turn=0;
uint8 have_road=0;                                                      //三岔路口标志位
uint8 turn=0;
uint8 flag=0;//该往哪里转，1直走，2右转，3左转,0没有三岔路口
// 软件靶心中点。屏幕宽度的一半默认是正中心 (通常是 188/2=94 或者 160/2=80)
// 如果你发现车子放在正中间，屏幕上的误差显示是 +10，你就可以在这里直接改成：
// int target_center = (MT9V03X_1_W / 2) + 10;
int target_center = MT9V03X_1_W / 2 + 4;
int16 mid_err=0;
uint8 left_duandian=0;
uint8 right_duandian=0;
uint8 flag_three_road=0;                                                //三岔路口状态机标志位
// ================= 新增：转弯冷却护盾 =================
int turn_cooldown = 0;
uint8 threshold_update_cnt=0;
uint8 threshold_far=0;
uint8 threshold_mid=0;
uint8 threshold_near=0;

//test
uint8 test1=0;
uint8 test2=0;
uint8 test3=0;
uint8 count=0,count2=0;
#define end_line            5
#define scarch_left         12
#define scarch_right        12
#define scarch_left_double  8
#define scarch_right_double 8
#define THRESHOLD_MIN       120
#define THRESHOLD_MAX       200
#define DILATE_THRESHOLD    (255 * 5)
#define ERODE_THRESHOLD     (255 * 2)

uint8 Find_road(uint8 image[MT9V03X_1_H][MT9V03X_1_W],uint8 y);
uint8 Find_above_road(uint8 image[MT9V03X_1_H][MT9V03X_1_W],uint8 x);

//转向数组，1直走，2右转，3左转
uint8 Turn_road[]=
{
        3,1,2,2,2,1,3,2,2,2,2,3,3,3,4
//        2,3,3,2,3,3,2,3,1,1,4,
};

uint8 San_cha[]=
{
    0,0,0,1
};

/**
 * @brief Line reconstruction
 * @details 根据给定两点对中线和边界进行线性重构
 * @param x1 起点列坐标
 * @param y1 起点行坐标
 * @param x2 终点列坐标
 * @param y2 终点行坐标
 * @return None
 */
void change_line(uint8 x1, uint8 y1, uint8 x2, uint8 y2)
{
    uint8 i, a1, a2, temp;
    uint8 hx;

    if (x1 > MT9V03X_1_W - 1)
        x1 = MT9V03X_1_W - 1;
    else if (x1 <= 0)
        x1 = 0;
    if (y1 > MT9V03X_1_H - 1)
        y1 = MT9V03X_1_H - 1;
    else if (y1 <= 0)
        y1 = 0;
    if (x2 > MT9V03X_1_W - 1)
        x2 = MT9V03X_1_W - 1;
    else if (x2 <= 0)
        x2 = 0;
    if (y2 > MT9V03X_1_H - 1)
        y2 = MT9V03X_1_H - 1;
    else if (y2 <= 0)
        y2 = 0;

    a1 = y1;
    a2 = y2;

    if (a1 > a2)
    {
        temp = a1;
        a1 = a2;
        a2 = temp;
    }

    for (i = a1; i <= a2; i++)
    {
        hx = (i - y1) * (x2 - x1) / (y2 - y1) + x1;
        if (hx >= MT9V03X_1_W)
            hx = MT9V03X_1_W - 1;
        else if (hx <= 0)
            hx = 0;
        image_mid[i] = hx;
        image_left[i] = hx;
        image_right[i] = hx;
    }
}
//采样函数（复制一份像素数组）
void photo_sample(uint8 image[MT9V03X_1_H][MT9V03X_1_W])
{
    int y,x;
    for(y=0;y<MT9V03X_1_H;y++)
    {
        for(x=0;x<MT9V03X_1_W;x++)
        {
            image_sample[y][x]=image[y][x];
        }
    }
}

//图像滤波（均值）
void photo_fliter(uint8 image[MT9V03X_1_H][MT9V03X_1_W])
{
    int y,x;
    for(y=1;y<MT9V03X_1_H-1;y++)
    {
        for(x=1;x<MT9V03X_1_W-1;x++)
        {
            image_temp[y][x]=(image[y-1][x-1]+2*image[y-1][x]+image[y-1][x+1]+
                              2*image[y][x-1]+4*image[y][x]+2*image[y][x+1]+
                              image[y+1][x-1]+2*image[y+1][x]+image[y+1][x+1])/16;
        }
    }
}

//大津法
uint8 Otsu(uint8 image[MT9V03X_1_H][MT9V03X_1_W])
{
    uint8  threshold=0;
    uint32 total=MT9V03X_1_H*MT9V03X_1_W;
    int pixel_count[256] ={0} ;  // 初始化所有灰度级计数为0
    float pixel_pro[256] ={0};  // 每个灰度级的像素占比（0~1）
    int x,y,i,j;
    double w0 = 0.0;           // 背景像素占整幅图像的比例（≤阈值T的像素）
    double w1 = 0.0;           // 前景像素占整幅图像的比例（>阈值T的像素）
    double u0_temp = 0.0;      // 背景像素的灰度值加权和（灰度×比例）
    double u1_temp = 0.0;      // 前景像素的灰度值加权和（灰度×比例）
    double u0 = 0.0;           // 背景像素的平均灰度
    double u1 = 0.0;           // 前景像素的平均灰度
    double delta_temp = 0.0;   // 当前阈值T对应的类间方差
    double delta_max = 0.0;    // 遍历过程中最大的类间方差（用于找最优阈值）

    //分布直方图
    for(y=0;y<MT9V03X_1_H;y++)
    {
        for(x=0;x<MT9V03X_1_W;x++)
        {
            pixel_count[image[y][x]]++;
        }
    }

    //对分布直方图进行归一
    for(i=0;i<256;i++)
    {
        pixel_pro[i]=(float)(pixel_count[i])/total;
    }
    //遍历算最大阈值
    for (i = 0; i < 256; i++)
    {
        // 每次遍历新的阈值T，重置所有中间变量为0（避免上一次的计算结果干扰）
        w0 = w1 = u0_temp = u1_temp = u0 = u1 = delta_temp = 0;
        for(j=0;j<256;j++)
        {
            if (j <= i)   // 灰度级j ≤ 阈值T
            {
                w0 += pixel_pro[j];          // 累加背景像素的比例
                u0_temp += j * pixel_pro[j]; // 累加背景的灰度加权和（灰度×比例）
            }
            else   // 灰度级j > 阈值T
            {
                w1 += pixel_pro[j];          // 累加前景像素的比例
                u1_temp += j * pixel_pro[j]; // 累加前景的灰度加权和（灰度×比例）
            }
            // 计算背景的平均灰度
            u0 = u0_temp / w0;
            // 计算前景的平均灰度
            u1 = u1_temp / w1;

            // 计算当前阈值T对应的类间方差
            delta_temp = (float)(w0 * w1 * pow((u0 - u1), 2));

            // 核心逻辑：如果当前类间方差大于历史最大值，更新最大值和最优阈值
            if (delta_temp > delta_max)
            {
                delta_max = delta_temp;  // 更新最大类间方差
                threshold = i;           // 记录当前阈值为最优阈值
            }
        }
    }
    return threshold;
}

// 快速大津法阈值计算
uint8 otsuThreshold(uint8 *image)
{
    #define GrayScale 256
    int Pixel_Max=0;
    int Pixel_Min=255;
    uint16 width = MT9V03X_1_W;
    uint16 height = MT9V03X_1_H;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height/4;
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum=0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i+=2)
    {
        for (j = 0; j < width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
            if(data[i * width + j]>Pixel_Max)   Pixel_Max=data[i * width + j];
            if(data[i * width + j]<Pixel_Min)   Pixel_Min=data[i * width + j];
        }
    }

    //计算每个像素值的点在整幅图像中的比例

    for (i = Pixel_Min; i < Pixel_Max; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;

    }

    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;

    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = Pixel_Min; j < Pixel_Max; j++)
    {

        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;

        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        u = u0tmp + u1tmp;            //全局平均灰度
        deltaTmp = (float)(w0 *w1* (u0 - u1)* (u0 - u1)) ;
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }

    }
    return threshold;
}

uint8 block_otsu_threshold(uint8 *image, uint16 col, uint16 row_start, uint16 row_end)
{
    uint32 histogram[256] = {0}; // 灰度直方图
    uint32 pixel_count = 0;      // 像素总数
    uint32 pixel_sum = 0;        // 灰度总和

    // 统计灰度直方图
    for (uint16 row = row_start; row < row_end; row++)
    {
        for (uint16 col_idx = 0; col_idx < col; col_idx++)
        {
            uint8 gray = image[row * col + col_idx];
            histogram[gray]++;
            pixel_count++;
            pixel_sum += gray;
        }
    }

    // 找到有效灰度范围
    uint8 min_gray = 0, max_gray = 255;
    for (min_gray = 0; min_gray < 256 && histogram[min_gray] == 0; min_gray++)
        ;
    for (max_gray = 255; max_gray > min_gray && histogram[max_gray] == 0; max_gray--)
        ;

    // 边界情况处理
    if (max_gray == min_gray)
        return max_gray;
    if (min_gray + 1 == max_gray)
        return min_gray;

    // 计算最佳阈值
    float max_variance = 0.0f;
    uint8 best_threshold = min_gray;

    uint32 background_count = 0;
    uint32 background_sum = 0;

    for (uint8 threshold = min_gray; threshold < max_gray; threshold++)
    {
        // 累计背景像素
        background_count += histogram[threshold];
        background_sum += histogram[threshold] * threshold;

        // 计算前景像素
        uint32 foreground_count = pixel_count - background_count;
        if (foreground_count == 0)
            continue;

        uint32 foreground_sum = pixel_sum - background_sum;

        // 计算背景和前景的平均灰度
        float background_mean = (float)background_sum / background_count;
        float foreground_mean = (float)foreground_sum / foreground_count;

        // 计算类间方差
        float variance = (float)background_count * foreground_count *
                         (background_mean - foreground_mean) *
                         (background_mean - foreground_mean);

        // 记录最大方差对应的阈值
        if (variance > max_variance)
        {
            max_variance = variance;
            best_threshold = threshold;
        }
    }

    return best_threshold;
}

void image_binarization(uint8 (*src_image)[MT9V03X_1_W])
{
    // 每20帧更新一次阈值
    if (++threshold_update_cnt >= 20)
    {
        threshold_update_cnt = 0;

        // 分块计算阈值
        threshold_far = block_otsu_threshold(src_image[0], MT9V03X_1_W, 0, 40);
        threshold_mid = block_otsu_threshold(src_image[0], MT9V03X_1_W, 40, 60);
        threshold_near = block_otsu_threshold(src_image[0], MT9V03X_1_W, 60, MT9V03X_1_H-2);

        // 阈值限幅
        if (threshold_far < THRESHOLD_MIN)
            threshold_far = THRESHOLD_MIN;
        if (threshold_far > THRESHOLD_MAX)
            threshold_far = THRESHOLD_MAX;
        if (threshold_mid < THRESHOLD_MIN)
            threshold_mid = THRESHOLD_MIN;
        if (threshold_mid > THRESHOLD_MAX)
            threshold_mid = THRESHOLD_MAX;
        if (threshold_near < THRESHOLD_MIN)
            threshold_near = THRESHOLD_MIN;
        if (threshold_near > THRESHOLD_MAX)
            threshold_near = THRESHOLD_MAX;
    }

    // 分块二值化
    for (uint16 row = 0; row < MT9V03X_1_H-2; row++)
    {
        uint8 threshold;

        // 根据行数选择阈值
        if (row < 40)
        {
            threshold = threshold_far;
        }
        else if (row < 60)
        {
            threshold = threshold_mid;
        }
        else
        {
            threshold = threshold_near;
        }

        // 二值化处理
        for (uint16 col = 0; col < MT9V03X_1_W; col++)
        {
            if (src_image[row][col] > threshold)
            {
                image_already[row][col] = 255;
            }
            else
            {
                image_already[row][col] = 0;
            }
        }
    }
}

void image_filter(uint8 (*bin_image)[MT9V03X_1_W])
{
    uint32 pixel_sum;

    // 遍历图像
    for (uint8 row = 1; row < MT9V03X_1_H-2; row++)
    {
        for (uint8 col = 1; col < MT9V03X_1_W - 1; col++)
        {
            // 统计周围8个像素的灰度和
            pixel_sum = bin_image[row - 1][col - 1] + bin_image[row - 1][col] + bin_image[row - 1][col + 1] +
                        bin_image[row][col - 1] + bin_image[row][col + 1] +
                        bin_image[row + 1][col - 1] + bin_image[row + 1][col] + bin_image[row + 1][col + 1];

            // 膨胀操作：当前为黑色，周围白色多 → 变白色
            if (pixel_sum >= DILATE_THRESHOLD && bin_image[row][col] == 0)
            {
                bin_image[row][col] = 255;
            }

            // 腐蚀操作：当前为白色，周围黑色多 → 变黑色
            if (pixel_sum <= ERODE_THRESHOLD && bin_image[row][col] == 255)
            {
                bin_image[row][col] = 0;
            }
        }
    }
}

//对图像进行二值化，保存到image_ready数组里
void Get_image_ready(uint8 image[MT9V03X_1_H][MT9V03X_1_W],uint8 threshold)
{
    int x,y;
    for(y=0;y<MT9V03X_1_H;y++)
    {
        for(x=0;x<MT9V03X_1_W;x++)
        {
            if(image[y][x]<threshold)
            {
                image_already[y][x]=0;
            }
            else
            {
                image_already[y][x]=255;
            }
        }
    }
}

//获取大津法的阈值
uint8 Get_yuzhi(uint8 image[MT9V03X_1_H][MT9V03X_1_W])
{
    uint8  threshold=0;
//    photo_sample(image);
//    photo_fliter(image_sample);
//    threshold=Otsu(image_temp);
//    threshold=otsuThreshold(image[0]);
//    Get_image_ready(image,threshold);
    image_binarization(image);  // 1.6ms
//    image_filter(image_already);            // 3.3ms
    return threshold;
}

//寻找基点
void Find_jidian(uint8 image[MT9V03X_1_H][MT9V03X_1_W])
{
    uint8 j;
    //以中心线为开始
    if(image[MT9V03X_1_H-2][MT9V03X_1_W/2]==255 && image[MT9V03X_1_H-2][MT9V03X_1_W/2-1]==255 && image[MT9V03X_1_H-2][MT9V03X_1_W/2+1]==255)
    {
        for(j=MT9V03X_1_W/2;j>0;j--)//寻找左边界点
        {
            if(image[MT9V03X_1_H-2][j-1]==0 && image[MT9V03X_1_H-2][j]==255 && image[MT9V03X_1_H-2][j+1]==255)
            {
                left_jidian=j;
                break;
            }
            if(j-1==1)
            {
                left_jidian=1;
                break;
            }
        }
        for(j=MT9V03X_1_W/2;j<MT9V03X_1_W-2;j++)//寻找右边界点
        {
            if(image[MT9V03X_1_H-2][j-1]==255 && image[MT9V03X_1_H-2][j]==255 && image[MT9V03X_1_H-2][j+1]==0)
            {
                right_jidian=j;
                break;
            }
            if(j+1==MT9V03X_1_W-2)
            {
                right_jidian=MT9V03X_1_W-2;
                break;
            }
        }
    }
    //以四分之一为开始
    else if(image[MT9V03X_1_H-2][MT9V03X_1_W/4]==255 && image[MT9V03X_1_H-2][MT9V03X_1_W/4-1]==255 && image[MT9V03X_1_H-2][MT9V03X_1_W/4+1]==255)
    {
        for(j=MT9V03X_1_W/4;j>0;j--)//寻找左边界点
        {
            if(image[MT9V03X_1_H-2][j-1]==0 && image[MT9V03X_1_H-2][j]==255 && image[MT9V03X_1_H-2][j+1]==255)
            {
                left_jidian=j;
                break;
            }
            if(j-1==1)
            {
                left_jidian=1;
                break;
            }
        }
        for(j=MT9V03X_1_W/4;j<MT9V03X_1_W-2;j++)//寻找右边界点
        {
            if(image[MT9V03X_1_H-2][j-1]==255 && image[MT9V03X_1_H-2][j]==255 && image[MT9V03X_1_H-2][j+1]==0)
            {
                right_jidian=j;
                break;
            }
            if(j+1==MT9V03X_1_W-2)
            {
                right_jidian=MT9V03X_1_W-2;
                break;
            }
        }
    }
    //以四分之三为开始
    else if(image[MT9V03X_1_H-2][MT9V03X_1_W/4*3]==255 && image[MT9V03X_1_H-2][MT9V03X_1_W/4*3-1]==255 && image[MT9V03X_1_H-2][MT9V03X_1_W/4*3+1]==255)
    {
        for(j=MT9V03X_1_W/4*3;j>0;j--)//寻找左边界点
        {
            if(image[MT9V03X_1_H-2][j-1]==0 && image[MT9V03X_1_H-2][j]==255 && image[MT9V03X_1_H-2][j+1]==255)
            {
                left_jidian=j;
                break;
            }
            if(j-1==1)
            {
                left_jidian=1;
                break;
            }
        }
        for(j=MT9V03X_1_W/4*3;j<MT9V03X_1_W-2;j++)//寻找右边界点
        {
            if(image[MT9V03X_1_H-2][j-1]==255 && image[MT9V03X_1_H-2][j]==255 && image[MT9V03X_1_H-2][j+1]==0)
            {
                right_jidian=j;
                break;
            }
            if(j+1==MT9V03X_1_W-2)
            {
                right_jidian=MT9V03X_1_W-2;
                break;
            }
        }
    }
    else    //都失效的话就从两边开始搜
    {
        for(j=0;j<MT9V03X_1_W-3;j++)//寻找左基点
        {
            if(image[MT9V03X_1_H-2][j]==0 && image[MT9V03X_1_H-2][j+1]==255 && image[MT9V03X_1_H-2][j+2]==255)
            {
                left_jidian=j+1;
                break;
            }
        }
        for(j=MT9V03X_1_W-1;j>2;j--)//寻找右基点
        {
            if(image[MT9V03X_1_H-2][j]==0 && image[MT9V03X_1_H-2][j-1]==255 && image[MT9V03X_1_H-2][j-2]==255)
            {
                right_jidian=j-1;
                break;
            }
        }
    }
    if(left_jidian==0 && right_jidian==0)
    {
        left_jidian=1;
        right_jidian=MT9V03X_1_W-2;
    }
}

// 限幅：a=下限，b=上限
uint8 Limit(uint8 a, uint8 b, uint8 c)
{
    if(c < a)
        return a;      // 低于下限，返回下限
    else if(c > b)
        return b;      // 高于上限，返回上限
    else
        return c;      // 在范围内，返回原值
}

uint8 mid_weight[120]=              //中线加权数组
{
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
    6,6,6,6,6,6,6,6,6,6,
    7,8,9,10,11,12,13,14,15,16,
    17,18,19,20,20,20,20,19,18,17,
    16,15,14,13,12,11,10,9,8,7,
    6,6,6,6,6,6,6,6,6,6,
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
};

//寻找赛道边线
void Find_bianxian(uint8 image[MT9V03X_1_H][MT9V03X_1_W])
{
    uint8 i,j;
    uint8 left_point=left_jidian;
    uint8 right_point=right_jidian;

    left_duandian=0;
    right_duandian=0;

    for(i=MT9V03X_1_H-3;i>end_line;i--)
    {
        uint8 left_judge=0;
        uint8 right_judge=0;
        uint8 general_judge_left=0;
        uint8 general_judge_right=0;
        //寻找左边线
        for(j=left_point;j<left_point+scarch_left;j++)//先向右找十行
        {
//            if(left_duandian>0){left_point=1;break;}
            if(image[i][j-1]==0 && image[i][j]==255 && image[i][j+1]==255)
            {
                left_point=j;
                break;
            }
            else if(j==MT9V03X_1_W-2)
            {
                left_point=MT9V03X_1_W-5;
                break;
            }
            else if(j==left_point+scarch_left-1)
            {
                left_judge=1;//右找不到就向左，立标志位
                break;
            }
        }
        if(left_judge==1)
        {
            for(j=left_point;j>left_point-scarch_left_double;j--)//向左找8行
            {
                if(image[i][j-1]==0 && image[i][j]==255 && image[i][j+1]==255)
                {
                    left_point=j;
                    break;
                }
                else if(j==1)
                {
                    //left_point=1;
                    general_judge_left=1;
                    break;
                }
                else if(j==left_point-scarch_left_double+1)
                {
                    general_judge_left=1;
                    break;
                }
            }
        }
        if(general_judge_left==1)//都不行就从左开始全列去找
        {
            for(j=0;j<MT9V03X_1_W-2;j++)
            {
                if(image[i][j]==0 && image[i][j+1]==255 && image[i][j+2]==255)
                {
                    left_point=j+1;
                    break;
                }
                else{left_point=1;}
            }
        }
        //寻找右边线
        for(j=right_point;j>right_point-scarch_right;j--)//先向左找十行
        {
//            if(right_duandian>0){right_point=MT9V03X_1_W-2;break;}
            if(image[i][j-1]==255 && image[i][j]==255 && image[i][j+1]==0)
            {
                right_point=j;
                break;
            }
            else if(j==1)
            {
                right_point=4;
                break;
            }
            else if(j==right_point-scarch_right+1)
            {
                right_judge=1;//左找不到就向右，立标志位
            }
        }
        if(right_judge==1)
        {
            for(j=right_point;j<right_point+scarch_right_double;j++)//向右找8行
            {
                if(image[i][j-1]==255 && image[i][j]==255 && image[i][j+1]==0)
                {
                    right_point=j;
                    break;
                }
                else if(j==MT9V03X_1_W-2)
                {
                    //right_point=186;
                    general_judge_right=1;
                    break;
                }
                else if(j==right_point+scarch_right_double-1)
                {
                    general_judge_right=1;
                    break;
                }
            }
        }
        if(general_judge_right==1)//都不行就从最右边向最左边开始找
        {
            for(j=MT9V03X_1_W-1;j>2;j--)//寻找右基点
            {
                if(image[i][j]==0 && image[i][j-1]==255 && image[i][j-2]==255)
                {
                    right_point=j-1;
                    break;
                }
                else{right_point=MT9V03X_1_W-2;}
            }
        }

        image_left[i]=Limit(1,MT9V03X_1_W-2,left_point);
        image_right[i]=Limit(1,MT9V03X_1_W-2,right_point);
        image_mid[i]=(image_left[i]+image_right[i])/2;
    }
    // ---------------- 护盾生效区 ----------------
        if (turn_cooldown > 0)
        {
            turn_cooldown--; // 护盾倒计时递减
        }
        else // 只有护盾消失时，才允许寻找角点
        {
            if(have_left_turn!=1 && have_right_turn!=1)
            {
                for(i=72; i>30; i--) // 从下往上扫，寻找第一个满足条件的跳变
                {
                    if((abs((int16)image_mid[i] - (int16)image_mid[i-1])) > 3   &&
                      (abs((int16)image_mid[i-1] - (int16)image_mid[i-2])) > 2 &&
                      (abs((int16)image_mid[i-2] - (int16)image_mid[i-3])) > 1)
                    {
                        // ------------------ 场景 1：先发现左边有路 ------------------
                        if(Find_road(image, 2)==1 && Find_road(image, 5)==1)
                        {
                            have_left_turn = 1;

                            if(Find_road(image, MT9V03X_1_W-5)==1 && Find_road(image, MT9V03X_1_W-8)==1)
                            {
                                have_right_turn = 1; // 左右都有，完美确认为 T型
                            }
                            break; // 找到了就立刻退出
                        }

                        // ------------------ 场景 2：先发现右边有路 ------------------
                        if(Find_road(image, MT9V03X_1_W-3)==1 && Find_road(image, MT9V03X_1_W-8)==1)
                        {
                            have_right_turn = 1;
                            if(Find_road(image, 2)==1 && Find_road(image, 5)==1)
                            {
                                have_left_turn = 1; // 左右都有，完美确认为 T型
                            }
                            break; // 找到了就立刻退出
                        }
                    }
                }
            }
        }
}

//加权中线算法
uint8 Find_line(void)
{
    uint8 mid_line=MT9V03X_1_W/2;     //本次中线值
    uint8 mid_out=MT9V03X_1_W/2;      //最终输出值
    uint32 mid_line_sum=0;          //所有中线的加权值
    uint32 weight_total=0;          //加权系数总和
    for(uint8 i=MT9V03X_1_H-3;i>end_line;i--)
    {
        mid_line_sum+=image_mid[i]*mid_weight[i];
        weight_total+=mid_weight[i];
    }
    mid_line=(uint8)(mid_line_sum/weight_total);
    mid_out=(2*mid_last_line+8*mid_line)/10;         //滤波
    mid_last_line=mid_out;
    mid_err=Get_image_err(mid_out);
    return mid_out;
}

//判断三岔路口
// 判断三岔路口、三极管、T字路口
void Three_road(uint8 image[MT9V03X_1_H][MT9V03X_1_W])
{
#define One_line 5
    if (turn_cooldown > 0) return;
    // 【核心优化】：把锁提到最外面！只要上锁了，直接跳过，一滴算力都不浪费
    if (flag_three_road == 0)
    {
        // ================= 提取三种路况的纯粹特征 =================

        // 特征 1：T形三岔 (左右两边同时扫出角点跳变)
        uint8 is_T_shape = (have_left_turn == 1 && have_right_turn == 1);

        // 特征 2：Y形 / 十字路口 (有任意单边角点，且正前方远端有路)
        uint8 is_Y_shape = ((have_left_turn == 1 || have_right_turn == 1) && Find_above_road(image, One_line) == 1);

        // 特征 3：三极管大黑坑 (中间有断层 + 远端有路 + 左右边有侧路)
        uint8 is_Transistor = (Check_Black_Gap(image, 20, 100) == 1 &&
                               Find_above_road(image, 6) == 1 &&
                               (Find_road(image, 6) == 1 || Find_road(image, MT9V03X_1_W-8) == 1));

        // ================= 终极仲裁 =================

        // 只要满足上面任意一种“复杂元素”的特征，立刻宣判为三岔路口，并上锁！
        if (is_T_shape || is_Y_shape || is_Transistor)
        {
            have_road = 1;
            flag_three_road = 1;
        }
        // 如果上面都不是，仅仅只是一个普通的单边直角弯（用来纯切弯的）
        // 那也必须上锁！防止转弯过程中画面倾斜，被误判成上面的复杂元素
        else if (have_left_turn == 1 || have_right_turn == 1)
        {
            flag_three_road = 1;
        }
    }
}
//遇到三岔路口判断是否需要转向
void Turn_or_not_turn(void)
{
    static uint8 temp=0;
    if(have_road==1 && turn==0)
    {
        turn=Turn_road[temp++];
        // 加上这句保护，防止数组越界读出乱码！
        if(temp >= sizeof(Turn_road)) { temp = sizeof(Turn_road) - 1; }
        count=temp;
    }
}

// 寻找指定列的赛道下边界 (增加抗“加号/细线”噪点过滤版)
uint8 find_track_bottom_at_col(uint8 col, uint8 (*bin_image)[MT9V03X_1_W])
{
    uint8 white_count = 0; // 连续白点连击计数器

    // 如果最底部本身就是一片厚实的白色（比如车头已经压在线上），直接返回最底部
    if (bin_image[MT9V03X_1_H - 1][col] == 255 &&
        bin_image[MT9V03X_1_H - 2][col] == 255 &&
        bin_image[MT9V03X_1_H - 3][col] == 255)
    {
        return MT9V03X_1_H - 1;
    }

    // 从下往上扫描（留点顶部冗余防止数组越界）
    for (uint8 row = MT9V03X_1_H - 1; row >= 5; row--)
    {
        if (bin_image[row][col] == 255)
        {
            white_count++; // 发现白点，连击数+1

            // 【核心防伪逻辑】：
            // 真实的赛道线横向切入时，在这一列上会有一定的“纵向厚度”。
            // 如果连续扫描到 5 行都是白点，才确信这是赛道，而不是细小的“加号”！
            // (如果加号稍大，你可以把 5 改成 6 或 7)
            if (white_count >= 3)
            {
                // 因为是从下往上扫的，当找到连续5个白点时，真实的边界(最下方的白点)在 row + 4 的位置
                return row + 2;
            }
        }
        else
        {
            // 遇到黑点，连击立刻中断清零！
            // 那个只有 2~3 个像素宽的“加号”根本攒不够 5 个连击，直接被当成屁放掉了！
            white_count = 0;
        }
    }

    return 0; // 扫了一整列都没找到结实的赛道边界
}
//uint8 find_track_bottom_at_col(uint8 col, uint8 (*bin_image)[MT9V03X_1_W])
//{
//    uint8 track_bottom = 0;
//
//    // 如果最底部已经是白色，直接返回 ROW-1
//    if (bin_image[MT9V03X_1_H - 1][col] == 255 && bin_image[MT9V03X_1_H - 2][col] == 255)
//    {
//        return MT9V03X_1_H - 1;
//    }
//
//    // 从下往上扫描（从 ROW-1 到 2）
//    for (uint8 row = MT9V03X_1_H - 1; row >= 2; row--)
//    {
//        // 检测赛道下边界（进入赛道）：黑→白→白
//        if (bin_image[row][col] == 0 &&
//            bin_image[row - 1][col] == 255 &&
//            bin_image[row - 2][col] == 255)
//        {
//            track_bottom = row;  // 返回下边界位置
//            break;
//        }
//    }
//
//    return track_bottom;
//}
/********plus版******************/
void Turn_time(void)
{
    #define Angle_turn 70
    #define Sanjiguan  25
    #define Distance   5500
    float yaw_current;
    static float yaw_target;
    static int distance=0;
    yaw_current=Yaw;
    if((turn==2 || (have_right_turn==1 && turn==0)) && flag==0)//处理右转
    {
        flag=1;
        if(count==3){yaw_target=yaw_current+Angle_turn-Sanjiguan;}
        else {yaw_target=yaw_current+Angle_turn;}
    }
    if(flag==1)
    {

//        uint8 corner_row = right_duandian;
//
//        if (corner_row != 0)
//        {
//            // 角点存在
//           if(corner_row>50)
//            {change_line(MT9V03X_1_W - 1, corner_row, MT9V03X_1_W - 1, 0);}
////                        change_line(COL / 2, ROW - 1, COL - 1, corner_row);
//        }
//
//        else
//        {
//            uint8 track_bottom = find_track_bottom_at_col(MT9V03X_1_W - 2, image_already);
//
//            if (track_bottom != 0)
//            {
//                change_line(MT9V03X_1_W - 1, track_bottom, MT9V03X_1_W - 1, 0);
//            }
//            else
//            {
//                change_line(MT9V03X_1_W - 1, MT9V03X_1_H - 1, MT9V03X_1_W - 1, 0);
//            }
//        }
// 1. 实时寻找赛道横线切入右边缘的位置（这就等同于当前的物理拐点行数）
//            uint8 corner_row = find_track_bottom_at_col(MT9V03X_1_W - 2, image_already);
        uint8 corner_row = find_track_bottom_at_col(MT9V03X_1_W - 8, image_already);
            // 如果找到了拐点，并且拐点还没有跑到屏幕最顶端或最底端
            if (corner_row > 30 && corner_row < MT9V03X_1_H - 5)
            {
                // 2. 核心视觉魔法：
                // 从屏幕右上角顶点 (187, 0)
                // 连线到 拐点位置原有的中线 (image_mid[corner_row], corner_row)
                // 这行代码只会修改 0 到 corner_row 的中线，完美保留拐点下方的直线轨迹！
                change_line(MT9V03X_1_W - 1, 0, image_mid[corner_row], corner_row);
            }
            else
            {
                // 3. 兜底方案：如果车子已经深埋进弯道，拐点消失，那就全屏偏置防止丢线
                change_line(MT9V03X_1_W - 1, 0, MT9V03X_1_W - 1, MT9V03X_1_H - 1);
            }

        if(Yaw>yaw_target)
        {
            flag = 0;
            yaw_current = Yaw;
            yaw_target = Yaw;

            // 【终极格式化】：绝不留一点残留状态！
            turn = 0;
            have_right_turn = 0;
            have_left_turn = 0;  // 必须加上！
            have_road = 0;       // 必须加上！
            flag_three_road = 0;
            turn_cooldown =15;  // 【新增】：开启 40 帧无敌护盾！
            if(count==3){turn_cooldown = 5;}//转角距离太短
            if(count==6){turn_cooldown = 40;}//防止电阻误判
            if(count==7){turn_cooldown = 5;}//转角距离太短
//            if(count==8){turn_cooldown = 5;}//转角距离太短
        }
    }
    if((turn==3 || (have_left_turn==1 && turn==0)) && flag==0)//处理左转
    {
        flag=2;
        if(count==1){yaw_target=yaw_current-Angle_turn+Sanjiguan;}
        else {yaw_target=yaw_current-Angle_turn;}
    }
    if(flag==2)
    {
//        uint8 corner_row = left_duandian;
//
//     if (corner_row != 0)
//     {
//          // 角点存在
//          // 段1：从角点到顶部（垂直偏置）
//         if(corner_row>50)
//         {change_line(0, corner_row, 0, 0);}
//        //      // 段2：从底部到角点（斜线连接）
//        //      change_line(COL / 2, ROW - 1, 0, corner_row);
//      }
//         else
//           {
//             // 角点消失：使用动态偏置（起点为赛道下边界）
//             uint8 track_bottom = find_track_bottom_at_col(0, image_already);
//             if (track_bottom != 0)
//            {
//              // 从赛道下边界到顶部（垂直偏置）
//              change_line(0, track_bottom, 0, 0);
//            }
//                else
//              {
//               // 未检测到赛道，全局偏置
//               change_line(0, MT9V03X_1_H - 1, 0, 0);
//              }
//           }
//// 1. 实时寻找赛道横线切入左边缘的位置（左拐点行数）
//            uint8 corner_row = find_track_bottom_at_col(0, image_already);
        // 【修改点】：左转也一样，把 0 改成 7，避开最左侧的极度边缘
                    uint8 corner_row = find_track_bottom_at_col(7, image_already);
            // 如果找到了有效拐点
            if (corner_row > 10 && corner_row < MT9V03X_1_H - 10)
            {
                // 2. 从屏幕左上角顶点 (0, 0) 连线到拐点中线
                change_line(0, 0, image_mid[corner_row], corner_row);
            }
            else
            {
                // 3. 兜底方案：全屏向左拉线
                change_line(0, 0, 0, MT9V03X_1_H - 1);
            }
//yaw_current=Yaw;
        if(Yaw<yaw_target)
        {
             flag = 0;
            yaw_current = Yaw;
            yaw_target = Yaw;

            // 【终极格式化】
            turn = 0;
            have_left_turn = 0;
            have_right_turn = 0; // 必须加上！
            have_road = 0;       // 必须加上！
            flag_three_road = 0;
            turn_cooldown = 15;  // 【新增】：开启 40 帧无敌护盾！
            if(count==3){turn_cooldown = 0;}
            if(count==6){turn_cooldown = 40;}
            if(count==7){turn_cooldown = 5;}//转角距离太短
//            if(count==8){turn_cooldown = 5;}//转角距离太短
        }
    }
    if(turn==1 && flag!=3)//三岔路口直走
    {
        flag=3;
        distance=0;
    }
    if(flag==3)
    {
        distance+=(motor_encoder_left.encoder_speed+motor_encoder_right.encoder_speed)/2;
        if(distance>Distance)
        {
            flag = 0;
            yaw_current = Yaw;
            yaw_target = Yaw;

            // 【终极格式化】
            turn = 0;
            have_left_turn = 0;
            have_right_turn = 0; // 必须加上！
            have_road = 0;       // 必须加上！
            flag_three_road = 0;
            turn_cooldown = 10;  // 【新增】：开启 40 帧无敌护盾！
        }
    }

}
// 找左或者右有没有路，参数 y 是需要寻找哪一列，返回 1 表示有路，返回 0 表示没路
uint8 Find_road(uint8 image[MT9V03X_1_H][MT9V03X_1_W], uint8 y)
{
    uint8 white_count = 0;

    // 为了防视野最顶部和最底部的噪点，我们掐头去尾，只扫中间的有效区域
    for(uint8 i = 5; i < MT9V03X_1_H - 8; i++)
    {
        if(image[i][y] == 255)
        {
            white_count++; // 踩到白点，计数+1

            // 你的线宽是 4-5 像素。为了防噪点，只要连续踩到 3 个白点，
            // 就足以证明这绝对是一条横跨过来的路！立刻返回 1！
            if(white_count >= 2)
            {
                return 1;
            }
        }
        else
        {
            // 如果中间断了（踩到黑点），立刻清零，防止把散落的噪点拼凑起来
            white_count = 0;
        }
    }

    // 扫了一整列都没找到连续 3 个白点，说明真没路
    return 0;
}


// 找上面看有没有路, 参数 x 是需要寻找哪一行, 返回 1 表示有路，返回 0 表示没路
uint8 Find_above_road(uint8 image[MT9V03X_1_H][MT9V03X_1_W], uint8 x)
{
    uint8 white_count = 0;

    // 横向扫描这一行（掐头去尾各留 5 个像素，防屏幕边缘噪点毛刺）
    for(uint8 j = 5; j < MT9V03X_1_W - 5; j++)
    {
        if(image[x][j] == 255)
        {
            white_count++; // 踩到白点，连击数 +1

            // 只要连击数达到 3，证明这是一条结实的白线，绝不是散落的噪点！
            if(white_count >= 2)
            {
                return 1; // 立刻确信有路，退出函数，不浪费一滴算力
            }
        }
        else
        {
            // 一旦踩到黑点，连击数清零！完美破解“噪点三明治”陷阱！
            white_count = 0;
        }
    }

    // 扫了一整行，连个连续的 3 像素白块都凑不出来，确实没路
    return 0;
}

//中线误差
// 中线误差计算 (带软件零点补偿优化版)
int16 Get_image_err(uint8 mid_line_already)
{
    static int16 err_last = 0;
    int16 current_err = 0;
    int16 mid_out = 0;

    // 【核心补偿逻辑】：用算出的中线，减去我们定义的“软件靶心中点”
    current_err = (int16)mid_line_already - target_center;

    // 对误差本身进行一阶低通滤波 (比例 2:8，平滑画面毛刺防止舵机抽搐)
    mid_out = (err_last * 2 + current_err * 8) / 10;

    err_last = mid_out;

    return mid_out;
}

// ================= 连续黑色断层（无白点）检测函数 =================
// 作用：扫描指定行，如果连续出现的黑点（没有白点）超过了阈值，就返回1
// 参数1：image (图像数组)
// 参数2：row (需要搜索的行号)
// 参数3：threshold (阈值：超过多少个像素没有白点，就触发返回1)
uint8 Check_Black_Gap(uint8 image[MT9V03X_1_H][MT9V03X_1_W], uint8 row, uint8 threshold)
{
    uint8 black_count = 0; // 连续黑点计数器

    // 遍历这一行的所有列
    // (这里掐头去尾各留了2个像素，防止屏幕最边缘的死角黑边干扰)
    for(uint8 col = 2; col < MT9V03X_1_W - 2; col++)
    {
        if(image[row][col] == 0) // 如果是黑点 (没有白点)
        {
            black_count++; // 黑点计数器 +1

            // 如果连续的黑点数量超过了你设定的阈值
            if(black_count > threshold)
            {
                return 1; // 确认存在大面积断层，立刻返回 1
            }
        }
        else if(image[row][col] == 255) // 如果踩到了白点
        {
            black_count = 0; // 连击中断！计数器清零，重新开始数黑点
        }
    }

    // 如果扫完了整行，最大的连续黑区都没超过阈值，说明白线分布比较密集
    return 0;
}






