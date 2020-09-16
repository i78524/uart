/******************************************************************************/
/* 處理uart函式                                                                */
/* Author:YiWen Chen                                                          */
/* Last update: 2020/07/28                                                    */
/*                                                                            */
/******************************************************************************/
#include "baseUart.h"

//------------------------------- Function source code start -------------------------------



void cPaserHeadTail(char* buf, int res)
{
  // do somethig...
}

/*主要工作函式*/
void recUART_new()
{
    printf("[recUART_new] init part 01\n");
    //************* uartWrite FIFO init start ***********
    // 以下為 uartWrite FIFO 做準備
    // 當有人將資料寫入至 UART_FIFO_4Write 時，select就會有反應
    // 未來就可以進一步將資料寫入至串列埠
    if (access(UART_FIFO_4Write, F_OK) == -1) {
        if (mkfifo(UART_FIFO_4Write, 0777) != 0) {
          perror(UART_FIFO_4Write);    
          printf("mkfifo UART_FIFO_4Write failed!\n");
        }
    }


    int fd4FIFO_uartWrite = -1;
    if ((fd4FIFO_uartWrite = open(UART_FIFO_4Write, O_RDWR|O_NONBLOCK)) == -1) {
        perror(UART_FIFO_4Write);    
        printf("open UART_FIFO_4Write fifo failed!");
    }
    // 一定要是O_RDWR http://stackoverflow.com/questions/14594508/fifo-pipe-is-always-readable-in-select
    // http://outflux.net/blog/archives/2008/03/09/using-select-on-a-fifo/
    //************* uartWrite FIFO init end ***********   





    
    printf("[recUART_new] init part 02\n");
    //************* Uart Read init start **************
    // 以下為串列埠本體做準備
    // 在Linux中將串列埠視為一個檔案，可以使用open()函數來開啟串列埠
    // 最後可以使用select來判斷串列埠是不是有動靜
    int fd4UARTdevice = -1;    
    if ((fd4UARTdevice = open(MODEMDEVICE, O_RDWR|O_NOCTTY|O_NONBLOCK)) < 0) {
        perror(MODEMDEVICE);
        printf("open MODEMDEVICE failed!");        
    }

    struct termios readio; // 在Linux中設定串列埠的參數，例如鮑率、字元長度等，readio在這裡負責設定read的屬性
    readio.c_cflag = BAUDRATE|CS8|CLOCAL|CREAD/*|ECHO*/;
    //readio.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);// Input，將串列埠設為非正規模式
    // readio.c_oflag &= ~OPOST; // Output，非正規模式，將OPOST選項設為disable
    readio.c_lflag = ICANON;  // 設定為正規模式  20200727測試後確定設定成正規也可以執行 
    readio.c_oflag |= OPOST;  // 設定為正規模式  20200727測試後確定設定成正規也可以執行
    readio.c_iflag = IGNPAR;  // 串列埠忽略同位錯誤，接收傳入的字元
    tcflush(fd4UARTdevice, TCIOFLUSH);// 清空發送與接收緩衝區
    tcsetattr(fd4UARTdevice, TCSANOW, &readio); // 用來將參數設定至指令的串列埠
    //************* Uart Read init End **************




    
    printf("[recUART_new] fd4UARTdevice =%d\n", fd4UARTdevice);
    printf("[recUART_new] fd4FIFO_uartWrite =%d\n", fd4FIFO_uartWrite);
    printf("[recUART_new] init part 03\n");
    //************* select init start ***********
    // 以下為select的工作做準備
    fd_set rfds;
    struct timeval tv;
    int maxFd; 
    int  res = 0;
    char buf[BALength + 1];// General Read buffer
    memset(buf, 0, BALength + 1); // 在這裡一次讀寫最多只有BALength長度, buf[BALength]永遠是0, 在string的處理上確保不會超過buf長度  
    //************* select init END ***********




         
    while(1) 
    {
        tv.tv_sec  = 0;
        tv.tv_usec = 5;
    
        FD_ZERO(&rfds);
        FD_SET(fd4UARTdevice, &rfds);
        FD_SET(fd4FIFO_uartWrite, &rfds);
        
        maxFd = fd4UARTdevice;
        if(maxFd < fd4FIFO_uartWrite) {
          maxFd = fd4FIFO_uartWrite;
        }


        if (select(maxFd + 1, &rfds, NULL, NULL, &tv) > 0) {
        
            if (FD_ISSET(fd4UARTdevice, &rfds))
            {
                res = read(fd4UARTdevice, buf, BALength);
                if(res > 0) {
                  buf[res] = 0; // 讓字串最後面一定是0 
                                    
                  if(0) { // debug Used
                      int i = 0;
                      for(i = 0; i < res; i++) {
                        printf("===>>> uart buf[%d] = %d = %c\n", i, buf[i], buf[i]);
                      }                
                  }

                  // 在這邊自行作parser處理，當然 接下來已經跟uart讀取沒有任何關係了
                  cPaserHeadTail(buf, res);
                }
            }


            if (FD_ISSET(fd4FIFO_uartWrite, &rfds))
            {
                res = read(fd4FIFO_uartWrite, buf, BALength);
                if(res > 0) {
                  buf[res] = 0; // 讓字串最後面一定是0 

                  write(fd4UARTdevice, buf, res);// 寫出去        
                }
            }

        }


    } // end of while
    
    close(fd4UARTdevice);
    close(fd4FIFO_uartWrite);
     
    pthread_exit(0);   
}






