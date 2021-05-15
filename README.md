# 八爪鱼驱动-AT24Cxx
## 介绍

八爪鱼驱动-AT24Cxx软件包提供了at24cxx 系列 EEPROM 全部功能。 目前已在 at24c04, at24c512验证通过。

### 目录结构

| 名称                  | 说明              |
| --------------------- | ----------------- |
| demo                  | 例程              |
| octo_at24cxx.h        | EEPROM 使用头文件 |
| octo_at24cxx.c        | EEPROM 使用源代码 |
| octo_at24cxx_port.c   | EEPROM 移植文件   |
| README.md             | 驱动使用说明      |
| at24cxx_datasheet.pdf | 官方数据手册      |
| AT24C512C.pdf         | 官方数据手册      |

### 许可证

​	八爪鱼驱动-AT24Cxx软件包遵循  Apache-2.0 许可，详见 LICENSE 文件。

### 依赖

​	依赖 `RT-Thread I2C` 设备驱动框架，或者`octo八爪鱼框架`。

## API

### 初始化 

```c
void octo_at24cxx_init(octo_at24cxx_t ops);
```

*eeprom初始化函数*

| 参数  |     描述     |
| :---: | :----------: |
| *ops* | *eeprom对象* |

### 写

```c
uint8_t octo_at24cxx_write(octo_at24cxx_t ops, uint32_t addr, uint8_t* txbuf, uint32_t len);
```

*eeprom写入函数（包含页写算法）*

|   参数   |       描述       |
| :------: | :--------------: |
|  *ops*   |   *eeprom对象*   |
|  *addr*  |      *地址*      |
| *txbuf*  |   *待发送数据*   |
|  *len*   | *待发送数据长度* |
| **返回** |                  |
|   *0*    |    *写入成功*    |
|   *1*    |    *写入失败*    |

### 读

```c
uint8_t octo_at24cxx_read(octo_at24cxx_t ops, uint32_t addr, uint8_t* rxbuf, uint32_t len);
```

*eeprom读取函数*

|   参数   |       描述       |
| :------: | :--------------: |
|  *ops*   |   *eeprom对象*   |
|  *addr*  |      *地址*      |
| *rxbuf*  |   *待读取数据*   |
|  *len*   | *待读取数据长度* |
| **返回** |                  |
|   *0*    |    *读取成功*    |
|   *1*    |    *读取失败*    |

### 擦除


```c
uint8_t octo_at24cxx_erase(octo_at24cxx_t ops, uint32_t addr, uint8_t data, uint32_t len);
```

*eeprom擦除函数，将指定的数据块擦除成指定的数据，其实就是对octo_at24cxx_write函数的一个再次封装*

|   参数   |       描述       |
| :------: | :--------------: |
|  *ops*   |   *eeprom对象*   |
|  *addr*  |      *地址*      |
|  *data*  |   *指定的数据*   |
|  *len*   | *需要擦除的长度* |
| **返回** |                  |
|   *0*    |    *擦除成功*    |
|   *1*    |    *擦除失败*    |


### 检查


```c
uint8_t octo_at24cxx_check(octo_at24cxx_t ops);
```

*eeprom检查函数*

|   参数   |       描述       |
| :------: | :--------------: |
|  *ops*   |   *eeprom对象*   |
| **返回** |                  |
|   *0*    |    *检查成功*    |
|   *1*    |    *检查失败*    |

## 移植说明

移植非常简单。

只需要在`octo_at24cxx_port.c`文件中将所需要实现的接口实现即可。

### i2c_send_then_recv

```c
static uint16_t i2c_send_then_recv(uint8_t address, uint8_t* txbuf, uint32_t txlen, uint8_t* rxbuf, uint32_t rxlen);
```
*i2c发送后再接收*

|   参数   |       描述       |
| :------: | :--------------: |
|  *address*  |      *地址*      |
|  *txbuf*  |   *待发送的数据*   |
|  *txlen*   | *待发送的数据长度* |
|  *rxbuf*  |   *待接收的数据*   |
|  *rxlen*   | *待接收的数据长度* |
| **返回** |                  |
|   *0*    |    *成功*    |
|   *1*    |    *失败*    |

### i2c_send_then_send

```c
static uint16_t i2c_send_then_send(uint8_t address, uint8_t* txbuf1, uint32_t txlen1, uint8_t* txbuf2, uint32_t txlen2);
```
*i2c发送后再发送*

|   参数   |       描述       |
| :------: | :--------------: |
|  *address*  |      *地址*      |
|  *txbuf1*  |   *待发送的数据1*   |
|  *txlen1*   | *待发送的数据1长度* |
|  *txbuf2*  |   *待接收的数据2*   |
|  *txlen2*   | *待接收的数据2长度* |
| **返回** |                  |
|   *0*    |    *成功*    |
|   *1*    |    *失败*    |

### delay_ms

```c
static void delay_ms(uint16_t ms);
```

*延时函数，单位为1ms*

### mutex_take

```c
static void mutex_take();
```

*获取互斥量，如果为裸机系统，空函数即可*

### mutex_release

```c
static void mutex_release();
```

*释放互斥量，如果为裸机系统，空函数即可*

### octo_at24cxx_port_init

```c
octo_at24cxx_t octo_at24cxx_port_init()
```

*此函数需自己调用*

```c
octo_at24cxx_t octo_at24cxx_port_init()
{
    octo_at24cxx_obj.type = OCTO_AT24C04;
    octo_at24cxx_obj.isFRAM = 0;
    octo_at24cxx_obj.mutex_release = mutex_release;
    octo_at24cxx_obj.mutex_take = mutex_take;
    octo_at24cxx_obj.delay_ms = delay_ms;
    octo_at24cxx_obj.i2c_send_then_recv = i2c_send_then_recv;
    octo_at24cxx_obj.i2c_send_then_send = i2c_send_then_send;
    octo_at24cxx_obj.address = 0x50;

    octo_at24cxx_init(&octo_at24cxx_obj);

    return &octo_at24cxx_obj;
}
```

## 更多信息

[AT24CXX系列读写算法 | xqyjlj](https://xqyjlj.github.io/2021/05/15/AT24CXX系列读写算法/)

[八爪鱼驱动-AT24CXX | xqyjlj](https://xqyjlj.github.io/2021/05/15/八爪鱼驱动-AT24CXX/)

## 参考

https://github.com/Prry/_24Cxx-EEPROM-FRAM-Dev

https://github.com/RT-Thread/rt-thread

