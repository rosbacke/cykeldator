#include <cstdint>

#include "U8g2lib.h"


// const u8g2_cb_t *rotation, uint8_t clock, uint8_t data, uint8_t reset = U8X8_PIN_NONE)

//U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g(U8G2_R0, 4, 5);


void __NOP()
{
	volatile int x = 0;
}

uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
	switch(msg){
		//Initialize SPI peripheral
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
            // Setup GPIO.
		break;

		//Function which implements a delay, arg_int contains the amount of ms
		case U8X8_MSG_DELAY_MILLI:
		//HAL_Delay(arg_int);

		break;
		//Function which delays 10us
		case U8X8_MSG_DELAY_10MICRO:
		for (uint16_t n = 0; n < 320; n++)
		{
			__NOP();
		}

		break;
		//Function which delays 100ns
		case U8X8_MSG_DELAY_100NANO:
		__NOP();
		break;

		case U8X8_PIN_I2C_CLOCK:
			// Set gpio pin.
			break;

		case U8X8_PIN_I2C_DATA:
			// Set gpio pin.
			break;

		//Function to define the logic level of the RESET line
		case U8X8_MSG_GPIO_RESET:
			//if (arg_int) HAL_GPIO_WritePin(RST_LCD_PORT, RST_LCD_PIN, SET);
			//else HAL_GPIO_WritePin(RST_LCD_PORT, RST_LCD_PIN, RESET);

		break;
		default:
			return 0; //A message was received which is not implemented, return 0 to indicate an error
	}

	return 1; // command processed successfully.
}

class Lcd : U8G2
{
public:
	Lcd()
	{
	    u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8g2_gpio_and_delay_stm32, u8g2_gpio_and_delay_stm32);
	    u8g2_InitDisplay(&u8g2);     // transfer init sequence to the display
	    u8g2_SetPowerSave(&u8g2, 0);  // turn on display
	}
};

int main()
{
	Lcd lcd;
    // u8x8_SetPin_SW_I2C(getU8x8(), clock,  data,  reset);
    while(1)
    	;

}
