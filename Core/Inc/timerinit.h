#include "stm32g431xx.h"

#define ARR_VALUE 4250


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
	TIM1->ARR=ARR_VALUE;

 	TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE | TIM_CCMR1_OC2M | TIM_CCMR1_OC2PE);
 	TIM1->CCMR1 |= (0b0110 << TIM_CCMR1_OC1M_Pos)
 				| TIM_CCMR1_OC1PE
 				| (0b0110 << TIM_CCMR1_OC2M_Pos)
 				| TIM_CCMR1_OC2PE;


 	TIM1->CCMR2 &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_OC3PE);
 	TIM1->CCMR2 |= (0b0110 << TIM_CCMR2_OC3M_Pos)
 				| TIM_CCMR2_OC3PE
 				| (0b0110 << TIM_CCMR2_OC4M_Pos)
				| TIM_CCMR2_OC4PE;

 	TIM1->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC1NE)
 	            | (TIM_CCER_CC2E | TIM_CCER_CC2NE)
 	            | (TIM_CCER_CC3E | TIM_CCER_CC3NE)
				| (TIM_CCER_CC4E);


 	TIM1->BDTR = (140 << TIM_BDTR_DTG_Pos) | TIM_BDTR_MOE | TIM_BDTR_AOE;




 //	TIM1->CCR1 = (8499 *0.1);
 //	TIM1->CCR2 = (8499 *0.4);
 //	TIM1->CCR3 = (8499 *0.1);

 	TIM1->CCR4=ARR_VALUE;

 	//TIM1->RCR = 1; // Este registo é o repetition Counter, supostamente garante que o ADC
 	// só dispara no "Underflow"

	TIM1->CR1 |= (0b11 << TIM_CR1_CMS_Pos);

	TIM1->CR2 |= (0b0111 << TIM_CR2_MMS_Pos); //




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

			}  if (cycle2<0.0f){

			cycle2=0.0f;

			}

			if(cycle3>1.0f){

			cycle3=1.0f;

			} if (cycle3<0.0f){

			cycle3=0.0f;

			}


		TIM1->CCR1 = (cycle1 * ARR_VALUE);// >> 10;
		TIM1->CCR2 = (cycle2 * ARR_VALUE);// >> 10;
		TIM1->CCR3 = (cycle3 * ARR_VALUE);// >> 10;

  }
