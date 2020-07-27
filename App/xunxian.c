#include "include.h"

#define	IMG_BLACK		0
#define	IMG_WHITE		255
#define FIND_CENTER		0
#define FIND_LEFT		1
#define FIND_RIGHT		2


uint8 pixel[120][188];  //二值化数组
uint16 pixelcount[256] = {0}; 

/*****寻线*****/
int16 midline[120] = {0}; //中线
int16 old_leftline = 40,old_rightline = 140;//上次左线上次右线
int16 mid1 = 94;
int16 misline[4] ={0};//中线处理行
int16 border_left[120] = {0};//左线边界
int16 border_right[120] = {0};//右线边界
uint8 track_width[120] = {0};//赛道宽度
uint8 leftmissing_flag[120] = {0},rightmissing_flag[120] = {0};//左线右线丢线标志
/*****十字*****/
uint8 shizi = 0;
/*****最小二乘法*****/
int createPoint(int type, int line);//最小二乘法
//阈值处理
uint8 otsu_threshold()
{
  uint8 T = 0;
  uint8 height = 120,width = 188;
  uint16 N0 = 0;
  uint16 N1 = 0;
  uint16 u0 = 0;
  uint16 u1 = 0;
  float w0 = 0;
  float w1 = 0;
  uint16 g = 0,MAX = 0;
  uint16 N = height*width;
  uint32 gsum0 = 0,gsum1 = 0;
  //static uint8 old_T = 100;
  
  for(int16 i=0;i<256;i++)
  {
    pixelcount[i] = 0;
  }
  
  for(uint8 i =0;i<height;i++)
  {
    for(uint8 j = 0;j<width;j++)
    {
      pixelcount[(uint16)image[i][j]]++;
    }
  }
  
  for(int16 t = 50;t<=200;t++)
  {
    gsum0 = 0;
    gsum1 = 0;
    N0 = 0;
    N1 = 0;

    for(int16 i = 0;i<=t;i++)
    {
      gsum0 += pixelcount[i]*i;
      N0 +=  pixelcount[i];
    }
    u0 = (uint16)(gsum0/N0);
    w0 = (float)N0/N;
    for(int16 k = t+1;k<256;k++)
    {
      gsum1 += pixelcount[k]*k;
      N1 +=  pixelcount[k];
    }
    u1 = (uint16)(gsum1/N1);
    w1 = (float)N1/N;
    
    g = (uint16)(w0*w1*(u1 - u0)*(u1 - u0));
    //OLED_Print_Num(35, 6, u0);   OLED_Print_Num(75, 6, u1);
    if(g>MAX)
    {
      MAX = g;
      T = t;
    }
  }
  
//  if((T<=10)||abs(T - old_T)>6)
//    T = old_T;
  //old_T = T;   
  //LCD_PrintU16(5,7,T);
  return T;
}

