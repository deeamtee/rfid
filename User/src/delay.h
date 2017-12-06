// STM32F103 SysTick_Config(72000) + IWDG

extern void initDelay(void);
extern void Delay_ms(uint32_t nTime);
extern void initIWDG(uint8_t prer, uint16_t rlr);
extern void IWDG_Feed(void);
