#include "stm32g431xx.h"


void clockinit(void);
void init();

void clockinit(void) {

	RCC->CR &= ~RCC_CR_HSION;
	FLASH->ACR |= FLASH_ACR_LATENCY_4WS | ( 1 << 10)| ( 1 << 9)| ( 1 << 8);
	PWR->CR5 &= ~PWR_CR5_R1MODE;



	RCC->CFGR &= ~RCC_CFGR_HPRE_3;		// reset sysclk prescaler to 1


	RCC->CR |= RCC_CR_HSEON;      // turn on HSE
	while (((RCC->CR)&RCC_CR_HSERDY)==0);          //wait til HSE is ready

	RCC->PLLCFGR &= ~0x00001000 ;
		RCC->PLLCFGR |= (0b11 << 0)
						| ( 0b0001 <<RCC_PLLCFGR_PLLM_Pos)
						| ( 85 << RCC_PLLCFGR_PLLN_Pos )
						| ( 0 << RCC_PLLCFGR_PLLR_Pos)
						| RCC_PLLCFGR_PLLREN;
						//| RCC_PLLCFGR_PLLPEN;

	RCC->CR |= RCC_CR_PLLON;
	while (((RCC->CR)&RCC_CR_PLLRDY)==0);        //wait til PLL is ready
		RCC->CFGR |= RCC_CFGR_SW_0 | RCC_CFGR_SW_1;

	SystemCoreClockUpdate();
	//RCC->CFGR |= (0b01<<RCC_CFGR_SW_Pos); // HSI16 selected as system clock

}

void init(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN;

	GPIOA->MODER &= ~(0x3 << GPIO_MODER_MODE8_Pos);
	GPIOA->MODER &= ~(0x3 << GPIO_MODER_MODE9_Pos);
	GPIOA->MODER &= ~(0x3 << GPIO_MODER_MODE10_Pos);
	GPIOA->MODER &= ~(0x3 << GPIO_MODER_MODE12_Pos);

	GPIOA->MODER |= (0b10 << GPIO_MODER_MODE8_Pos)
					| (0b10 << GPIO_MODER_MODE9_Pos)
					| (0b10 << GPIO_MODER_MODE10_Pos)
					| (0b10 << GPIO_MODER_MODE12_Pos);

	GPIOA->OSPEEDR |= (0b10 << GPIO_OSPEEDR_OSPEED8_Pos)
					| (0b10 << GPIO_OSPEEDR_OSPEED9_Pos)
					| (0b10 << GPIO_OSPEEDR_OSPEED10_Pos)
					| (0b10 << GPIO_OSPEEDR_OSPEED12_Pos);

	GPIOA->OTYPER &= ~(1 << GPIO_OTYPER_OT8_Pos);
	GPIOA->OTYPER &= ~(1 << GPIO_OTYPER_OT9_Pos);
	GPIOA->OTYPER &= ~(1 << GPIO_OTYPER_OT10_Pos);
	GPIOA->OTYPER &= ~(1 << GPIO_OTYPER_OT12_Pos);

	GPIOA->PUPDR &= ~(0x3 << GPIO_PUPDR_PUPD8_Pos);
	GPIOA->PUPDR &= ~(0x3 << GPIO_PUPDR_PUPD9_Pos);
	GPIOA->PUPDR &= ~(0x3 << GPIO_PUPDR_PUPD10_Pos);
	GPIOA->PUPDR &= ~(0x3 << GPIO_PUPDR_PUPD12_Pos);


	GPIOA->AFR[1] |=  6 << GPIO_AFRH_AFSEL8_Pos
					| 6 << GPIO_AFRH_AFSEL9_Pos
					| 6 << GPIO_AFRH_AFSEL10_Pos
					| 6  << GPIO_AFRH_AFSEL12_Pos;

// GPIOB

	GPIOB->MODER &= ~(0x3 << GPIO_MODER_MODE15_Pos);

	GPIOB->MODER |= (0b10 << GPIO_MODER_MODE15_Pos);

	GPIOB->OSPEEDR |= (0b10 << GPIO_OSPEEDR_OSPEED15_Pos);

	GPIOB->OTYPER &= ~(1 << GPIO_OTYPER_OT15_Pos);

	GPIOB->PUPDR &= ~(0x3 << GPIO_PUPDR_PUPD15_Pos);

	GPIOB->AFR[1] |=  4 << GPIO_AFRH_AFSEL15_Pos;

	// GPIOC

	GPIOC->MODER &= ~(0x3 << GPIO_MODER_MODE13_Pos);

	GPIOC->MODER |= (0b10 << GPIO_MODER_MODE13_Pos);

	GPIOC->OSPEEDR |= (0b10 << GPIO_OSPEEDR_OSPEED13_Pos);

	GPIOC->OTYPER &= ~(1 << GPIO_OTYPER_OT13_Pos);

	GPIOC->PUPDR &= ~(0x3 << GPIO_PUPDR_PUPD13_Pos);

	GPIOC->AFR[1] |=  4 << GPIO_AFRH_AFSEL13_Pos;

	// Board LED for Debugging
/*
	GPIOC->MODER &= ~(0x3 << GPIO_MODER_MODE6_Pos);

	GPIOC->MODER |= (0b10 << GPIO_MODER_MODE6_Pos);

	GPIOC->OSPEEDR |= (0x1 << GPIO_OSPEEDR_OSPEED6_Pos);

	GPIOC->OTYPER &= ~(1 << GPIO_OTYPER_OT6_Pos);

	GPIOC->PUPDR |= (0b01 << GPIO_PUPDR_PUPD6_Pos);
*/
}






