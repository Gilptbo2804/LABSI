#ifndef FOCALGORITHM_H
#define FOCALGORITHM_H

#include <stdint.h>

// --- Constantes ---
#define ONE_BY_SQRT3  0.57735026919f
#define TWO_THIRDS    0.66666666667f
#define SQRT3_BY_TWO  0.86602540378f

// --- Protótipos das Funções (O "Menu") ---
void clarke(float Ia, float Ib, float Ic);
void parke(float I_alpha, float I_beta, float sin_t, float cos_t);
void inv_park(float V_d, float V_q, float sin_t, float cos_t);
void inv_clarke(float V_alpha, float V_beta);
void current_control_pi(void);

// --- Variáveis Globais Partilhadas (EXTERN) ---
// O "extern" diz: "Estas variáveis existem em algum lugar, não as cries aqui"
extern volatile float I_alpha, I_beta;
extern volatile float I_d, I_q;
extern volatile float V_alpha, V_beta;
extern volatile float Va, Vb, Vc; // Saídas finais
extern float V_d, V_q;

// Ganhos PI
extern float Id_ref, Iq_ref;
extern float Kp, Ki;

#endif // FOC_ALGORITHM_H
