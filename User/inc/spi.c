// STM32F103 SPI1

// PA4  - (OUT)	SPI1_NSS
// PA5  - (OUT)	SPI1_SCK
// PA6  - (IN)	SPI1_MISO (Master In)
// PA7  - (OUT)	SPI1_MOSI (Master Out)

#include "stm32f10x.h"
#include "spi.h"

void initSPI1(void) {
	RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN);

	// вывод управления NSS: выход двухтактный, общего назначения,50MHz
	GPIOA->CRL   |=  GPIO_CRL_MODE4;
	GPIOA->CRL   &= ~GPIO_CRL_CNF4;
	GPIOA->BSRR   =  GPIO_BSRR_BS4;

	// вывод SCK: выход двухтактный, альтернативная функция, 50MHz
	GPIOA->CRL   |=  GPIO_CRL_MODE5;
	GPIOA->CRL   &= ~GPIO_CRL_CNF5;
	GPIOA->CRL   |=  GPIO_CRL_CNF5_1;

	// вывод MISO: вход цифровой с подтягивающим резистором, подтяжка к плюсу
	GPIOA->CRL   &= ~GPIO_CRL_MODE6;
	GPIOA->CRL   &= ~GPIO_CRL_CNF6;
	GPIOA->CRL   |=  GPIO_CRL_CNF6_1;
	GPIOA->BSRR   =  GPIO_BSRR_BS6;

	// вывод MOSI: выход двухтактный, альтернативная функция, 50MHz
	GPIOA->CRL   |=  GPIO_CRL_MODE7;
	GPIOA->CRL   &= ~GPIO_CRL_CNF7;
	GPIOA->CRL   |=  GPIO_CRL_CNF7_1;

	// Настройка SPI1 (STM32F103)
	SPI1->CR2     = 0x0000;
	SPI1->CR1     = SPI_CR1_MSTR;	// Мастер
	SPI1->CR1    |= SPI_CR1_BR;		// Маленькуя скорость SPI Baud rate = Fpclk/256	(2,4,8,16,32,64,128,256)
	SPI1->CR1    |= SPI_CR1_SSM;	// Программный режим NSS
	SPI1->CR1    |= SPI_CR1_SSI;	// Аналогично состоянию, когда на входе NSS высокий уровень
	SPI1->CR1    |= SPI_CR1_SPE;	// Разрешить работу модуля SPI
//	SPI1->CR1 &= ~SPI_CR1_CPOL; 	// Полярность тактового сигнала (CK to 0 when idle)
//	SPI1->CR1 &= ~SPI_CR1_CPHA; 	// Фаза тактового сигнала (|= SPI_CR1_CPHA - по второму фронту)
//	SPI1->CR1 |= SPI_CR1_DFF; 		// 16 бит данных
//	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;	// Младший бит (MSB) передается первым
//	SPI1->CR2 |= SPI_CR2_SSOE; 		// NSS - используется как выход управления slave select
}

uint8_t SPI1SendByte(uint8_t data) {
	while (!(SPI1->SR & SPI_SR_TXE));      				// убедиться, что предыдущая передача завершена (STM32F103)
	SPI1->DR=data;										// вывод в SPI1
	while (!(SPI1->SR & SPI_SR_RXNE));     				// ждем окончания обмена (STM32F103)
	return SPI1->DR;		         					// читаем принятые данные
}

void SPI1_WriteReg(uint8_t address, uint8_t value) {
	SPI1_NSS_ON();										// CS_Low
	SPI1SendByte(address);
	SPI1SendByte(value);
	SPI1_NSS_OFF();										// CS_HIGH
}

uint8_t SPI1_ReadReg(uint8_t address) {
	uint8_t	val;

	SPI1_NSS_ON();										// CS_Low
	SPI1SendByte(address);
	val = SPI1SendByte(0x00);
	SPI1_NSS_OFF();										// CS_HIGH
	return val;
}
