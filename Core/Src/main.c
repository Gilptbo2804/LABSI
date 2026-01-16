#include "stm32g431xx.h"
#include "systeminit.h"
#include "timerinit.h"
#include "mysin.h"
#include "focalgorithm.h"

volatile uint32_t clk_freq;
#define PI 3.141592653589793f
#define VF_RATIO 0.009090909f
#define V_BOOST_OFFSET     1.5f
#define V_BUS 14.8f
/* Contas para este ratio :

RPM(max)= KV * VOLTS = 1100*14.8 = 16280 RPM

Conversão de rpm para frequência elétrica

Hz(max) = (RPM(max)*Pares_Polos)/60 = (16280*6)/60 = 1628Hz

Ratio = VOLTS/Hz(max)= 14.8/1628 = 0.007789

*/

#define OFFSET 1260


#define ADC_MIN_VAL   7.0f    // Leitura a 0 graus
#define ADC_MAX_VAL   4095.0f   // Leitura a 360 graus
#define ADC_RANGE     (ADC_MAX_VAL - ADC_MIN_VAL) // = 3558.0f
#define PARPOLOS 7

//******************** debug******
// Variável para leres no Debug
volatile float leitura_sensor_no_zero = 0.0f;
volatile float offset_final_calculado = 0.0f;
//***************************************

volatile float offset_eletricoglobal=0.0f;

volatile float target_hz=35.0f;
volatile uint16_t counter=0,seconds;

volatile uint8_t flag_motor=0; // 0 Parado, 1 Alinhamento, 2 Andamento


void TIM7_DAC_IRQHandler(){

	//float tensao;

    if (TIM7->SR & TIM_SR_UIF) {

    			counter++;

		if(counter>=20000){
			seconds++;
			counter=0;
		}
		TIM7->SR &= ~TIM_SR_UIF;
    }

}


volatile float Ia = 0.0f;
volatile float Ib = 0.0f;
volatile float Ic = 0.0f;
volatile float Posgraus = 0.0f;
volatile float pos_temp = 0.0f;
volatile float OffsetIa=-0.101173162,OffsetIb=-0.0996201262,OffsetIc=-0.117147371,debugPos=0;
volatile float theta_mec=0.0f,theta_e=0.0f;
volatile float testecorrentes=0;
volatile float duty_a,duty_b,duty_c,debugPos;


volatile float angulo_aberto = 0.0f;
volatile float velocidade_aberta = 0.002f; // Mantém a velocidade
volatile float tensao_teste = 0.5f;

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

        debugPos=adc_Pos;
		Ia=-(((float)(adc_Ia) * 0.029373) - 73.844224357467);
		Ib=-(((float)adc_Ib * 0.029373) - 73.668130469894);
		Ic=-(((float)adc_Ic * 0.029373) - 73.785526394942);

/*
		if (flag_motor==2){
		OffsetIa=0.000001*Ia+(1-0.000001)*OffsetIa;
		OffsetIb=0.000001*Ib+(1-0.000001)*OffsetIb;
		OffsetIc=0.000001*Ic+(1-0.000001)*OffsetIc;
		}*/

		Ia=Ia-OffsetIa;
		Ib=Ib-OffsetIb;
		Ic=Ic-OffsetIc;

//fusível por software
		if (flag_motor == 2) {
		    if (Ia > 10.0f || Ia < -10.0f || Ib > 10.0f || Ib < -10.0f) {
		        flag_motor = 0;
		        setdutycycle(0.5f, 0.5f, 0.5f);
		        TIM1->BDTR &= ~TIM_BDTR_MOE;
		        while(1); // Morte térmica do código
		    }
		}

		//debugPos=adc_Pos;
		//pos_temp=0.1*adc_Pos+(1-0.10)*pos_temp;
		pos_temp=adc_Pos;


		//debug
		Posgraus = (pos_temp - ADC_MIN_VAL) * (360.0f / (ADC_MAX_VAL - ADC_MIN_VAL));
		testecorrentes=Ia+Ib+Ic;
		//******************************************************************************


		theta_mec=((pos_temp - ADC_MIN_VAL) / ADC_RANGE) * (2.0f * PI); // normalizado
		theta_e = (theta_mec*PARPOLOS) - offset_eletricoglobal;

		while (theta_e < 0.0f) theta_e += (2.0f * PI);
		while (theta_e >= (2.0f * PI)) theta_e -= (2.0f * PI);

// FOC

		if (flag_motor == 2) {

		clarke(Ia,Ib,Ic);

		parke(I_alpha,I_beta,fastsin(theta_e),fastcos(theta_e));

		current_control_pi();

		inv_park(V_d,V_q,fastsin(theta_e),fastcos(theta_e));

		inv_clarke(V_alpha,V_beta);




		duty_a = (Va / V_BUS) + 0.5f;
		duty_b = (Vb / V_BUS) + 0.5f;
		duty_c = (Vc / V_BUS) + 0.5f;

			if (duty_a > 0.95f) duty_a = 0.95f; else if (duty_a < 0.05f) duty_a = 0.05f;
			if (duty_b > 0.95f) duty_b = 0.95f; else if (duty_b < 0.05f) duty_b = 0.05f;
			if (duty_c > 0.95f) duty_c = 0.95f; else if (duty_c < 0.05f) duty_c = 0.05f;


		setdutycycle(duty_a, duty_b, duty_c);
		if (seconds == 10) {
				        setdutycycle(0.5f, 0.5f, 0.5f);
				        TIM1->BDTR &= ~TIM_BDTR_MOE;
				        while(1); // Morte térmica do código

				}
			}
		}
}




void alinharsensor(){

	flag_motor=1;
	counter = 0;
	seconds = 0;


	setdutycycle(0.53f, 0.48f, 0.48f);

	while(seconds<2);

	uint16_t alinhar_posraw = ADC1->JDR2;

	float theta_mec = ((((float)alinhar_posraw) - ADC_MIN_VAL) / ADC_RANGE) * (2.0f * PI);

	float offset_eletrico = theta_mec * PARPOLOS;

		while(offset_eletrico >= (2.0f * PI)) offset_eletrico -= (2.0f * PI);
		while(offset_eletrico < 0.0f)         offset_eletrico += (2.0f * PI);

	offset_eletricoglobal=offset_eletrico;


		while(seconds<3);
		flag_motor=2;
}

int main(void)
{
	clockinit();
	clk_freq= SystemCoreClock;
	init();
	timer1init();
	timer7init();
	opamp_init();
	adc_init();

	//alinharsensor();

/*
	offset_eletricoglobal -= 1.57f; //rapido 5.9334445

	    // Normalização
	    while(offset_eletricoglobal >= 2*PI) offset_eletricoglobal -= 2*PI;
	    while(offset_eletricoglobal < 0)     offset_eletricoglobal += 2*PI;
*/

	setdutycycle(0.53f, 0.48f, 0.48f);
	while(seconds<10);
	offset_eletricoglobal=2.6308754f;
	flag_motor=2;
	seconds=0;



	while (1)
  {
//empt

  }

}
