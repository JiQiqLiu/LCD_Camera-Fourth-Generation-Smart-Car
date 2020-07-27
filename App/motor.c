#include "include.h"

int16 speed = 0;
int16 real_speed = 0;
int16 speed_now = 60;
float dir, pwm_l_z = 0, pwm_r_z = 0, pwm_m = 0,pwm_l_f = 0 ,pwm_r_f = 0,pwm_r_last = 0,pwm_l_last = 0;  
int16 speed_l = 0,speed_r = 0,speed_ave = 0,l_motor = 0;
void motor_control()
{ //speed=speed1;
   static int16 error0_r = 0,error1_r= 0,error2_r = 0,
               error0_l = 0,error1_l= 0,error2_l = 0,
                 error0_m = 0,error1_m = 0,error2_m = 0;

  //float p = 15,i = 6.5,d=0,A,K;
//  float p = 69,i = 38,d=0,A,K;
    float p1 = 0,i1 = 0,d1=0,p2 = 0,i2 = 0,d2 = 0,A,K;
    float p_stop_l = 0,i_stop_l = 0,p_stop_r = 0,i_stop_r = 0;
    uint8 pid_stop = 0,l_dir = 0;
    uint32 time_motor = 0;
    pid_stop = 0;
     if(r>=0)  
     {
       l_dir = 1;
       l_motor = l;
     }
     else if(r<0) 
     {
       l_dir = 0;   
       l_motor = -l;
     }
     real_speed = (l_motor+r)/2;
     speed_ave = (l_motor+r)/2;
//      A = 0.002;
//      K = 0.5;
//    dir = (float)(speed*A*(K + err/2));
//    if ( dir > 40  ) dir =  40;
//    if ( dir < -40 ) dir = -40;
//    if(dir>0)           ///简单粗暴定差速
//    {
//       speed_l = speed+15;
//       speed_r = 150;
//    }
//    if(dir<0)
//    {
//       speed_r = speed+15;
//       speed_l = 150;
//    }
//  if(flag_circle != 0)
//  {
//    speed = 130;
//        //gpio_set   (PTA29,1);
//  }

    


  if(valid_line<=8)
  {
    //speed = 0;
  }
//  if((flag_stop == 1) && (time_stop >= 931500000))
//  {
//    //gpio_set   (PTA29,1);
//    time_stop = 0;
//    speed = 0;
//    p_stop = 4.8;
//    i_stop = 3.5;
//    pid_stop = 1;
//  }
  if(gpio_get(PTA25) == 0)
  {
    if(/*straight_flag_s == 1&& */flag_stop== 1&& time_stop >= 1242000000)
    {
      speed = 0;
//      p_stop_l = 5;
//      i_stop_l = 3.5;
//      p_stop_r = 1;
//      i_stop_r = 3.7;
      pid_stop = 1;
      pit_time_start  (PIT2);
    } 
    else flag_stop  = 0;
  }
  if(gpio_get(PTA24) == 0)
  {
    if(/*straight_flag_s == 1&& */flag_stop== 1&& time_stop >= 1305000000)
    {
      
//      p_stop_l = 5;
//      i_stop_l = 3.5;
//      p_stop_r = 1;
//      i_stop_r = 3.7;
            
      pid_stop = 1;
      pit_time_start  (PIT2);
    }
    else flag_stop  = 0;
  }
//  //speed_ave = (r+l)/2;
//  speed_r = speed_l = speed;
//  error0_r = speed_r - r ;
//  error0_l = speed_l - l ;
//  error2_r = error1_r;
//  error2_l = error1_l; 
  error0_m = speed - speed_ave; 
  error2_m = error1_m;
  i2 =3.7;    i1 = 3.5; 
  p2 = 5;    p1 = 9;
  i2=i2*2.8;
  i1=i1*2.8;
  time_motor = pit_time_get    (PIT2);
  if(time_motor>= 9000000000&&pid_stop == 1)
  {
//      pwm_l_z = 0;
//      pwm_r_z = 0;
      time_motor = 0;
      speed = 0;
      pid_stop = 0;
  }
//  if(speed == 0)
//  {
//    i2 = i_stop_r;
//    i1 = i_stop_l;
//    p2 = p_stop_r;
//    p1 = p_stop_l;
//    //DELAY_MS(800);
//  }
//  if(err>0) 
//  {
//     i2 = 0.7;
//     p2 = 1.7;
//  }
//  else if(err<0)
//  {
//     i1 = 0.7;
//     p1 = 1.7;
//  }
//  pwm_r_z+=i2*error0_r+p2*(error0_r - error1_r);
//  pwm_l_z+=i1*error0_l+p1*(error0_l - error1_l);
//  pwm_r_f-=(i2*error0_r+p2*(error0_r - error1_r))*0.4;
//  pwm_l_f-=(i1*error0_l+p1*(error0_l - error1_l))*0.4;
  //pwm_m+=i*error0_m+p*(error0_m - error1_m);
  pwm_r_z+=i2*error0_m+p2*(error0_m - error1_m);
  pwm_l_z+=i1*error0_m+p1*(error0_m - error1_m);
//  error1_r = error0_r;
//  error1_l = error0_l;
  error1_m = error0_m;
  
  /*if(err>0) 
  {
    pwm_l_z = pwm_l_z + 3*err;
    pwm_r_z = pwm_r_z - 3*err;
  }*/
 // if(err<0) 
  //{
    pwm_l_z = pwm_l_z - 0.1*(angle - servo_mid) + (pwm_l_z - pwm_l_last);
    pwm_r_z = pwm_r_z + 0.1*(angle - servo_mid) + (pwm_r_z - pwm_r_last);
 // }
    pwm_l_last = pwm_l_z;
    pwm_r_last = pwm_r_z;

  /************转角偏差调脉冲差速********************/
//  if(err>0)
//  {
//    pwm_r_z += ((abs(angle-servo_mid)*15)*0.3f+(fabs(err)*42.85)*0.05f)/2.0f;//内轮
//    pwm_l_z += ((abs(angle-servo_mid)*17)*0.7f+(fabs(err)*42.85)*0.3f)/2.0f;
//  }
//  if(err<0)
//  {
//    pwm_l_z += ((abs(angle-servo_mid)*15)*0.3f+(fabs(err)*42.85)*0.05f)/2.0f;//内轮
//    pwm_r_z += ((abs(angle-servo_mid)*17)*0.7f+(fabs(err)*42.85)*0.3f)/2.0f;
//  }
//  
//  if ( pwm_r_z > 8000 )        pwm_r_z = 8000;
//  if ( pwm_r_z <1)        pwm_r_z = 1;
//
//  if ( pwm_l_z > 8000 )        pwm_l_z = 8000;
//  if ( pwm_l_z < 1 )        pwm_l_z = 1;
  
  if ( pwm_r_z > 8000 )        pwm_r_z = 8000;
  if ( pwm_r_z <-8000)        pwm_r_z = -8000;

  if ( pwm_l_z > 8000 )        pwm_l_z = 8000;
  if ( pwm_l_z < -8000 )        pwm_l_z = -8000;
//  if ( pwm_r_f > 8000 )        pwm_r_f = 8000;
//  if ( pwm_r_f <0)        pwm_r_f = 0;
//
//  if ( pwm_l_f > 8000 )        pwm_l_f = 8000;
//  if ( pwm_l_f < 0 )        pwm_l_f = 0;
 //pwm_l_f = 4000;pwm_r_f = 4000;
//  if ( pwm_m > 8000 )        pwm_m = 8000;
//  if ( pwm_m < 1 )        pwm_m = 1;
  if(pwm_l_z >= 0)//正转
  {
    ftm_pwm_duty(FTM0, FTM_CH0, (int16)(pwm_l_z)); //左电机正
    ftm_pwm_duty(FTM0, FTM_CH1,/*4000*/0);//左电机反 
  }
  else if(pwm_l_z < 0)
  {
    ftm_pwm_duty(FTM0, FTM_CH0, 0); //左电机正
    ftm_pwm_duty(FTM0, FTM_CH1,(int16)(ABS(pwm_l_z)));//左电机反 
  }
  
  if(pwm_r_z >= 0)
  {
    ftm_pwm_duty(FTM0, FTM_CH2, (int16)(pwm_r_z)); //右电机正
    ftm_pwm_duty(FTM0, FTM_CH3,/*4000*/0);//右电机反  
  }
  else if(pwm_r_z < 0)
  {
    ftm_pwm_duty(FTM0, FTM_CH2, 0); //左电机正
    ftm_pwm_duty(FTM0, FTM_CH3,(int16)(ABS(pwm_r_z)));//左电机反 
  }
  
//  if(pwm_l>=0)
//  {
//    ftm_pwm_duty(FTM0, FTM_CH0, (int16)(pwm_l)); //左电机正
//    ftm_pwm_duty(FTM0, FTM_CH1,0);//左电机反 
//  }
//  else
//  {
//    ftm_pwm_duty(FTM0, FTM_CH0, 0); //左电机正
//    ftm_pwm_duty(FTM0, FTM_CH1,(int16)(ABS(pwm_l)));//左电机反 
//  }
//  if(pwm_r>=0)
//  {
//    ftm_pwm_duty(FTM0, FTM_CH2, (int16)(pwm_r)); //右电机正
//    ftm_pwm_duty(FTM0, FTM_CH3,0);//右电机反 
//  }  
//  else
//  {
//    ftm_pwm_duty(FTM0, FTM_CH2, 0); //右电机正
//    ftm_pwm_duty(FTM0, FTM_CH3,(int16)(ABS(pwm_r)));//右电机反 
//  }  
//  
  
 /*     
      
      pwm_l = pwm_r = pwm ;
      
      error1 = error0;
  
    
    ftm_pwm_duty(FTM0, FTM_CH0, 3000); //右电机反
    ftm_pwm_duty(FTM0, FTM_CH1, (int16)(pwm_r));//右电机正  
    ftm_pwm_duty(FTM0, FTM_CH2, (int16)(3000)); //左电机反
    ftm_pwm_duty(FTM0, FTM_CH3, pwm_l);//左电机正    */ 
  /*if ( pwm_r >= 0 )//电机正转
  {
    if ( pwm_r > 7000 )        pwm_r = 7000;
    ftm_pwm_duty(FTM0, FTM_CH0, 0); //右电机反
    ftm_pwm_duty(FTM0, FTM_CH1, (int16)(pwm_r));//右电机正   
  }
 else
  {
    if ( pwm_r < -7000 )        pwm_r = -7000;
    ftm_pwm_duty(FTM0, FTM_CH0, (int16)(-pwm_r)); //右电机反
    ftm_pwm_duty(FTM0, FTM_CH1, 0);//右电机正 
  }
  
  if ( pwm_l >= 0 )
  {
    if ( pwm_l > 10000 )        pwm_l = 10000;
    ftm_pwm_duty(FTM0, FTM_CH2, 0); //右电机反
    ftm_pwm_duty(FTM0, FTM_CH3, (int16)(pwm_l));//右电机正  
  }
  else
  {
    if ( pwm_l < -10000 )        pwm_l = -10000;
    ftm_pwm_duty(FTM0, FTM_CH2, (int16)(-pwm_l)); //右电机反
    ftm_pwm_duty(FTM0, FTM_CH3, 0);//右电机正      
  }*/
  
}
