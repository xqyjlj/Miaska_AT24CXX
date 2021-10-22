/**
 * @file Misaka_at24cxx_port_rtt.c
 * @brief
 * @author xqyjlj (xqyjlj@126.com)
 * @version 0.0
 * @date 2021-09-07
 * @copyright Copyright © 2020-2021 xqyjlj<xqyjlj@126.com>
 * @SPDX-License-Identifier: Apache-2.0
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
#include "rtthread.h"
#include "rtdevice.h"

misaka_at24cxx misaka_at24cxx_obj = {0};
static struct rt_i2c_bus_device* i2c_bus = RT_NULL;     /* I2C总线设备句柄 */
static struct rt_mutex misaka_at24cxx_mutex;
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
    uint16_t ret;
    struct rt_i2c_msg msg[2];

    msg[0].addr = address;
    msg[0].flags = RT_I2C_WR;
    msg[0].buf = txbuf;
    msg[0].len = txlen;

    msg[1].addr = address;
    msg[1].flags = RT_I2C_WR;
    msg[1].buf = rxbuf;
    msg[1].len = rxlen;

    ret = rt_i2c_transfer(i2c_bus, msg, 2);

    if (ret == 2)
    {
        return 0;
    }
    else
    {
        return 1;
    }
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
    uint16_t ret;
    struct rt_i2c_msg msg[2];

    msg[0].addr = address;
    msg[0].flags = RT_I2C_WR;
    msg[0].buf = txbuf1;
    msg[0].len = txlen1;

    msg[1].addr = address;
    msg[1].flags = RT_I2C_WR | RT_I2C_NO_START;
    msg[1].buf = txbuf2;
    msg[1].len = txlen2;

    ret = rt_i2c_transfer(i2c_bus, msg, 2);
    if (ret == 2)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief 延时函数，单位为1ms
 * @param  ms               desc
 */
static void delay_ms(uint16_t ms)
{
    rt_thread_mdelay(ms);
}

/**
 * @brief 获取互斥量，如果为裸机系统，空函数即可
 */
static void mutex_take()
{
    rt_mutex_take(&misaka_at24cxx_mutex, RT_WAITING_FOREVER);
}

/**
 * @brief 释放互斥量，如果为裸机系统，空函数即可
 */
static void mutex_release()
{
    rt_mutex_release(&misaka_at24cxx_mutex);
}

/**
 * @brief i2c错误回调函数，不需要设置成空函数即可
 */
void misaka_at24cxx_i2c_error_callback()
{
    rt_kprintf("misaka at24cxx i2c error\r\n");
}

/**
 * @brief
 * @return misaka_at24cxx_t @c
 */
misaka_at24cxx_t misaka_at24cxx_port_init()
{
    i2c_bus = (struct rt_i2c_bus_device*)rt_device_find("i2c1");

    rt_mutex_init(&misaka_at24cxx_mutex, "misaka_at24cxx_mutex", RT_IPC_FLAG_FIFO);

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

static int at24cxx_init()
{
    misaka_at24cxx_t obj = misaka_at24cxx_port_init();

    uint8_t ret = misaka_at24cxx_check(obj);
    if (ret == 0)
    {
        rt_kprintf("misaka at24cxx check ok");
    }
    else
    {
        rt_kprintf("misaka at24cxx check error");
    }
    return 0;
}

INIT_APP_EXPORT(at24cxx_init);
