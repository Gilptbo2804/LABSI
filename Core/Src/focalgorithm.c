#include "focalgorithm.h"
#include "mysin.h"

void clarke(float Ia, float Ib, float Ic);
void parke(float I_alpha, float I_beta,float sin_t, float cos_t);

#define ONE_BY_SQRT3  0.57735026919f
#define TWO_THIRDS    0.66666666667f
volatile float V_alpha = 0.0f;
volatile float V_beta = 0.0f;
volatile float Va = 0.0f;
volatile float Vb = 0.0f;
volatile float Vc = 0.0f;
volatile float I_alpha,I_beta,I_d,I_q;



void clarke(float Ia, float Ib, float Ic) {

    // I_alpha = 2/3 * (Ia - 0.5*Ib - 0.5*Ic)
    I_alpha = TWO_THIRDS * (Ia - 0.5f * Ib - 0.5f * Ic);

    // I_beta = 2/3 * (sqrt(3)/2 * Ib - sqrt(3)/2 * Ic)
    // Simplificando a matemática: 2/3 * sqrt(3)/2 = 1/sqrt(3)
    I_beta = ONE_BY_SQRT3 * (Ib - Ic);

}

void parke(float I_alpha, float I_beta,float sin_t, float cos_t){

	I_d = (I_alpha * cos_t) + (I_beta * sin_t);

	I_q = (-I_alpha*sin_t) + (I_beta * cos_t);

}

void inv_park(float V_d, float V_q, float sin_t, float cos_t) {
    V_alpha = (V_d * cos_t) - (V_q * sin_t);
    V_beta  = (V_d * sin_t) + (V_q * cos_t);
}

void inv_clarke(float V_alpha, float V_beta) {
    Va = V_alpha;
    Vb = (-0.5f * V_alpha) + (SQRT3_BY_TWO * V_beta);
    Vc = (-0.5f * V_alpha) - (SQRT3_BY_TWO * V_beta);
}


//PI CONTROLS

float V_d, V_q;
float Id_ref = 0.0f;
float Iq_ref = 1.5f; // 2 Amperes de força
// Ganhos do PI (Têm de ser sintonizados!) kp=0.01 ki=0.001
float Kp = 0.1f, Ki = 0.001f;
float integral_d = 0, integral_q = 0;

void current_control_pi() {
    // Erro
    float err_d = Id_ref - I_d;
    float err_q = Iq_ref - I_q;

    // Integral (com anti-windup simplificado)
    integral_d += err_d * Ki;
	integral_q += err_q * Ki;



        if(integral_d > 10.0f) integral_d = 10.0f; else if(integral_d < -10.0f) integral_d = -10.0f;
        if(integral_q > 10.0f) integral_q = 10.0f; else if(integral_q < -10.0f) integral_q = -10.0f;



    // Saída (Voltagem necessária nos eixos D e Q)
    V_d = (err_d * Kp) + integral_d;
    V_q = (err_q * Kp) + integral_q;

}





