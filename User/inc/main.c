#include "main.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "delay.h"
#include "gpio.h"
#include "rc522.h"
#include "spi.h"
#include  "uart.h"



// UART
uint8_t		uart1_rx_buf[U1RXBUF_SIZE];
uint8_t		uart1_rx_bit;
uint8_t		uart1_rx_status;
uint32_t	number;
uint8_t 	adress;
uint8_t 	value;

// RC522
uint8_t		str[MFRC522_MAX_LEN];												// MFRC522_MAX_LEN = 16
uint8_t		lastID[4];

int main(void) {
    initGPIO();
    initDelay();
    initUART1();
    initSPI1();
    MFRC522_Init();
    initIWDG(4,10100); 															// Init IWDG 3s
    SPI1_NSS_OFF();																// Deactivate RC522
    LED0_ON();
    Delay_ms(100);
    send_str("STM32F103 Mifare RC522 RFID Card reader 13.56 MHz", 1);
    LED0_OFF();

	while (1) {
		uint8_t i, j, q;

		Delay_ms(1);															// Delay and clear IWDG <3s

		if (!MFRC522_Request(PICC_REQIDL, str)) {
			if (!MFRC522_Anticoll(str)) {
				j = 0;
				q = 0;
				for (i=0; i<4; i++) if (lastID[i] != str[i]) j = 1;
				if (j) {
					for (i=0; i<4; i++) lastID[i] = str[i];
					NVIC_DisableIRQ (USART1_IRQn);
					send_str("Card ID: ", 0);
					for (i=0; i<3; i++) SendHEX(str[i], 0);
					SendHEX(str[3], 2);
					NVIC_EnableIRQ (USART1_IRQn);
				}
			}
		}

		if (uart1_rx_status == CHEK_MSG) {
			NVIC_DisableIRQ (USART1_IRQn);
			number = hex_to_char(0);
			send_str("RC522 adress = ", 0);
			SendNum(number, 1);
			adress = number;
			value = MFRC522_ReadRegister(adress);
			number = value;
			send_str("REG value = ", 0);
			SendNum(number, 2);
			uart1_rx_buf[0] = 0;
			uart1_rx_buf[1] = 0;
			NVIC_EnableIRQ (USART1_IRQn);
		}

		q++;
		if (!q) for (i=0; i<4; i++) lastID[i] = 0;								// Delay reading the same card 3s
	}
}



//===========================================================

