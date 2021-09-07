/**
 * @file Misaka_at24cxx_port.c
 * @brief
 * @author xqyjlj (xqyjlj@126.com)
 * @version 0.0
 * @date 2021-09-07
 * @copyright Copyright © 2020-2021 xqyjlj<xqyjlj@126.com>
 *
 * *********************************************************************************
 * @par ChangeLog:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2021-09-07 <td>0.0     <td>xqyjlj  <td>内容
 * </table>
 * *********************************************************************************
 */
#include "misaka_at24cxx.h"

misaka_at24cxx misaka_at24cxx_obj = {0};

/**
 * @brief i2c发送后再接收
 * @param  address          地址
 * @param  txbuf            发送数据
 * @param  txlen            发送长度
 * @param  rxbuf            接收数据
 * @param  rxlen            接收长度
 * @return 0 @c             成功
 * @return 1 @c             失败
 */
static uint16_t i2c_send_then_recv(uint8_t address, uint8_t* txbuf, uint32_t txlen, uint8_t* rxbuf, uint32_t rxlen)
{
    return 1;
}

/**
 * @brief i2c发送后再发送
 * @param  address          地址
 * @param  txbuf1           发送数据
 * @param  txlen1           发送长度
 * @param  txbuf2           发送数据再发送数据
 * @param  txlen2           发送数据再发送长度
 * @return 0 @c             成功
 * @return 1 @c             失败
 */
static uint16_t i2c_send_then_send(uint8_t address, uint8_t* txbuf1, uint32_t txlen1, uint8_t* txbuf2, uint32_t txlen2)
{
    return 1;
}

/**
 * @brief 延时函数，单位为1ms
 * @param  ms               desc
 */
static void delay_ms(uint16_t ms)
{

}

/**
 * @brief 获取互斥量，如果为裸机系统，空函数即可
 */
static void mutex_take()
{

}

/**
 * @brief 释放互斥量，如果为裸机系统，空函数即可
 */
static void mutex_release()
{

}

/**
 * @brief i2c错误回调函数，不需要设置成空函数即可
 */
void misaka_at24cxx_i2c_error_callback()
{

}

/**
 * @brief
 * @return misaka_at24cxx_t @c
 */
misaka_at24cxx_t misaka_at24cxx_port_init()
{
    misaka_at24cxx_obj.type = MISAKA_AT24C04;
    misaka_at24cxx_obj.isFRAM = 0;
    misaka_at24cxx_obj.mutex_release = mutex_release;
    misaka_at24cxx_obj.mutex_take = mutex_take;
    misaka_at24cxx_obj.delay_ms = delay_ms;
    misaka_at24cxx_obj.i2c_send_then_recv = i2c_send_then_recv;
    misaka_at24cxx_obj.i2c_send_then_send = i2c_send_then_send;
    misaka_at24cxx_obj.address = 0x50;

    misaka_at24cxx_init(&misaka_at24cxx_obj);

    return &misaka_at24cxx_obj;
}
