// STM32F103 SysTick_Config(72000) + IWDG

#include "stm32f10x.h"
#include "delay.h"

uint32_t 	TimingDelay;

// Запуск прерываний на системном счетчике при частоте 72 MHz
void initDelay(void) {
	SysTick_Config(72000);
}
// Обработчик прерывания системного таймера, прерывания каждую 1 ms
void SysTick_Handler() {
	if (TimingDelay != 0) TimingDelay--;
}

// Функция временной задержки в милисекундах
void Delay_ms(uint32_t nTime) {
	IWDG_Feed();								// Clear IWDG <3s
	TimingDelay = nTime;
	while (TimingDelay != 0);
}

// Настройка независимого сторожевого таймера. Настройка на 3s: prer=4, rlr=10100
void initIWDG(uint8_t prer, uint16_t rlr) {
	IWDG->KR = 0X5555;							// Ключ для доступа к таймеру
  	IWDG->PR = prer;  							// Обновление предделителя
  	IWDG->RLR = rlr;  							// Загружаем регистр перезагрузки, счет идет вниз от этого значения
	IWDG->KR = 0XAAAA;							// Перезагрузка
  	IWDG->KR = 0XCCCC;							// Запуск в работу
}

// Clear IWDG <3s
void IWDG_Feed(void) {IWDG->KR=0XAAAA;}
