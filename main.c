/******************************************************************************/
/* main進入點                                                                 */
/* Author:YiWen Chen                                                          */
/* Last update: 2020/07/28                                                    */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>  // for pthread

#include "baseUart.h"

 
int main(int argc, char *argv[])
{ 
    pthread_t pret_1, threadUART;    
    if ((pret_1 = pthread_create(&threadUART, NULL, (void*)&recUART_new, NULL)) != 0) {
        printf("uartLinkPolling thread creation failed!");
    }
    pthread_detach(threadUART); 
    
    while(1) {
      sleep(1);
      printf("in mian\n");
    }
}
