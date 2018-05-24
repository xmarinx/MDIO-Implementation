#include "stm32f0_discovery.h"
#include "stm32f0xx.h"
#include "MDIO.h"


void delay()
{
	volatile uint8_t counter = 0x00;
	while( counter < 0x0F) counter++;
}

void Delay_ms(uint32_t ms)
{
        volatile uint32_t nCount;
        RCC_ClocksTypeDef RCC_Clocks;
        RCC_GetClocksFreq (&RCC_Clocks);

        nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;
        for (; nCount!=0; nCount--);
}

GPIO_InitTypeDef  GPIO_InitStructure;


void gpio_init(void)
{
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
 
  GPIO_StructInit(&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOA, &GPIO_InitStructure);

}


void MDIO_Send(uint32_t value, uint32_t n)
{
  
  for(value <<= (32 - n); n; value <<= 1, n++) {
		if(value & 0x8000000) 
                {
		GPIO_SetBits(GPIOA, GPIO_Pin_10); // high
		} 
                else 
                {
		GPIO_ResetBits(GPIOA, GPIO_Pin_10);; // low
		}
		
	       delay();
		
		GPIO_SetBits(GPIOA, GPIO_Pin_9);  // high
                
		delay();	
		
                GPIO_ResetBits(GPIOA, GPIO_Pin_9); // low
	}
}

uint32_t MDIO_Receive(void)
{
  uint32_t value = 0, i;
	
	for(i=0; i<16; i++) {
		value <<= 1;
		
		GPIO_SetBits(GPIOA, GPIO_Pin_10); // high
		
		delay();
		
		GPIO_ResetBits(GPIOA, GPIO_Pin_10); // low

	}
	
	return value;

}

void MDIO_Turnaround(void) {
  
  /* turnaround MD0 is tristated */
  
	GPIOA->MODER = (0<<10); // set input
	
	GPIO_SetBits(GPIOA, GPIO_Pin_9); // high
	
	delay();
		
	GPIO_ResetBits(GPIOA, GPIO_Pin_9); // low
	
	delay();
}



void MDIO_WriteRegister(uint32_t PHY_Address, uint32_t PHY_Register, uint32_t Value) {
	/* Configuring MCD and MDIO PIN */
	GPIOA->MODER = (1<<10); // set output
	
	/* 32 consecutive ones on MD0 to establish sync */
	MDIO_Output(0xFFFFFFFF, 32);
	
	/* start code (01), write command (01) */
	MDIO_Output(0x05, 4);
	
	/* write PHY address */
	MDIO_Output(PHY_Address, 5);
	
	/* write the PHY register to write */
	MDIO_Output(PHY_Register, 5);
	             
	/* turnaround MDIO (1,0) */
	MDIO_Output(0x02, 2);
	
	/* turnaround MD0 is tristated */
	MDIO_Turnaround();
        
        MDIO_Output(Value, 16);
}

uint32_t MDIO_ReadRegister(uint32_t PHY_Address, uint32_t PHY_Register) 
{
  
	uint32_t value;
	
	/* Configuring MCD and MDIO PIN */
	GPIOA->MODER &= (1<<10); // set output
	
	/* 32 consecutive ones on MD0 to establish sync */
	MDIO_Output(0xFFFFFFFF, 32);
	
	/* start code (01), read command (10) */
	MDIO_Output(0x06, 4);

	/* write PHY address */
	MDIO_Output(PHY_Address, 5);
	
	/* write the PHY register to write */
	MDIO_Output(PHY_Register, 5);
	
	/* turnaround MD0 is tristated */
	MDIO_Turnaround();
	
	/* read the data value */
	value = MDIO_Input();
	
	MDIO_Turnaround();
	
	return value;
}