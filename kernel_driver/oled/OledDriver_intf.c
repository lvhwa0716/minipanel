
#include <linux/gpio.h>
#include "mt-plat/mt_gpio.h" // just op directly not use gpiolib, kkkk



#include "OledMono.h"

#if defined(USE_LCM_RST)
	#include "ddp_reg.h"
	#define DSI_OUTREG32(cmdq, addr, val) DISP_REG_SET(cmdq, addr, val)
#endif

extern const int GPIO_LCD_CS_IDX;
extern const int GPIO_LCD_RST_IDX;
extern const int GPIO_LCD_DC_IDX;
extern const int GPIO_LCD_SCLK_IDX;
extern const int GPIO_LCD_SDIN_IDX;
extern const unsigned int OLEDDRIVER_PIN_INDEX[];
extern const int LOW_LEVEL;
extern const int HIGH_LEVEL;

#define GPIO_LCD_CS_MODE		GPIO_MODE_00
#define	GPIO_LCD_RST_MODE		GPIO_MODE_00
#define	GPIO_LCD_DC_MODE		GPIO_MODE_00
#define	GPIO_LCD_SCLK_MODE		GPIO_MODE_00
#define	GPIO_LCD_SDIN_MODE		GPIO_MODE_00

void OledDriver_SetPin(int pin, int level)
{
	OLED_PRINT("PIN_INDEX = %d, level = %d\n", OLEDDRIVER_PIN_INDEX[pin], (level == 0 ? 0 : 1));
	if( (pin < 0 ) || (pin > GPIO_LCD_DC_IDX) ) {
		return ; // error
	}
	#if defined(USE_LCM_RST)
		if(GPIO_LCD_RST_IDX == pin) {
			DSI_OUTREG32(NULL, DISPSYS_CONFIG_BASE + 0x150, (level == 0 ? 0 : 1));
			//mt_set_gpio_out(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX], (level == 0 ? 0 : 1));
			//gpio_set_value(OLEDDRIVER_PIN_INDEX[GPIO_LCD_RST_IDX], (level == 0 ? 0 : 1));
			return ;
		}
	#endif
	#ifdef DIRECT_GPIO
		if( level == 0) {
			mt_set_gpio_out(OLEDDRIVER_PIN_INDEX[pin], GPIO_OUT_ZERO);
		} else {
			mt_set_gpio_out(OLEDDRIVER_PIN_INDEX[pin], GPIO_OUT_ONE);
		}
	#else
		if( level == 0) {
			gpio_set_value(OLEDDRIVER_PIN_INDEX[pin], 0);
		} else {
			gpio_set_value(OLEDDRIVER_PIN_INDEX[pin], 1);
		}
	#endif
}

#if defined(USE_GPIO)
	#include "OledDriver_intfGPIO.c"
#elif defined(USE_SPI)
	// when use spi , must remove drivers/spi/mediatek/mt6580/spi_dev.c
	// obj-$(CONFIG_MTK_SPI) += spi.o spi-dev.o => obj-$(CONFIG_MTK_SPI) += spi.o
	#include "OledDriver_intfSPI.c"
#else
	#error "must define USE_GPIO or USE_SPI"	
#endif
