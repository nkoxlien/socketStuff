

#include <inttypes.h>
#include "ArmTranslator.h"

int GetXLevel(uint16_t x){

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


int GetYLevel(uint16_t y){

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


int GetZLevel(uint16_t z){

        int resLength, level;
        
        if(z > MAXZ || z < MINZ){
                
                return 0;       
        }

        resLength = (MAXZ - MINZ) / RESOLUTION;

        for(int i = 1; i <= RESOLUTION; i++){
        
                if( z > (MAXZ - (i * resLength))){
                        level = i;
                        return level; 
                }                       
        }
        
        return 0;

}


int GetMotor1Value(int x){

 	int value, resolution;
        resolution = RESOLUTION;
        value = (x-1) * (2400 -600) / (RESOLUTION - 1) + 600;
        return  value;
}

int GetMotor2Value(int y, int z){

	return 0;

}

int GetMotor3Value(int y, int z){


	return 0;
}
