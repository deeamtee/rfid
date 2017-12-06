// STM32F103 SPI1

// PA4  - (OUT)	SPI1_NSS
// PA5  - (OUT)	SPI1_SCK
// PA6  - (IN)	SPI1_MISO (Master In)
// PA7  - (OUT)	SPI1_MOSI (Master Out)

#define 	SPI_I2S_FLAG_BSY	((uint16_t)0x0080)
#define 	SPI1_NSS_ON()		GPIOA->BSRR = GPIO_BSRR_BR4
#define 	SPI1_NSS_OFF()		GPIOA->BSRR = GPIO_BSRR_BS4

extern void initSPI1(void);
extern uint8_t SPI1SendByte(uint8_t data);
extern void SPI1_WriteReg(uint8_t address, uint8_t value);
extern uint8_t SPI1_ReadReg(uint8_t address);
