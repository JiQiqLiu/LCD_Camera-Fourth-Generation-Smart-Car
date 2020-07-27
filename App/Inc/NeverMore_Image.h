#ifndef __NEVERMORE_IMAGE_H__
#define __NEVERMORE_IMAGE_H__

#define STEP        3
#define START       119
#define VALID_LINE  41
#define BORDER_L    1
#define BORDER_R    186

#define ERR_MAX      75
#define ERR_MIN     -75
//COL
//ROW
extern uint8 straight_flag_s ,straight_flag;
extern int16 flag_stop, flag_line, valid_line, see ,mid[41];   // flag of stop line;
extern uint8 straight_flag,straight_flag_s;
extern int16 flag_circle ,start_flag;
extern void Otsu(void);
extern void Deal_Image(void);
extern void zx_type(/*int16 *Line*/);

#endif