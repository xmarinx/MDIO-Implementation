#include "stm32f0_discovery.h"
#include "stm32f0xx.h"

void delay();
void Delay_ms(uint32_t ms);
void gpio_init(void);
void MDIO_Send(uint32_t value, uint32_t n);
uint32_t MDIO_Receive(void);
void MDIO_Turnaround(void);
void MDIO_WriteRegister(uint32_t PHY_Address, uint32_t PHY_Register, uint32_t Value);
uint32_t MDIO_ReadRegister(uint32_t PHY_Address, uint32_t PHY_Register);