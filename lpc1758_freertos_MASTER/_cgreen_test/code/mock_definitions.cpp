
// Mock definitions
I2C2::I2C2() : I2C_Base((LPC_I2C_TypeDef*)0)
{
    mock();
}

I2C_Base::I2C_Base(LPC_I2C_TypeDef* pI2CBaseAddr) :
        mpI2CRegs(pI2CBaseAddr),
        mDisableOperation(false)
{
    mock();
}

void LED_Display::setNumber(char a)
{
    mock();
}

void LED_Display::setLeftDigit(char a)
{
    mock();
}

void LED_Display::setRightDigit(char alpha)
{
    mock();
}

void LED::toggle(uint8_t ledNum)
{
    mock();
}

void LED::set(uint8_t ledNum, bool on)
{
    mock();
}

int u0_dbg_printf(const char *format, ...)
{
    return mock();
}

//CAN driver mock
bool CAN_init(can_t can, uint32_t baudrate_kbps, uint16_t rxq_size, uint16_t txq_size,
              can_void_func_t bus_off_cb, can_void_func_t data_ovr_cb)
{
   return mock();
}

bool CAN_tx (can_t can, can_msg_t *pCanMsg, uint32_t timeout_ms)
{
    return mock();
}

void CAN_bypass_filter_accept_all_msgs(void)
{

}

void CAN_reset_bus(can_t can)
{

}

bool CAN_is_bus_off(can_t can)
{
    return mock();
}


bool CAN_rx(can_t can, can_msg_t *msg, uint32_t timeout_ms)
{
    return mock();
}