int writeStr_in_UARTFIFO4Write(char* inputStr)
{
    int fd4FIFO_uartWrite = -1;
    int failCount = 0;
    

    // Access
    if (access(UART_FIFO_4Write, F_OK) == -1) {
        if (mkfifo(UART_FIFO_4Write, 0777) != 0) {
            printf("[writeStr_in_UARTFIFO4Write] Could not create UART_FIFO_4Write\n");
            usleep(1000);
            return -1;
        }
    }
    
    // OPEN
    while ((fd4FIFO_uartWrite = open(UART_FIFO_4Write, O_WRONLY)) == -1) {
        failCount++;
        if(failCount > 5) {
          break;
        }
        
        printf("[writeStr_in_UARTFIFO4Write] Open UART_FIFO_4Write fifo failed");
        usleep(300);
    }
    
    
    // Write
    if (failCount > 5) {
      printf("[writeStr_in_UARTFIFO4Write] Open UART_FIFO_4Write fifo failed, Dont write....\n");
    } else {
      write(fd4FIFO_uartWrite, inputStr, strlen(inputStr) );    
    }


    // Close
    close(fd4FIFO_uartWrite); // 放在最後 close -1不會怎麼樣
}





void recUART_Test()
{
    while(1) 
    {
        writeStr_in_UARTFIFO4Write("HELLO_WORlD_testABCDEFGHIJKLMNOPQRSTUVWXYZ");
        sleep(1);
    } // end of while

    pthread_exit(0);   
}





int char2cint(char input)
{
      if(input == '0') {
          return 0x00;
      } else if(input == '1') {
          return 0x01;
      } else if (input == '2') {
          return 0x02;
      } else if (input == '3') {
          return 0x03;
      } else if (input == '4') {
          return 0x04;
      } else if (input == '5') {
          return 0x05;
      } else if (input == '6') {
          return 0x06;
      } else if (input == '7') {
          return 0x07;
      } else if (input == '8') {
          return 0x08;
      } else if (input == '9') {
          return 0x09;
      } else if (input == 'A') {
          return 0x0A;
      } else if (input == 'B') {
          return 0x0B;
      } else if (input == 'C') {
          return 0x0C;
      } else if (input == 'D') {
          return 0x0D;
      } else if (input == 'E') {
          return 0x0E;
      } else if (input == 'F') {
          return 0x0F;
     } else;
     
     return 0;

}





/*******************************************************************************
int sendUART(char* sendstring)
如果需要獨立發送再使用 通常建議發送時將資料送入fifo統一管理
也就是使用 recUART_new()

return 1 成功
return 0 失敗
*******************************************************************************/
int sendUART(char* sendstring)
{
    int fd4write = -1;

    fd4write = open(MODEMDEVICE, O_RDWR|O_NOCTTY|O_NONBLOCK);
    if (fd4write < 0) {
        perror(MODEMDEVICE);
        return -1;
    }

    struct termios writeio;
    bzero(&writeio, sizeof(writeio));
    writeio.c_cflag = BAUDRATE|CS8|CLOCAL|CREAD;
    writeio.c_iflag = IGNPAR;
    writeio.c_oflag = 0;
    writeio.c_lflag = ICANON;  // 設定為正規模式 
    
    
    tcflush(fd4write, TCOFLUSH); // 清空發送緩衝區
    tcsetattr(fd4write, TCSANOW, &writeio); // 設新的
    
    int res = 0;    
    res = write(fd4write, sendstring, strlen(sendstring));

    close(fd4write);
    //printf("[sendUART] input=%s, strlen=%d, res=%d\n", sendstring, strlen(sendstring), res);
    if(res == strlen(sendstring)) {
      return 1;
    } else {
      return 0;   
    }
}
