/**
 * Well, everything was taken from stack overflow, with just a few modification.
 * You should go instead for IRQ interruption.
 */
#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <pthread.h>

//#include <sys/poll.h>
#include <poll.h>
#include "Postman_UART.h"
#include "DispenserManager.h"
#include "Dispenser.h"
//#include "Translator.h"
//#include <wiringSerial.h>
//#include <sys/ioctl.h>

//#define _GNU_SOURCE         /* See feature_test_macros(7) */
//#include <signal.h>


int uart0_filestream = -1;
static char rx_buffer[15];
static pthread_t postman_uart_thread;

/**
 * @def the same serial path are defined twice, 'cause it seems like it can not work once.
 */
static const char *SerialPath[] =
      {		"/dev/ttyUSB0",
      		"/dev/ttyUSB1",
      		"/dev/ttyUSB0",
      		"/dev/ttyUSB1"
	   };
static int SelectedSerialPath;

/**
 * @brief This function is called at the end of the init, to create the thread that will listen to the UART.
 */
static void Postman_UART_run();

/**
 * @brief This function is called by the listening thread on the UART it poll on the UART, and act when there
 * is data to read.
 */
static void Postman_UART_read();

/**
 * @brief This function is called when we need to transform a decimal number in binary (O-1 in a text) ?
 *
 * @Depreciated
 *
 * @param[in] int the decimal number to transform
 *
 * @return char* pointer to the text (0/1)
 */
static char* Postman_UART_decimal_to_binary(int);

/**
 * @brief This function is called when we want to transform a binary in a int. Don't know how it works or
 * if it works.
 *
 * @Depreciated
 *
 * @param[in] char, the binary ?
 *
 * @return int, the decimal number
 */
static int Postman_UART_binaryToDecimal(char);

/**
 * @brief This function is called when we want to transform a binary in a int. Don't know how it works or
 * if it works. It works better than the one before at least.
 *
 * @Depreciated
 *
 * @param[in] char, the binary ?
 *
 * @return int, the decimal number
 */
static int Postman_UART_bin2int(const char*);

/*
int main(){
	Postman_UART_init();
	while(1){
		Postman_UART_read();
	}
	Postman_UART_close();
	return 0;
}
*/



void Postman_UART_init(){
	//-------------------------
	//----- SETUP USART 0 -----
	//-------------------------
	//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
	
	//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.


	SelectedSerialPath = 0;

	do
	{
		uart0_filestream = open(SerialPath[SelectedSerialPath], O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
		printf("tried to open %s\n", SerialPath[SelectedSerialPath]);
		if (uart0_filestream == -1)
		{	
			//ERROR - CAN'T OPEN SERIAL PORT
			SelectedSerialPath++;
			//printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
		}
	}
	while ( (uart0_filestream == -1) && (SelectedSerialPath < sizeof (SerialPath) / sizeof(char*)));

	if(uart0_filestream == -1)
	{
		perror("Unable to open UART - Exit");
		exit(-2);
	}
	
	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);

	int check = pthread_create(&postman_uart_thread, NULL, (void*)&Postman_UART_run, NULL);
	if (check != 0) {
		perror("[Postman_UART] - Issue while creating Postman_UART thread\n");
	}
	
	printf("init succeed\n");
}

void Postman_UART_send(char* string){
	//----- TX BYTES -----
	uint8_t len = strlen(string);
	char* tx_buffer = string;		//TODO : à checker
	//unsigned char *p_tx_buffer;
	
	//p_tx_buffer = &tx_buffer[0];
	//*p_tx_buffer++ = 'a';
	
	if (uart0_filestream != -1)
	{
		//int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));		//Filestream, bytes to write, number of bytes to write
		int count = write(uart0_filestream, (void*)tx_buffer, len);		//Filestream, bytes to write, number of bytes to write
		if (count < 0)
		{
			printf("UART TX error\n");
		}
	}
	printf("sending hello\n");
}

void Postman_UART_close(){
	close(uart0_filestream);
}

