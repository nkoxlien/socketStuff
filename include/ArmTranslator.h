/****************************************
*
* Author: Nick Koxlien
*
*****************************************/



#ifndef ARM_TRANSLATOR_H__
#define ARM_TRANSLATOR_H__

#include <stdio.h>



int MOTOR4;
int MOTOR3;

void InitMotorValues();

int GetXLevel(short x);
int GetYLevel(short y);
int GetZLevel(short z);
void SetMotor4Value(int x);
void SetMotor3Value(int z);




#endif //ARM_TRANSLATOR_H__
