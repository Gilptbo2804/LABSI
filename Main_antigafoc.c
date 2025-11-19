#include "stm32g431xx.h"
#include "systeminit.h"
#include "timerinit.h"
#include "mysin.h"

#define PI 3.141592653589793f
#define SINE_SIZE 1024



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





void TIM6_DAC_IRQHandler(){

    if (TIM6->SR & TIM_SR_UIF) {

		GPIOB->ODR ^= GPIO_ODR_OD4;

		TIM6->SR &= ~TIM_SR_UIF;
    }

}




void TIM7_DAC_IRQHandler(){
	static uint16_t angle=0,cycle1=0,cycle2=0,cycle3=0;
	static uint64_t loopcounter=0;

    if (TIM7->SR & TIM_SR_UIF) {

		angle=(loopcounter%20000)/20000.0f;

		cycle1=(sin(angle*2*PI)*0.5f)+0.5f;
		cycle2=(sin(angle*2*PI+(2*PI/3))*0.5f)+0.5f;
		cycle3=(sin(angle*2*PI-(2*PI/3))*0.5f)+0.5f;

		//cycle1 = cycle2 = cycle3 = 0.5f;

		setdutycycle(cycle1, cycle2, cycle3);

		loopcounter++;



		TIM7->SR &= ~TIM_SR_UIF;
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
	timer7init();

  while (1)
  {

	 uint16_t adcvalue=ler_ADC();
	 TIM2->CCR3 = adcvalue;

  }

}
