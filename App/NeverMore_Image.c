#include "common.h"
#include "include.h"

/* VARIBEL */
int16 left[41];     
int16 right[41];
int16 width[41]; 
int16 mid[41];   

int16 valid_line;

int16 cnt_all_white;
int16 inflection_l;
int16 inflection_r;

// 丢线相关变量(一定要初始化)
int16 lost_l[41];
int16 cnt_lostline_l[41];
int16 cnt_lost_l;
int16 flag_first_lost_l, flag_first_lost_r;
int16 lost_r[41];
int16 cnt_lost_r;

int16 cnt_lostline_r[41];
int16 cnt_lost_cnt_narrowr;
int16 cnt_narrow[41];

// 记录丢线起始位置
int16 lost_start_l[10];
int16 lost_end_l[10];
int16 lost_start_r[10];
int16 lost_end_r[10];
int16 cnt_lostarea_l;
int16 cnt_lostarea_r;
int16 c_lost_l;
int16 c_lost_r;
// 赛道类型
int16 flag_cross;   // 十字
int16 flag_stop = 0;   // 障碍
int16 flag_circle;  // 圆环
int16 flag_circle_start;  // 圆环
int16 flag_line;    // 直线
uint8 straight_flag_s = 0,straight_flag= 0;
/* FUNCTIONS */

int16 B_TO_W1 = 0,W_TO_B1 = 0 ,B_TO_W2 = 0,W_TO_B2 = 0;
int16 start_flag = 0;

void Otsu(void);

void Deal_Image(void);

int16 Get_Edge_Line(int16 op);
int16 Search_Line(int16 line, int16 mid);

void start_line();//起跑线处理
void Get_Inflection(void);
void Deal_Cross(void);
void Deal_Cirque(void);
void Get_Up_Inflection(int16 start, int16 end, int16 * l, int16 * r);
void Deal_BlockOrStart(void);
int8 Is_LeftBlock(int16 line);
int8 Is_RightBlock(int16 line);
void Deal_BlockOrStart1(void);//
void Updata_Line_Inf(void);
void Correct_Width(void);
void Calculate_Midline(void);
void Path_Optimization(int16 i);
void Get_Dir(void);


void Least_Squares(int16 end, int16 * y, float * k, float * b);
void zx_type(/*int16 *Line*/);
void Debug_Image(void);
/* FUNCTIONS */

/* to binaryzation the image */
void Otsu(void)
{
    uint8 value, threshold;
    int16 total, i, j;
    int16 hisData[256];
    memset(hisData, 0, sizeof(hisData));
    // Calculate histogram
    for ( i = 0; i < ROW; i ++ )
    {
        for ( j = 0; j < COL; j ++ )
        {
            value = image[i][j];
            hisData[value] ++;
        }
    }
    total = ROW * COL;
    float sum = 0;
    for ( i = 0; i < 256; i ++ )    sum += (float)(i*hisData[i]);
    float sumB = 0;
    int32 wB = 0, wF = 0;
    float varMax = 0;
    threshold = 110;
    for ( i = 0; i < 255; i ++ )
    {
        wB += hisData[i];
        if ( wB == 0 )  continue;
        wF = total - wB;
        if ( wF == 0 )  break;
        
        sumB += (float)(i*hisData[i]);
        
        float mB = sumB*1.0f / wB;
        float mF = (sum - sumB)*1.0f / wF;
        
        float varBetween = (float)wB*(float)wF*(mB-mF)*(mB-mF);
        if ( varBetween > varMax )
        {
            threshold = i;
            varMax = varBetween;
        }
    }
    for ( i = 0; i < ROW; i ++ )
    {
        for ( j = 0; j < COL; j ++ )
        {
            if ( image[i][j] > (uint8)threshold )   img[i][j] = 1;
            else                           img[i][j] = 0;
        }
    }
}

void Deal_Image()
{
    /* initial */
    memset(lost_l, 0, sizeof(lost_l));
    memset(lost_r, 0, sizeof(lost_r));
    cnt_lost_l = cnt_lost_r = 0;
    cnt_lostline_r[0] = cnt_lostline_l[0] = cnt_narrow[0] = 0;
    flag_first_lost_l = flag_first_lost_r = 1;
    flag_cross = 0;
    flag_circle = 0;
    flag_line = 0;
    //flag_stop = 0;
    cnt_all_white = 0;
    /* initial */
    
    if ( Get_Edge_Line(0) == 0 )    return;

    Get_Inflection();
    Deal_Cross();
    Deal_BlockOrStart1();
    Updata_Line_Inf();
    Correct_Width();
    Calculate_Midline();
    //Path_Optimization(2);
    zx_type(/*int16 *Line*/);
    start_line();//起跑线处理
    Get_Dir();  
    //Debug_Image();  // for image debug
}



