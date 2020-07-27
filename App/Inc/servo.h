#ifndef _SERVO_H_
#define _SERVO_H_

#define servo_right 1285//1350 //1325
#define servo_mid  1430//1495
#define servo_left 1590//1650//1670

extern void servo_control();
extern float kp,kp1_l,kp1_r ;
extern uint16 angle;
extern float pd_err,last_err;
extern float err, errDot, errLast;
extern int16 speed;

#endif