#include "octo_at24cxx.h"

/**
 * @brief   获得eeprom的页大小
 *
 * @param   type: eeprom型号
 *
 * @return  页大小
*/
static uint16_t octo_at24cxx_get_pagesize(octo_at24cxx_type type)
{
    switch (type)
    {
    case OCTO_AT24C01:
    case OCTO_AT24C02:
        return 8;

    case OCTO_AT24C04:
    case OCTO_AT24C08:
    case OCTO_AT24C16:
        return 16;

    case OCTO_AT24C32:
    case OCTO_AT24C64:
        return 32;

    case OCTO_AT24C128:
    case OCTO_AT24C256:
        return 64;

    case OCTO_AT24C512:
        return 128;

    case OCTO_AT24C1024:
        return 256;

    default:
        return 0;
    }
}

/**
 * @brief   获得eeprom的总容量大小
 *
 * @param   type: eeprom型号
 *
 * @return  总容量大小
*/
static uint32_t octo_at24cxx_get_capacity(octo_at24cxx_type type)
{
    switch (type)
    {
    case OCTO_AT24C01:
        return 128;
    case OCTO_AT24C02:
        return 256;
    case OCTO_AT24C04:
        return 512;
    case OCTO_AT24C08:
        return 1024;
    case OCTO_AT24C16:
        return 2048;
    case OCTO_AT24C32:
        return 4096;
    case OCTO_AT24C64:
        return 8192;
    case OCTO_AT24C128:
        return 16384;
    case OCTO_AT24C256:
        return 32768;
    case OCTO_AT24C512:
        return 65536;
    case OCTO_AT24C1024:
        return 131072;
    default:
        return 0;
    }
}

/**
 * @brief   eeprom页写算法
 *
 * @param   ops:        eeprom对象
 *          addr:       地址
 *          txbuf:      待发送数据
 *          len:        待发送数据长度
 *
 * @return  0:          写入成功
 *          1:          写入失败
*/
static uint8_t octo_at24cxx_write_page(octo_at24cxx_t ops, uint32_t addr, uint8_t* txbuf, uint32_t len)
{
    uint8_t buf[2];
    uint8_t buf_size, address, ret;

    if (((addr % ops->pagesize) + len) > ops->pagesize) /*the over flow of page size*/
    {
        return 1;
    }
    if (ops->type > OCTO_AT24C16)
    {
        address = ops->address;
        buf[0] = (addr >> 8) & 0xff;
        buf[1] = addr & 0xff;
        buf_size = 2;
    }
    else
    {
        address = ops->address | (addr >> 8);
        buf[0] = addr & 0xff;
        buf_size = 1;
    }

    ops->mutex_take();
    ret = ops->i2c_send_then_send(address, buf, buf_size, txbuf, len);
    ops->mutex_release();

    return ret;
}

