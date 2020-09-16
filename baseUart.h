/******************************************************************************/
/* 處理uart函式                                                                */
/* Author:YiWen Chen                                                          */
/* Last update: 2020/07/28                                                    */
/*                                                                            */
/******************************************************************************/
#ifndef bASEUART_V20170407_H
#define bASEUART_V20170407_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include <fcntl.h>   // for O_NOCTTY/O_NONBLOCK
#include <termios.h> // for UART B57600/IGNPAR

#include <unistd.h>  // for sleep
#include <pthread.h>  // for pthread
#include <time.h>


//------------------------------- Variable declaration start -------------------------------
#define BAUDRATE        B57600
#define MODEMDEVICE     "/dev/ttyS0" // uart裝置本體


#define BALength 64 // buf array length

#define UART_FIFO_4Write                "/tmp/uart_fifo_4Write"
//------------------------------ Function declaration start -------------------------------


//------------------------------- Function source code start -------------------------------
int char2cint(char input); // 暫時沒用到 目前備用中
void recUART_new(); // 讀取一般uart並且讀取fifo來發送uart 是最重要的uart函式
int writeStr_in_UARTFIFO4Write(char* inputStr); //如果需要發送uart 請用此函式 將str寫入至fifo


// 以下為測試函式================================================================
void recUART_Test(); //注意他是thread 形式


#endif  // #ifndef _H
