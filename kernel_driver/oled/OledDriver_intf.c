#if defined(USE_GPIO)
	#include "OledDriver_intfGPIO.c"
#elif defined(USE_SPI)
	// when use spi , must remove drivers/spi/mediatek/mt6580/spi_dev.c
	// obj-$(CONFIG_MTK_SPI) += spi.o spi-dev.o => obj-$(CONFIG_MTK_SPI) += spi.o
	#include "OledDriver_intfSPI.c"
#else
	#error "must define USE_GPIO or USE_SPI"	
#endif
