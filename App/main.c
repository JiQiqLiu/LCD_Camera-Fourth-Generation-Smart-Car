/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,山外科技
 *     All rights reserved.
 *     技术讨论：山外论坛 http://www.vcan123.com
 *
 *     除注明出处外，以下所有内容版权均属山外科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留山外科技的版权声明。
 *
 * @file       main.c
 * @brief      山外K60 平台主程序
 * @author     山外科技
 * @version    v5.0
 * @date       2013-08-28
 */

#include "common.h"
#include "include.h"
uint8 k,j; 
uint8 num,time;
uint32 a;
vint16 r;
uint16 l;
vint16 r_old,r_old_1;
uint16 l_old,l_old_1;
uint8 dis_image[64][128];
uint8 uartbuff[CAMERA];
uint32 time_stop;
char buff[DATA_PACKET] = {16,3,~3,0,0,0,0,0,0,0,0,0,0,0,0,~3,3};
float * data = (float *)(((uint8 *)&buff) + 3);

//函数声明
void PORTD_IRQHandler();
void PORTE_IRQHandler();
void DMA0_IRQHandler();
void PIT0_IRQHandler();
void nrf_debug();
int16 zx_filter(int16 new_value,int16 old_value,int16 old_value_1);
int16 ave_filter(int16 value);

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外摄像头 LCD 测试实验
 */
void  main(void)
{

    my_delay(9999999);    //上电延时以便于摄像头初始化
    //uart_init (UART4, 256000);
    //while(!nrf_init());
    set_vector_handler(PORTE_VECTORn ,PORTE_IRQHandler);                //设置 PORTE 的中断服务函数为 PORTE_VECTORn
    set_irq_priority(PORTE_IRQn,10);  
    enable_irq(PORTE_IRQn);
    
    set_vector_handler(PORTD_VECTORn , PORTD_IRQHandler);   //设置 PORTA 的中断服务函数为 PORTA_IRQHandler  
    set_vector_handler(DMA0_VECTORn , DMA0_IRQHandler);     //设置 DMA0 的中断服务函数为 PORTA_IRQHandler
    
    camera_init1();
    
    OLED_Init(); 
    LCD_Init(); 
    led_init(LED2);
    
    ftm_pwm_init(FTM2, FTM_CH1,100, servo_mid);
    ftm_pwm_init(FTM0, FTM_CH0,10000, 0); //左正
    ftm_pwm_init(FTM0, FTM_CH1,10000, 0);//左反
    ftm_pwm_init(FTM0, FTM_CH2,10000, 0);//右正
    ftm_pwm_init(FTM0, FTM_CH3,10000, 0);//右反
    
    ftm_quad_init   (FTM1); 
    lptmr_pulse_init(LPT0_ALT1,0xFFFF,LPT_Rising);

    port_init(PTC8, IRQ_EITHER | PF |ALT1 | PULLUP);
    port_init(PTC9, IRQ_EITHER | PF |ALT1 | PULLUP);
    port_init(PTC10, IRQ_EITHER | PF |ALT1 | PULLUP);
    port_init(PTC11, IRQ_EITHER | PF |ALT1 | PULLUP);
    port_init(PTA24, IRQ_EITHER | PF |ALT1 | PULLUP);
    port_init(PTA25, IRQ_EITHER | PF |ALT1 | PULLUP);
    port_init(PTA26, IRQ_EITHER | PF |ALT1 | PULLUP);
    port_init(PTA27, IRQ_EITHER | PF |ALT1 | PULLUP);
    gpio_init (PTA29, GPO, 0);
    set_vector_handler(PIT0_VECTORn,PIT0_IRQHandler);
    set_irq_priority(PIT0_IRQn,5); 
    enable_irq(PIT0_IRQn); 
    pit_init_ms(PIT0,5); 
    pit_time_start  (PIT1);
    nrf_msg_init();
    while(1)
    {  
      time_stop = pit_time_get(PIT1);
//       if(gpio_get(PTA27) == 0)
//        {  
//          //nrf_debug((uint8)(pwm_r),(uint8)(pwm_l));
//          
//          nrf_debug();
// 
//         }
      if(mt9v032_finish_flag)
      {
         led_turn(LED2);
         mt9v032_finish_flag = 0; 
         Otsu();
         Deal_Image();
//         for(uint8 i=0;i<120;i++)
//         {
//          img[i][94]=0;
//         }
         servo_control();
           
         
         //buff[1] = (uint8)(pwm_r);
              
//         if(gpio_get(PTC8) == 0)
//         {     
//           img_zip((uint8 *)uartbuff,(uint8 *)img,CAMERA);
//           vcan_sendimg((uint8 *)uartbuff,CAMERA);
//         }
      }     
      
      if(gpio_get(PTA27) == 0)  //速度一档
      {
         /* if(straight_flag == 1)
          {
            speed = 175;        
          }
          else
          {
            speed = 150;
          } */
          kp = 3.25;
          //kp1_r = 3.25;
        speed = 170-ABS(err)*0.58; 
      }
      if(gpio_get(PTA26) == 0)  //速度二档
      {
          if(straight_flag == 1)
          {
            speed = 170;        
          }
          else
          {
            speed = 145;
          } 
          kp = 3.25;
          //kp1_r = 3.25;
      }      
       
      if(gpio_get(PTC8) == 0)
      {
        
//           if(straight_flag == 1)
//          {
//            speed = 180;        
//          }
//          else
//          {
//            speed = 155;
//          } 
          kp = 3.35;
          speed = 180 - ABS(err)*0.55;
      }
      if(gpio_get(PTC9) == 0) //图像中
      {
        
         for(uint8 j = 0;j<188;j++)
         {
            pixel[50][j] = 0;
            pixel[110][j] = 0;
         }
         for(num=0; num<59; num++)
         {
            memcpy(dis_image[num],&img[num*2][30],127);
         }
         dis_bmp(60,128,dis_image[0],k); 
       }
       if(gpio_get(PTC10) == 0) //图像左
       {
        
         for(uint8 j = 0;j<188;j++)
         {
            pixel[50][j] = 0;
         }
         for(num=0; num<59; num++)
         {
            memcpy(dis_image[num],&img[num*2][0],127);
         }
         dis_bmp(60,128,dis_image[0],k); 
       }
       if(gpio_get(PTC11) == 0)  //图像右
       {
        
         for(uint8 j = 0;j<188;j++)
         {
            pixel[50][j] = 0;
         }
         for(num=0; num<59; num++)
         {
            memcpy(dis_image[num],&img[num*2][60],127);
         }
         dis_bmp(60,128,dis_image[0],k); 
       }

    }
}

