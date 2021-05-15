#include "octo_at24cxx.h"

octo_at24cxx octo_at24cxx_obj = {0};


static uint16_t i2c_send_then_recv(uint8_t address, uint8_t* txbuf, uint32_t txlen, uint8_t* rxbuf, uint32_t rxlen)
{
    return 1;
}

static uint16_t i2c_send_then_send(uint8_t address, uint8_t* txbuf1, uint32_t txlen1, uint8_t* txbuf2, uint32_t txlen2)
{
    return 1;
}

static void delay_ms(uint16_t ms)
{

}

static void mutex_take()
{

}

static void mutex_release()
{

}

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
