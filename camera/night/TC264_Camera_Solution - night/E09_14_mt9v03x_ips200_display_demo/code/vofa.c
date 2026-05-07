/*
 * vofa.c
 *
 *  Created on: 2026年3月7日
 *      Author: sun
 */
#include"vofa.h"

#define DEBUG_BUZZER_PIN            (P33_10)
#define DEBUG_BUZZER_TOGGLE_US      (200000)
#define DEBUG_BUZZER_TOGGLE_COUNT   (2)
#define DEBUG_SERIAL_BUFFER_SIZE    (256)


uint8 uart_get_data[64];                                                 // 串口接收数据缓冲区
uint8 fifo_get_data[64];                                                // fifo 输出读出缓冲区
uint8  get_data = 0;                                                  // 接收数据变量
uint32 fifo_data_count = 0;                                         // fifo 数据个数
fifo_struct uart_data_fifo;
PIDNUM NUM;
static uint8 debug_assistant_ready = 0;
static uint8 debug_special_pending = 0;
static uint8 debug_last_corner_flag = 0;
static uint8 debug_buzzer_active = 0;
static uint8 debug_buzzer_toggle_count = 0;
static uint32 debug_buzzer_last_us = 0;

void Debug_Assistant_Init(void)
{
    wireless_uart_init();

    gpio_init(DEBUG_BUZZER_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    debug_assistant_ready = 1;
    wireless_uart_send_string("TC264 debug serial ready.\r\n");
}

void Debug_Buzzer_BeepOnce(void)
{
    if(!debug_assistant_ready)
    {
        return;
    }

    gpio_set_level(DEBUG_BUZZER_PIN, GPIO_HIGH);
    debug_buzzer_last_us = system_getval_us();
    debug_buzzer_toggle_count = 1;
    debug_buzzer_active = 1;
    debug_special_pending = 1;
}

void Debug_Buzzer_Task(void)
{
    if(!debug_buzzer_active)
    {
        return;
    }

    if((uint32)(system_getval_us() - debug_buzzer_last_us) < DEBUG_BUZZER_TOGGLE_US)
    {
        return;
    }

    debug_buzzer_last_us = system_getval_us();
    if(debug_buzzer_toggle_count < DEBUG_BUZZER_TOGGLE_COUNT)
    {
        gpio_toggle_level(DEBUG_BUZZER_PIN);
        debug_buzzer_toggle_count++;
    }
    else
    {
        gpio_set_level(DEBUG_BUZZER_PIN, GPIO_LOW);
        debug_buzzer_active = 0;
    }
}

void Debug_Assistant_SendFrame(void)
{
    char debug_serial_buffer[DEBUG_SERIAL_BUFFER_SIZE];
    uint8 corner_flag = 0;

    if(!debug_assistant_ready)
    {
        return;
    }

    if(debug_special_pending)
    {
        debug_special_pending = 0;
        debug_last_corner_flag = flag;
        zf_sprintf((int8 *)debug_serial_buffer,
               "\r\n========== SPECIAL ROAD ==========\r\n"
               "count:%d  turn:%d  flag:%d\r\n"
               "err:%d  mid:%d  L:%d  R:%d  road:%d  CD:%d\r\n"
               "==================================\r\n",
               count,
               turn,
               flag,
               mid_err,
               mid_line_already,
               have_left_turn,
               have_right_turn,
               have_road,
               turn_cooldown);
        wireless_uart_send_string(debug_serial_buffer);
        return;
    }

    if(have_road == 0 && turn == 0)
    {
        if(flag == 1)
        {
            corner_flag = 1;
        }
        else if(flag == 2)
        {
            corner_flag = 2;
        }
    }

    if(corner_flag == 0)
    {
        debug_last_corner_flag = 0;
        return;
    }

    if(corner_flag == debug_last_corner_flag)
    {
        return;
    }

    debug_last_corner_flag = corner_flag;
    if(corner_flag == 1)
    {
        zf_sprintf((int8 *)debug_serial_buffer,
                   "\r\n------------ RIGHT CORNER ------------\r\n"
                   "err:%d  mid:%d  L:%d  R:%d  flag:%d  CD:%d\r\n"
                   "--------------------------------------\r\n",
                   mid_err,
                   mid_line_already,
                   have_left_turn,
                   have_right_turn,
                   flag,
                   turn_cooldown);
    }
    else
    {
        zf_sprintf((int8 *)debug_serial_buffer,
                   "\r\n------------- LEFT CORNER ------------\r\n"
                   "err:%d  mid:%d  L:%d  R:%d  flag:%d  CD:%d\r\n"
                   "--------------------------------------\r\n",
                   mid_err,
                   mid_line_already,
                   have_left_turn,
                   have_right_turn,
                   flag,
                   turn_cooldown);
    }
    wireless_uart_send_string(debug_serial_buffer);
}

void Init_uart(void)
{
      fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64); // 初始化 fifo 挂载缓冲区
      uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);   // 初始化串口
      uart_rx_interrupt(UART_INDEX, 1);                   // 开启 UART_INDEX 的接收中断

      uart_write_string(UART_INDEX, "UART Text.");                  // 输出测试信息
      uart_write_byte(UART_INDEX, '\r');                                   // 输出回车
      uart_write_byte(UART_INDEX, '\n');
}