/*
 * Get Image Basis Information
 * If op: 1
 *      first priority --> COL/2 ; then last mid;
 * If op: 0
 *      first priority --> last mid ; ( then last right ; then last left )
 * return 0 :  fail
 * return 1 :  success
*/
int16 Get_Edge_Line(int16 op)
{
    int16 first, row, mid, num, tmp;
    valid_line = 0;
    first      = 0;
    row        = START;
    mid        = COL / 2;
    num        = VALID_LINE;
    
    while ( row > 0 && num -- )
    {
        /* Binary search the first line */
        if ( first == 0 )
        {
            if ( img[row][COL/2] == 1 )
            {
                mid = Search_Line(valid_line, COL/2);
                first = 1;
            }
            else
            {
                if ( img[row][COL/4] == 1 )
                {
                    mid = Search_Line(valid_line, COL/4);
                    first = 1;
                }
                else if ( img[row][COL*3/4] == 1 )
                {
                    mid = Search_Line(valid_line, COL*3/4);
                    first = 1;                   
                }
                else    return 0;
            }
        }
        else
        {
            if ( op == 1 )  
            {
                tmp = Search_Line(valid_line, COL/2);
                if ( tmp == -1 )    tmp = Search_Line(valid_line, mid);
                mid = tmp;
            }
            else
            {
                tmp = Search_Line(valid_line, mid);
                if ( tmp == -1 ) tmp = Search_Line(valid_line, right[valid_line-1]-2);
                if ( tmp == -1 ) tmp = Search_Line(valid_line, left[valid_line-1]+2);
                mid = tmp;
            }       
            if ( mid == -1 )    return 1;
        }
        valid_line ++;
        row = START - valid_line*STEP;
    }
    return 1;
}

/*
 * The basic function to get the line information
 *
 * We should be attention to the information we updata there
 * If we want to change the left right point, we must updata
 * these information by myself!
*/
int16 Search_Line(int16 line, int16 mid)
{
    int16 l, r, fl, fr;
    int16 row = START - line*STEP;
    
    //if the pix is black return false!
    if ( img[row][mid] == 0 )   return -1;
    //initial
    l = r = mid;
    fl = fr = 0;
    
    while ( !fl || ! fr )
    {
        if ( l == BORDER_L )    fl = 1;
        if ( r == BORDER_R )    fr = 1;
        
        if ( !fl )
        {
            if ( img[row][l] == 1 && img[row][l-1] == 0 && img[row][l-2] == 0 ) fl = 1;
            else                                                                l --;
        }
        
        if ( !fr )
        {
            if ( img[row][r] == 1 && img[row][r+1] == 0 && img[row][r+2] == 0 ) fr = 1;
            else                                                                r ++;
        }
    }
    /* Updata information */
    left[line]  = l;
    right[line] = r;
    width[line] = r - l;
    
    /* while lost, record
       If Border or break */
    if ( l == BORDER_L || left[MAX(0, line-1)] - l >= 10 )
    {
        lost_l[line] = 1;
        
        cnt_lost_l ++;
        if ( flag_first_lost_l )
        {
            flag_first_lost_l = 0;
            lost_l[line] = 2;
        }
    }
    else
    {
        flag_first_lost_l = 1;
        if ( line >= 1 && lost_l[line-1] == 1 )  lost_l[line-1] = 3;
    }
    
    if ( r == BORDER_R || r - right[MAX(0, line-1)] >= 10 )
    {
        lost_r[line] = 1;
        cnt_lost_r ++;
        if ( flag_first_lost_r )
        {
            flag_first_lost_r = 0;
            lost_r[line] = 2;
        }
    }
    else
    {
        flag_first_lost_r = 1;
        if ( line >= 1 && lost_r[line-1] == 1 )  lost_r[line-1] = 3;
    }

    // 
    //  if lost_x[line] != 0 ---> there is break or border
    if ( lost_l[line] != 0 )    cnt_lostline_l[line] = cnt_lostline_l[MAX(line-1, 0)]+1;
    else                        cnt_lostline_l[line] = cnt_lostline_l[MAX(line-1, 0)];
        
    if ( lost_r[line] != 0 )    cnt_lostline_r[line] = cnt_lostline_r[MAX(line-1, 0)]+1;
    else                        cnt_lostline_r[line] = cnt_lostline_r[MAX(line-1, 0)];
    
    // if width[line] <= 20 ---> there is narrow line
    // record the narrow line to deal with cross and start
    if ( width[line] <= 20 )    cnt_narrow[line] = cnt_narrow[MAX(0, line-1)] + 1;
    else                        cnt_narrow[line] = cnt_narrow[MAX(0, line-1)];
    
    // all lost while cross or circle
    if ( width[line] > BORDER_R - BORDER_L - 1 )   cnt_all_white ++;
    
    return (l+r) >> 1;
}

