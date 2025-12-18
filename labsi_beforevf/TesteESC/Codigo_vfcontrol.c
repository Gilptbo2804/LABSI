#include "stm32g431xx.h"
#include "systeminit.h"
#include "timerinit.h"
#include "mysin.h"


#define PI 3.141592653589793f
#define VF_RATIO 0.009090909f
#define V_BOOST_OFFSET     1.5f
/* Contas para este ratio :

RPM(max)= KV * VOLTS = 1100*14.8 = 16280 RPM

Conversão de rpm para frequência elétrica

Hz(max) = (RPM(max)*Pares_Polos)/60 = (16280*6)/60 = 1628Hz

Ratio = VOLTS/Hz(max)= 14.8/1628 = 0.007789

*/


volatile float target_hz=50.0f;
volatile uint16_t counter=0,seconds;

void TIM7_DAC_IRQHandler(){

	static float angle=0.0f;

		volatile float stepcounter=0.0f,cycle1=0,cycle2=0,cycle3=0;
	float amplitude = 0.0f;
	//float tensao;

    if (TIM7->SR & TIM_SR_UIF) {

    	stepcounter=(target_hz*(2*PI))/20000.f;

    	angle+=stepcounter;
    	if( angle >= (2*PI)){

    		angle -= (2*PI);

    	}

    	amplitude=target_hz*VF_RATIO;



    	amplitude=(amplitude/14.7991f)+0.1f; // + 0.1f offset


    //	if(amplitude<0.10f) amplitude=0.10f; // Tensão mínima de arranque
    	if(amplitude>0.96f) amplitude=0.96f; // tem que ser devido ao bootstraping,
    										 // valor escolhido através do osciloscópio

		cycle1=(fastsin(angle)*amplitude*0.5f)+0.5f;
		cycle2=(fastsin(angle+(2*PI/3))*amplitude*0.5f)+0.5f;
		cycle3=(fastsin(angle-(2*PI/3))*amplitude*0.5f)+0.5f;



		setdutycycle(cycle1, cycle2, cycle3);
		//setdutycycle(1, 1, 1);

		counter++;

		if(counter>=20000){
			seconds++;
			counter=0;
		}


		target_hz = 40.0f + (seconds * 5.0f);



    		//GPIOB->ODR ^= GPIO_ODR_OD4;
		TIM7->SR &= ~TIM_SR_UIF;
    }

}


volatile uint32_t clk_freq;


int main(void)
{
	clockinit();
	clk_freq= SystemCoreClock;
	init();
	timer1init();
	timer7init();

  while (1)
  {
		//if(target_hz>=155.0f) target_hz=155.0f;
//empty

  }

}
}

#include "stm32g431xx.h"



void clockinit(void);

void init();


