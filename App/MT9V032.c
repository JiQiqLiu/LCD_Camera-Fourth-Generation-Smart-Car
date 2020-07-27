/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2016,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��179029047
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		SEEKFREE_MT9V032.c
 * @brief      		�����(�Ҷ�����ͷ)������
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		Go For It(1325536866)
 * @version    		v1.0
 * @Software 		IAR 7.2 or MDK 5.17
 * @Target core		MK60DN512VLL10
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2016-02-25
 * @note	
					MT9V032���߶��壺
					------------------------------------ 
						ģ��ܽ�            ��Ƭ���ܽ�
						SDA(51��RX)         C17
						SCL(51��TX)         C16
						���ж�              C6
						�����ж�            C18           
						���ݿ�              C8-C15 
					------------------------------------ 
	
					Ĭ�Ϸֱ�����            188*120
					Ĭ��FPS                 50֡
 ********************************************************************************************************************/



#include "include.h"


uint8   image[ROW][COL];      //ͼ������
uint8 img[ROW][COL];
void my_delay(long t)
{
    while(t--);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032����ͷ��ʼ��
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:		
//-------------------------------------------------------------------------------------------------------------------
void camera_init1(void)
{
    //���ò���    ������ο�ʹ���ֲ�
    uint16 light;
    //����ͷ��������
	uint8 MT9V032_CFG[8];
	
	my_delay(9999999);			//��ʱ�Ա�֤����ͷ�����51����ϵ��ʼ��
	
    uart_init(UART0, 115200);	//��ʼ������ ��������ͷ                     
    
    MT9V032_CFG[0] = 0xFF;     	//֡ͷ         

    //����λ
	//������ο�ʹ���ֲ�
	MT9V032_CFG[1] = 0x00;   
    
    //�ֱ���ѡ��λ   �ֱ����л�����öϵ�һ��ϵͳ
	//�����ֱ��ʾ��忴˵���飬�����г���188��K60�޷��ɼ����ṩ�ĸ���ķֱ����Ǹ�STM32��������ͷ�ӿڵ��û�
	switch(ROW)
	{
		case 480:MT9V032_CFG[2] = 8;break;
		case 240:MT9V032_CFG[2] = 9;break;
		case 120:MT9V032_CFG[2] = 10;break;
		default :MT9V032_CFG[2] = 10;break;
	}
            
    
    //����ͼ��֡��        ������ͬ�����õķ�ΧҲ��ͬ		��Χ����  120�е�ʱ����1-200		240�е�ʱ����1-132		480�е�ʱ����1-66
    MT9V032_CFG[3] = 50;                    
    
    //�ع�ʱ��Խ��ͼ��Խ��     ��������ع�ʱ���ܵ�fps��ֱ��ʵĹ�ͬӰ�죬���ﲻ�������巶Χ������ֱ�Ӱ��ع�����Ϊ1000������ͷ�ϵ�51�յ�֮�����ݷֱ��ʼ�FPS��������ع�ʱ�䣬Ȼ���
	//�ع�ʱ������Ϊ����ع�ʱ�䣬���һ᷵�ص�ǰ���õ�����ع�ʱ�䣬�����Ϳ���֪�����������õ��ع�ʱ���Ƕ����ˣ�Ȼ�����ͼ��������Ϳ������������ع�ֵ�Ļ�������Ӧ���١�
    light = 600;
    MT9V032_CFG[4] = light>>8;          //�ع�ʱ��߰�λ
    MT9V032_CFG[5] = (uint8)light;      //�ع�ʱ��Ͱ�λ
    
    //����Ϊ0��ʾ�ر��Զ��ع⣬����1-63�������Զ��ع⣬���õ�Խ��ͼ���Խ��
    //��������ⲻ���ȿ��Գ��Կ����˹���
    MT9V032_CFG[6] = 30;
    
    //֡β  
    MT9V032_CFG[7] = 0x5A;                
    //ͨ�����ڷ������ò���
    uart_putbuff(UART0,MT9V032_CFG,8);
	
	//��ʱ�Ա����ϸ���������51�Ѿ��ɹ�д�뵽����ͷ
	my_delay(999999);
		
	
	//����������������ͷ����   ��������ع�ʱ���ǲ�һ����
    MT9V032_CFG[0] = 0xFF;     	//֡ͷ
	MT9V032_CFG[1] = 0x02; 		//����λ  
    MT9V032_CFG[2] = 0; 		//���ã�����Ϊ0
    MT9V032_CFG[3] = 0;     	//���ã�����Ϊ0
    MT9V032_CFG[4] = 0;			//���ã�����Ϊ0
    MT9V032_CFG[5] = 32;      	//���ȵȼ�ѡ��            ���ȵȼ� 1 - 64
    MT9V032_CFG[6] = 0x35; 
    MT9V032_CFG[7] = 0x5A; 		//֡β 
    uart_putbuff(UART0,MT9V032_CFG,8);


    //����ͷ�ɼ���ʼ��
    DisableInterrupts;
    //DMAͨ��0��ʼ����PTE0����Դ(Ĭ��������)��Դ��ַΪC_IN_DATA(1)(PTC8-PTC15)��Ŀ�ĵ�ַΪ��image��ÿ�δ���1Byte ������ϱ���Ŀ�ĵ�ַ
    dma_portx2buff_init(DMA_CH0, (void *)&PTB_B0_IN, (void *)image, PTD4, DMA_BYTE1, COL, DADDR_KEEPON);
    port_init(PTD4, ALT1 | DMA_FALLING | PULLDOWN);  			//PCLK  ����Դ����
	DMA_DIS(DMA_CH0);                                     		//����DMAͨ��
    DMA_IRQ_CLEAN(DMA_CH0);                               		//���ͨ�������жϱ�־λ
    DMA_IRQ_EN(DMA_CH0);                                  		//����DMAͨ���ж�
	DMA_EN(DMA_CH0);  											//ʹ��DMA
	
	disable_irq(PORTD_IRQn);                             		//�ر�PTC���ж�
    //port_init(C7, ALT1 | IRQ_FALLING | PULLDOWN);      			//���ж�
    port_init(PTD5, ALT1 | IRQ_FALLING | PULLDOWN);        		//���жϣ��½��ش����жϡ�����
    set_irq_priority(PORTD_IRQn,1);                             // �ж����ȼ�
    enable_irq (PORTD_IRQn);
	EnableInterrupts;
}


uint16  now_row = 0;		  //��ǰ���ڲɼ�����
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032����ͷ���ж�
//  @param      NULL
//  @return     void			
//  @since      v1.0
//  Sample usage:				��isr.c�����ȴ�����Ӧ���жϺ�����Ȼ����øú���(֮�����������жϱ�־λ)
//-------------------------------------------------------------------------------------------------------------------
void VSYNC(void)
{
	now_row = 0;
	dma_repeat(DMA_CH0,(void *)&PTB_B0_IN,(void *)image[0],COL);
}

uint8   mt9v032_finish_flag = 0;      //һ��ͼ��ɼ���ɱ�־λ
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032����ͷDMA����ж�
//  @param      NULL
//  @return     void	
//  @since      v1.0
//  Sample usage:				��isr.c�����ȴ�����Ӧ���жϺ�����Ȼ����øú���(֮�����������жϱ�־λ)
//-------------------------------------------------------------------------------------------------------------------
void row_finished(void)
{
	now_row++;
	if(now_row<ROW) dma_repeat(DMA_CH0,(void *)&PTB_B0_IN,(void *)image[now_row],COL);
	if(now_row >= ROW)	mt9v032_finish_flag = 1;
	
          
	//�����̶Ժڱ߲������������ʹ�����ݵ�ʱ��ʹ��image��������������ұ߼���
	//����ֱ��ʹ��󣬾�û�취ת���ˣ���ΪK60��RAM�᲻��
    //һ��ͼ��Ӳɼ���ʼ���ɼ�������ʱ3.33MS����(50FPS��188*120�ֱ���)
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ���������ͷͼ��������λ���鿴ͼ��
//  @param      NULL
//  @return     void			
//  @since      v1.0
//  Sample usage:				���øú���ǰ���ȳ�ʼ��uart2
//-------------------------------------------------------------------------------------------------------------------
void seekfree_sendimg_032(void)
{
	uart_putchar(UART3,0x00);uart_putchar(UART3,0xff);uart_putchar(UART3,0x01);uart_putchar(UART3,0x01);//��������
    uart_putbuff(UART3, (uint8_t *)pixel, ROW*COL);  //����ͼ��
}

void seekfree_sendimg_032bin(void)
{
	int i;
    int16 bin_value = 0x7f; //��ֵ����ֵ
    
    uart_putchar(UART3,0x00);uart_putchar(UART3,0xff);uart_putchar(UART3,0x01);uart_putchar(UART3,0x01);//��������
    i = 0;
    while(i==ROW*COL)
    {
        if(*(uint8_t *)image[i] > bin_value)    uart_putchar(UART3,0XFF);
        else                                    uart_putchar(UART3,0X00);
        i++;
    }

    uart_putbuff(UART3, (uint8_t *)image, ROW*COL);  //����ͼ��
}

void SendImageData(uint16 * ImageData) 
{

    unsigned char i;
    
    uart_putchar(UART3,0x00);                   //����
    uart_putchar(UART3,0xff);
    uart_putchar(UART3,0x01);
    uart_putchar(UART3,0x00);
    
    for(i=0; i<188; i++)                        //����
    {
         uart_putchar(UART3,ImageData[i]>>8);   //���͸�8λ
         uart_putchar(UART3,ImageData[i]&0XFF); //���͸ߵ�8λ
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