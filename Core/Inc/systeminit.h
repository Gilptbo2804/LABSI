#include "stm32g431xx.h"


void clockinit(void);
void init();
void opamp_init();
void adc_init();


// AI
void i2c_init();
uint16_t ler_as5600();





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
						| RCC_PLLCFGR_PLLREN
						| RCC_PLLCFGR_PLLPEN;

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
	// ADC OpAmps


	GPIOA->MODER &= ~(0x3 << GPIO_MODER_MODE2_Pos);
	GPIOA->MODER |= (0b11 << GPIO_MODER_MODE2_Pos);
	// Dentro da função init():

	// PA1 (Entrada + OPAMP1) -> Analog Mode
	GPIOA->MODER |= (0b11 << GPIO_MODER_MODE1_Pos);

	GPIOA->MODER &= ~(0x3 << GPIO_MODER_MODE5_Pos);
	GPIOA->MODER |= (0b11 << GPIO_MODER_MODE5_Pos);

	// PA7 (Entrada + OPAMP2) -> Analog Mode
	GPIOA->MODER |= (0b11 << GPIO_MODER_MODE7_Pos);

	GPIOA->MODER &= ~(0x3 << GPIO_MODER_MODE6_Pos);
	GPIOA->MODER |= (0b11 << GPIO_MODER_MODE6_Pos);

	GPIOB->MODER &= ~(0x3 << GPIO_MODER_MODE1_Pos);
	GPIOB->MODER |= (0b11 << GPIO_MODER_MODE1_Pos);

	GPIOB->MODER &= ~(0x3 << GPIO_MODER_MODE12_Pos);
	GPIOB->MODER |= (0b11 << GPIO_MODER_MODE12_Pos);


}

void opamp_init(){
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	OPAMP1->CSR |= ( 0b00011 << OPAMP_CSR_PGGAIN_Pos) // Ganho x16, shunt de 3mOhm
				//| OPAMP_CSR_OPAMPINTEN
				| 0b00 << OPAMP_CSR_VPSEL_Pos // VINP0 ligado ao Ampop+, segundo o datasheet pag 777
				// VINP0 é o pino PA1
				| 0b10 << OPAMP_CSR_VMSEL_Pos
				| OPAMP_CSR_OPAMPxEN;

	OPAMP2->CSR |= ( 0b00011 << OPAMP_CSR_PGGAIN_Pos)
					| 0b00 << OPAMP_CSR_VPSEL_Pos
					| 0b10 << OPAMP_CSR_VMSEL_Pos
					| OPAMP_CSR_OPAMPxEN;


	OPAMP3->CSR |= ( 0b00011 << OPAMP_CSR_PGGAIN_Pos)
					| OPAMP_CSR_OPAMPINTEN
					| 0b00 << OPAMP_CSR_VPSEL_Pos
					| 0b10 << OPAMP_CSR_VMSEL_Pos
 					| OPAMP_CSR_OPAMPxEN;

// Acho que o terceiro não é necessário, mas tá aqui na mesma.

}


void adc_init(){
    RCC->AHB2ENR |= RCC_AHB2ENR_ADC12EN | (1<<14);



	RCC->CCIPR|= 0b01 << RCC_CCIPR_ADC12SEL_Pos | (0b01<<30U) ;


    ADC1->CR &= ~ADC_CR_ADEN;
    ADC2->CR &= ~ADC_CR_ADEN;

    ADC1->CR &= ~ADC_CR_DEEPPWD;
    ADC2->CR &= ~ADC_CR_DEEPPWD;

    ADC1->CR |= ADC_CR_ADVREGEN;
    ADC2->CR |= ADC_CR_ADVREGEN;

    for(volatile int i = 0; i < 3000; i++); // isto é um delay de 20us, segundo o
    //datasheet é necessário

    ADC12_COMMON->CCR |= (0b0010 << ADC_CCR_PRESC_Pos);

    ADC1->CR |= ADC_CR_ADCAL;
    ADC2->CR |= ADC_CR_ADCAL;
       while ((ADC1->CR & ADC_CR_ADCAL ) || (ADC2->CR & ADC_CR_ADCAL ));

    ADC12_COMMON->CCR |= ADC_CCR_VREFEN;

			ADC1->JSQR |= (3 << ADC_JSQR_JSQ1_Pos)    //
						| (11 << ADC_JSQR_JSQ2_Pos)  // IN11, PB12 Leitura hall
						| (0b01 << ADC_JSQR_JEXTEN_Pos) // Trigger: Rising Edge
						|  (0 << ADC_JSQR_JEXTSEL_Pos)    // Source: 00000 = TIM1_TRGO
						|  (1 << ADC_JSQR_JL_Pos);        // 2 conversões

            // Configurar Sample Time para o Canal 13 (Recomendado 24.5 ou 47.5 ciclos para canais internos)
            // Em adc_init():
            ADC1->SMPR2 |= (0b010 << ADC_SMPR2_SMP13_Pos); // 12.5 ciclos

			ADC2->JSQR |= (3 << ADC_JSQR_JSQ1_Pos)
						| (18 << ADC_JSQR_JSQ2_Pos)
						| (0b01 << ADC_JSQR_JEXTEN_Pos)
						|  (0 << ADC_JSQR_JEXTSEL_Pos)
						|  (1 << ADC_JSQR_JL_Pos);

            ADC2->SMPR2 |= (0b010 << ADC_SMPR2_SMP16_Pos); // 12.5 ciclos


		ADC1->ISR |= ADC_ISR_ADRDY; // Limpar flag antiga
		ADC2->ISR |= ADC_ISR_ADRDY; // Limpar flag antiga


    ADC1->CR |= ADC_CR_ADEN;
    ADC2->CR |= ADC_CR_ADEN;
    while( ( (ADC1->ISR & ADC_ISR_ADRDY) && (ADC2->ISR & ADC_ISR_ADRDY) ) == 0 );

    ADC1->IER |= ADC_IER_JEOCIE;

    NVIC_EnableIRQ(ADC1_2_IRQn);

    ADC1->CR |= ADC_CR_JADSTART;
    ADC2->CR |= ADC_CR_JADSTART;


}


