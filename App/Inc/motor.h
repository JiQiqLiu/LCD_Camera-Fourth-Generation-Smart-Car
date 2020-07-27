#ifndef _MOTOR_H_
#define _MOTOR_H_

extern vint16 r;
extern uint16 l;
extern vint16 r_old;
extern uint16 l_old;
extern float dir, pwm_l_z, pwm_r_z,pwm_l_f,pwm_r_f, pwm_m;
extern int16 speed_ave;
extern void motor_control();
extern int16 duty1,duty2;
extern int16 speed;
extern int16 real_speed ;
extern uint32 time_stop;
#endif