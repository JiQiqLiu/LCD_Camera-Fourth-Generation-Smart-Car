/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2016,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��179029047
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		SEEKFREE_MT9V032.h
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



#ifndef _MT9V032_h
#define _MT9V032_h

#include "include.h"



#define COL     188//ͼ����       ���������
#define ROW     120//ͼ��߶�		��ѡ������120 240 480   �ֱ����л�����öϵ�һ��ϵͳ
#define CAMERA (ROW * COL/8) 

extern uint8 mt9v032_finish_flag;  //һ��ͼ��ɼ���ɱ�־λ
extern uint8 image[ROW][COL];      //�ڲ�ʹ��
extern uint8 img[ROW][COL];      //�ڲ�ʹ��


void   VSYNC(void);
void   camera_init1(void);
void   seekfree_sendimg_032(void);
void   row_finished(void);
void   my_delay(long t);
void   seekfree_sendimg_032bin(void);
void SendImageData(uint16 * ImageData) ;
void img_zip(uint8 *dst, uint8 *src, uint32 srclen);

#endif
