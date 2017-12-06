// STM32F103 UART1

// PA9	- (OUT)	TX UART1 (RX-RS232)
// PA10	- (IN)	RX UART1 (TX-RS232)

#define 	WAIT_HEAD 		0				// Ожидание начала сообщения
#define 	WAIT_TAIL 		1 				// Ожидание конца сообщения
#define 	CHEK_MSG 		2 				// Проверка сообщения
#define 	TIME_OUT 		3 				// Окончание времени ожидания сообщения
#define 	U1RXBUF_SIZE	255 			// Размер приёмного буфера UART1

extern void initUART1(void);
extern void read_uart1(void);
extern uint8_t hex_to_char(uint8_t data);
extern void char_to_hex(uint8_t data);
extern void StrTo(void);
extern void ToStr(uint32_t number);
extern void SendNum(uint32_t number, uint8_t st);
extern void SendHEX(uint8_t data, uint8_t st);
extern void send_to_uart1(char data);
extern void send_str(char * string, uint8_t st);
extern void uartEndFlash(void);