void clockinit(void) {


RCC->CR &= ~RCC_CR_HSION;

FLASH->ACR |= FLASH_ACR_LATENCY_4WS | ( 1 << 10)| ( 1 << 9)| ( 1 << 8);

PWR->CR5 &= ~PWR_CR5_R1MODE;




RCC->CFGR &= ~RCC_CFGR_HPRE_3; // reset sysclk prescaler to 1



RCC->CR |= RCC_CR_HSEON; // turn on HSE

while (((RCC->CR)&RCC_CR_HSERDY)==0); //wait til HSE is ready


RCC->PLLCFGR &= ~0x00001000 ;

RCC->PLLCFGR |= (0b11 << 0)

| ( 0b0001 <<RCC_PLLCFGR_PLLM_Pos)

| ( 85 << RCC_PLLCFGR_PLLN_Pos )

| ( 0 << RCC_PLLCFGR_PLLR_Pos)

| RCC_PLLCFGR_PLLREN;

//| RCC_PLLCFGR_PLLPEN;


RCC->CR |= RCC_CR_PLLON;

while (((RCC->CR)&RCC_CR_PLLRDY)==0); //wait til PLL is ready

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



GPIOA->AFR[1] |= 6 << GPIO_AFRH_AFSEL8_Pos

| 6 << GPIO_AFRH_AFSEL9_Pos

| 6 << GPIO_AFRH_AFSEL10_Pos

| 6 << GPIO_AFRH_AFSEL12_Pos;


// GPIOB


GPIOB->MODER &= ~(0x3 << GPIO_MODER_MODE15_Pos);


GPIOB->MODER |= (0b10 << GPIO_MODER_MODE15_Pos);


GPIOB->OSPEEDR |= (0b10 << GPIO_OSPEEDR_OSPEED15_Pos);


GPIOB->OTYPER &= ~(1 << GPIO_OTYPER_OT15_Pos);


GPIOB->PUPDR &= ~(0x3 << GPIO_PUPDR_PUPD15_Pos);


GPIOB->AFR[1] |= 4 << GPIO_AFRH_AFSEL15_Pos;


// GPIOC


GPIOC->MODER &= ~(0x3 << GPIO_MODER_MODE13_Pos);


GPIOC->MODER |= (0b10 << GPIO_MODER_MODE13_Pos);


GPIOC->OSPEEDR |= (0b10 << GPIO_OSPEEDR_OSPEED13_Pos);


GPIOC->OTYPER &= ~(1 << GPIO_OTYPER_OT13_Pos);


GPIOC->PUPDR &= ~(0x3 << GPIO_PUPDR_PUPD13_Pos);


GPIOC->AFR[1] |= 4 << GPIO_AFRH_AFSEL13_Pos;


// Board LED for Debugging

/*

GPIOC->MODER &= ~(0x3 << GPIO_MODER_MODE6_Pos);


GPIOC->MODER |= (0b10 << GPIO_MODER_MODE6_Pos);


GPIOC->OSPEEDR |= (0x1 << GPIO_OSPEEDR_OSPEED6_Pos);


GPIOC->OTYPER &= ~(1 << GPIO_OTYPER_OT6_Pos);


GPIOC->PUPDR |= (0b01 << GPIO_PUPDR_PUPD6_Pos);

*/

}







#include "stm32g431xx.h"



void timer1init();

void setdutycycle (float cycle1,float cycle2,float cycle3);

void timer7init();



void timer7init(){


RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN;

// F_out = F_clk / ((PSC + 1) * (ARR + 1))

// Se PSC = 16999, ARR tem de ser 9999 para 1hz

TIM7->ARR&= ~(0x0000FFFF);

TIM7->PSC=0;

TIM7->ARR=8499;


TIM7->DIER |= TIM_DIER_UIE;

NVIC_EnableIRQ(TIM7_DAC_IRQn);

TIM7->CR1 |= TIM_CR1_CEN;

}



void timer1init(){


RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;


TIM1->PSC=0;

TIM1->ARR=8499;


TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE | TIM_CCMR1_OC2M | TIM_CCMR1_OC2PE);

TIM1->CCMR1 |= (0b0110 << TIM_CCMR1_OC1M_Pos)

| TIM_CCMR1_OC1PE

| (0b0110 << TIM_CCMR1_OC2M_Pos)

| TIM_CCMR1_OC2PE;



TIM1->CCMR2 &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_OC3PE);

TIM1->CCMR2 |= (0b0110 << TIM_CCMR2_OC3M_Pos)

| TIM_CCMR2_OC3PE;


TIM1->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC1NE)

| (TIM_CCER_CC2E | TIM_CCER_CC2NE)

| (TIM_CCER_CC3E | TIM_CCER_CC3NE);



TIM1->BDTR = (140 << TIM_BDTR_DTG_Pos) | TIM_BDTR_MOE | TIM_BDTR_AOE;



// TIM1->CCR1 = (8499 *0.1);

// TIM1->CCR2 = (8499 *0.4);

// TIM1->CCR3 = (8499 *0.1);



TIM1->BDTR |= TIM_BDTR_MOE;

TIM1->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;



}


// Added for debugging

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





void setdutycycle(float cycle1, float cycle2, float cycle3) {



if(cycle1>1.0f){


cycle1=1.0f;

}


if (cycle1<0.0f){

cycle1=0.0f;

}


if(cycle2>1.0f){


cycle2=1.0f;


} if (cycle2<0.0f){


cycle2=0.0f;


}


if(cycle3>1.0f){


cycle3=1.0f;


} if (cycle3<0.0f){


cycle3=0.0f;


}



TIM1->CCR1 = (cycle1 * 8499);// >> 10;

TIM1->CCR2 = (cycle2 * 8499);// >> 10;

TIM1->CCR3 = (cycle3 * 8499);// >> 10;


}