/*
 * 判断拐点
*/
void Get_Inflection(void)
{
    int16 i, flag;
    int16 err0, err1, err2, err22;
    
    err2 = left[2] - left[1];
    flag = 0;
    
    for ( i = 2; i < MIN(valid_line-2, 38); i ++ )
    {
        err1 = err2;
        err2 = left[i+1] - left[i];
        err0 = left[i+2] - left[i];
        err22 = (left[i] - left[i-2])*(left[i+2] - left[i]);

        if ( (err1*err2 < -3 && err2 < 0) || (err2 <= 0 && err0 <-15)
             || (err22 < -6 && left[i] - left[i-2] > 0) )
        {
            inflection_l = i;
            flag = 1;
            break;
        }
    }
    if ( flag == 0 )     inflection_l = 0;

    err2 = right[2] - right[1];
    flag = 0;
    for ( i = 2; i < MIN(valid_line-2, 38); i ++ )
    {
        err1 = err2;
        err2 = right[i+1] - right[i];
        err0 = right[i+2] - right[i];
        err22 = (right[i] - right[i-2])*(right[i+2] - right[i]);
        if ( (err1*err2 < -3 && err2 > 0) || (err2 >= 0 && err0 > 15)
             || (err22 < -6 && right[i+2] - right[i] > 0) )
        {
            inflection_r = i;
            flag = 1;
            break;
        }
    }
    if ( flag == 0 )     inflection_r = 0;
}

/*
 * 判断上拐点
*/
void Get_Up_Inflection(int16 start, int16 end, int16 * l, int16 * r)
{
    int16 i, flag;
    int16 err1, err2;
    flag = 0;
    for ( i = end-1; i > start+1; i -- )
    {
        err1 = left[i] - left[i-1];
        err2 = left[i] - left[i-2];

        if ( (err1*err2 >= 18 && err2 > 0) || (err1 >= 2 && err2 > 8) )
        {
            *l = i;
            flag = 1;
            break;
        }
    }
    if ( flag == 0 )     *l = 0;

    flag = 0;
    for ( i = end-1; i > start+1; i -- )
    {
        err1 = right[i-1] - right[i];
        err2 = right[i-2] - right[i];
        if ( (err1*err2 >= 18 && err2 > 0) || (err1 > 2 && err2 > 8) )
        {
            *r = i;
            flag = 1;
            break;
        }
    }
    if ( flag == 0 )     *r = 0;
}