void nrf_debug()
{
    data[0] = (float)l;
    data[1] = (float)r;
    data[2] = (float)speed;
     // data[2] = (float)r;
    nrf_tx(buff,DATA_PACKET) ;
    while(nrf_tx_state() == NRF_TXING);         //等待发送完成
    
}
int16 zx_filter(int16 new_value,int16 old_value,int16 old_value_1)
{
  float a = 0.2f;
  return a*(2*old_value-old_value_1) + (1- a)*new_value;
}
int16 ave_filter(int16 value)
{
    uint8 ave = 0;
    int32 GATE_LAST = 0;            //上次阈值
    uint8 GATE_LAST_S = 0;
    const uint8 GATE_LAST_N= 5; 
     if(ave >= GATE_LAST_N+1)
     {
        ave=0;
        GATE_LAST_S= GATE_LAST/GATE_LAST_N;
        GATE_LAST = 0;

     }
     else if(ave < GATE_LAST_N+1)
     {
        GATE_LAST += value;
     }
     ave++;
     return GATE_LAST_S;
}
/*!
 *  @brief      PORTA中断服务函数
 *  @since      v5.0
 */
void PORTD_IRQHandler()
{
    uint8  n;    //引脚号

    n = 5;
    if(PORTD_ISFR & (1 << n))            
    {
        PORTD_ISFR  = (1 << n); 
        VSYNC(); 
    }
}

/*!
 *  @brief      DMA0中断服务函数
 *  @since      v5.0
 */
void DMA0_IRQHandler()
{
  DMA_IRQ_CLEAN(DMA_CH0);
  row_finished();
 
}


/*!
 *  @brief      PIT0中断服务函数
 *  @since      v5.0
 */
void PIT0_IRQHandler()
{
    r_old_1 = r_old;
    l_old_1 = l_old;
    r = ftm_quad_get    (FTM1);
    ftm_quad_clean  (FTM1); 

    l = lptmr_pulse_get();
    lptmr_pulse_clean ();
    
    r = zx_filter( r, r_old,r_old_1);
    l = zx_filter( l, l_old,l_old_1);
    r_old = r;
    l_old = l;
    //flag_stop = 0;
    motor_control();  
    PIT_Flag_Clear(PIT0);
}

void PORTE_IRQHandler()
{
    uint8  n;    //引脚号
    uint32 flag;

    flag = PORTE_ISFR;
    PORTE_ISFR  = ~0;                                   //清中断标志位

    n = 27;
    if(flag & (1 << n))                                 //PTE27触发中断
    {
        nrf_handler();
    }
}