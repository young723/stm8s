
#include "stm8s.h"
#include "qst_i2c.h"
#include "delay.h"

#if defined(QST_CONFIG_QMCX983)
#define QMC6983_A1_D1				0
#define QMC6983_E1					1	
#define QMC7983						2
#define QMC7983_LOW_SETRESET		3
#define QMC6983_E1_Metal			4
#define QMC7983_Vertical			5
#define QMC7983_Slope				6

extern void qst_printf(const char *format, ...);
static uint8_t mag_chip_id=QMC7983_Vertical;

void qmcX983_read_xyz(void)
{
	uint8_t reg_data[6];
	int16_t raw[3];

	qst_iic_read((0x2c<<1), 0x00, reg_data, 6);
 	raw[0] = (int16_t)((reg_data[1]<<8)|(reg_data[0]));
	raw[1] = (int16_t)((reg_data[3]<<8)|(reg_data[2]));
	raw[2] = (int16_t)((reg_data[5]<<8)|(reg_data[4]));
	if(mag_chip_id >= 3)
	{
	}

	qst_printf("mag %f %f %f\n",(float)raw[0]/25.0f,(float)raw[1]/25.0f,(float)raw[2]/25.0f);
}

uint8_t qmcX983_init(void)
{
	uint8_t chip;

	qst_iic_write((0x2c<<1), 0x09, 0x1d);
	delay_ms(2);
	qst_iic_read((0x2c<<1), 0x0d, &chip, 1);
	qst_printf("qmcX983_init id=%d\n", (int16_t)chip);
	if(0x31 == chip)
	{
		mag_chip_id = QMC6983_E1;
	}
	else if(0x32 == chip)
	{
		qst_iic_write((0x2c<<1), 0x2e, 0x01);
		qst_iic_read((0x2c<<1),0x2f, &chip, 1);
		if(((chip&0x04 )>> 2))
		{
			mag_chip_id = QMC6983_E1_Metal;
		}
		else
		{
			qst_iic_write((0x2c<<1),0x2e, 0x0f);
			qst_iic_read((0x2c<<1),0x2f, &chip, 1);
			if(0x02 == ((chip&0x3c)>>2))
			{
				mag_chip_id = QMC7983_Vertical;
			}
			if(0x03 == ((chip&0x3c)>>2))
			{
				mag_chip_id = QMC7983_Slope;
			}
		}
	}
	else
	{
		return 0;
	}

	qst_iic_write((0x2c<<1), 0x21, 0x01);
	qst_iic_write((0x2c<<1), 0x20, 0x40);
	if(mag_chip_id != QMC6983_A1_D1)
	{
		qst_iic_write((0x2c<<1), 0x29, 0x80);
		qst_iic_write((0x2c<<1), 0x0a, 0x0c);
	}

	if(mag_chip_id == QMC6983_E1_Metal || mag_chip_id == QMC7983_Slope)
	{		
		qst_iic_write((0x2c<<1), 0x1b, 0x80);
	}

	qst_iic_write((0x2c<<1), 0x0b, 0x01);
	qst_iic_write((0x2c<<1), 0x09, 0x1d);

	return chip;
}
#endif