/*
 * 处理十字
*/
void Deal_Cross(void)
{
    float k, b;
    float d;

    int16 inf_up_l = 0, inf_up_r = 0, pos, cir_up, tmp, x, y;
    int16 row;

    int16 i;
    
    /* find both inflection, may cross or circle */
    if (    inflection_l >= 5 && inflection_r >= 5
         && inflection_l <= 38 && inflection_r <= 38
         && ABS(inflection_l-inflection_r) < 15 
         && cnt_lostline_l[inflection_l] <= 2
         && cnt_lostline_r[inflection_r] <= 2
         && cnt_narrow[MAX(inflection_l, inflection_r)] == 0)
         //&& cnt_all_white != 0    )
    {
        Least_Squares(inflection_l, left, &k, &b);
        for ( i = inflection_l; i < 40; i ++ )
            left[i] = (int16)(k*i + b + 0.5f);
        
        Least_Squares(inflection_r, right, &k, &b);
        for ( i = inflection_r; i < 40; i ++ )
            right[i] = (int16)(k*i + b + 0.5f);

        /* Is a circle, just round and round */
        for ( i = MAX(inflection_l, inflection_r)+2; i < 39 && width[i-1] >= 80; i ++ )
        {
            pos = (left[i] + right[i]) / 2;
            row = START - i*STEP;
            
            /* find circle */
            if ( img[row][pos] == 0 )
            {
                cir_up = -1;
                for ( valid_line = inflection_r+2; valid_line < 38; valid_line ++ )
                {
                    // go right
                    tmp = Search_Line(valid_line, right[valid_line-1]-20);
                    if ( tmp == -1 )    break;
                    if ( cir_up == -1 && (left[valid_line] - left[valid_line-1] > 25) )
                    {
                        cir_up = valid_line;
                    }
                }
                if ( cir_up == -1 ) break;
                
                x = MAX(0, inflection_l-2);
                left[x] += 10;
                y = left[x];
                for ( i = 0; i < cir_up; i ++ )
                {
                    if ( i < x )    left[i] += 10; 
                    else            left[i] = (i-x)*(left[cir_up]-y)/(cir_up-x)+y;
                }
                
//                cir_up = MIN(valid_line, inflection_r + 2);
//                y = right[0];
//                for ( i = 0; i < cir_up; i ++ )
//                {
//                    right[i] = (i)*(right[cir_up]-y)/(cir_up)+y;
//                }
                flag_circle = 1;
                flag_circle_start = 1;
                break;
            }
        }
        flag_cross = 1;
    }// left
    else if ( inflection_l >= 5 && inflection_l <= 25 
             && cnt_lost_r > 10 && lost_r[inflection_l] != 0
             && cnt_all_white != 0)
    {
        Least_Squares(inflection_l, left, &k, &b);
        for ( valid_line = inflection_l; valid_line < 40; valid_line ++ )
        {
            d = (int16)(k*valid_line + b + 0.5f);
            tmp = Search_Line(valid_line, (int16)(d + 15));
            if ( tmp == -1 )    break;
            left[valid_line] = (int16)d;
        }
        flag_cross = 2;
    }// right
    else if ( inflection_r >= 5 && inflection_r <= 25
             && cnt_lost_l > 10 && lost_l[inflection_r] != 0
             && cnt_all_white != 0)
    {
        Least_Squares(inflection_r, right, &k, &b);
        for ( valid_line = inflection_r; valid_line < 40; valid_line ++ )
        {
            d = (int16)(k*valid_line + b + 0.5f);
            tmp = Search_Line(valid_line, (int16)(d - 15));
            if ( tmp == -1 )    break;
            right[valid_line] = (int16)d;
        }
        flag_cross = 3; 
    }// in cross
    else if ( cnt_lost_l > 10 && cnt_lost_r > 10 && cnt_all_white >= 10 )
    {
        Get_Edge_Line(1);
        for ( i = 10; i < valid_line; i ++ )
        {
            // 找到都未丢线就退出
            if ( lost_l[i] == 0 && lost_r[i] == 0 )
            {
                break;
            }
        }
        // if there is cross. ---> if there is not all white line and valid_line is enough
        if ( i < valid_line && valid_line >= 33 && flag_circle_start == 0 )
        {
            Get_Up_Inflection(i-10, i+5, &inf_up_l, &inf_up_r);
            if ( inf_up_l )
            {
                if ( inf_up_l > inflection_l && inflection_l != 0 )
                {
                   d = (left[inf_up_l] - left[inflection_l])*1.0f / (inf_up_l - inflection_l);
                   for ( int i = 0; i < inf_up_l; i ++ )
                   {
                       left[i] = (int16)(left[inflection_l] + d*(i - inflection_l) + 0.5f);
                   }
                }
                else
                {
                    Least_Squares(inf_up_l+6, left, &k, &b);
                    for ( i = 0; i < inf_up_l; i ++ )
                    {
                        left[i] = (int16)(k*i + b + 0.5f);
                        if ( left[i] < BORDER_L )   left[i] = BORDER_L;
                    }
                }
            }
            if ( inf_up_r )
            {
                if ( inf_up_r > inflection_r && inflection_r != 0 )
                {
                    d = (right[inf_up_r] - right[inflection_r])*1.0f / (inf_up_r - inflection_r);
                    for ( int i = 0; i < inf_up_r; i ++ )
                    {
                        right[i] = (int16)(right[inflection_r] + d*(i - inflection_r) + 0.5f);
                    }
                }
                else
                {
                    Least_Squares(inf_up_r+6, right, &k, &b);
                    for ( i = 0; i < inf_up_r; i ++ )
                    {
                        right[i] = (int16)(k*i + b + 0.5f);
                        if ( right[i] > BORDER_R )   right[i] = BORDER_R;
                    }
                }
            }
        }
        else // in circle
        {
            // we choose right
            for ( valid_line = 1; valid_line < 40; valid_line ++ )
            {
                tmp = Search_Line(valid_line, right[valid_line-1]-20);
                if ( tmp == -1 )    break;
            }
            for ( tmp = 5; tmp < valid_line-8; tmp ++ )
                if ( left[tmp] - left[tmp-1] > 15 )
                    break;
  
            tmp = MIN(valid_line, tmp+1);
            for ( int i = 0; i < tmp; i ++ )
                left[i] = (int16)(i*(left[tmp]-50)*1.0/tmp + 50);
            
            flag_cross = 4;
        }
        //flag_cross = 4;
    }
    
    if ( flag_circle_start )
    {
        flag_circle_start++;
       
        if ( flag_circle_start <= 230 )
        {
            if ( flag_cross != 1 && flag_cross != 4 )   Deal_Cirque();
            //gpio_set(PTD7, 1);
        }
        else  
        {
            flag_circle_start = 0;
            //gpio_set(PTD7, 0);
        }       
    }
}

/*
 * while it is not a cross
 * we will make sure wheather a
 * Deal circle
*/
void Deal_Cirque(void)
{
    int16 p = 0;
    float d;
    // We can get all line nearly. If not, it's not cirque.
    if ( valid_line > 35 )
    {
        if ( (inflection_r > 0 && inflection_r < 35) )
         //&& cnt_lost_l >= 8 && lost_r[inflection_r] <= 8 )
        {
            for ( int i = MIN(inflection_r+18, valid_line-3); i > MAX(5, inflection_r-8); i --  )
            {
                if ( left[i-1] == BORDER_L || 
                     left[i-1]-left[i]<=(left[i]-left[i+1])*2)
                {
                    p = i+3;
                    break;
                }
            }
            if ( p != 0 )
            {
                d = (left[0] - left[p])*1.0/(p);
                for ( int i = 0; i <= p; i ++)
                    left[i] = (int16)(left[0] - i*d);
                flag_circle = 1;
            }
        }
        else if ( (inflection_l > 0 && inflection_l < 35) )
             //&& cnt_lost_r >= 8 && lost_l[inflection_l] <= 8 )
         {
            for ( int i = MIN(inflection_l+18, valid_line-3); i > MAX(5, inflection_l-8); i --  )
            {
                if ( right[i-1] == BORDER_R || 
                     right[i-1]-right[i]>=(right[i]-right[i+1])*2)
                {
                    p = i+3;
                    break;
                }
            }
            if ( p != 0 )
            {
                d = (right[0] - right[p])*1.0/(p);
                for ( int i = 0; i <= p; i ++)
                    right[i] = (int16)(right[0] - i*d); 
                flag_circle = 1;
            }
         }
        
    }
}

