/*
   This code was made by Odair Jose Sanson Neto as a internship project on Exati Tecnologia. Curitiba - (09/12/19)
   
   In this RTOS with 2 threads, i decided to declare one thread and start another explicitly in the main function, just for didactic reasons. 
   I used the FRDM-K64F board to develop this project.
   FRDM-K64F has only one LED in board. So I decided to connect two leds in the board, each one in a digital output port. 
   When i tested the code, i used 2 blue leds and a resistence of 20 Ohm in series with each one, to restrict the current.
*/

#include "mbed.h"


//Declaring the output ports used by the leds
DigitalOut led1(D8);
DigitalOut led2(D9);

//starting a thread
Thread thread;

//Mutex to protect the threads execution
Mutex mutex;

//Defining timers. I used the wait() function so the time must be in seconds.
#define timer_1     0.5
#define timer_2     0.2

//Declaring the spi ports, used to send a massage to another device.
SPI spi(D11, D12, D13); //D11 is mosi, D12 is miso and D13 is sclk
DigitalOut cs(D10);     //chip select port

//Function used by the first thread. This function blinks the led and send a message with a timer of 200 ms.
void thread_1() {
    //Constructs the first message with memory allocation
        char *message_1;
        message_1 = (char *)malloc(12*sizeof(char));
        
        message_1[0] = 'H';
        message_1[1] = 'e';
        message_1[2] = 'l';
        message_1[3] = 'l';
        message_1[4] = 'o';
        message_1[5] = ' ';
        message_1[6] = 'W';
        message_1[7] = 'o';
        message_1[8] = 'r';
        message_1[9] = 'l';
        message_1[10] = 'd';
        message_1[11] = '\0';
        
    while (true) {
        //Changes the digital level (HIGH to LOW and LOW to HIGH)
        led1 = !led1;
    
        //chip select turns LOW level to start the communication
        cs=0;
        
        //Send the first byte
        spi.write(0x7F);
        
        //Send the message
        for(int i =0; message_1[i]!='\0';i++)
        {  
            spi.write(message_1[i]);
        }
        
        //Send the CRC polynomial, to detect errors
        spi.write(0xC002);
        
        //Chip select turns HIGH level to stop communication
        cs=1;
        
        //Turns thread in waiting state
        wait(timer_1);
    }
    
    //Release the dinamic memory space 
    free(message_1);
}
 
int main() {
    
    //start mutex to protect the thread execution
    mutex.lock();
    
    //Deselect chip select
    cs = 1;
    
    //Defining the spi format
    spi.format(16,0);
    
    //Constructs the second message with memory allocation
    char  *message_2;
   
    message_2 = (char *)malloc(10*sizeof(char));

    message_2[0] = 'T';
    message_2[1] = 'h';
    message_2[2] = 'i';
    message_2[3] = 's';
    message_2[4] = ' ';
    message_2[5] = 'i';
    message_2[6] = 's';
    message_2[7] = ' ';
    message_2[8] = 'C';
    message_2[9] = '\0';
    
    //Starts the first thread by calling the function thread_1
    thread.start(thread_1);
    
    //Starts the second thread explicitly
    while (true) {
        
        //Make the second led blink
        led2 = !led2;
        
        //Here the same procedure is used to send the another message
        cs=0;
        spi.write(0x7F);
        
        for(int i =0; message_2[i]!='\0';i++)
        {  
            spi.write(message_2[i]);
        }
        
        spi.write(0xC002);
        
        wait(timer_2);
    }
    
    //Release the memory space
    free(message_2); 
    
    //Unlock mutex
    mutex.unlock();
}
