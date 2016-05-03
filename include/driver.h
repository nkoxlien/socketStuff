#ifndef DRIVER_H__
#define DRIVER_H_


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include "ArmTranslator.h"
#define BUF_SIZE 4


struct addrinfo hints;
struct addrinfo *result, *rp;
int sfd, s, n;
struct sockaddr_storage peer_addr;
socklen_t peer_addr_len;
ssize_t nread;
char xyzBuf[BUF_SIZE];



/**********************************************
* Method creates socket and binds it to port 22
* returns 0 on success
**********************************************/
int socketBind();

/*********************************************
* Function reads the data from the kinect over
* the LAN socket.  Returns 0 on success
*********************************************/	
int readHandInfo();




#endif //DRIVER_H___
