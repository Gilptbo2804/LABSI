#include "stm32g431xx.h"
#include "systeminit.h"
#include "timerinit.h"
#include "mysin.h"

#define PI 3.141592653589793f




volatile uint64_t loopcounter=0;
void TIM7_DAC_IRQHandler(){
	//static uint16_t angle1=0,angle2=0,angle3=0,cycle1=0,cycle2=0,cycle3=0;
	//static uint64_t loopcounter=0;
	volatile float angle=0,cycle1=0,cycle2=0,cycle3=0;
	float amplitude = 0.96f; // tem que ser

    if (TIM7->SR & TIM_SR_UIF) {

		//angle=(2*PI)*(loopcounter%20000)/20000.0;
    	angle=(2*PI)*(loopcounter%20000)/333.0;

		cycle1=(fastsin(angle)*amplitude*0.5f)+0.5f;
		cycle2=(fastsin(angle+(2*PI/3))*amplitude*0.5f)+0.5f;
		cycle3=(fastsin(angle-(2*PI/3))*amplitude*0.5f)+0.5f;



		setdutycycle(cycle1, cycle2, cycle3);
		//setdutycycle(1, 1, 1);

		loopcounter++;

    		//GPIOB->ODR ^= GPIO_ODR_OD4;
		TIM7->SR &= ~TIM_SR_UIF;
    }

}
/*
void TIM6_DAC_IRQHandler(){

    if (TIM6->SR & TIM_SR_UIF) {

    	GPIOC->ODR ^= GPIO_ODR_OD6;

		TIM6->SR &= ~TIM_SR_UIF;
    }

}*/



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

//empty

  }

}