/*
 * 线性拟合
*/
void Least_Squares(int16 end, int16 * y, float * k, float * b)
{
    int16 i;
    int32 n = 0;
    int32 x_sum = 0;
    int32 y_sum = 0;
    int32 xy_sum= 0;
    int32 x2_sum= 0;
    int32 tmp;
    int32 start = MAX(0, end - 5);
    
    /* filter the point */
    for ( i = end - 1; i >= start; i -- )
    {
        if ( ABS(y[i]-y[i+1]) > 6 ) y[i] = y[i+1];
    }
    
    for ( ; start < end; start ++ )
    {
        x_sum  += start;
        y_sum  += y[start];
        x2_sum += start * start;
        xy_sum += start*y[start];
        n ++;
    }
    tmp = n * x2_sum - x_sum * x_sum;
    
    if ( tmp != 0 )
    {
	  	*k = 1.0f*(n*xy_sum - x_sum*y_sum) / tmp;
	  	*b = y_sum*1.0f/n - (*k)*x_sum/n;        
    }
    else
	{
	  	*k = 0.0f;
		*b = x_sum*1.0f/n;
	}
}


void Updata_Line_Inf(void)
{
    int16 i, delta;
    flag_first_lost_l = flag_first_lost_r = 1;
    cnt_lost_r = cnt_lost_l = 0;
    cnt_lostarea_l = cnt_lostarea_r = 0;
    for ( i = 0; i < valid_line; i ++ )
    {
        width[i] = right[i] - left[i];

        lost_l[i] = lost_r[i] = 0;

        if ( i == 0 )   delta = 0;
        else            delta = left[i-1] - left[i];

        if ( left[i] == BORDER_L || delta >= 12 )
        {
            lost_l[i] = 1;
            cnt_lost_l ++;
            if ( flag_first_lost_l )
            {
                flag_first_lost_l = 0;
                lost_l[i] = 2;
                lost_start_l[cnt_lostarea_l] = i;
            }
            if ( i == valid_line-1 && (lost_l[i-1] == 1 || lost_l[i-1] == 2 || lost_l[i] == 2) )
            {
                lost_l[i] = 3;
                lost_end_l[cnt_lostarea_l++] = i;
            }
        }
        else
        {
            flag_first_lost_l = 1;
            if ( i != 0 && lost_l[i-1] == 1 || lost_l[i-1] == 2 )
            {
                lost_l[i] = 3;
                lost_end_l[cnt_lostarea_l++] = i;
            }
        }

        if ( i == 0 )   delta = 0;
        else            delta = right[i] - right[i-1];

        if ( right[i] == BORDER_R || delta >= 12 )
        {
            lost_r[i] = 1;
            cnt_lost_r ++;
            if ( flag_first_lost_r )
            {
                flag_first_lost_r = 0;
                lost_r[i] = 2;
                lost_start_r[cnt_lostarea_r] = i;
            }
            if  ( i == valid_line-1 && (lost_r[i-1] == 1 || lost_r[i-1] == 2 || lost_r[i] == 2) )
            {
                lost_r[i] = 3;
                lost_end_r[cnt_lostarea_r++] = i;
            }
        }
        else
        {
            flag_first_lost_r = 1;
            if ( i != 0 && lost_r[i-1] == 1 || lost_r[i-1] == 2 )
            {
                lost_r[i] = 3;
                lost_end_r[cnt_lostarea_r++] = i;
            }
        }
    }    
}

void Correct_Width(void)
{
    int16 i, pos;
    float d;
    c_lost_l = c_lost_r = -1;
    for ( i = cnt_lostarea_l - 1; i >= 0; i -- )
    {
        pos =lost_end_l[i];
        if ( pos == valid_line-1 )
        {
            // 左C弯
            if ( right[pos-1] - right[pos] >= 3 
                 && ((lost_start_l[i] >= 30 && valid_line >= 38) || valid_line < 38 ) )
                c_lost_l = lost_start_l[i];
        }
        else
        {
            // 左C弯
            if ( pos > valid_line-12 && right[pos-1] - right[pos+1] >= 5
                 && i == cnt_lostarea_l - 1 )
            {
                c_lost_l = lost_start_l[i];
            }
            else
            {
                if ( lost_start_l[i] == 0 )
                {
                    d = (152 - width[lost_end_l[i]])*1.0f / (lost_end_l[i] - lost_start_l[i]);
                    for ( int j = lost_start_l[i]; j < lost_end_l[i]; j ++ )
                    {
                        width[j] = (int16)(152 - d*(j - lost_start_l[i]));
                    }
                }
            }
        }
    }
    //                    修正右边边线
    for ( i = cnt_lostarea_r-1; i >= 0; i -- )
    {
        pos =lost_end_r[i];
        if ( pos == valid_line-1 )
        {
            if ( left[pos] - left[pos-1] >= 3 
                 && ((lost_start_l[i] >= 30 && valid_line >= 38) || valid_line < 38 ) )
                c_lost_r = lost_start_r[i];
        }
        else
        {
            if ( pos > valid_line-12 && left[pos+1] - left[pos-1] >= 5
                 && i == cnt_lostarea_r-1)
            {
                c_lost_r = lost_start_r[i];
            }
            else
            {
                if ( lost_start_r[i] == 0 )
                {
                    d = (152 - width[lost_end_r[i]])*1.0f / (lost_end_r[i] - lost_start_r[i]);
                    for ( int j = lost_start_r[i]; j < lost_end_r[i]; j ++ )
                        width[j] = (int16)(152 - d*(j - lost_start_r[i]));
                }
            }
        }
    }
}