//寻线
void xunxain(void)
{
  uint8 black_line =100; //寻线起始行
  int16 slope_left[120] = {0},slope_right[120] = {0}; //斜率左、斜率右
  int16 left_low = 0,left_high = 0,right_low = 0,right_high  = 0;//左线起始行、结束行
                                                          //右线起始行、结束行
  //阈值
  k = otsu_threshold();
  //二值化
  for(uint8 i = 0;i<120;i++)
  {
    for(uint8 j = 0;j<188;j++)
    {
      if(image[i][j]<k)
        pixel[i][j] = 0;
      else pixel[i][j] = 255;
    }
  }
  
  for(uint8 i = 0;i<120;i++)
  {
      leftmissing_flag[i] = 0;
      rightmissing_flag[i] = 0;
  }
  //前十行搜线
  for(uint8 i = black_line;i>black_line - 10;i--) 
  {   
      //从上一次中线向左边搜线   
        for(int16 j = midline[i];j>2;j--)
        {
          if((!(((pixel[i][j-3]+pixel[i][j-2]+pixel[i][j-1]) == 0)&&((pixel[i][j]+pixel[i][j+1]+pixel[i][j+2]+pixel[i][j+3])==255*4)))&&(j < 5)) 
          {      
            border_left[i] = 0;
            break;  
          }
          else if(((pixel[i][j-3]+pixel[i][j-2]+pixel[i][j-1]) == 0)&&((pixel[i][j]+pixel[i][j+1]+pixel[i][j+2]+pixel[i][j+3])==255*4)&&(j>2)&&(j<185))
          { 
            border_left[i] = j;
            break;
          }
          
        }
   
        //从上一次中线向右边搜线
        for(int16 j = midline[i];j<188;j++)
        {
          if(!(((pixel[i][j-3]+pixel[i][j-2]+pixel[i][j-1]+pixel[i][j]) == 255*4)&&((pixel[i][j+1]+pixel[i][j+2]+pixel[i][j+3])==0))&&(j>183)) 
          {
            border_right[i] = 187;  
            break;
          }
          else if(((pixel[i][j-3]+pixel[i][j-2]+pixel[i][j-1]+pixel[i][j]) == 255*4)&&((pixel[i][j+1]+pixel[i][j+2]+pixel[i][j+3])==0)&&(j>2)&&(j<185))
          {
            border_right[i] = j; 
            break;
          } 
        }
        
        slope_left[i] = border_left[i] - border_left[i+1];
        slope_right[i] = border_right[i] - border_right[i+1];
        track_width[i] = border_right[i] - border_left[i];
        midline[i] = (border_right[i]+border_left[i])/2;
        if(midline[i]<0) midline[i] = 0;
        if(midline[i]>187) midline[i] = 187;
        
        pixel[i][midline[i]] = 0;
        pixel[i][border_left[i] + 5] = 0;
        pixel[i][border_right[i] - 5] = 0;  
  }
  //第十行后从上次黑线开始搜线 
  for(uint8 i = black_line - 10 ;i>20;i--)
  {
      //从上一次左线向左边搜线
      left_low = border_left[i+1] + slope_left[i+1] - 10;
      left_high = border_left[i+1] + slope_left[i+1] + 10;
      if(left_low<3) left_low = 3;
      if(left_high>173) left_high = 173;
      for(int16 j = left_high;j>left_low;j--)
      {
        if((pixel[i][j-2]+pixel[i][j-1]+pixel[i][j]+pixel[i][j+1]+pixel[i][j+2]+pixel[i][j+3]) == 0)
        {
          border_left[i] = border_left[i+1];
          leftmissing_flag[i] = 0;
          break;
        }
        else if((!(((pixel[i][j-3]+pixel[i][j-2]+pixel[i][j-1]) == 0)&&((pixel[i][j]+pixel[i][j+1]+pixel[i][j+2]+pixel[i][j+3])==255*4)))&&(j<=left_low+1)) 
        {     
          leftmissing_flag[i] = 1;
          break;
        }
        else if(((pixel[i][j-3]+pixel[i][j-2]+pixel[i][j-1]) == 0)&&((pixel[i][j]+pixel[i][j+1]+pixel[i][j+2]+pixel[i][j+3])==255*4)&&(j>2)&&(j<185))
        {       
          border_left[i] = j;
          leftmissing_flag[i] = 0;
          break;
        }       
      }
      //从上一次右线向右边搜线
      right_low = border_right[i+1] + slope_right[i+1] - 10;
      right_high = border_right[i+1] + slope_right[i+1] + 10;
      if(right_low<10) right_low = 10;
      if(right_high>184) right_high = 184;
      for(int16 j = right_low;j<right_high;j++)
      {       
          if((pixel[i][j-2]+pixel[i][j-1]+pixel[i][j]+pixel[i][j+1]+pixel[i][j+2]+pixel[i][j+3]) == 0)
          {
            border_right[i] = border_right[i+1];
            rightmissing_flag[i] = 0;
            break;
          }
          else if((!(((pixel[i][j-3]+pixel[i][j-2]+pixel[i][j-1]+pixel[i][j]) == 255*4)
                     &&((pixel[i][j+1]+pixel[i][j+2]+pixel[i][j+3])==0)))&&(j >= right_high-1)) 
          {           
            rightmissing_flag[i] = 1;   
            break;
          }
          else if(((pixel[i][j-3]+pixel[i][j-2]+pixel[i][j-1]+pixel[i][j]) == 255*4)
                  &&((pixel[i][j+1]+pixel[i][j+2]+pixel[i][j+3])==0)&&(j>2)&&(j<185))
          {
            rightmissing_flag[i] = 0;
            border_right[i] = j;
            break;
          } 
       }

      if(abs(border_left[i] - border_left[i+1])>10) 
        border_left[i] = border_left[i+1];
      if((abs(border_right[i] - border_right[i+1])>10)) 
        border_right[i] = border_right[i+1];
      
      /***判断是否右线为垂直小车前瞻***/
      /***弯道特殊补线***/
      //右线丢失
      if((rightmissing_flag[i] == 1)&&(leftmissing_flag[i] == 0))  
      {
         border_right[i] = border_left[i]+track_width[i+1];
         if((abs(border_right[i] - border_right[i + 1])>10)) 
           border_right[i] = border_right[i + 1];
         if(border_right[i]>=240)
           border_right[i] = 240;
      }
      //左线丢失
      else if((leftmissing_flag[i] == 1)&&(rightmissing_flag[i] == 0))
      {
        border_left[i] = border_right[i]-track_width[i+1];
        if((abs(border_right[i] - border_right[i + 1])>10)) 
          border_right[i] = border_right[i + 1];
        if(border_left[i]<=-30)
             border_left[i] = -30;           
      }    
      
      else if((leftmissing_flag[i] == 1)&&(rightmissing_flag[i] == 1))
      {
          border_left[i] = 0;
          border_right[i] = 187;
          shizi++;
      }
      else if((leftmissing_flag[i] == 0)&&(rightmissing_flag[i] == 0))
      {

      }
      slope_left[i] = border_left[i] - border_left[i+1];
      slope_right[i] = border_right[i] - border_right[i+1];
      track_width[i] = border_right[i] - border_left[i];
      midline[i] = (border_right[i]+border_left[i])/2;     
   
      //判断中线是否可靠      
      if(midline[i]<0) midline[i] = 0;
      if(midline[i]>187) midline[i] = 187;
       
      pixel[i][border_right[i] - 5] = 0;
      pixel[i][border_left[i] + 5] = 0;  
      pixel[i][midline[i]] = 0; 
 
  }
        
      misline[0] = (midline[60]+midline[61]+midline[62])/3;
      misline[1] = (midline[53]+midline[54]+midline[55])/3;
      misline[2] = (midline[36]+midline[37]+midline[38])/3;
      misline[3] = (int16)(misline[0]*0.5+misline[1]*0.3+misline[2]*0.2);
      //动态小前瞻
      //赛道类型判断
}

