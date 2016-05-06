/*
 * signal_user.c
 * Signal recving program in the user space
 * Originated from http://people.ee.ethz.ch/~arkeller/linux/code/signal_user.c
 * Modified by daveti
 * Aug 23, 2014
 * root@davejingtian.org
 * http://davejingtian.org
 */
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "servo_sig.h"

#define UART_DEV "/dev/ttyAL0"
#define RAW_DATA 0

#define SIG_TEST 44 /* we define our own signal, hard coded since SIGRTMIN is different in user and in kernel space */ 

#define NO_OP_MASK		0x00
#define ROTATE_MASK		0x40
#define WRIST_MASK		0x80
#define CLAW_MASK		0xC0
#define CLEAR_COMMAND_MASK	0x3F
#define COMMAND_MASK		0xC0

bool busy = true;


//*****************************************************************************
//*****************************************************************************
int main(int argc, char **argv)
{
	
	//struct sigaction servo_sig;
	struct sigaction ctrl_c_sig;
	int position = 0;	// 0 means 0 degress, 1 means 180 degrees (for testing)
	
	// Set up handler for when the user presses CNTL-C to stop the application
	ctrl_c_sig.sa_sigaction = control_c_handler;
	ctrl_c_sig.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &ctrl_c_sig, NULL);

	// Configure the IP module 
	set_pid();
	disable_interrupts();
	enable_servos();

	/* read "man 2 open" to learn about O_NONBLOCK and O_NOCTTY */
   int fd = open(UART_DEV, O_RDWR, 0);
   if (fd == -1)
      goto error;
  
   // for seeking
   /*FILE* fp = fdopen(fd, "r+");
   if (fp == NULL){
	   int errval = errno;
	   printf("ERROR: %d\n", errval);
   }*/
   

   struct termios termInfo;
   if (tcgetattr(fd, &termInfo) == -1)
      goto error;

   /* flow control */
   termInfo.c_cflag &= ~CRTSCTS;
   //termInfo.c_cflag |= CRTSCTS;

   /* BAUD rate */
   cfsetispeed(&termInfo, B115200);
   cfsetospeed(&termInfo, B115200);

#ifdef RAW_DATA
   /* If you need to process the data in a raw format, you'll want to enable
    * this code.  Basically, if your transmitting/receiving anything other
    * than text data, enable this code.
    */
   cfmakeraw(&termInfo);
#endif

   if (tcsetattr(fd, TCSANOW, &termInfo) == -1)
      goto error;

   /* read "man select" for more advanced/event driven reading */
   char buffer[1];
   char rawServoData;
   int servoData;
   
   fd_set rfds;
   struct timeval tv;
   int retval;
   int rv;
   int xLevel, yLevel, zLevel;
   FD_ZERO(&rfds);
   FD_SET(fd, &rfds);
	
   int flags = fcntl(fd, F_GETFL, 0);
   fcntl(fd, F_SETFL, flags | O_NONBLOCK);	//sets uart buffer to only be read when data (blocking)
   
   if((rv =socketBind()) != 0){
	exit(EXIT_FAILURE);
   }
   
   InitMotorValues();
 
   while(busy){
	   // Wait up to 10us 
	   //tv.tv_sec = 0;
	   //tv.tv_usec = 100;
	  
	   
	   
		read(fd, buffer, 1);
		if((COMMAND_MASK & buffer[0]) == CLAW_MASK)
		{
			rawServoData = buffer[0] & CLEAR_COMMAND_MASK;
			servoData = (int)map(rawServoData, 0, 10, 600, 2400);
			write_servo6(servoData);
			printf("CLAW: %d\n", servoData);
		}
		else if((COMMAND_MASK & buffer[0]) == ROTATE_MASK)
		{
			rawServoData = buffer[0] & CLEAR_COMMAND_MASK;
			servoData = (int)map(rawServoData, 0, 10, 600, 2400);
			write_servo2(servoData);
			printf("ROTATE: %d\n",servoData);
		}
		else if((COMMAND_MASK & buffer[0]) == WRIST_MASK)
		{
			rawServoData = buffer[0] & CLEAR_COMMAND_MASK;
			servoData = (int)map(rawServoData, 0, 10, 600, 2400);
			write_servo5(servoData);
			printf("WRIST: %d", servoData);
		}
		
		moveArm();
   }
  
  //sleep(10);
  /* Loop forever, waiting for interrupts */
	/*while (busy) {
		position = (position == 1300) ? 1700 : 1300;	// toggle position
		sleep(1);
		write_servo1(position);
		sleep(1);
		write_servo2(position);
		sleep(1);
		write_servo3(position);
		sleep(1);
		write_servo4(position);
		sleep(1);
		write_servo5(position);
		sleep(1);
		write_servo6(position);
	}*/

	return 0;
	

error:
   perror(UART_DEV);
   return 1;
   
}




void moveArm(){

				
	readHandInfo();

	if(MOTOR4 >= 600 && MOTOR4 <= 2400){
		write_servo4(MOTOR4);
	}
	
	if(MOTOR3 >= 600 && MOTOR3 <= 2400){
		write_servo3(MOTOR3);
	}

	return;
 } 

//*****************************************************************************
//*****************************************************************************
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//*****************************************************************************
//*****************************************************************************
void receiveData(int n, siginfo_t *info, void *unused)
{
	printf("receiveData received value %i\n", info->si_int);
}

