
#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <stdlib.h>
#include <string.h>
#include <math.h>


int uart0_filestream = -1;
static unsigned char rx_buffer[2];

void init(){
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
	uart0_filestream = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
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
	
	printf("init succeed\n");
}

void transmittData(){
	//----- TX BYTES -----
	unsigned char tx_buffer[20];
	unsigned char *p_tx_buffer;
	
	p_tx_buffer = &tx_buffer[0];
	*p_tx_buffer++ = 'H';
	*p_tx_buffer++ = 'e';
	*p_tx_buffer++ = 'l';
	*p_tx_buffer++ = 'l';
	*p_tx_buffer++ = 'o';
	
	
	
	if (uart0_filestream != -1)
	{
		int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));		//Filestream, bytes to write, number of bytes to write
		if (count < 0)
		{
			printf("UART TX error\n");
		}
	}
	printf("sending hello\n");
}

void closeUart(){
	//----- CLOSE THE UART -----
	close(uart0_filestream);
}

void readingData(){
	//----- CHECK FOR ANY RX BYTES -----
	if (uart0_filestream != -1)
	{
		// Read up to 255 characters from the port if they are there
		//static unsigned char rx_buffer[2];
		int rx_length = read(uart0_filestream, (void*)rx_buffer, 2);		//Filestream, buffer to store in, number of bytes to read (max)
		if (rx_length < 0)
		{
			//An error occured (will occur if there are no bytes)
		}
		else if (rx_length == 0)
		{
			//No data waiting
		}
		else
		{
			//Bytes received
			rx_buffer[rx_length] = '\0';
			printf("%i bytes read : %s\n", rx_length, rx_buffer);
		}
	}
}

char *decimal_to_binary(int n)
{
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

int binaryToDecimal(char bin){
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


int bin2int(const char *bin) 
{
    int i, j;
    j = sizeof(int)*8;
    while ( (j--) && ((*bin=='0') || (*bin=='1')) ) {
        i <<= 1;
        if ( *bin=='1' ) i++;
        bin++;
    }
    return i;
}


void main(){
	//unsigned char rx_buffer[1];
	//int decVal;
	int maxAnalogVal= 3500;
	int minAnalogVal= 2500;
	int scaledResult = 0;
	init();
	int a = 1;
	while(1){
		readingData();
		
		usleep(1000000);
		int a = 1;
		//decVal = atoi(rx_buffer);
		//printf("%d\n", decVal);
		
		char *highByteVal=decimal_to_binary((int) rx_buffer[0]);
		printf("%s\n", highByteVal);
		char *lowByteVal=decimal_to_binary((int) rx_buffer[1]);
		printf("%s\n", lowByteVal);
		strcat(highByteVal, lowByteVal);
		
		//char analogVal [100] = highByteVal;
		/*
		printf("%s\n", highByteVal);
		long binResult = (long)highByteVal;
		int decResult = binaryToDecimal(binResult);*/
		//int result = binaryToDecimal(*highByteVal);
		int result = bin2int(highByteVal);
		printf("result final%d\n", result);
		scaledResult = 100*(result/(maxAnalogVal-minAnalogVal));
		if (result < 2
		if (a == 1){
			for(int b=0;b<2;b++){
				printf("%d\n", rx_buffer[b]);
			}
		a =0;
		
		}
		
			/*if (rx_buffer[0] != 0){
				decVal = atoi(rx_buffer);
				printf("%d\n", decVal);
			}*/
		//char s[] = "-975310";
		//printf(asc2int(rx_buffer)\n);
		//transmittData();
		//usleep(1000000);
	}
}