//利用最小二乘法生成需要补全的点 
int createPoint(int type,int line)
{
	int16 *linePointer;
	int8 tmp = 0;
	double sumX = 0;
	double sumY = 0;
	double averageX = 0;
	double averageY = 0;
	double sumUp = 0;
	double sumDown = 0;
	double parameterA;
	double parameterB;
	
	if(type == FIND_LEFT)
		linePointer = &border_left[line];
	else if(type == FIND_RIGHT)
		linePointer = &border_right[line];
	else
		linePointer = &midline[line];  
	
	// 取邻近的 POINT_COUNT 个点进行拟合
	while(++tmp <= 10)//POINT_COUNT
	{
		sumX += (line+tmp);
		sumY += linePointer[tmp];
	}
	--tmp;
	averageX = sumX/tmp;
	averageY = sumY/tmp;
	do
	{
		sumUp += (linePointer[tmp]-averageY) * (line+tmp-averageX);
		sumDown += (line+tmp-averageX) * (line+tmp-averageX);
	} while(--tmp > 0);
	
	if(sumDown == 0)
		parameterB = 0;
	else
		parameterB = sumUp/sumDown;
	parameterA = averageY-parameterB*averageX;
	return (int16)(parameterA+parameterB*line);
}
//十字or圆环
void inflection()
{
  int8 x_left[5] = {0},x_right[5] = {0};
  int8 a =0 ,b = 0,c = 0;
  uint8 mid_white = 0;
  uint8 shizi_flag_1 = 0,shizi_flag_2 = 0;
  
  for(uint8 i = 100;i>30;i--)
  {
    if((pixel[i+1][midline[70]] == 255)&&(pixel[i][midline[70]] == 255)&&
       (pixel[i-1][midline[70]] == 255))
      mid_white ++ ;
  }
  //找拐点
  for(int16 i = 90;i>20;i--)
  {
    if(border_left[i]<0)
      a = 0;
    else 
      a = border_left[i];
    if(border_left[i+1]<0)
      b = 0;
    else
      b = border_left[i+1];
    if(border_left[i-1]<0)
      c = 0;
    else
      c = border_left[i-1];
    x_left[0] = a - b;
    x_left[1] = c - a;
    x_left[2] = x_left[0]*x_left[1];
    x_right[0] = border_right[i] - border_right[i+1];
    x_right[1] = border_right[i-1] - border_right[i];
    if(x_left[1]<-10&&x_left[2]<0)
    {
      led_turn(LED0);
    }

    if(gpio_get(PTA24) == 0)
    {
      OLED_Print_Num1(1, 3, x_left[0]);
      OLED_Print_Num1(1, 6, x_left[1]);
    }
    if(gpio_get(PTA25) == 0)
    {
      OLED_Print_Num1(45, 3, x_right[0]);
      OLED_Print_Num1(45, 6, x_right[1]);
    }
  }
   //十字
  if((shizi == 3)&&(mid_white == 60))
  {
     shizi_flag_1 = 1;
     mid_white = 0;
  }
}

