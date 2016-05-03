
#include "driver.h"

int readHandInfo(){
           
        peer_addr_len = sizeof(struct sockaddr_storage);
        nread = recvfrom(sfd, xyzBuf, BUF_SIZE, 0,
        	(struct sockaddr *) &peer_addr, &peer_addr_len);
	int xLevel, yLevel, zLevel;
	        
        if (nread == -1){
        	printf("Failed read\n");  
		return 1;              /* Ignore failed request */
	}
		
	if( xyzBuf[2]   == 0x00 ){	
		xLevel = GetXLevel((short)(xyzBuf[0] | (xyzBuf[1] << 8)));
		if(xLevel != 0)
			SetMotor4Value(xLevel);
	}

	else if( xyzBuf[2]  == 0x01 ){
		yLevel = GetYLevel((short)(xyzBuf[0] | (xyzBuf[1] << 8)));
	}

	else if( xyzBuf[2]  == 0x02 ){	
		zLevel = GetZLevel((short)(xyzBuf[0] | (xyzBuf[1] << 8)));
		if(zLevel != 0)
			SetMotor3Value(zLevel);
	}

	else
		printf("Received garbage...\n");
	return 0;
}

int socketBind(){
	
	memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
        hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
        hints.ai_protocol = 0;          /* Any protocol */
        hints.ai_canonname = NULL;
        hints.ai_addr = NULL;
        hints.ai_next = NULL;

        s = getaddrinfo(NULL, "22", &hints, &result);
           
	if (s != 0) {
        	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
        }


        for (rp = result; rp != NULL; rp = rp->ai_next) {
            sfd = socket(rp->ai_family, rp->ai_socktype,
                    rp->ai_protocol);
            if (sfd == -1)
                continue;

            if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0){
		printf("Bound!!\n");
		break;                  /* Success */
	    }
               close(sfd);
        }

        if (rp == NULL) {               /* No address succeeded */
            fprintf(stderr, "Could not bind\n");
            exit(EXIT_FAILURE);
        }
        freeaddrinfo(result);           /* No longer needed */

	return 0;
}
