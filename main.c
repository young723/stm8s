#include "stdio.h"
//#include "stdlib.h"
#include <stdarg.h>
#include "stm8s.h"
#include "qst_i2c.h"
#include "delay.h"

#define STM_IRQ_RX_ENABLE
#define QST_UART_DEBUG
//#define QST_WRITE_EEPROM
#if defined(QST_WRITE_EEPROM)
#include "bsp_eeprom.h"
#define EEPROM_ADDR               0x4000         //EEPROM addr
#endif

enum
{
	DEVICE_ACC = 0x01,
	DEVICE_MAG = 0x02,
	DEVICE_PRESS = 0x04,
	DEVICE_GYRO = 0x08,

	DEVICE_MAX = 0xff
};

enum
{
  	CMD_NONE,
	CMD_WRITE_ROM,
	CMD_WITRE_REG,
	CMD_READ_REG,
	CMD_ACC_SET_PARA,
	CMD_MAG_SET_PARA,
	CMD_PRE_SET_PARA,

	CMD_MAX = 0xff
};

static u32 qst_run_count = 0;
uint8_t RxTxbuf[40];
uint8_t RxTxLen = 0;
uint8_t uartCmd = CMD_NONE;

#if defined(QST_CONFIG_QMAX981)
extern uint8_t qmaX981_init(void);
extern void qma6981_read_xyz(void);
#endif
#if defined(QST_CONFIG_QMP6988)
extern uint8_t qmp6988_init(void);
extern void qma6988_calc_press(void);
extern void qmp6988_set_power_mode(unsigned char power_mode);
extern void qmp6988_set_app(unsigned char app);
#endif
#if defined(QST_CONFIG_QMCX983)
extern uint8_t qmcX983_init(void);
extern void qmcX983_read_xyz(void);
#endif



#if defined(QST_UART_DEBUG)
uint8_t itoa10(uint8_t *buf, int32_t i)
{
     uint32_t rem;
     uint8_t *s,length=0;
	 uint8_t flag=0;

     s = buf;
     if (i == 0) 
     {
     	*s++ = '0'; 
     }
     else 
     { 
          if (i < 0) 
          {
	          *buf++ = '-';
			  flag = 1;
	          s = buf;
	          i = -i;
          }
          while (i) 
          {
	          ++length;
	          rem = i % 10;
	          *s++ = rem + '0';
	          i /= 10;
          }
          for(rem=0; ((unsigned char)rem)<length/2; rem++) 
          {
	          *(buf+length) = *(buf+((unsigned char)rem));
	          *(buf+((unsigned char)rem)) = *(buf+(length-((unsigned char)rem)-1));
	          *(buf+(length-((unsigned char)rem)-1)) = *(buf+length);
          }
     }
     *s=0;

	 return ((s-buf)+flag);
}

void ftoa(uint8_t *buf, float i)
{
	int32_t i_data;
	uint8_t length=0;

	if((i>=0.000001f) && (i<=0.000001f))		
	{
	   *buf++ = '0'; 
	   *buf++ = '.'; 
	   *buf++ = '0'; 
	}
	else
	{
		if (i < 0) 
		{
			*buf++ = '-';
			i = -i;
		}
		i_data = (int32_t)i;
		length = itoa10(buf, i_data);
		buf += length;
		*buf++ = '.';
		i_data = (int32_t)((i-i_data)*10000);
		length = itoa10(buf, i_data);
		buf += length;
	}
	
	*buf=0;
}

void qst_send_string(uint8_t *str)
{
	while(*str)
	{
		while((UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET));
		UART1_SendData8(*str);
		while((UART1_GetFlagStatus(UART1_FLAG_TC)==RESET));
		str++;

	}
}
#endif

