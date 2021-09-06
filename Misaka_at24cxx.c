#include "misaka_at24cxx.h"

/**
 * @brief 获得eeprom的页大小
 * @param  type             eeprom型号
 * @return uint16_t @c 页大小
 */
static uint16_t misaka_at24cxx_get_pagesize(misaka_at24cxx_type type)
{
    switch (type)
    {
    case MISAKA_AT24C01:
        return 8;
    case MISAKA_AT24C02:
        return 8;

    case MISAKA_AT24C04:
        return 16;
    case MISAKA_AT24C08:
        return 16;
    case MISAKA_AT24C16:
        return 16;

    case MISAKA_AT24C32:
        return 32;
    case MISAKA_AT24C64:
        return 32;

    case MISAKA_AT24C128:
        return 64;
    case MISAKA_AT24C256:
        return 64;

    case MISAKA_AT24C512:
        return 128;

    case MISAKA_AT24C1024:
        return 256;

    default:
        return 0;
    }
}

/**
 * @brief 获得eeprom的总容量大小
 * @param  type             eeprom型号
 * @return uint32_t @c 总容量大小
 */
static uint32_t misaka_at24cxx_get_capacity(misaka_at24cxx_type type)
{
    switch (type)
    {
    case MISAKA_AT24C01:
        return 128;
    case MISAKA_AT24C02:
        return 256;
    case MISAKA_AT24C04:
        return 512;
    case MISAKA_AT24C08:
        return 1024;
    case MISAKA_AT24C16:
        return 2048;
    case MISAKA_AT24C32:
        return 4096;
    case MISAKA_AT24C64:
        return 8192;
    case MISAKA_AT24C128:
        return 16384;
    case MISAKA_AT24C256:
        return 32768;
    case MISAKA_AT24C512:
        return 65536;
    case MISAKA_AT24C1024:
        return 131072;
    default:
        return 0;
    }
}

/**
 * @brief eeprom页写算法
 * @param  ops              eeprom对象
 * @param  addr             地址
 * @param  txbuf            待发送数据
 * @param  len              待发送数据长度
 * @return 0 @c             写入成功
 * @return 1 @c             写入失败
 */
static uint8_t misaka_at24cxx_write_page(misaka_at24cxx_t ops, uint32_t addr, uint8_t* txbuf, uint32_t len)
{
    uint8_t buf[2];
    uint8_t buf_size, address, ret;

    if (((addr % ops->pagesize) + len) > ops->pagesize)
    {
        return 1;
    }
    if (ops->type > MISAKA_AT24C16)
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
    if (ret)
    {
        misaka_at24cxx_i2c_error_callback();
    }
    ops->mutex_release();

    return ret;
}

/**
 * @brief eeprom写入函数（不完整页）
 * @param  ops              eeprom对象
 * @param  addr             地址
 * @param  txbuf            待发送数据
 * @param  len              待发送数据长度
 * @return 0 @c             写入成功
 * @return 1 @c             写入失败
 */
