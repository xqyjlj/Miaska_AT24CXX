# 御坂网络驱动-AT24CXX

更多资料见：[御坂网络驱动-AT24CXX | xqyjlj](https://xqyjlj.github.io/2021/05/15/御坂网络驱动-AT24CXX/)

## 介绍

御坂网络驱动-AT24CXX 软件包提供了 at24cxx 系列 EEPROM 全部功能（带页写算法）。 目前已在 at24c02,at24c04, at24c512 验证通过。

### 目录结构

| 名称                  | 说明              |
| --------------------- | ----------------- |
| Examples              | 例程              |
| Misaka_at24cxx.h      | EEPROM 使用头文件 |
| Misaka_at24cxx.c      | EEPROM 使用源代码 |
| Misaka_at24cxx_port.c | EEPROM 移植文件   |
| README.md             | 驱动使用说明      |
| at24cxx_datasheet.pdf | 官方数据手册      |
| AT24C512C.pdf         | 官方数据手册      |

### 许可证

 御坂网络驱动-AT24Cxx 软件包遵循 Apache-2.0 许可，详见 LICENSE 文件。

### 依赖

 依赖 `RT-Thread I2C` 设备驱动框架，或者`misaka御坂框架`。

## 移植说明

移植非常简单。

只需要在`Misaka_at24cxx_port.c`文件中将所需要实现的接口实现即可。

### i2c_send_then_recv

```c
static uint16_t i2c_send_then_recv(uint8_t address, uint8_t* txbuf, uint32_t txlen, uint8_t* rxbuf, uint32_t rxlen);
```

_i2c 发送后再接收_

|   参数    |        描述        |
| :-------: | :----------------: |
| _address_ |       _地址_       |
|  _txbuf_  |   _待发送的数据_   |
|  _txlen_  | _待发送的数据长度_ |
|  _rxbuf_  |   _待接收的数据_   |
|  _rxlen_  | _待接收的数据长度_ |
| **返回**  |                    |
|    _0_    |       _成功_       |
|    _1_    |       _失败_       |

### i2c_send_then_send

```c
static uint16_t i2c_send_then_send(uint8_t address, uint8_t* txbuf1, uint32_t txlen1, uint8_t* txbuf2, uint32_t txlen2);
```

_i2c 发送后再发送_

|   参数    |         描述          |
| :-------: | :-------------------: |
| _address_ |        _地址_         |
| _txbuf1_  |   _待发送的数据 1_    |
| _txlen1_  | _待发送的数据 1 长度_ |
| _txbuf2_  |   _待接收的数据 2_    |
| _txlen2_  | _待接收的数据 2 长度_ |
| **返回**  |                       |
|    _0_    |        _成功_         |
|    _1_    |        _失败_         |

### delay_ms

```c
static void delay_ms(uint16_t ms);
```

_延时函数，单位为 1ms_

### mutex_take

```c
static void mutex_take();
```

_获取互斥量，如果为裸机系统，空函数即可_

### mutex_release

```c
static void mutex_release();
```

_释放互斥量，如果为裸机系统，空函数即可_

### misaka_at24cxx_port_init

```c
Misaka_at24cxx_t misaka_at24cxx_port_init()
```

_此函数需自己调用_

```c
Misaka_at24cxx_t misaka_at24cxx_port_init()
{
    misaka_at24cxx_obj.type = MISAKA_AT24C04;
    misaka_at24cxx_obj.isFRAM = 0;
    misaka_at24cxx_obj.mutex_release = mutex_release;
    misaka_at24cxx_obj.mutex_take = mutex_take;
    misaka_at24cxx_obj.delay_ms = delay_ms;
    misaka_at24cxx_obj.i2c_send_then_recv = i2c_send_then_recv;
    misaka_at24cxx_obj.i2c_send_then_send = i2c_send_then_send;
    misaka_at24cxx_obj.address = 0x50;

    Misaka_at24cxx_init(&misaka_at24cxx_obj);

    return &misaka_at24cxx_obj;
}
```

### Misaka_at24cxx_i2c_error_callback

```c
void Misaka_at24cxx_i2c_error_callback()
```

此函数在i2c发生错误时会调用。

## 更多信息

[AT24CXX 系列读写算法 | xqyjlj](https://xqyjlj.github.io/2021/05/15/AT24CXX系列读写算法/)

[御坂网络驱动-AT24CXX | xqyjlj](https://xqyjlj.github.io/2021/05/15/御坂网络驱动-AT24CXX/)

## 参考

https://github.com/Prry/_24Cxx-EEPROM-FRAM-Dev

https://github.com/RT-Thread/rt-thread
