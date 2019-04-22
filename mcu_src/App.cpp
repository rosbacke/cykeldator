/*
 * App.cpp
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#include "App.h"

#include "isr_project.h"
#include "mcuaccess.h"

OdoTimer* s_timer = nullptr;


static uint8_t u8g2_gpio_and_delay_stm32_2(U8X8_UNUSED u8x8_t* u8x8,
                                    U8X8_UNUSED uint8_t msg,
                                    U8X8_UNUSED uint8_t arg_int,
                                    U8X8_UNUSED void* arg_ptr)
{
    switch (msg)
    {
    // Initialize SPI peripheral
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        // Setup GPIO.
        break;

    // Function which implements a delay, arg_int contains the amount of ms
    case U8X8_MSG_DELAY_MILLI:
        s_timer->delay(1);
        // HAL_Delay(arg_int);

        break;
    // Function which delays 10us
    case U8X8_MSG_DELAY_10MICRO:
        for (uint16_t n = 0; n < 320; n++)
        {
            __NOP();
        }

        break;
    // Function which delays 100ns
    case U8X8_MSG_DELAY_100NANO:
        __NOP();
        break;

    case U8X8_MSG_GPIO(U8X8_PIN_I2C_CLOCK):
        using hwports::gpiob;
        if (arg_int)
            gpiob->ODR |= 1 << 6;
        else
            gpiob->ODR &= ~uint32_t(1 << 6);
        // Set gpio pin.
        break;

    case U8X8_MSG_GPIO(U8X8_PIN_I2C_DATA):
        // Set gpio pin.
        if (arg_int)
            gpiob->ODR |= 1 << 7;
        else
            gpiob->ODR &= ~uint32_t(1 << 7);
        break;

    // Function to define the logic level of the RESET line
    case U8X8_MSG_GPIO_RESET:
        // if (arg_int) HAL_GPIO_WritePin(RST_LCD_PORT, RST_LCD_PIN, SET);
        // else HAL_GPIO_WritePin(RST_LCD_PORT, RST_LCD_PIN, RESET);

        break;
    default:
        return 0; // A message was received which is not implemented, return 0
                  // to indicate an error
    }

    return 1; // command processed successfully.
}

void Lcd::setup()
{
    using hwports::gpiob;
    uint32_t t;

    hwports::rcc->APB2ENR |= RCC_APB2ENR_IOPBEN;

    t = gpiob->CRL;
    // t &= ~(0xffu << 4);
    t |= (6 << (4 * 6)) | (6 << (7 * 4));
    gpiob->CRL = t;

    uint8_t i2caddr = 0x78;

    u8g2_SetI2CAddress(&u8g2, i2caddr);
    u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c,
                                          u8g2_gpio_and_delay_stm32_2);

    u8g2_InitDisplay(&u8g2);     // transfer init sequence to the display
    u8g2_SetPowerSave(&u8g2, 0); // turn on display

    clearBuffer();
    setFont(u8g2_font_ncenB14_tr);
    drawStr(0,20,"Hello World!");
    sendBuffer();
#if 0
    begin();
    do
    {
        setFont(u8g2_font_ncenB24_tr);
        drawStr(28, 45, "14,2");
    } while (nextPage());
#endif
}

void Lcd::write(int x)
{
    char buff[10];
    uint2str(buff, (unsigned char)x);

    clearBuffer();
    setFont(u8g2_font_ncenB24_tr);
    drawStr(28, 45, buff);
    sendBuffer();
}


App::App() :
		m_timer(hwports::tim2.addr()), m_usart1(hwports::usart1.addr()) {
	s_timer = &m_timer;
	setup();
}


void App::write()
{
    static char buffer[200];

    TickPoint tp;
    {
        if (!m_newVal)
        {
            return;
        }
        tp = m_timer.getTP();
        m_newVal = false;
    }
    char* p = buffer;
    p = uint2str(p, tp.m_count);
    *p++ = ' ';
    p = uint2str(p, tp.m_failingEdge);
    *p++ = ' ';
    p = uint2str(p, tp.m_raisingEdge);
    *p++ = ' ';
    p = uint2str(p, m_timer.sysTick());
    *p++ = '\r';
    *p++ = '\n';
    *p++ = '\0';
    m_usart1.blockwrite(buffer);

    lcd.write(tp.m_count);
}

void App::setLed(bool on)
{
    using hwports::gpioc;
    if (on)
        gpioc->ODR &= ~uint32_t(1 << 13);
    else
        gpioc->ODR |= 1 << 13;
}

void App::setup()
{
    using hwports::gpioc;
    hwports::rcc->APB2ENR |= RCC_APB2ENR_IOPCEN;
    gpioc->CRH |= GPIO_CRH_MODE13_1;
    gpioc->CRH |= 0;
    Usart::setupUsart1(m_usart1);
    m_timer.pulseCB.set<App, &App::timerCB>(*this);
    lcd.setup();
}

void App::run()
{
    while (1)
    {
        write();
        setLed((m_timer.sysTick() & 0x200) != 0);
        m_timer.delay<__WFI>(0);
    }
}