/*
 * 依据之前的数据推算中点
*/
void  Calculate_Midline(void)
{
    int16  tmp, i;
    // 我们要去修正两边都是c的误判(应该是有的, 反正该考虑的)
    if ( c_lost_l > c_lost_r )  c_lost_r = -1;
    if ( c_lost_r > c_lost_l )  c_lost_l = -1;
    mid[0] = (BORDER_L + BORDER_R) / 2;
    for ( i = 0; i < valid_line; i ++ )
    {
        if ( c_lost_l != -1 )
        {
            if ( i >= c_lost_l )
            {
                if ( i == 0 )
                    tmp = right[i] - BORDER_L;
                
                mid[i] = right[i] - (tmp ++)/2;
                if ( mid[i] < BORDER_L )
                {
                    valid_line = i;
                    break;
                }
            }
            else
            {
                if ( left[i] == BORDER_L && right[i] == BORDER_R )
                    mid[i] = mid[MAX(0, i-1)];
                else
                    mid[i] = (int16)((left[i] + right[i]) / 2.0f + 0.5f);
                tmp = width[i] + 1;
            }
        }
        else if ( c_lost_r != -1 )
        {
            if ( i >= c_lost_r )
            {
                if ( i == 0 )
                    tmp = BORDER_R - left[i];
 
                mid[i] = left[i] + (tmp ++)/2;
                if ( mid[i] > BORDER_R )
                {
                    valid_line = i;
                    break;
                }
            }
            else
            {
                if ( left[i] == BORDER_L && right[i] == BORDER_R )
                    mid[i] = mid[MAX(0, i-1)];
                else
                    mid[i] = (int16)((left[i] + right[i]) / 2.0f + 0.5f);
                tmp = width[i] + 1;
            }
        }
        else
        {
            if ( left[i] == BORDER_L && right[i] == BORDER_R )
                mid[i] = mid[MAX(0, i-1)];
            else
                mid[i] = (int16)((left[i] + right[i]) / 2.0f + 0.5f);
        }
    }
    if ( flag_stop )
    {
        for ( i = 0; i < valid_line; i ++ )
        {
            if ( mid[i] < 20 )  mid[i] = mid[MAX(i-1, 0)];
        }
    }
}

void Path_Optimization(int16 i)
{
    int8 j;
    while ( i -- )
    {
        for ( j = 1; j < valid_line-1; j ++ )
        {
            mid[j] = (mid[j-1] + mid[j+1]) >> 1;
        }
    }
}

void  Get_Dir(void)
{
    int16 see, i, cnt = 0;
    float t = 0;
    float sum = 0, avr, ee = 0;
   
    errLast = err;
    see = 26;
    //if ( speed_now > 50 )  
    //  t = (speed_now-50);/*(speed_now-50);*/
    
    //see = (int16)(  t*0.1- 0.598*angle + 26.5);// 33.5
    if(straight_flag == 0)
    {
      if(valid_line > 38) valid_line = 28;
     
    }
    see = (int16)( abs(real_speed - 149)*0.15 + 26);// 33.5
    if(see>28) see = 28;
    if ( see >  valid_line - 2 )      see = valid_line - 2;
    if ( see < 15  )                  see = 15;        
//    err = mid[see-4]+mid[see-3]+mid[see-2]+mid[see-1]+mid[see]
//         +mid[see-5];
//    err = err/6.0f - COL/2.0f;
//    for(uint8 i = 0;i<188;i++)
//    {
//       img[see][i] = 0;
//       img[see+6][i] = 0;
//    }
    err = mid[see]+mid[see+1]/*+mid[see+6]/*+mid[see]+mid[see-2]*/;
    err = err/2.0f - COL/2.0f;
    //if(abs(err - errLast)>25) err = errLast;

    if ( valid_line <= 17)
    {
        if ( err > 0 )      err = ERR_MAX;
        else if ( err < 0 ) err = ERR_MIN;
    }
    
    if (err > ERR_MAX )         err = ERR_MAX;
    else if ( err < ERR_MIN )   err = ERR_MIN;
    
    
    if ( err-errLast > 12 )         err = errLast + 12;
    else if ( err-errLast < -12 )   err = errLast - 12;
 
    errDot = err - errLast;
//    if(gpio_get(PTC8) == 0)
//    {
//      OLED_Print_Num1(1,1, err);
//      LCD_PrintU16(45,1,valid_line);
////      OLED_Print_Num1(25,6, r);  
////      LCD_PrintU16(25,7,l);
//      OLED_Print_Num1(25,6, errDot); 
//    }
//    if ( valid_line >= 30 )
//    {
//        for ( i = 0; i < valid_line; i += 2 )
//        {
//            sum += mid[i] - COL/2;
//            cnt ++;
//        }
//        avr = sum / cnt;
//
//        for ( i = 0; i < valid_line; i += 2 )
//        {
//            ee += (mid[i] - COL/2 - avr)*(mid[i] - COL/2 - avr);
//        }
//        ee /= cnt*1.0;
//        if ( ee <= 10 ) flag_line = 1;
//    }
}

