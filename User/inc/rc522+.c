// http://electronics.byethost7.com
// when you try it again, please change it into your new password
uint8_t sectorKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},};
							 
uint8_t sectorNewKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0x19,0x84,0x07,0x15,0x76,0x14},
                                {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0x19,0x33,0x07,0x15,0x34,0x14},
                                {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0x19,0x33,0x07,0x15,0x34,0x14},};
 
void RF_Process(void) {
	uint8_t i,tmp;
	uint8_t status;
	uint8_t str[MAX_LEN];
	uint8_t RC_size;
	uint8_t blockAddr;																			// Select the operating block address, 0-63
 
	// Look for the card, return type
	status = MFRC522_Request(PICC_REQIDL, str);													// Запрос на спячку
	if (status == MI_OK) printf("Find out a card 0x%x, 0x%x \r\n", str[0], str[1]);
 
	// Anti-collision, return the card's 4-byte serial number
	status = MFRC522_Anticoll(str);
//	memcpy(serNum, str, 5);																											// !!!
	if (status == MI_OK) printf("The card's number is: %x%x%x%x\r\n", serNum[0],serNum[1],serNum[2],serNum[3]);
 
	// Election card, return capacity
	RC_size = MFRC522_SelectTag(serNum);
	if (RC_size != 0) printf("The size of the card is: %d Kbits\r\n", RC_size);
 
	// Registration card
	blockAddr = 11;																				// Data block 11
	status = MFRC522_Auth(PICC_AUTHENT1A, blockAddr, sectorKeyA[blockAddr/4], serNum);			// Authentication
	if (status == MI_OK) {
		// Write data
		status = MFRC522_Write(blockAddr, sectorNewKeyA[blockAddr/4]);
		printf("Set the new card password, and can modify the data of the Sector %d:", blockAddr/4);
		for (i=0; i<6; i++) printf("0x%x, ",sectorNewKeyA[blockAddr/4][i]);
		printf("\r\n");
		blockAddr = blockAddr - 3 ;
		status = MFRC522_Write(blockAddr, writeData);
		if (status == MI_OK) printf("The card has  $100 !\r\n");
 
	}
 
	// Card reader
	blockAddr = 11;// Data block 11
	status = MFRC522_Auth(PICC_AUTHENT1A, blockAddr, sectorNewKeyA[blockAddr/4], serNum);		// Authentication
	if (status == MI_OK) {
		// Read data
		blockAddr = blockAddr - 3 ;
		status = MFRC522_Read(blockAddr, str);
		if (status == MI_OK)
		{
			printf("Read from the card ,the data is : ");
			for (i=0; i<16; i++)
			{
				printf("%d, ",str[i]);
			}
			printf("\r\n");
		}
	}
 
	// Consumer(Потребление)
	blockAddr = 11;																				// Data block 11
	status = MFRC522_Auth(PICC_AUTHENT1A, blockAddr, sectorNewKeyA[blockAddr/4], serNum);		// Authentication
	if (status == MI_OK) {
		// Read data
		blockAddr = blockAddr - 3 ;
		status = MFRC522_Read(blockAddr, str);
		if (status == MI_OK) {
			if (str[15] < 1) {
				printf("The money is not enough !\r\n");
			} else {
				str[15] = str[15] - moneyConsume;
				status = MFRC522_Write(blockAddr, str);
				if (status == MI_OK) {
					printf("You pay $1. Now, Your money balance is : %d $ \r\n", str[15]);
				}
			}
		}
	}
	
	// Recharge(Зарядка)
	blockAddr = 11;																				// Data block 11
	status = MFRC522_Auth(PICC_AUTHENT1A, blockAddr, sectorNewKeyA[blockAddr/4], serNum);		// Authentication
	if (status == MI_OK) { 																		// Read data
		blockAddr = blockAddr - 3;
		status = MFRC522_Read(blockAddr, str);
		if (status == MI_OK) {
			tmp = (int)(str[15] + moneyAdd);
			// printf("Balance %d $\r\n",tmp);
			if (tmp > (char)254) {
				printf("The money of the card can not be more than 255$!\r\n");
			} else {
				str[15] = str[15] + 2 ;
				status = MFRC522_Write(blockAddr, str);
				if (status == MI_OK) {
					printf("Add 10 $. Your money balance is : %d $ \r\n", str[15]);
				}
			}
		}
	}
	MFRC522_Halt();																				// Put card in sleep mode
}
 
// Function:Write_MFRC5200
// Description:Write a byte of data to MFRC522 register
// Input parameters:addr--register address;val--value to be written
void MFRC522_WriteRegister(uint8_t addr, uint8_t val) {
        RF_Activate();
        addr = (addr << 1) & 0x7E;																	// Address format: 0XXXXXX0
        SPI1_WriteReg(addr, val);
        RF_Deactivate();
}
 
// Function:MFRC522_ReadRegister
// Description:Read a byte of data from MFRC522 register
// Input parameters:addr--register address
// Return value: Returns a byte of data
uint8_t MFRC522_ReadRegister(uint8_t addr) {
	uint8_t val;

	RF_Activate();
	addr = ((addr << 1) & 0x7E) | 0x80;									// Address format: 1XXXXXX0
	val = SPI1_ReadReg(addr);
	RF_Deactivate();
	return val;
}
 
// Function:SetBitMask
// Description:Set RC522 register bit
// Input parameters:reg--register address;mask--set value
// Return value:
void SetBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp;
    tmp = MFRC522_ReadRegister(reg);
    MFRC522_ReadRegister(reg, tmp | mask);  													// set bit mask
}
 
// Function:ClearBitMask
// Description:Clear RC522 register bit
// Input parameters:reg--register address;mask--clear bit value
void ClearBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp;
    tmp = MFRC522_ReadRegister(reg);
    MFRC522_ReadRegister(reg, tmp & (~mask));  												// clear bit mask
}
 
// Function:MFRC522_AntennaOn
// Description:Power up ready for use, minimum 1ms interval between on/off cycle
// Input values:
// Return value:
void MFRC522_AntennaOn(void) {
	uint8_t temp;

	// TxControlReg - состояние антенных пинов Tx1 и Tx2
	// 0 и 1 биты в единице - передатчик будет выдавать 13,56 МГц
	temp = MFRC522_ReadRegister(TxControlReg);
	if (!(temp & 0x03)) SetBitMask(TxControlReg, 0x03);
}

// Function:MFRC522_AntennaOff
// Description:Power down after use, minimum 1ms interval between on/off cycle
void MFRC522_AntennaOff(void) {
	ClearBitMask(TxControlReg, 0x03);
}

void MFRC522_Reset(void) {
	// Включает и выключает выполнение команд,
	// По умолчанию CommandReg = 0х20 - аналоговая часть приемника выкл.
	// Устанавливаем 0хFF - Soft Reset
    MFRC522_ReadRegister(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}
