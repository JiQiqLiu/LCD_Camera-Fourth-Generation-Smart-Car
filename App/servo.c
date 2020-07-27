#include "common.h"
#include "include.h"


float err, errDot, errLast;
uint16 angle=servo_mid,angle1;
float kd=0,kp=0,kp1_l,kp1_r ;//p 1  8.3   p2  7.7  
void servo_control()
{                                            //uint8 last_mid=54;
   angle1=angle;
   
//   kd = 0;
   //if(errDot*errDot>400)  err = errLast;
   if(err < 0 )       //实际 中线 在右边  舵机往右打角
   {
        //kp= 2+speed_now*0.008;
        //kp1 = err*0.07+kp;
        //kd = kp1 *3 - 4;
//     if(fabs(err)<=5)
//       kp1 = 2.5;
//     else
       //kp1_l = 3.25/*+fabs(err)*0.002*/; //2.52    //2.2 0.018
     
     //kp = kp1_l;//+real_speed/4000.0f;
     //kp = 2.8;
     kd = 3*kp -4;
    // kd  =0;
        angle = (uint16)(servo_mid - kp*err - errDot*kd);
    }
    if(err > 0 )       //实际 中线 在右边  舵机往右打角
   {
        //kp= 2+speed_now*0.008;
        //kp1 = err*0.07+kp;
        //kd = kp1 *3 - 4;
//     if(fabs(err)<=5)
//       kp1 = 2.5;
//     else
    //kp1_r = 3.25/*+fabs(err)*0.002*/; //2.52
//     kp1 = 2.8;
     //kp = kp1_r;//+real_speed/4000.0f;
     //kp = 2.8;
     kd = 3*kp -4;
     //kd = 0;
        angle = (uint16)(servo_mid - kp*err - errDot*kd);
    }
    else if(err == 0 )
    {
        angle = servo_mid;   
    }
    if(angle < servo_right) angle = servo_right;
    if(angle > servo_left) angle = servo_left;

   ftm_pwm_duty(FTM2,FTM_CH1,angle);    //10000u  精度10000  
}