void  Deal_BlockOrStart(void)
{
    float k, b;
    int16 l = 0, r = 0;
    int16 i = 0;
    
    for ( i = 2; i < valid_line; i ++ )
    {
        if ( left[i] - left[i-2] >= 15 )
        {
            l = i;
            break;
        }
    }
    
    for ( i = 2; i < valid_line; i ++ )
    {
        if ( right[i-2] - right[i] >= 15 )
        {
            r = i;
            break;
        }
    }
    
    if ( cnt_lost_l <= 5 && cnt_lost_r <= 5
        && cnt_narrow[valid_line] >= 2 && (l || r))
    {
            flag_stop = 1;
        //if ( valid_line <= 25 )
        //"GET START LINE";
    }
    else if ( cnt_lost_l <= 15 && cnt_lost_r == 0
              && l && r == 0 && l < 35
              && Is_LeftBlock(l)
              && valid_line >= 35
              )
    {
        Least_Squares(MIN(l+4, valid_line-1), left, &k, &b);
        left[0] += 50;
        for ( int i = 0; i < valid_line; i ++ )
        {
            if ( i < l )
            {
                left[i] = (int16)(left[0] + (left[l]-left[0])*i*1.0/(l));
            }
            else
            {
                left[i] = (int16)(k*i + b + 0.5f);
            }
        }
        //
    }
    else if (  cnt_lost_r <= 15 && cnt_lost_l == 0
               && r && l == 0 && r < 35
               && Is_RightBlock(r)
               && valid_line >= 35
              )
    {
        //"GET Right Block";
        Least_Squares(MIN(r+4, valid_line-1), right, &k, &b);
        right[0] -= 45;
        for ( int i = 0; i < valid_line; i ++ )
        {
            if ( i < r )
            {
                right[i] = (int16)(right[0] + (right[r]-right[0])*i*1.0/(r));
            }
            else
            {
                right[i] = (int16)(k*i + b + 0.5f);
            }
        }
    }
}

int8 Is_RightBlock(int16 line)
{
    int row = START - line*STEP;
    int col = right[line] + 1;
    int cnt_w = 0, cnt_b = 0, cnt = 0;
    while ( 1 )
    {
        //  1 0
        if ( (img[row][col] == 1 && img[row][col+1] == 0) )
            break;

        if ( cnt ++ >= 50 || col+1 == BORDER_R ) return 0;

        if ( img[row][col] == 1 )   cnt_w ++;
        if ( img[row][col] == 0 )   cnt_b ++;
        col ++;
    }

    if ( cnt_b >= 5 && cnt_w >= 1 )
        return 1;
    else
        return 0;

}

