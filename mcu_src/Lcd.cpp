/*
 * Lcd.cpp
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */
#include "Lcd.h"

#include <mcuaccess.h>
#include "timer.h"
#include "Strings.h"

OdoTimer* Lcd::m_timer = nullptr;

uint8_t Lcd::u8g2_gpio_and_delay_stm32_2(U8X8_UNUSED u8x8_t* u8x8,
                                    U8X8_UNUSED uint8_t msg,
                                    U8X8_UNUSED uint8_t arg_int,
                                    U8X8_UNUSED void* arg_ptr)
{
    switch (msg)
    {
    // Initialize SPI peripheral
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        // Setup GPIO.mcu_src/App.h
        break;

    // Function which implements a delay, arg_int contains the amount of ms
    case U8X8_MSG_DELAY_MILLI:
    	m_timer->delay(1);
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
}

void Lcd::write(int x)
{
    char buff[10];
    uint2str(buff, (unsigned char)x);

    clearBuffer();
    setFont(u8g2_font_ncenB12_tr);
    drawStr(10, 14, buff);
    drawStr(90, 14, buff);
    setFont(u8g2_font_ncenB24_tr);
    drawStr(20, 60, buff);
    sendBuffer();
}
