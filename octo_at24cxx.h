/*
 * Copyright (c) 2020-2020,  by xqyjlj
 *
 * SPDX-License-Identifier: Apache License 2.0
 *
 * Change Logs:
 * Date           Author       Notes                    Email
 * 2021-05-13     xqyjlj       the first version        xqyjlj@126.com
 */

#ifndef __OCTO_AT24CXX_H__
#define __OCTO_AT24CXX_H__

#include "stdint.h"

#define AT24CXX_ASSERT (void *)

typedef enum
{
    OCTO_AT24C01 = 0,
    OCTO_AT24C02,
    OCTO_AT24C04,
    OCTO_AT24C08,
    OCTO_AT24C16,
    OCTO_AT24C32,
    OCTO_AT24C64,
    OCTO_AT24C128,
    OCTO_AT24C256,
    OCTO_AT24C512,
    OCTO_AT24C1024
} octo_at24cxx_type;

typedef struct
{
    uint16_t (*i2c_send_then_recv)(uint8_t address, uint8_t* txbuf, uint32_t txlen, uint8_t* rxbuf, uint32_t rxlen);//i2c发送后再接收

    uint16_t (*i2c_send_then_send)(uint8_t address, uint8_t* txbuf1, uint32_t txlen1, uint8_t* txbuf2, uint32_t txlen2);//i2c发送后再接收

    octo_at24cxx_type type;//eeprom种类

    uint8_t address;//地址

    void (*delay_ms)(uint16_t ms);//延时函数，单位为1ms

    void (*mutex_take)();//获取互斥量，如果为裸机系统，空函数即可

    void (*mutex_release)();//释放互斥量，如果为裸机系统，空函数即可

    uint32_t capacity;//eeprom容量，单位byte

    uint16_t pagesize;//页大小

    uint8_t isFRAM;//是否为FRam
} octo_at24cxx, *octo_at24cxx_t;

/**
 * @brief   eeprom写入函数（包含页写算法）
 *
 * @param   ops:        eeprom对象
 *          addr:       地址
 *          txbuf:      待发送数据
 *          len:        待发送数据长度
 *
 * @return  0:          写入成功
 *          1:          写入失败
*/
uint8_t octo_at24cxx_write(octo_at24cxx_t ops, uint32_t addr, uint8_t* txbuf, uint32_t len);

/**
 * @brief   eeprom读取函数
 *
 * @param   ops:        eeprom对象
 *          addr:       地址
 *          rxbuf:      待读取数据
 *          len:        待读取数据长度
 *
 * @return  0:          读取成功
 *          1:          读取失败
*/
uint8_t octo_at24cxx_read(octo_at24cxx_t ops, uint32_t addr, uint8_t* rxbuf, uint32_t len);

/**
 * @brief   eeprom擦除函数，将指定的数据块擦除成指定的数据
 *
 * @param   ops:        eeprom对象
 *          addr:       地址
 *          data:       指定的数据
 *          len:        需要擦除的长度
 *
 * @return  0:          擦除成功
 *          1:          擦除失败
*/
uint8_t octo_at24cxx_erase(octo_at24cxx_t ops, uint32_t addr, uint8_t data, uint32_t len);

/**
 * @brief   eeprom检查函数
 *
 * @param   ops:        eeprom对象
 *
 * @return  0:          检查成功
 *          1:          检查失败
*/
uint8_t octo_at24cxx_check(octo_at24cxx_t ops);

/**
 * @brief   eeprom初始化函数
 *
 * @param   ops:        eeprom对象
 *
 * @return  void
*/
void octo_at24cxx_init(octo_at24cxx_t ops);

#endif //__OCTO_AT24CXX_H__