/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2016,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：179029047
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		SEEKFREE_MT9V032.c
 * @brief      		总钻风(灰度摄像头)函数库
 * @company	   		成都逐飞科技有限公司
 * @author     		Go For It(1325536866)
 * @version    		v1.0
 * @Software 		IAR 7.2 or MDK 5.17
 * @Target core		MK60DN512VLL10
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2016-02-25
 * @note	
					MT9V032接线定义：
					------------------------------------ 
						模块管脚            单片机管脚
						SDA(51的RX)         C17
						SCL(51的TX)         C16
						场中断              C6
						像素中断            C18           
						数据口              C8-C15 
					------------------------------------ 
	
					默认分辨率是            188*120
					默认FPS                 50帧
 ********************************************************************************************************************/



#include "include.h"


uint8   image[ROW][COL];      //图像数组
uint8 img[ROW][COL];
void my_delay(long t)
{
    while(t--);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032摄像头初始化
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:		
//-------------------------------------------------------------------------------------------------------------------
void camera_init1(void)
{
    //设置参数    具体请参看使用手册
    uint16 light;
    //摄像头配置数组
	uint8 MT9V032_CFG[8];
	
	my_delay(9999999);			//延时以保证摄像头上面的51完成上电初始化
	
    uart_init(UART0, 115200);	//初始换串口 配置摄像头                     
    
    MT9V032_CFG[0] = 0xFF;     	//帧头         

    //命令位
	//具体请参看使用手册
	MT9V032_CFG[1] = 0x00;   
    
    //分辨率选择位   分辨率切换后，最好断电一次系统
	//其他分辨率具体看说明书，不过列超过188后K60无法采集，提供的更大的分辨率是给STM32具有摄像头接口的用户
	switch(ROW)
	{
		case 480:MT9V032_CFG[2] = 8;break;
		case 240:MT9V032_CFG[2] = 9;break;
		case 120:MT9V032_CFG[2] = 10;break;
		default :MT9V032_CFG[2] = 10;break;
	}
            
    
    //设置图像帧率        行数不同可设置的范围也不同		范围限制  120行的时候是1-200		240行的时候是1-132		480行的时候是1-66
    MT9V032_CFG[3] = 50;                    
    
    //曝光时间越大图像越亮     由于最大曝光时间受到fps与分辨率的共同影响，这里不给出具体范围，可以直接把曝光设置为1000，摄像头上的51收到之后会根据分辨率及FPS计算最大曝光时间，然后把
	//曝光时间设置为最大曝光时间，并且会返回当前设置的最大曝光时间，这样就可以知道最大可以设置的曝光时间是多少了，然后觉得图像过亮，就可以在这个最大曝光值的基础上相应减少。
    light = 600;
    MT9V032_CFG[4] = light>>8;          //曝光时间高八位
    MT9V032_CFG[5] = (uint8)light;      //曝光时间低八位
    
    //设置为0表示关闭自动曝光，设置1-63则启用自动曝光，设置的越大图像就越亮
    //如果环境光不均匀可以尝试开启此功能
    MT9V032_CFG[6] = 30;
    
    //帧尾  
    MT9V032_CFG[7] = 0x5A;                
    //通过串口发送配置参数
    uart_putbuff(UART0,MT9V032_CFG,8);
	
	//延时以保障上个配置数据51已经成功写入到摄像头
	my_delay(999999);
		
	
	//以下用于设置摄像头亮度   与上面的曝光时间是不一样的
    MT9V032_CFG[0] = 0xFF;     	//帧头
	MT9V032_CFG[1] = 0x02; 		//命令位  
    MT9V032_CFG[2] = 0; 		//无用，需设为0
    MT9V032_CFG[3] = 0;     	//无用，需设为0
    MT9V032_CFG[4] = 0;			//无用，需设为0
    MT9V032_CFG[5] = 32;      	//亮度等级选择            亮度等级 1 - 64
    MT9V032_CFG[6] = 0x35; 
    MT9V032_CFG[7] = 0x5A; 		//帧尾 
    uart_putbuff(UART0,MT9V032_CFG,8);


    //摄像头采集初始化
    DisableInterrupts;
    //DMA通道0初始化，PTE0触发源(默认上升沿)，源地址为C_IN_DATA(1)(PTC8-PTC15)，目的地址为：image，每次传输1Byte 传输完毕保持目的地址
    dma_portx2buff_init(DMA_CH0, (void *)&PTB_B0_IN, (void *)image, PTD4, DMA_BYTE1, COL, DADDR_KEEPON);
    port_init(PTD4, ALT1 | DMA_FALLING | PULLDOWN);  			//PCLK  触发源设置
	DMA_DIS(DMA_CH0);                                     		//禁用DMA通道
    DMA_IRQ_CLEAN(DMA_CH0);                               		//清除通道传输中断标志位
    DMA_IRQ_EN(DMA_CH0);                                  		//允许DMA通道中断
	DMA_EN(DMA_CH0);  											//使能DMA
	
	disable_irq(PORTD_IRQn);                             		//关闭PTC的中断
    //port_init(C7, ALT1 | IRQ_FALLING | PULLDOWN);      			//行中断
    port_init(PTD5, ALT1 | IRQ_FALLING | PULLDOWN);        		//场中断，下降沿触发中断、下拉
    set_irq_priority(PORTD_IRQn,1);                             // 中断优先级
    enable_irq (PORTD_IRQn);
	EnableInterrupts;
}


uint16  now_row = 0;		  //当前正在采集行数
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032摄像头场中断
//  @param      NULL
//  @return     void			
//  @since      v1.0
//  Sample usage:				在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
void VSYNC(void)
{
	now_row = 0;
	dma_repeat(DMA_CH0,(void *)&PTB_B0_IN,(void *)image[0],COL);
}

uint8   mt9v032_finish_flag = 0;      //一场图像采集完成标志位
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032摄像头DMA完成中断
//  @param      NULL
//  @return     void	
//  @since      v1.0
//  Sample usage:				在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
void row_finished(void)
{
	now_row++;
	if(now_row<ROW) dma_repeat(DMA_CH0,(void *)&PTB_B0_IN,(void *)image[now_row],COL);
	if(now_row >= ROW)	mt9v032_finish_flag = 1;
	
          
	//本例程对黑边不做处理，大家在使用数据的时候不使用image数组最左边与最右边即可
	//如果分辨率过大，就没办法转存了，因为K60的RAM会不够
    //一副图像从采集开始到采集结束耗时3.33MS左右(50FPS、188*120分辨率)
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      总钻风摄像头图像发送至上位机查看图像
//  @param      NULL
//  @return     void			
//  @since      v1.0
//  Sample usage:				调用该函数前请先初始化uart2
//-------------------------------------------------------------------------------------------------------------------
void seekfree_sendimg_032(void)
{
	uart_putchar(UART3,0x00);uart_putchar(UART3,0xff);uart_putchar(UART3,0x01);uart_putchar(UART3,0x01);//发送命令
    uart_putbuff(UART3, (uint8_t *)pixel, ROW*COL);  //发送图像
}

void seekfree_sendimg_032bin(void)
{
	int i;
    int16 bin_value = 0x7f; //二值化阀值
    
    uart_putchar(UART3,0x00);uart_putchar(UART3,0xff);uart_putchar(UART3,0x01);uart_putchar(UART3,0x01);//发送命令
    i = 0;
    while(i==ROW*COL)
    {
        if(*(uint8_t *)image[i] > bin_value)    uart_putchar(UART3,0XFF);
        else                                    uart_putchar(UART3,0X00);
        i++;
    }

    uart_putbuff(UART3, (uint8_t *)image, ROW*COL);  //发送图像
}

void SendImageData(uint16 * ImageData) 
{

    unsigned char i;
    
    uart_putchar(UART3,0x00);                   //命令
    uart_putchar(UART3,0xff);
    uart_putchar(UART3,0x01);
    uart_putchar(UART3,0x00);
    
    for(i=0; i<188; i++)                        //数据
    {
         uart_putchar(UART3,ImageData[i]>>8);   //发送高8位
         uart_putchar(UART3,ImageData[i]&0XFF); //发送高低8位
    }
    
}

void img_zip(uint8 *dst, uint8 *src, uint32 srclen)
{
    uint8 colour[2] = {1, 0};
    uint8 num, i, *last = src + 8*srclen - 1;
    while ( srclen-- )
    {
        num = 0;
        for ( i = 0; i < 8; i++ )
        {
          if(*src==50)
          {
            *src=1;
          }
            num += colour[*src];
            if ( src != last )
                src++;

            if ( i != 7 )
                num <<= 1;
        }
        *dst = num;
        if ( srclen != 0 )
            dst++;
    }
}