void qst_printf(const char *format, ...)
{
#if defined(QST_UART_DEBUG)
	int8_t *pc;
	int32_t value;
	float f_value;
	uint8_t buf[50];
	
	va_list arg;
	va_start(arg, format);
	//buf[0]=va_arg(arg, char);
	while (*format)
	{
		int8_t ret = *format;
		if(ret == '%')
		{
			switch (*++format)
			{
			case 'c':
			{
				buf[0] = va_arg(arg, char);
				//putchar(ch);
				buf[1] = 0;
				qst_send_string(buf);
				break;
			}
			case 's':
			{
				pc = va_arg(arg, int8_t *);
				//while (*pc)
				//{
				//	putchar(*pc);
				//	pc++;
				//}
				qst_send_string((uint8_t*)pc);
				break;
			}
			case 'd':
			{
				value =	va_arg(arg, int16_t);
				itoa10(buf, value);
				qst_send_string(buf);
				break;
			}
			case 'l':
			{
				value =	va_arg(arg, int32_t);
				itoa10(buf, value);
				qst_send_string(buf);
				break;
			}			
			case 'f':
			{
				f_value = va_arg(arg, float);
				ftoa(buf, f_value);
				qst_send_string(buf);
				break;
			}			
			default:
				break;
			}
		}
		else
		{
			//putchar(*format);
			//qst_send_string((uint8_t*)&ret);
			while((UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET));
			UART1_SendData8(ret);
			while((UART1_GetFlagStatus(UART1_FLAG_TC)==RESET));
		}
		format++;
	}
	va_end(arg);
#endif
}

void clk_config(void)
{
	CLK_DeInit(); 
	// system clock
	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO,CLK_SOURCE_HSI,ENABLE,CLK_CURRENTCLOCKSTATE_ENABLE);
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
	CLK_HSICmd(ENABLE);
}

void uart_config(void)
{
	// uart1
	UART1_DeInit();
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1,ENABLE);
	UART1_Init(115200,UART1_WORDLENGTH_8D,UART1_STOPBITS_1,UART1_PARITY_NO,UART1_SYNCMODE_CLOCK_DISABLE,
		UART1_MODE_TXRX_ENABLE);
	UART1_Cmd(ENABLE);
#if defined(STM_IRQ_RX_ENABLE)
	UART1_ITConfig(UART1_IT_RXNE_OR,ENABLE);
#else
	UART1_ITConfig(UART1_IT_RXNE_OR,DISABLE);
#endif
}

void i2c_config(uint16_t addr)
{
	//hardware i2c
#if defined(QST_SW_IIC)||defined(QST_SW_IIC_MTK)
#else
	I2C_DeInit();
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C,ENABLE);	
	I2C_Init(400000, addr, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
	I2C_Cmd(ENABLE);
#endif
}

void spi_config(void)
{
	//PA3:CS	PC5:CLK		PC6:MOSI	PC7:MISO
	SPI_DeInit();
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);	
	GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);		// CS
    //GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);		//CLK
    //GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_FAST);		//MOSI
    //GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT);			//MISO 
   
	SPI_Init(SPI_FIRSTBIT_MSB,SPI_BAUDRATEPRESCALER_4,SPI_MODE_MASTER,SPI_CLOCKPOLARITY_LOW,SPI_CLOCKPHASE_1EDGE,
		SPI_DATADIRECTION_2LINES_FULLDUPLEX,SPI_NSS_SOFT,0x07);
	SPI_Cmd(ENABLE);
	GPIO_WriteHigh(GPIOA,GPIO_PIN_3);		// CS	
}

void gpio_config(void)
{
#if defined(QST_SW_IIC)||defined(QST_SW_IIC_MTK)
	GPIO_DeInit(GPIOB);
	GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);	// clk
	GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_FAST);	// data
#else
	//GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);	// clk
	//GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);	// data
#endif
}

void sys_init(void)
{
	clk_config();
	disableInterrupts();
	gpio_config();
	uart_config();
	i2c_config(0x12);
	spi_config();
#if defined(QST_WRITE_EEPROM)
	EEPROM_Initializes();
#endif
	delay_init(16);
	enableInterrupts();
	delay_ms(1000);
}

