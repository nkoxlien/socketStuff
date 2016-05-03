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


int MOTOR4;
int MOTOR3;

void InitMotorValues();

int GetXLevel(short x);
int GetYLevel(short y);
int GetZLevel(short z);
void SetMotor4Value(int x);
void SetMotor3Value(int z);




#endif //ARM_TRANSLATOR_H__
