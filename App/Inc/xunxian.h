
#ifndef _XUNXIAN_h
#define _XUNXIAN_h

#include "include.h"

/********变量声明**********/
extern uint8 k;
extern uint8 pixel[120][188];
extern int16 midline[120];
extern int16 misline[4];
extern int8  x_left[5],x_right[5];
/********函数声明*********/
uint8 otsu_threshold();
void xunxain(void);
void inflection();

#endif