/**
 * @brief   eeprom写入函数（不完整页）
 *
 * @param   ops:        eeprom对象
 *          addr:       地址
 *          txbuf:      待发送数据
 *          len:        待发送数据长度
 *
 * @return  0:          写入成功
 *          1:          写入失败
*/
static uint8_t octo_at24cxx_write_continue(octo_at24cxx_t ops, uint32_t addr, uint8_t* txbuf, uint32_t len)
{
    uint8_t offset, write_size, address, ret;
    uint8_t buf[2];

    ops->mutex_take();

    if (ops->type > OCTO_AT24C16)
    {
        address = ops->address;
        buf[0] = (addr >> 8) & 0xff;
        buf[1] = addr & 0xff;
        ret = ops->i2c_send_then_send(address, buf, 2, txbuf, len);
    }
    else
    {
        while (len > 0)
        {
            offset = 0xff - (addr % 0xff);
            write_size = len > offset ? offset : len;
            address = ops->address | (addr >> 8);
            buf[0] = addr & 0xff;

            ret = ops->i2c_send_then_send(address, buf, 1, txbuf, write_size);
            if (ret != 0)
            {
                break;
            }
            len = len - write_size;
            if (len > 0)
            {
                txbuf = txbuf + write_size;
                addr = addr + write_size;
            }
        }
    }

    ops->mutex_release();

    return ret;
}

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
uint8_t octo_at24cxx_write(octo_at24cxx_t ops, uint32_t addr, uint8_t* txbuf, uint32_t len)
{
    uint8_t write_len, page_offset;
    uint8_t error = 0;

    if (ops == 0)
    {
        return 1;
    }
    if ((addr + len) > ops->capacity)    /*the over flow of chip size*/
    {
        return 1;
    }

    if (ops->isFRAM)
    {
        /* FRAM */
        error = octo_at24cxx_write_continue(ops, addr, txbuf, len);
    }
    else
    {
        /* EEPROM */
        while (len > 0)
        {
            page_offset = ops->pagesize - (addr % ops->pagesize);
            write_len = len > page_offset ? page_offset : len;
            error = octo_at24cxx_write_page(ops, addr, txbuf, write_len);
            if (error)
            {
                break;
            }
            len = len - write_len;
            if (len > 0)
            {
                txbuf = txbuf + write_len;
                addr = addr + write_len;
            }
            ops->delay_ms(6);        /*eeprom need wait*/
        }
    }
    return error;
}

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
uint8_t octo_at24cxx_read(octo_at24cxx_t ops, uint32_t addr, uint8_t* rxbuf, uint32_t len)
{
    uint8_t buf[2];
    uint8_t buf_size, slave_addr, ret;

    if ((addr + len) > ops->capacity)
    {
        return 1;
    }
    if (ops->type > OCTO_AT24C16)
    {
        slave_addr = ops->address;
        buf[0] = (addr >> 8) & 0xff;
        buf[1] = addr & 0xff;
        buf_size = 2;
    }
    else
    {
        slave_addr = ops->address | (addr >> 8);
        buf[0] = addr & 0xff;
        buf_size = 1;
    }

    ret = ops->i2c_send_then_recv(slave_addr, buf, buf_size, rxbuf, len);

    return ret;
}

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
uint8_t octo_at24cxx_erase(octo_at24cxx_t ops, uint32_t addr, uint8_t data, uint32_t len)
{
    uint8_t error = 0;
    uint32_t i;

    if ((addr + len) > ops->capacity)
    {
        return 1;
    }
    for (i = 0; i < len; i++)
    {
        error = octo_at24cxx_write_page(ops, addr, &data, len);
        if (error)
        {
            break;
        }
    }
    return error;
}

/**
 * @brief   eeprom检查函数
 *
 * @param   ops:        eeprom对象
 *
 * @return  0:          检查成功
 *          1:          检查失败
*/
uint8_t octo_at24cxx_check(octo_at24cxx_t ops)
{
    uint8_t txbuf = 0x55;
    uint8_t rxbuf = 0;
    uint8_t ret;

    ret = octo_at24cxx_read(ops, ops->capacity - 1, &rxbuf, 1);
    if (rxbuf == 0x55 && ret == 0)
    {
        return 0;
    }
    else
    {
        octo_at24cxx_write(ops, ops->capacity - 1, &txbuf, 1);
        ret = octo_at24cxx_read(ops, ops->capacity - 1, &rxbuf, 1);
        if (rxbuf == 0x55 && ret == 0)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}

/**
 * @brief   eeprom初始化函数
 *
 * @param   ops:        eeprom对象
 *
 * @return  void
*/
void octo_at24cxx_init(octo_at24cxx_t ops)
{
    AT24CXX_ASSERT(ops);
    AT24CXX_ASSERT(ops->i2c_send_then_recv);
    AT24CXX_ASSERT(ops->i2c_send_then_send);
    AT24CXX_ASSERT(ops->delay_ms);
    AT24CXX_ASSERT(ops->mutex_release);
    AT24CXX_ASSERT(ops->mutex_take);

    ops->capacity = octo_at24cxx_get_capacity(ops->type);
    ops->pagesize = octo_at24cxx_get_pagesize(ops->type);
}