void Send_test(void)
{
    fifo_data_count = fifo_used(&uart_data_fifo);                           // 查看 fifo 是否有数据
            if(fifo_data_count != 0)                                                // 读取到数据了
            {
                fifo_read_buffer(&uart_data_fifo, fifo_get_data, &fifo_data_count, FIFO_READ_AND_CLEAN);    // 将 fifo 中数据读出并清空 fifo 挂载的缓冲
                uart_write_string(UART_INDEX, "\r\nUART get data:");                // 输出测试信息
                uart_write_buffer(UART_INDEX, fifo_get_data, fifo_data_count);      // 将读取到的数据发送出去
            }
            system_delay_ms(10);
}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介       UART_INDEX 的接收中断处理函数 这个函数将在 UART_INDEX 对应的中断调用
// 参数说明       void
// 返回参数       void
// 使用示例       uart_rx_interrupt_handler();
//-------------------------------------------------------------------------------------------------------------------
void uart_rx_interrupt_handler (void)
{
//    get_data = uart_read_byte(UART_INDEX);                                      // 接收数据 while 等待式 不建议在中断使用
    uart_query_byte(UART_INDEX, &get_data);                                     // 接收数据 查询式 有数据会返回 TRUE 没有数据会返回 FALSE
    fifo_write_buffer(&uart_data_fifo, &get_data, 1);                           // 将数据写入 fifo 中
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介       将vofa中回传的数据解析
// 参数说明       void
// 返回参数       void
// 使用示例      data=Get_Data();

//-------------------------------------------------------------------------------------------------------------------
float Get_Data(void)
{
    uint8_t data_Start_Num = 0; // 记录数据位开始的地方
    uint8_t data_End_Num = 0; // 记录数据位结束的地方
    uint8_t data_Num = 0; // 记录数据位数
    uint8_t minus_Flag = 0; // 判断是不是负数
    float data_return = 0; // 解析得到的数据
    uint8_t valid = 0;


    for(uint8_t i=0; i<64; i++)
    {
        if(fifo_get_data[i] == '=')
        {
            data_Start_Num = i + 1;
            valid |= 0x01;  // 标记找到等号
        }
        if(fifo_get_data[i] == '!')
        {
            data_End_Num = i - 1;
            valid |= 0x02;  // 标记找到感叹号
            break;
        }
    }
    if(valid != 0x03) return 0.0f; // 检查格式有效性

    // 优化2：安全处理负数
    uint8_t cursor = data_Start_Num;
    if(cursor <= data_End_Num && fifo_get_data[cursor] == '-')
    {
        minus_Flag = 1;
        cursor++;
    }

    // 优化3：动态计算数值
    uint8_t decimal_pos = 0;
    uint8_t has_decimal = 0;
    data_return = 0.0f;

    for(uint8_t i=cursor; i<=data_End_Num; i++)
    {
        if(fifo_get_data[i] == '.')
        {
            has_decimal = 1;
            decimal_pos = i - cursor;
            continue;
        }

        // 字符转数字（更安全的写法）
        uint8_t digit = fifo_get_data[i] - '0';  // 替换原48为'0'
        if(digit > 9) return 0.0f; // 非法字符

        if(has_decimal)
            data_return += digit * powf(0.1, i - cursor - decimal_pos);
        else
            data_return = data_return * 10.0f + digit;
    }

    return minus_Flag ? -data_return : data_return;
   }

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       将vofa中回传的数据解析分别赋值
// 参数说明       void
// 返回参数       void
// 使用示例      USART_PID_Adjust()
//-------------------------------------------------------------------------------------------------------------------

void USART_PID_Adjust()
{
    float data_Get = Get_Data(); // 存放接收到的数据
    printf("data=%.2f\r\n",data_Get);
        // if(fifo_get_data[0]=='P' && fifo_get_data[1]=='1') // 角度环P
        //     NUM.angle_p = data_Get;
        // else if(fifo_get_data[0]=='I' && fifo_get_data[1]=='1') // 角度环I
        //     NUM.angle_i = data_Get;
        // else if(fifo_get_data[0]=='D' && fifo_get_data[1]=='1') // 角度环D
        //     NUM.angle_d = data_Get;
        if(fifo_get_data[0] == 'P')
         {
            if(fifo_get_data[1] == '1')
            NUM.angle_p = data_Get;      // 角度环P
            else if(fifo_get_data[1] == '2')
            NUM.anglev_p = data_Get; // 角速度环P
            else if(fifo_get_data[1] == '3')
            l_speed_pid.kp = data_Get; // 速度环P
        }
        else if(fifo_get_data[0] == 'I')
        {
            if(fifo_get_data[1] == '1')
             NUM.angle_i = data_Get;      // 角度环I
            else if(fifo_get_data[1] == '2')
             NUM.anglev_i = data_Get; // 角速度环I
            else if(fifo_get_data[1] == '3')
             l_speed_pid.ki = data_Get; // 速度环I
        }
        else if(fifo_get_data[0] == 'D')
         {
            if(fifo_get_data[1] == '1')
            NUM.angle_d = data_Get;      // 角度环D
            else if(fifo_get_data[1] == '2')
             NUM.anglev_d= data_Get; // 角速度环D
            else if(fifo_get_data[1] == '3')
             l_speed_pid.kd = data_Get; // 速度环D
        }
 }