int8 Is_LeftBlock(int16 line)
{
    int16 row = START - line*STEP;
    int16 col = left[line] - 1;
    int16 cnt_w = 0, cnt_b = 0, cnt = 0;
    while ( 1 )
    {
        //  1 0
        if ( (img[row][col] == 1 && img[row][col-1] == 0) )
            break;

        if ( cnt ++ >= 50 || col-1 == BORDER_L ) return 0;

        if ( img[row][col] == 1 )   cnt_w ++;
        if ( img[row][col] == 0 )   cnt_b ++;
        col --;
    }

    if ( cnt_b >= 5 && cnt_w >= 1 )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void Debug_Image(void)
{
    int16 i, row;
    OLED_nurbem_int(0, 0, valid_line);
    OLED_nurbem_int(0, 2, cnt_lost_r);
    OLED_nurbem_int(0, 4, cnt_lost_l);
    OLED_nurbem_int(0, 6, (int16)err);
    if ( flag_circle == 1 ) gpio_set(PTD7, 1);
    else                    gpio_set(PTD7, 0);

    for ( i = 0; i < valid_line; i ++ )
    {
        row = START - i*STEP;
        img[row][mid[i]] = 0;
        img[row][right[i]-1] = 0;
        img[row][left[i]+1] = 0;
    }     
    row = START - inflection_l*STEP;
    img[row][left[inflection_l]+2] = 0;
    img[row][left[inflection_l]+3] = 0;
    img[row][left[inflection_l]+4] = 0;
    row = START - inflection_r*STEP;
    img[row][right[inflection_r]-2] = 0;
    img[row][right[inflection_r]-3] = 0;  
    img[row][right[inflection_r]-4] = 0;    
}

void zx_type(/*int16 *Line*/)
{
  int16 ave_mid = 0;
  uint16 D[45] = {0};
  D[44] = 0;
  for(uint8 i = 3;i<41;i++)
  {
    ave_mid += mid[i];
  }
  ave_mid = ave_mid/38;
  for(uint8 k=3;k<41;k++)
  {
    D[k] = abs(mid[k]-ave_mid);
    D[44] += D[k];
  }
//  if(gpio_get(PTC8) == 0)
//  {
//      OLED_Print_Num1(1,1, D[44]);
////      LCD_PrintU16(45,1,valid_line);
// //     OLED_Print_Num1(25,6, err);  
////      LCD_PrintU16(25,7,l);
//  }
//  
  if((D[44]<250)/*&&servo_con<10*/)
  {
    straight_flag = 1;
  }
  else
  {
    straight_flag = 0;
    //start_conver_flag = 0;

  }
  
  /*********起跑线处理用**********/
  D[44] = 0;
  ave_mid = 0;
  for(uint8 i = 0;i<10;i++)
  {
    ave_mid += mid[i];
  }
  ave_mid = ave_mid/10;
  for(uint8 k=0;k<10;k++)
  {
    D[k] = abs(mid[k]-ave_mid);
    D[44] += D[k];
  }
////  if(gpio_get(PTC8) == 0)
////  {
////      OLED_Print_Num1(35,1, D[44]);
//////      LCD_PrintU16(45,1,valid_line);
//// //     OLED_Print_Num1(25,6, err);  
//////      LCD_PrintU16(25,7,l);
////  }
  
  if((D[44]<16)/*&&servo_con<10*/)
  {
    straight_flag_s = 1;
  }
  else
  {
    straight_flag_s = 0;
  }
  if((straight_flag_s==1)&&(flag_stop==1))
  {
    //err=0;7/
    

  }

}

void  Deal_BlockOrStart1(void)
{
    float k, b;

    if ( cnt_lost_l <= 2 && cnt_lost_r <= 2
         && inflection_l && inflection_r
         && cnt_narrow[MAX(inflection_l, inflection_r)] >= 2
         )
    {
        flag_stop = 1;
        //"GET START LINE";
    }
    else if ( cnt_lost_l <= 2 && cnt_lost_r == 0
              && inflection_l && inflection_r == 0
              && Is_LeftBlock(inflection_l-2)
              && valid_line >= 38
              )
    {
        Least_Squares(inflection_l, left, &k, &b);
        left[0] += 50;
        for ( int i = 0; i < valid_line; i ++ )
        {
            if ( i < inflection_l )
            {
                left[i] = (int16)(left[0] + (left[inflection_l]-left[0])*i*1.0/(inflection_l));
            }
            else
            {
                left[i] = (int16)(k*i + b + 0.5f);
            }
        }
        //
    }
    else if (  cnt_lost_r <= 2 && cnt_lost_l == 0
               && inflection_r && inflection_l == 0
               && Is_RightBlock(inflection_r-2)
               && valid_line >= 38
              )
    {
        //"GET Right Block";
        Least_Squares(inflection_r, right, &k, &b);
        right[0] -= 45;
        for ( int i = 0; i < valid_line; i ++ )
        {
            if ( i < inflection_r )
            {
                right[i] = (int16)(right[0] + (right[inflection_r]-right[0])*i*1.0/(inflection_r));
            }
            else
            {
                right[i] = (int16)(k*i + b + 0.5f);
            }
        }
    }
}


void start_line()//起跑线处理
{
  B_TO_W1 = 0;
  W_TO_B1 = 0;
  B_TO_W2 = 0;
  W_TO_B2 = 0;
  for(int16 k=188;k>0;k--)//R TO L
  {
    if((img[80][k-1]-img[80][k])==1)
    {
      B_TO_W1++;//BLACK TO WHITE
      W_TO_B1++;//WHITE TO BLACK
    }
    if((img[30][k-1]-img[30][k])==1)
    {
      B_TO_W2++;//BLACK TO WHITE
      W_TO_B2++;//WHITE TO BLACK
    }
    if((img[80][k]-img[80][k-1])==1)
    {
      W_TO_B1++;//WHITE TO BLACK
      B_TO_W1++;//BLACK TO WHITE
    }
    if((img[30][k]-img[30][k-1])==1)
    {
      W_TO_B2++;//WHITE TO BLACK
      B_TO_W2++;//BLACK TO WHITE
    }
  }
//  if((B_TO_W1>=15&&W_TO_B1>=15)||(B_TO_W2>=15&&W_TO_B2>=15))
//  {
//    start_flag = 1;
////    led_turn (LED1);
//  }
   if((B_TO_W1>=8&&W_TO_B1>=8)&&(straight_flag_s== 1))
  {
    start_flag = 1;
//    led_turn (LED1);
  }
//   if((B_TO_W2>=15&&W_TO_B2>=15)&&(straight_flag_black==1))
//  {
//    start_conver_flag = 1;
////    led_turn (LED1);
//  }
//  if((start_conver_flag==1)&&(start_flag==0))
//  {
////    midlinebuff[ahead_2]=94;
////    midlinebuff[ahead_2+1]=94;
////    midlinebuff[ahead_2+2]=94;
//    straight_flag = 1;
//  }
  //else
    //start_flag = 0;
  
}