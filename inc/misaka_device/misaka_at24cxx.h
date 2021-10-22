/**
 * @file Misaka_at24cxx.h
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

#ifndef __MISAKA_AT24CXX_H__
#define __MISAKA_AT24CXX_H__

#include "stdint.h"

#define AT24CXX_ASSERT(expr) ((void)0U)

typedef enum
{
    MISAKA_AT24C01 = 0,
    MISAKA_AT24C02,
    MISAKA_AT24C04,
    MISAKA_AT24C08,
    MISAKA_AT24C16,
    MISAKA_AT24C32,
    MISAKA_AT24C64,
    MISAKA_AT24C128,
    MISAKA_AT24C256,
    MISAKA_AT24C512,
    MISAKA_AT24C1024
} misaka_at24cxx_type;

typedef struct
{
    uint16_t (*i2c_send_then_recv)(uint8_t address, uint8_t* txbuf, uint32_t txlen, uint8_t* rxbuf, uint32_t rxlen);//i2c发送后再接收

    uint16_t (*i2c_send_then_send)(uint8_t address, uint8_t* txbuf1, uint32_t txlen1, uint8_t* txbuf2, uint32_t txlen2);//i2c发送后再发送

    misaka_at24cxx_type type;//eeprom种类

    uint8_t address;//地址

    void (*delay_ms)(uint16_t ms);//延时函数，单位为1ms

    void (*mutex_take)();//获取互斥量，如果为裸机系统，空函数即可

    void (*mutex_release)();//释放互斥量，如果为裸机系统，空函数即可

    uint32_t capacity;//eeprom容量，单位byte

    uint16_t pagesize;//页大小

    uint8_t isFRAM;//是否为FRam
} misaka_at24cxx, *misaka_at24cxx_t;

/**
 * @brief eeprom写入函数（包含页写算法）
 * @param  ops              eeprom对象
 * @param  addr             地址
 * @param  txbuf            待发送数据
 * @param  len              待发送数据长度
 * @return 0 @c             写入成功
 * @return 1 @c             写入失败
 */
uint8_t misaka_at24cxx_write(misaka_at24cxx_t ops, uint32_t addr, uint8_t* txbuf, uint32_t len);

/**
 * @brief eeprom读取函数
 * @param  ops              eeprom对象
 * @param  addr             地址
 * @param  rxbuf            待读取数据
 * @param  len              待读取数据长度
 * @return 0 @c             读取成功
 * @return 1 @c             读取失败
 */
uint8_t misaka_at24cxx_read(misaka_at24cxx_t ops, uint32_t addr, uint8_t* rxbuf, uint32_t len);

/**
 * @brief eeprom擦除函数，将指定的数据块擦除成指定的数据
 * @param  ops              eeprom对象
 * @param  addr             地址
 * @param  data             指定的数据
 * @param  len              需要擦除的长度
 * @return 0 @c             擦除成功
 * @return 1 @c             擦除失败
 */
uint8_t misaka_at24cxx_erase(misaka_at24cxx_t ops, uint32_t addr, uint8_t data, uint32_t len);

/**
 * @brief eeprom检查函数
 * @param  ops              eeprom对象
 * @return 0 @c             检查成功
 * @return 1 @c             检查失败
 */
uint8_t misaka_at24cxx_check(misaka_at24cxx_t ops);

/**
 * @brief eeprom初始化函数
 * @param  ops              eeprom对象
 */
void misaka_at24cxx_init(misaka_at24cxx_t ops);

/**
 * @brief i2c错误回调函数，不需要设置成空函数即可
 */
void misaka_at24cxx_i2c_error_callback();

#endif //__MISAKA_AT24CXX_H__