void main( void )
{
	uint8_t chip_id = 0;
	uint8_t device_type = 0;
	uint8_t	index, value;
	
	sys_init();
	
	RxTxbuf[0] = 0;
	RxTxLen = 0;
#if defined(QST_CONFIG_QMAX981)
	if((chip_id=qmaX981_init())!=0)
	{
		device_type |= DEVICE_ACC;
		qst_printf("qmaX981 OK! chip_id=%d\n",chip_id);
	}
#endif
#if defined(QST_CONFIG_QMP6988)
	if((chip_id=qmp6988_init())!=0)
	{
		device_type |= DEVICE_PRESS;
		qst_printf("qmp6988 OK! chip_id=%d\n",chip_id);
	}
#endif
#if defined(QST_CONFIG_QMCX983)
	if((chip_id=qmcX983_init())!=0)
	{
		device_type |= DEVICE_MAG;
		qst_printf("qmcX983 OK!chip_id=%d\n",chip_id);
	}
#endif
#if defined(QST_WRITE_EEPROM)
	EEPROM_ReadNByte(RxTxbuf, EEPROM_ADDR, 20);
	qst_printf("EEPROM:%s\n", RxTxbuf);
	delay_ms(200);
#endif
	while(1)
	{
		qst_run_count++;
#if defined(STM_IRQ_RX_ENABLE)
		index = 0;
		if(uartCmd == CMD_PRE_SET_PARA)
		{
		  qst_printf("res %s\n", RxTxbuf);
			while(RxTxbuf[index]&&(index<40))
			{
				if(RxTxbuf[index]=='m')
				{
				  	index++;
					value = RxTxbuf[index]-48;
					qmp6988_set_power_mode(value);
				}
				else if(RxTxbuf[index]=='a')
				{
				  	index++;
					value = RxTxbuf[index]-48;
					qmp6988_set_app(value);
					//qmp6988_set_power_mode(RxTxbuf[index]-48);
				}
			index++;
			}
			uartCmd=CMD_NONE;
		}
#endif
#if defined(QST_CONFIG_QMAX981)
		if(device_type & DEVICE_ACC)
		{
			qma6981_read_xyz();
            delay_ms(20);
		}
#endif
#if defined(QST_CONFIG_QMP6988)
		if(device_type & DEVICE_PRESS)
		{
			qma6988_calc_press();
            delay_ms(20);
		}
#else
		delay_ms(20);
#endif
#if defined(QST_CONFIG_QMCX983)
		if(device_type & DEVICE_MAG)
		{
			qmcX983_read_xyz();
			delay_ms(10);
		}
#else
		delay_ms(20);
#endif
		if(device_type == 0)
		{
			qst_printf("no sensor\n");
			delay_ms(200);
		}
		else
		{
			delay_ms(20);
		}
	}
}


#if defined(STM_IRQ_RX_ENABLE)
INTERRUPT_HANDLER(UART1_RX_IRQHandler, ITC_IRQ_UART1_RX)
{
	if(UART1_GetITStatus(UART1_IT_RXNE_OR) != RESET)		// interrupt flag
	{
		UART1_ClearITPendingBit(UART1_IT_RXNE_OR);
	}
	RxTxbuf[RxTxLen] = UART1_ReceiveData8();
	if(RxTxbuf[RxTxLen] == '\n')
	{
		RxTxbuf[RxTxLen] = 0;
		if((RxTxLen>3))
		{
			if((RxTxbuf[0]=='P')&&(RxTxbuf[1]=='R')&&(RxTxbuf[2]=='E'))
			{
				uartCmd = CMD_PRE_SET_PARA;
			}
#if defined(QST_WRITE_EEPROM)
			else if((RxTxbuf[0]=='R')&&(RxTxbuf[1]=='O')&&(RxTxbuf[2]=='M'))
			{
				EEPROM_WriteNByte(RxTxbuf+3, EEPROM_ADDR, RxTxLen-3);
			}
#endif
		}
		RxTxLen = 0;
	}
	else
	{
		RxTxLen++;
	}
	//if(UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET)
	//{
	//	RxTxLen = 0;
	//}
}
#endif

