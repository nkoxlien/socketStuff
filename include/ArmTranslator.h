/****************************************
*
* Author: Nick Koxlien
*
*****************************************/



#ifndef ARM_TRANSLATOR_H__
#define ARM_TRANSLATOR_H__

#include <stdio.h>

#define RESOLUTION 	3
#define MAXX            750
#define MINX            -750
#define MAXY            450
#define MINY            -450
#define MAXZ            1400
#define MINZ            600


int XCURR;
int YCURR;
int ZCURR;
int MOTOR1;
int MOTOR2;
int MOTOR3;


int GetXLevel(uint16_t x);
int GetYLevel(uint16_t y);
int GetZLevel(uint16_t z);
int GetMotor1Value(int x);
int GetMotor2Value(int y, int z);
int GetMotor3Value(int y, int z);




#endif //ARM_TRANSLATOR_H__