//*****************************************************************************
//*****************************************************************************
void control_c_handler(int n, siginfo_t *info, void *unused)
{
  disable_interrupts();
  disable_shift();
  clear_pid();
  busy = false;
}

//*****************************************************************************
//*****************************************************************************
int enable_interrupts(void)
{
  char buf[1] = "1";
	int fd = open("/sys/kernel/rhoy_servos/interrupt_mask", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	if (write(fd, buf,1) < 0) {
		perror("fwrite"); 
		return -1;
	}
  close(fd);
  return 0;
}

//*****************************************************************************
//*****************************************************************************
int disable_interrupts(void)
{
  char buf[1] = "0";
	int fd = open("/sys/kernel/rhoy_servos/interrupt_mask", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	if (write(fd, buf,1) < 0) {
		perror("fwrite"); 
		return -1;
	}
  close(fd);
  return 0;
}


//*****************************************************************************
//*****************************************************************************
int enable_servos(void)
{
  char buf[1] = "1";
	int fd = open("/sys/kernel/rhoy_servos/control_status", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	if (write(fd, buf,1) < 0) {
		perror("fwrite"); 
		return -1;
	}
  close(fd);
  return 0;
}

//*****************************************************************************
//*****************************************************************************
int disable_shift(void)
{
  char buf[1] = "0";
	int fd = open("/sys/kernel/rhoy_servos/control_status", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	if (write(fd, buf,1) < 0) {
		perror("fwrite"); 
		return -1;
	}
  close(fd);
  return 0;
}


//*****************************************************************************
//*****************************************************************************
int set_pid(void)
{

	char buf[10];
	int fd = open("/sys/kernel/rhoy_servos/pid", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	sprintf(buf, "%i", getpid());
	if (write(fd, buf, strlen(buf) + 1) < 0) {
		perror("fwrite"); 
		return -1;
	}
  close(fd);
  return 0;
}

//*****************************************************************************
//*****************************************************************************
int clear_pid(void)
{

	char buf[10];
	int fd = open("/sys/kernel/rhoy_servos/pid", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	
 memset(buf,0,10);
 if (write(fd, buf, strlen(buf) + 1) < 0) {
		perror("fwrite"); 
		return -1;
	}
  close(fd);
  return 0;
}

//*****************************************************************************
//*****************************************************************************
int write_servo1(int val)
{
  char* buf = (char*)malloc(11);
  
  sprintf(buf, "0x%08x\n", val);
  
  /*if (val)
	  strcpy(buf, "0x00000672");
  else
	  strcpy(buf, "0x00000546");*/
  //strcpy(buf, "0x000005dc");
  printf("buffer val: %s\n", buf);
	int fd = open("/sys/kernel/rhoy_servos/servo_1", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	if (write(fd, buf,10) < 0) {
		perror("fwrite"); 
		return -1;
	}
  close(fd);
  free(buf);
  return 0;
}


//*****************************************************************************
//*****************************************************************************
int write_servo2(int val)
{
  char* buf = (char*)malloc(11);
  sprintf(buf, "0x%08x\n", val);
  //printf("buffer val: %s\n", buf);
	int fd = open("/sys/kernel/rhoy_servos/servo_2", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	if (write(fd, buf,10) < 0) {
		perror("fwrite"); 
		return -1;
	}
  close(fd);
  free(buf);
  return 0;
}




//*****************************************************************************
//*****************************************************************************
int write_servo3(int val)
{
  char* buf = (char*)malloc(11);
  sprintf(buf, "0x%08x\n", val);
  //printf("buffer val: %s\n", buf);
	int fd = open("/sys/kernel/rhoy_servos/servo_3", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	if (write(fd, buf,10) < 0) {
		perror("fwrite"); 
		return -1;
	}
  close(fd);
  free(buf);
  return 0;
}




//*****************************************************************************
//*****************************************************************************
int write_servo4(int val)
{
  char* buf = (char*)malloc(11);
  sprintf(buf, "0x%08x\n", val);
  //printf("buffer val: %s\n", buf);
	int fd = open("/sys/kernel/rhoy_servos/servo_4", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	if (write(fd, buf,10) < 0) {
		perror("fwrite"); 
		return -1;
	}
  close(fd);
  free(buf);
  return 0;
}




//*****************************************************************************
//*****************************************************************************
int write_servo5(int val)
{
  char* buf = (char*)malloc(11);
  sprintf(buf, "0x%08x\n", val);
  //printf("buffer val: %s\n", buf);
	int fd = open("/sys/kernel/rhoy_servos/servo_5", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	if (write(fd, buf,10) < 0) {
		perror("fwrite"); 
		return -1;
	}
  close(fd);
  free(buf);
  return 0;
}




//*****************************************************************************
//*****************************************************************************
int write_servo6(int val)
{
  char* buf = (char*)malloc(11);
  sprintf(buf, "0x%08x\n", val);
  //printf("buffer val: %s\n", buf);
	int fd = open("/sys/kernel/rhoy_servos/servo_6", O_WRONLY);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	if(write(fd, buf, 10) < 0) {
		perror("fwrite");
		return -1;
	}
 close(fd);
 free(buf);
 return 0;
}
