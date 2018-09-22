
#ifndef _QST_SPI_H_
#define _QST_SPI_H_

extern uint8_t qst_spi_write(uint8_t Addr, uint8_t Data);
extern uint8_t qst_spi_read(uint8_t Addr, uint8_t *pData, uint16_t Length);
#endif

