// STM32F103 UART1

// PA9	- (OUT)	TX UART1 (RX-RS232)
// PA10	- (IN)	RX UART1 (TX-RS232)

#include "stm32f10x.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"

extern uint8_t		uart1_rx_buf[U1RXBUF_SIZE]; 							// Буфер для приёма сообщения
extern uint8_t		uart1_rx_bit; 											// Номер байта принимаемого в буфер
extern uint8_t		uart1_rx_status;
extern uint32_t		number;
uint8_t				uart1_rx_data;
uint8_t 			retstr[30];
uint32_t 			ret;

// Прерывание по приему данных по USART
void USART1_IRQHandler(void) {
	if ((USART1->SR & USART_SR_RXNE) != 0) {								// Если пришло что-то от USART
		uart1_rx_data = USART1->DR;											// Считываем то что пришло в переменную
		uart1_rx_buf[uart1_rx_bit] = uart1_rx_data; 						// Помещаем принятый байт в буфер
		if (uart1_rx_bit != 64) uart1_rx_bit++; else uart1_rx_bit=0;		// Наращиваем счётчик байтов буфера
		if (uart1_rx_data == 0x0D) uart1_rx_status = CHEK_MSG;				// Если пришел конец посылки <CR>=0x0D=13 (<LF>=0x0A=10)
	}
}

void initUART1(void) {
	RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN);

	GPIOA->CRH |= (GPIO_CRH_MODE9 | GPIO_CRH_CNF9_1 | GPIO_CRH_CNF10_0);

    // Заполняем структуру настройками UART 1,2,3 (easystm32.ru/15/)
	// (72000000 + 9600 / 2)/ 9600 * 2 = 7501 = 0x1d4d (stm32f103 ext 8 Mz for USART1)
	// (36000000 + 9600 / 2)/ 9600 * 2 = 3751 = 0xea7 (stm32f103 ext 8 Mz for USART2, USART3)
	// (24000000 + 9600 / 2)/ 9600 * 2 = 2501 = 0x9c5 (stm32f100 ext 8 Mz for USART1, USART2, USART3)
	// потом отсекаем дробную часть, переводим число в шестнадцатеричную систему счисления и получаем 0x9c5
	// 9600 baud USARTDIV = 8000000 / (16*9600) = 52.08 = 0x341 (8 MHz - частота шины APB)
	// 9600 baud USARTDIV = 24000000 / (16*9600) = 156.25 = 0x9c4 (24 MHz - частота шины APB)
	// 9600 baud USARTDIV = 36000000 / (16*9600) = 234.375 = 0x9c4 (36 MHz - частота шины APB)
	// 115200 baud USARTDIV = 24000000 / (16*115200) = 13.0 = 0x0d0 (24 MHz - частота шины APB)
	USART1->BRR = 0x1d4d;
//	USART2->BRR = 0xea7;
//	USART3->BRR = 0xea7;
 	// Разрешить передатчик, приёмник и USART
	USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE | USART_CR1_UE); 			// RS232
	USART1->CR1 |= USART_CR1_RXNEIE;       									// прерывание по приему данных
	NVIC_EnableIRQ (USART1_IRQn);           								// разрешить прерывания от USART1
//	NVIC_SetPriority(USART1_IRQn, 1);										// Установка приоритета 1
}

// string hex to char number
uint8_t hex_to_char(uint8_t data) {
	uint8_t number;
	if (uart1_rx_buf[data] < 58) number = (uart1_rx_buf[data]-48)*16; else number = (uart1_rx_buf[data]-55)*16;
	data++;
	if (uart1_rx_buf[data] < 58) number = number+(uart1_rx_buf[data]-48); else number = number+(uart1_rx_buf[data]-55);
	return number;
}

// char number to string hex (FF) (Only big letters!)
void char_to_hex(uint8_t data) {
	uint8_t digits[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	if (data < 16) {
		retstr[0] = '0';
		retstr[1] = digits[data];
	} else {
		retstr[0] = digits[(data & 0xF0)>>4];
		retstr[1] = digits[(data & 0x0F)];
	}
}

// Number to string
void StrTo() {
	number=0;
	ret=0;
	ret=retstr[0]-0x30;
	number=ret*1000000;
	ret=retstr[1]-0x30;
	number=number+ret*100000;
	ret=retstr[2]-0x30;
	number=number+ret*10000;
	ret=retstr[3]-0x30;
	number=number+ret*1000;
	ret=retstr[4]-0x30;
	number=number+ret*100;
	ret=retstr[5]-0x30;
	number=number+ret*10;
	ret=retstr[6]-0x30;
	number=number+ret;
}

// String to number
void ToStr(uint32_t number) {
	uint32_t i;
	i=number/1000000;
	number=number-i*1000000;
	retstr[0] = i + 0x30;
	i=number/100000;
	number=number-i*100000;
	retstr[1] = i + 0x30;
	i=number/10000;
	number=number-i*10000;
	retstr[2] = i + 0x30;
	i=number/1000;
	number=number-i*1000;
	retstr[3] = i + 0x30;
	i=number/100;
	number=number-i*100;
	retstr[4] = i + 0x30;
	i=number/10;
	number=number-i*10;
	retstr[5] = i + 0x30;
	retstr[6] = number + 0x30;
}

// Number in UART1
void SendNum(uint32_t number, uint8_t st) {
	uint8_t i, j, q;

    ToStr(number);
	for (i=0; i<6; i++) {q = retstr[i];	if (q != '0') break;}
	for (j=i; j<7; j++) send_to_uart1(retstr[j]);
	if (st == 1) send_str(", ", 0);
	if (st == 2) uartEndFlash();
}

// HEX (FF) in UART1
void SendHEX(uint8_t data, uint8_t st) {
	char_to_hex(data);
	send_to_uart1(retstr[0]);
	send_to_uart1(retstr[1]);
	if (st == 1) send_str(", ", 0);
	if (st == 2) uartEndFlash();
}

// Byte in UART1
void send_to_uart1(char data) {
	while (!(USART1->SR & USART_SR_TC));									// Ждем пока бит TC в регистре SR станет 1
	USART1->DR = data;														// Byte in UART1
}

// String in UART1
void send_str(char * string, uint8_t st) {
	uint8_t i=0;
	while (string[i]) {send_to_uart1(string[i]); i++;}
	if (st == 1) uartEndFlash();
}

void uartEndFlash() {
	send_to_uart1('\r');		// 0x0D
//	send_to_uart1('\n');		// 0x0A
	LED0_ON();
	Delay_ms(10);
	LED0_OFF();
	uart1_rx_bit = 0;
	uart1_rx_status = WAIT_HEAD;
}
