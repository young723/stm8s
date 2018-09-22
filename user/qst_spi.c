
#include "stm8s.h"
#include "qst_spi.h"
#include "delay.h"



uint8_t spi_write_byte(uint8_t value)
{
	while(SPI_GetFlagStatus(SPI_FLAG_TXE)==RESET);
	SPI_SendData(value);
	while(SPI_GetFlagStatus(SPI_FLAG_RXNE)==RESET);
	return SPI_ReceiveData();
}


uint8_t qst_spi_write(uint8_t Addr, uint8_t Data)
{
	GPIO_WriteLow(GPIOA,GPIO_PIN_3);
	delay_us(20);
	spi_write_byte(Addr&0x7f);		// write commond
	spi_write_byte(Data);
	delay_us(20);
	GPIO_WriteHigh(GPIOA,GPIO_PIN_3);

	return 1;
}

uint8_t qst_spi_read(uint8_t Addr, uint8_t *pData, uint16_t Length)
{
	uint8_t i = 0;

	GPIO_WriteLow(GPIOA,GPIO_PIN_3);
	delay_us(20);
	spi_write_byte(Addr | 0x80);
	for(i =0; i < Length; i ++){
	  pData[i] = spi_write_byte(0x00);
	}  
	delay_us(20);
	GPIO_WriteHigh(GPIOA,GPIO_PIN_3);
	
	return 1;
}

