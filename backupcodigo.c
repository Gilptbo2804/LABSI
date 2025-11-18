#include "stm32g431xx.h"



void clockinit(void) {

	RCC->CR &= ~RCC_CR_HSION;
	FLASH->ACR |= FLASH_ACR_LATENCY_4WS | ( 1 << 10)| ( 1 << 9)| ( 1 << 8);
	PWR->CR5 &= ~PWR_CR5_R1MODE;



	RCC->CFGR &= ~RCC_CFGR_HPRE_3;		// reset sysclk prescaler to 1


	RCC->CR |= RCC_CR_HSEON;      // turn on HSE
	while (((RCC->CR)&RCC_CR_HSERDY)==0);          //wait til HSE is ready



	RCC->PLLCFGR &= ~0x00001000 ;
	RCC->PLLCFGR |= (0b11 << 0)
					| (0b0101<<RCC_PLLCFGR_PLLM_Pos)
					| ( 85 << RCC_PLLCFGR_PLLN_Pos )
					| (0b00 << RCC_PLLCFGR_PLLR_Pos)
					| RCC_PLLCFGR_PLLREN
					| RCC_PLLCFGR_PLLPEN;  // PLL source = HSE, PLLM=6, PLLN=85, PLLR=divisor por 2, PLLR enable


	RCC->CR |= RCC_CR_PLLON;      // turn on PLL

	while (((RCC->CR)&RCC_CR_PLLRDY)==0);        //wait til PLL is ready
	RCC->CFGR |= RCC_CFGR_SW_0 | RCC_CFGR_SW_1; // PLL selected as system clock

}


 void timer6init(){

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
	// F_out = F_clk / ((PSC + 1) * (ARR + 1))
	// Se PSC = 16999, ARR tem de ser 9999 para 1hz
	TIM6->ARR&= ~(0x0000FFFF);
	TIM6->PSC=16999;
	TIM6->ARR=9999;

	TIM6->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	TIM6->CR1 |= TIM_CR1_CEN;
} 


void timer2init(){
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	// F_out = F_clk / ((PSC + 1) * (ARR + 1))
	// Se PSC = 16999, ARR tem de ser ~166 para 60hz
	TIM2->ARR&= ~(0x0000FFFF);
	TIM2->PSC=692;
	TIM2->ARR=4085; // meter este no máximo

	TIM2->CCMR2 &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_OC3PE); // Clear mode bits
	TIM2->CCMR2 |= (0b0110 << TIM_CCMR2_OC3M_Pos) | (1<<7); // PWM Mode 1, Preload enable

	TIM2->CCER|= TIM_CCER_CC3E;
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CCR3 = 0;
	TIM2->CR1 |= TIM_CR1_CEN;

}

void timer8init(){

	RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
	TIM8->ARR&= ~(0x0000FFFF);

	TIM8->PSC=692;
	TIM8->ARR=0xFFFF;

	TIM8->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE | TIM_CCMR1_OC2M | TIM_CCMR1_OC2PE);
	TIM8->CCMR1 |= (0b0110 << TIM_CCMR1_OC1M_Pos)
				| TIM_CCMR1_OC1PE
				| (0b0110 << TIM_CCMR1_OC2M_Pos)
				| TIM_CCMR1_OC2PE;


	TIM8->CCMR2 &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_OC3PE);
	TIM8->CCMR2 |= (0b0110 << TIM_CCMR2_OC3M_Pos)
				| TIM_CCMR2_OC3PE;

	TIM8->CCER|= TIM_CCER_CC1E
				| TIM_CCER_CC2E
				| TIM_CCER_CC3E;

	TIM8->CCR1 = (0xFFFF *0.8);
	TIM8->CCR2 = (0xFFFF *0.4);
	TIM8->CCR3 = (0xFFFF *0.1);


	TIM8->BDTR |= TIM_BDTR_MOE;
	TIM8->CR1 |= TIM_CR1_CEN;









}


void adcinit(){
    RCC->AHB2ENR |= RCC_AHB2ENR_ADC12EN;



	RCC->CCIPR|= 0b01 << RCC_CCIPR_ADC12SEL_Pos;


    ADC1->CR &= ~ADC_CR_ADEN;

    ADC1->CR &= ~ADC_CR_DEEPPWD;
    ADC1->CR |= ADC_CR_ADVREGEN;

    ADC1->CR |= ADC_CR_ADCAL;
    while ((ADC1->CR & ADC_CR_ADCAL )!= 0);

    ADC1->CFGR = 0;
    ADC1->SQR1 = (1 << ADC_SQR1_SQ1_Pos);

    ADC1->SMPR1 |= 0b111 << ADC_SMPR1_SMP1_Pos; // 640.5 adc clock cycles para channel 0

    ADC12_COMMON->CCR |= ADC_CCR_VREFEN;

    ADC1->CR |= ADC_CR_ADEN;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0);


}


