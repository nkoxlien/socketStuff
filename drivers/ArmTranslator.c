#include <inttypes.h>
#include "ArmTranslator.h"

#define RESOLUTION 	100
#define MAXX            750
#define MINX            -750
#define MAXY            450
#define MINY            -450
#define MAXZ            2000
#define MINZ            400


void InitMotorValues(){
	MOTOR3 = 0;
	MOTOR4 = 0;
	
	return;
}

int GetXLevel(short x){

	int resLength, level;
        
        if(x > MAXX || x < MINX){
                
                return 0;
        }       
        
        resLength = (MAXX - MINX) / RESOLUTION;
        
        for(int i = 1; i <= RESOLUTION; i++){
        
                if( x > (MAXX - (i * resLength))){
                        level = RESOLUTION - (i-1);
                        return level; 
                }                    
        }

        return 0;

}


int GetYLevel(short y){

        int resLength, level;

        if(y > MAXY || y < MINY){
                
                return 0;
        }

        resLength = (MAXY - MINY) / RESOLUTION;

        for(int i = 1; i <= RESOLUTION; i++){

                if( y > (MAXY - (i * resLength))){
                        level = i; 
                        return level;
                }                    
        }

        return 0;

}


int GetZLevel(short z){

        int resLength, level;
        
        if(z > MAXZ || z < MINZ){
                
                return 0;       
        }

        resLength = (MAXZ - MINZ) / RESOLUTION;

        for(int i = 1; i <= RESOLUTION; i++){
        
                if( z > (MAXZ - (i * resLength))){
                        level = RESOLUTION - i;
                        return level; 
                }                       
        }
        
        return 0;

}


void SetMotor4Value(int x){

 	int value, resolution;
        resolution = RESOLUTION;
        value = (x-1) * (2400 -600) / (RESOLUTION - 1) + 600;
        MOTOR4 =  value;
}

void SetMotor3Value(int z){


 	int value, resolution;
        resolution = RESOLUTION;
        value = (z-1) * (1800 - 1000) / (RESOLUTION - 1) + 1000;
        MOTOR3 =  value;
}