static void Postman_UART_read(){
	//----- CHECK FOR ANY RX BYTES -----
	if (uart0_filestream != -1)													// Si UART open
	{
		struct pollfd src;
		src.fd = uart0_filestream;
		src.events = POLLIN;
		src.revents = 0;   
		int check = poll(&src, 1, -1);											// Interruption : réception sur l'UART

		if(check ==-1){
			perror("[Postman_UART] erreur avec l'interruption poll");
		}
		else if(check ==0){
			perror("[Postman_UART] truc chelou...");
		}
		else{																	// Lecture des caractères
			int rx_length = read(uart0_filestream, (void*)rx_buffer, LENGTH_RECEIVED_STRING);		//Filestream, buffer to store in, number of bytes to read (max)
			if (rx_length < 0)
			{
				perror("Error : No bytes to read ??");
				//An error occured (will occur if there are no bytes)
			}
			else if (rx_length == 0)
			{
				perror("Error : No bytes to read");
				//No data waiting
			}
			else if(rx_buffer[rx_length-1] == ']')
			{
				printf("%s\n", rx_buffer);
				if(rx_buffer[0] == '['){
					//uint8_t id, filling, battery;
					int id, filling, battery;
					printf("%s", rx_buffer);
					sscanf(rx_buffer, "[%c/%c/%c]", &id, &filling, &battery);
					printf("FINAL : %u, %u, %u\n", id, filling, battery);
					DispenserManager_add_dispenser((Dispenser_Id)id, "Bloup", (Battery)battery, (Filling)filling);
				}
				else
				{
					printf("%s\n", rx_buffer);
					perror ("Pas de début de message détecté");
				}
			}
		}
	}
}


static void Postman_UART_run(){
	//printf("bonjour mon ami, est-ce que tu aimes les patates?");
	//unsigned char rx_buffer[1];
	//int decVal;
	//Postman_UART_init();
	//int a = 1;
	while(1){
		Postman_UART_read();
	}
		//readingData();
		//printf("bytes converted: %d\n", (uint8_t)rx_buffer[0]);
		//transmittAck();
		/*if(rx_buffer[0] != 1){
			transmittAck();
		}
		rx_buffer[0] = 1;
		//usleep(1000000);
		int a = 1;
		//decVal = atoi(rx_buffer);
		//printf("%d\n", decVal);
		
		char *highByteVal=decimal_to_binary((int) rx_buffer[0]);
		printf("%s\n", highByteVal);
		char *lowByteVal=decimal_to_binary((int) rx_buffer[1]);
		printf("%s\n", lowByteVal);
		strcat(highByteVal, lowByteVal);
		
		//char analogVal [100] = highByteVal;
		
		printf("%s\n", highByteVal);
		long binResult = (long)highByteVal;
		int decResult = binaryToDecimal(binResult);
		//int result = binaryToDecimal(*highByteVal);
		int result = bin2int(highByteVal);
		printf("result final%d\n", result);
		
		
		if (a == 1){
			for(int b=0;b<2;b++){
				printf("%d\n", rx_buffer[b]);
			}
		a =0;
		}
		
			if (rx_buffer[0] != 0){
				decVal = atoi(rx_buffer);
				printf("%d\n", decVal);
			}
		//char s[] = "-975310";
		//printf(asc2int(rx_buffer)\n);
		//transmittData();
		//usleep(1000000);*/
	//}
}




static char *Postman_UART_decimal_to_binary(int n) {
   int c, d, count;
   char *pointer;
 
   count = 0;
   pointer = (char*)malloc(8+1);
 
   if ( pointer == NULL )
      exit(EXIT_FAILURE);
 
   for ( c = 7 ; c >= 0 ; c-- )
   {
      d = n >> c;
 
      if ( d & 1 )
         *(pointer+count) = 1 + '0';
      else
         *(pointer+count) = 0 + '0';
 
      count++;
   }
   *(pointer+count) = '\0';
 
   return  pointer;
}

/*int binaryToDecimal(long binarynum)
{
    int decimalnum = 0, temp = 0, remainder;
    while (binarynum!=0)
    {
        remainder = binarynum % 10;
        binarynum = binarynum / 10;
        decimalnum = decimalnum + remainder*pow(2,temp);
        temp++;
    }
    return decimalnum;
}*/

static int Postman_UART_binaryToDecimal(char bin) {
	//char bin; int dec = 0;
	//char bin = 0;
	//int dec = 0;
	//bin = 0;
	int dec = 0;
	while (bin != '\n') { 
		scanf("%c",&bin); 
		if (bin == '1') 
			dec = dec * 2 + 1; 
		else if (bin == '0') 
			dec *= 2; 
	} 
	return dec;
}


static int Postman_UART_bin2int(const char *bin) {
    int i, j;
    j = sizeof(int)*8;
    while ( (j--) && ((*bin=='0') || (*bin=='1')) ) {
        i <<= 1;
        if ( *bin=='1' ) i++;
        bin++;
    }
    return i;
}


