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

#define OFFSET 1260


volatile float target_hz=35.0f;
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



		//setdutycycle(cycle1, cycle2, cycle3);
		setdutycycle(0.5f, 0.4f, 0.4f);

		counter++;

		if(counter>=20000){
			seconds++;
			counter=0;
		}


		target_hz = 35.0f + (seconds * 5.0f);
/*
if (target_hz>80){
	target_hz=80;
}*/


		TIM7->SR &= ~TIM_SR_UIF;
    }

}


volatile float Ia = 0.0f;
volatile float Ib = 0.0f;
volatile float Ic = 0.0f;
volatile float Pos = 0.0f;
volatile float pos_temp = 0.0f;
volatile uint16_t debugIa=0,debugIb=0,debugIc=0,debugPos=0;

void ADC1_2_IRQHandler(){
    if(ADC1->ISR & ADC_ISR_JEOC){
        ADC1->ISR |= ADC_ISR_JEOC;

        GPIOB->ODR ^= GPIO_ODR_OD4;

        uint16_t adc_Ia = ADC1->JDR1;

        uint16_t adc_Pos = ADC1->JDR2;

        uint16_t adc_Ib = ADC2->JDR1;


        uint16_t adc_Ic = ADC2->JDR2;


        /*
		debugIa=adc_Ia;
		debugIb=adc_Ib;
		debugIc=adc_Ic;
		debugPos=adc_Pos;
		*/

        debugIa=0.1*adc_Ia+(1-0.10)*debugIa;
        debugIb=0.1*adc_Ib+(1-0.10)*debugIb;
        debugIc=0.1*adc_Ic+(1-0.10)*debugIc;


		Ia=((float)debugIa * 0.029373) - 73.844224357467;
		Ib=((float)debugIb * 0.029373) - 73.668130469894;
		Ic=((float)debugIc * 0.029373) - 73.785526394942;

		//debugPos=adc_Pos;
		pos_temp=0.1*adc_Pos+(1-0.10)*pos_temp;

		if (pos_temp < 184) pos_temp = 184;
		if (pos_temp > 3742) pos_temp = 3742;

		Pos = (pos_temp - 184) * (360.0f / (3742 - 184));

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
	opamp_init();
	adc_init();

	while (1)
  {
//empty

  }

}
