// STM32F103 + GPIO (LED, BTN)

// PA0  - (OUT)	LED1
// PA1	- (IN)	BTN1
// PC13 - (OUT)	LED0

#include "stm32f10x.h"
#include "gpio.h"

// ������ ���������� �� ��������� �������� ��� ������� 72 MHz
void initGPIO(void) {
	SystemInit(); 															// ������������ ��������� ����������
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN);
//	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;							// ��������� JTAG (�� �������� ���� ������ ���)

	// Init GPIO
	GPIOA->CRL = 0;															// ��� �����
	GPIOA->CRH &= 0x0FF00000;												// ��� �����, PA13 � PA14 ��� ������������ �� SWD
	GPIOA->CRL |= GPIO_CRL_MODE0;											// PA0 - (OUT)	LED1
	GPIOA->BSRR |= GPIO_ODR_ODR1;											// PA1 - (IN)	BTN1 (������ 1, �������� � +)

	GPIOC->CRH |= GPIO_CRH_MODE13;											// PC13 - (OUT)	LED0
}
