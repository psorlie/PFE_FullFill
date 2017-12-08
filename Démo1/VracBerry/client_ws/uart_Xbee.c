#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "uartTestCrossCompil.h"

int uart0_filestream = -1;
static unsigned char rx_buffer[2];
//int scaledResult = 0;
int a =1;
uint32_t maxAnalogVal= 3500;//3500;
uint32_t minAnalogVal= 1300;//2500;
uint32_t scaledResult = 0;
uint32_t buf = 50;

/************* Calcule un pourcentage aléatoire, pour la démo1 ************/
// On suppose a<b
int rand_a_b(int a, int b){
    return rand()%(b-a) +a;
}

/************* Récupération du pourcentage à envoyer au serveur ***********/
// string_to_send : pointeur vers la chaine à envoyer au serveur; size : taille de la chaine en question
size_t get_value(char* string_to_send){
	size_t size;
	if((scaledResult<101) && (scaledResult> 0)){
		size = sprintf( (char*)string_to_send, "%d", scaledResult);
		buf = scaledResult;
		return size;
	}
	else{
		size = sprintf( (char*)string_to_send, "%d", buf);
		return size;
	}
		
}

/************** Initialisation de l'UART ****************/
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
	uart0_filestream = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
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

/*************** Fonction d'envoi de données à l'UART, non utilisée pour la démo1 ***************/
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

/*************** Fermeture de l'UART ******************/
void closeUart(){
	//----- CLOSE THE UART -----
	close(uart0_filestream);
}

/*************** Lecture de données venant de l'UART ***************/
void readingData(){
	
	//----- CHECK FOR ANY RX BYTES -----
	if (uart0_filestream != -1)
	{
		int rx_length;
		printf("entre dans la boucle\n");
		// Read up to 255 characters from the port if they are there
		//static unsigned char rx_buffer[2];
		rx_length = read(uart0_filestream, (void*)rx_buffer, 2);		//Filestream, buffer to store in, number of bytes to read (max)
		if (rx_length < 0)
		{
			printf("An error occured (will occur if there are no bytes\n");
		}
		else if (rx_length == 0)
		{
			printf("No data waiting\n");
		}
		else
		{
			//Bytes received
			rx_buffer[rx_length] = '\0';
			printf("%i bytes read : %s\n", rx_length, rx_buffer);
		}
	}
}

/************ Transforme en binaire les données reçues par l'UART **********/
char *decimal_to_binary(int n)
{
   int c, d, count;
   char *pointer;
 
   count = 0;
   pointer = (char*)malloc(sizeof(char)*9);
 
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


/************* Transforme en entier la valeur reçue par le capteur analogique ************/
// Fonction à déporter au niveau du crotex M0
uint32_t binstr2int(char *bs){
    uint32_t ret = 0;
    uint32_t val = 1;
	int i;

    for(i=15; i>=0; i--){
       if (*(bs+i) == '1') ret = ret + val;
       val = val*2;
    }
    return (ret);
}

/*************** Fonction qui initialise l'UART, lit les données et renvoie le taux de remplissage *******************/
int receiveFormatSend (){
	if (a == 1){
		init();
		usleep(1000000);
		a=0;
	}	
	readingData();
	usleep(10000); //wait 10 ms
	char *highByteVal=decimal_to_binary((int) rx_buffer[0]);
	printf("MSB %s\n", highByteVal);
	char *lowByteVal=decimal_to_binary((int) rx_buffer[1]);
	printf("LSB %s\n", lowByteVal);
	strcat(highByteVal, lowByteVal);	
	printf("cat : %s \n", highByteVal);
	
	uint32_t result = binstr2int(highByteVal);
	
	//int result = bin2int(highByteVal);
	printf("result bin2int %d\n", result);
	scaledResult = (uint32_t)((result - (uint32_t)2536)/1.14);
	//scaledResult = 100-((uint32_t) (100*(result-minAnalogVal)/(maxAnalogVal-minAnalogVal)));
	printf("scaled result%d\n", scaledResult);
	return scaledResult;
}