static uint8_t misaka_at24cxx_write_continue(misaka_at24cxx_t ops, uint32_t addr, uint8_t* txbuf, uint32_t len)
{
    uint8_t offset, write_size, address, ret;
    uint8_t buf[2];

    ops->mutex_take();

    if (ops->type > MISAKA_AT24C16)
    {
        address = ops->address;
        buf[0] = (addr >> 8) & 0xff;
        buf[1] = addr & 0xff;
        ret = ops->i2c_send_then_send(address, buf, 2, txbuf, len);
        if (ret)
        {
            misaka_at24cxx_i2c_error_callback();
        }
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
            if (ret)
            {
                misaka_at24cxx_i2c_error_callback();
            }
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
 * @brief eeprom写入函数（包含页写算法）
 * @param  ops              eeprom对象
 * @param  addr             地址
 * @param  txbuf            待发送数据
 * @param  len              待发送数据长度
 * @return 0 @c             写入成功
 * @return 1 @c             写入失败
 */
uint8_t misaka_at24cxx_write(misaka_at24cxx_t ops, uint32_t addr, uint8_t* txbuf, uint32_t len)
{
    uint8_t write_len, page_offset;
    uint8_t error = 0;

    if (ops == 0)
    {
        return 1;
    }
    if ((addr + len) > ops->capacity)
    {
        return 1;
    }

    if (ops->isFRAM)
    {
        /* FRAM */
        error = misaka_at24cxx_write_continue(ops, addr, txbuf, len);
    }
    else
    {
        /* EEPROM */
        while (len > 0)
        {
            page_offset = ops->pagesize - (addr % ops->pagesize);
            write_len = len > page_offset ? page_offset : len;
            error = misaka_at24cxx_write_page(ops, addr, txbuf, write_len);
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
            ops->delay_ms(6);
        }
    }
    return error;
}

/**
 * @brief eeprom读取函数
 * @param  ops              eeprom对象
 * @param  addr             地址
 * @param  rxbuf            待读取数据
 * @param  len              待读取数据长度
 * @return 0 @c             读取成功
 * @return 1 @c             读取失败
 */
uint8_t misaka_at24cxx_read(misaka_at24cxx_t ops, uint32_t addr, uint8_t* rxbuf, uint32_t len)
{
    uint8_t buf[2];
    uint8_t buf_size, slave_addr, ret;

    if ((addr + len) > ops->capacity)
    {
        return 1;
    }
    if (ops->type > MISAKA_AT24C16)
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
    if (ret)
    {
        misaka_at24cxx_i2c_error_callback();
    }

    return ret;
}

/**
 * @brief eeprom擦除函数，将指定的数据块擦除成指定的数据
 * @param  ops              eeprom对象
 * @param  addr             地址
 * @param  data             指定的数据
 * @param  len              需要擦除的长度
 * @return 0 @c             擦除成功
 * @return 1 @c             擦除失败
 */
uint8_t misaka_at24cxx_erase(misaka_at24cxx_t ops, uint32_t addr, uint8_t data, uint32_t len)
{
    uint8_t error = 0;
    uint32_t i;

    if ((addr + len) > ops->capacity)
    {
        return 1;
    }
    for (i = 0; i < len; i++)
    {
        error = misaka_at24cxx_write_page(ops, addr, &data, len);
        if (error)
        {
            break;
        }
    }
    return error;
}

/**
 * @brief eeprom检查函数
 * @param  ops              eeprom对象
 * @return 0 @c             检查成功
 * @return 1 @c             检查失败
 */
uint8_t misaka_at24cxx_check(misaka_at24cxx_t ops)
{
    uint8_t txbuf = 0x55;
    uint8_t rxbuf = 0;
    uint8_t ret;

    ret = misaka_at24cxx_read(ops, ops->capacity - 1, &rxbuf, 1);
    if (rxbuf == 0x55 && ret == 0)
    {
        return 0;
    }
    else
    {
        misaka_at24cxx_write(ops, ops->capacity - 1, &txbuf, 1);
        ret = misaka_at24cxx_read(ops, ops->capacity - 1, &rxbuf, 1);
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
 * @brief eeprom初始化函数
 * @param  ops              eeprom对象
 */
void misaka_at24cxx_init(misaka_at24cxx_t ops)
{
    AT24CXX_ASSERT(ops);
    AT24CXX_ASSERT(ops->i2c_send_then_recv);
    AT24CXX_ASSERT(ops->i2c_send_then_send);
    AT24CXX_ASSERT(ops->delay_ms);
    AT24CXX_ASSERT(ops->mutex_release);
    AT24CXX_ASSERT(ops->mutex_take);

    ops->capacity = misaka_at24cxx_get_capacity(ops->type);
    ops->pagesize = misaka_at24cxx_get_pagesize(ops->type);
}