uint16_t ler_ADC(void) {
    ADC1->CR |= ADC_CR_ADSTART;

    while((ADC1->ISR & ADC_ISR_EOC) == 0);
    return ADC1->DR;
}

void init(){
	RCC->AHB2ENR |= ( 1 << 1) | RCC_AHB2ENR_GPIOCEN;

	GPIOB->MODER &= ~(0x3 << (4 * 2));
	GPIOB->MODER &= ~(0x3 << (10 * 2));

	GPIOB->MODER |= ( 0x1 << 4*2 ) | (0b10 << GPIO_MODER_MODE10_Pos);

	GPIOC->MODER &= ~(0x3 << GPIO_MODER_MODE6_Pos);
	GPIOC->MODER &= ~(0x3 << GPIO_MODER_MODE7_Pos);
	GPIOC->MODER &= ~(0x3 << GPIO_MODER_MODE8_Pos);

	GPIOC->MODER |= (0b10 << GPIO_MODER_MODE6_Pos)
					| (0b10 << GPIO_MODER_MODE7_Pos)
					| (0b10 << GPIO_MODER_MODE8_Pos);

	GPIOC->OSPEEDR |= (0b10 << GPIO_MODER_MODE6_Pos)
					| (0b10 << GPIO_MODER_MODE7_Pos)
					| (0b10 << GPIO_MODER_MODE8_Pos);

	GPIOC->OTYPER &= ~(1 << GPIO_OTYPER_OT6_Pos);
	GPIOC->OTYPER &= ~(1 << GPIO_OTYPER_OT7_Pos);
	GPIOC->OTYPER &= ~(1 << GPIO_OTYPER_OT8_Pos);

	GPIOC->PUPDR &= ~(0x3 << GPIO_PUPDR_PUPD6_Pos);
	GPIOC->PUPDR &= ~(0x3 << GPIO_PUPDR_PUPD7_Pos);
	GPIOC->PUPDR &= ~(0x3 << GPIO_PUPDR_PUPD8_Pos);

	GPIOB->OTYPER &= ~(1 << 4) ;

	GPIOB->OSPEEDR |= (0x1 << 4 * 2);
	GPIOB->OTYPER &= ~(1 << 10); // push-pull



	GPIOC->AFR[0] |= (0b0100 << GPIO_AFRL_AFSEL6_Pos)
					| (0b0100 << GPIO_AFRL_AFSEL7_Pos);

	GPIOC->AFR[1] |= 0b0100 << GPIO_AFRH_AFSEL8_Pos;


	GPIOB->AFR[1] &= ~(0xF << ((10 - 8) * 4));
	GPIOB->AFR[1] |= (0b001 << GPIO_AFRH_AFSEL10_Pos);



	GPIOB->OSPEEDR |= (0x3 << (10*2));

	GPIOB->PUPDR &= ~(0x3 << (0 * 2));

	// Início do PA0

	RCC->AHB2ENR|= RCC_AHB2ENR_GPIOAEN;

	GPIOA->MODER &= ~(0b11 << GPIO_MODER_MODE0_Pos);
	GPIOA->MODER |= (0b11 << GPIO_MODER_MODE0_Pos);
	GPIOA->PUPDR &= ~(0x3 << (0*2));


	//GPIOB->BSRR = 1 << 20;
	//GPIOB->BSRR = 1 << 4;
}



void TIM6_DAC_IRQHandler(){

    if (TIM6->SR & TIM_SR_UIF) {

    	GPIOB->ODR ^= GPIO_ODR_OD4;

       TIM6->SR &= ~TIM_SR_UIF;
    }

}

volatile uint32_t clk_freq;


int main(void)
{
	clockinit();
	SystemCoreClockUpdate();
	clk_freq= SystemCoreClock;
	init();
	timer6init();
	timer2init();
	adcinit();
	timer8init();

  while (1)
  {

	 uint16_t adcvalue=ler_ADC();
	 TIM2->CCR3 = adcvalue;

  }

}

