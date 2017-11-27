/*
 *	Club Robot ESEO 2009 - 2013
 *
 *	Fichier : QS_sys.c
 *	Package : Qualite Soft
 *	Description : Compatibilit� avec la libc - redirection de _read et _write vers la liaison s�rie choisie.
 *	Auteur : Gwenn, Alexis, Nirgal
 *	Version 2013
 */

#define QS_SYS_C

#include "stm32f1_sys.h"
#include "macro_types.h"
#include "stm32f1xx_hal.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <malloc.h>


#ifndef STDOUT_USART
	#define DEFAULT_STDOUT_USART UART1_ID
#endif

#ifndef STDERR_USART
	#define DEFAULT_STDERR_USART UART1_ID
#endif

#ifndef STDIN_USART
	#define DEFAULT_STDIN_USART UART1_ID
#endif

static uart_id_e stdout_usart = DEFAULT_STDOUT_USART;
static uart_id_e stderr_usart = DEFAULT_STDERR_USART;
static uart_id_e stdin_usart 	= DEFAULT_STDIN_USART;



void SYS_NVIC_PriorityGrouping(void)
{
	//Pas de subpriority sur les interruptions
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}


/*
 * Configure les horloges du STM32F103 selon les fr�quences suivantes :
 * 	Utilisation de l'oscillateur interne (8MHz)
 * 	HSI	(high speed internal)		= 8MHz
 * 		--> /2 						= 4MHz
 * 			--> PLL 		(*16) 	= 64MHz -> SYSCLK
 * 				--> AHB 	(/1)	= 64MHz	-> HCLK		(AHB, core, memory, DMA)
 * 				--> APB1	(/2)	= 32MHz	-> PCLK1	(APB1 peripherals)
 * 					--> 	(/2)	= 16MHz -> TIMXCLK 	(TIM2, TIM3, TIM4)
 * 				--> APB2	(/1)	= 64MHz -> PCLK2	(APB2 peripherals)
 * 					-->		(/1)	= 64MHz	-> TIM1CLK	(TIM1)
 * 					--> ADC (/x)	= ... 	-> ADCCLK	(ADC)
 * 	HSE (high speed external)		= d�sactiv�
 * 	LSE (low speed external)		= d�sactiv�
 * 	LSI (low speed internal)		= d�sactiv�
 *
 */
void SYS_ClockConfig(void)
{
	RCC_OscInitTypeDef osc = {0};
	osc.PLL.PLLState = RCC_PLL_ON;
	osc.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
	osc.PLL.PLLMUL = RCC_PLL_MUL16;
	osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	osc.HSIState = RCC_HSI_ON;
	osc.HSEState = RCC_HSE_OFF;
	osc.LSEState = RCC_LSE_OFF;
	HAL_RCC_OscConfig(&osc);

	RCC_ClkInitTypeDef clkconfig;
	clkconfig.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clkconfig.APB1CLKDivider = RCC_HCLK_DIV2;
	clkconfig.APB2CLKDivider = RCC_HCLK_DIV1;
	clkconfig.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkconfig.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_HCLK;
	HAL_RCC_ClockConfig(&clkconfig,FLASH_LATENCY_2);

	//Les lignes ci-dessous permettent d'observer sur la broche PA8 la fr�quence d'horloge syst�me.
	//BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_8,GPIO_MODE_AF_PP,GPIO_NOPULL,GPIO_SPEED_HIGH);
	//HAL_RCC_MCOConfig(RCC_MCO,RCC_MCO1SOURCE_SYSCLK,RCC_MCODIV_1);

	SystemCoreClockUpdate();
}


/*  Fonctions appel�es par la libc (comme printf)  */
// https://sites.google.com/site/stm32discovery/open-source-development-with-the-stm32-discovery/getting-newlib-to-work-with-stm32-and-code-sourcery-lite-eabi
/*
 environ
 A pointer to a list of environment variables and their values.
 For a minimal environment, this empty list is adequate:
 */
char *__env[1] = { 0 };
char **environ = __env;

__attribute__((weak))
int _write(int file, char *ptr, int len);

__attribute__((weak))
void _exit(int status) {
	_write(1, "exit", 4);
	while (1) {
		;
	}
}


__attribute__((weak))
int _open(char *path, int flags, ...)
{
	/* Pretend like we always fail */
	return -1;
}

__attribute__((weak))
int _close(int file) {
	return -1;
}



/* Functions */
void initialise_monitor_handles()
{
}



/*
 execve
 Transfer control to a new process. Minimal implementation (for a system without processes):
 */
__attribute__((weak))
int _execve(char *name, char **argv, char **env) {
	errno = ENOMEM;
	return -1;
}

/*
 fork
 Create a new process. Minimal implementation (for a system without processes):
 */
__attribute__((weak))
int _fork() {
	errno = EAGAIN;
	return -1;
}

/*
 fstat
 Status of an open file. For consistency with other minimal implementations in these examples,
 all files are regarded as character special devices.
 The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */
__attribute__((weak))
int _fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 getpid
 Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal implementation, for a system without processes:
 */
__attribute__((weak))
int _getpid() {
	return 1;
}

/*
 isatty
 Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
__attribute__((weak))
int _isatty(int file) {
	switch (file){
		case STDOUT_FILENO:
		case STDERR_FILENO:
		case STDIN_FILENO:
			return 1;
		default:
			//errno = ENOTTY;
			errno = EBADF;
			return 0;
	}
}

/*
 kill
 Send a signal. Minimal implementation:
 */
__attribute__((weak))
int _kill(int pid, int sig) {
	errno = EINVAL;
	return (-1);
}

/*
 link
 Establish a new name for an existing file. Minimal implementation:
 */
__attribute__((weak))
int _link(char *old, char *new) {
	errno = EMLINK;
	return -1;
}

/*
 lseek
 Set position in a file. Minimal implementation:
 */
__attribute__((weak))
int _lseek(int file, int ptr, int dir) {
	return 0;
}

/*
__attribute__((strong))
_PTR _malloc_r _PARAMS ((struct _reent * reent, size_t size))
{
	return NULL;
}
*/

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
/*
__attribute__((weak))
caddr_t _sbrk(int incr) {

	extern int heap_start; 	// Defined by the linker
	extern int heap_end;	// Defined by the linker

	static int * heap_current = 0;
	static int * heap_end_address = 0;

	int *prev_heap_end;

	if (heap_current == 0)
		heap_current = &heap_start;

	if(heap_end_address == 0)
		heap_end_address = &heap_end;

	prev_heap_end = heap_current;

	if (heap_current + incr >  heap_end_address)
	{
		_write (STDERR_FILENO, "Heap overflow\n", 25);
		errno = ENOMEM;
		return  (caddr_t) -1;
	}

	heap_current += incr;
	return (caddr_t) prev_heap_end;
}
*/

register char * stack_ptr asm("sp");

caddr_t _sbrk(int incr)
{
	extern char end asm("end");
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0)
		heap_end = &end;

	prev_heap_end = heap_end;
	if (heap_end + incr > stack_ptr)
	{
//		write(1, "Heap and stack collision\n", 25);
//		abort();
		errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}


void * _sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
  char *ret;

  errno = 0;
  if ((ret = (char *)(_sbrk (incr))) == (void *) -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}


#define NB_MALLOC   40
void malloc_test()
{
	uint32_t * ptr[NB_MALLOC];
	uint8_t i;
	for(i=0;i<NB_MALLOC;i++)
	{
		ptr[i] = malloc(500);
		debug_printf("malloc %d : %s\n", i, (*ptr)?"ok":"ko!");
	}
	for(i=0;i<NB_MALLOC;i++)
		free(*ptr);
}



/*
caddr_t _sbrk ( int incr )
{
  static unsigned char *heap = NULL;
  unsigned char *prev_heap;

  if (heap == NULL) {
    heap = (unsigned char *)&__HEAP_START;
  }
  prev_heap = heap;
  // check removed to show basic approach

  heap += incr;

  return (caddr_t) prev_heap;
}
*/



void SYS_set_std_usart(uart_id_e in, uart_id_e out, uart_id_e err)
{
	stdin_usart = in;
	stdout_usart = out;
	stderr_usart = err;
}


/*
 read
 Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 Returns -1 on error or blocks until the number of characters have been read.
 */
__attribute__((weak))
int _read(int file, char *ptr, int len) {
	int n;
	int num = 0;
	switch (file) {
		case STDIN_FILENO:
			for (n = 0; n < len; n++)
			{
				/*while ((stdin_usart->SR & USART_FLAG_RXNE) == (uint16_t)RESET);
				char c = (char)(stdin_usart->DR & (uint16_t)0x01FF);*/
				char c;
				while(!UART_data_ready(stdin_usart));	//Blocant.
				c = UART_get_next_byte(stdin_usart);
				*ptr++ = c;
				num++;
			}
			break;
		default:
			errno = EBADF;
			return -1;
	}
	return num;
}

/*
 stat
 Status of a file (by name). Minimal implementation:
 int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */
__attribute__((weak))
int _stat(const char *filepath, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 times
 Timing information for current process. Minimal implementation:
 */
__attribute__((weak))
clock_t _times(struct tms *buf) {
	return -1;
}

/*
 unlink
 Remove a file's directory entry. Minimal implementation:
 */
__attribute__((weak))
int _unlink(char *name) {
	errno = ENOENT;
	return -1;
}

/*
 wait
 Wait for a child process. Minimal implementation:
 */
__attribute__((weak))
int _wait(int *status) {
	errno = ECHILD;
	return -1;
}

/*
 write
 Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 Returns -1 on error or number of bytes sent
 */
__attribute__((weak))
int _write(int file, char *ptr, int len) {
	int n;
	switch (file) {
		case STDOUT_FILENO: /*stdout*/
			//UART_puts(stdout_usart,ptr, len);

			for (n = 0; n < len; n++)
			{
				UART_putc(stdout_usart,*ptr++);
			}
			break;
		case STDERR_FILENO: /* stderr */
			for (n = 0; n < len; n++)
			{
				//while ((stderr_usart->SR & USART_FLAG_TC) == (uint16_t)RESET);
				//stderr_usart->DR = (*ptr++ & (uint16_t)0x01FF);
				UART_putc(stderr_usart,*ptr++);
			}
			break;
		default:
			errno = EBADF;
			return -1;
	}
	return len;
}

void assert_failed(uint8_t* file, uint32_t line)
{
	debug_printf("ASSERT FAILED file : %s, line %ld\n", file, line);
	while(1);
}


void dump_trap_info(uint32_t stack_ptr[], uint32_t lr) {
	extern char _estack;	//Defined by the linker, end of stack

	debug_printf("FATAL Error ! Exception %ld\n", __get_IPSR() & 0xFF);
	//See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/CHDBIBGJ.html
	/*  ISR_NUMBER
		This is the number of the current exception:
		0 = Thread mode
		1 = Reserved
		2 = NMI
		3 = HardFault
		4 = MemManage
		5 = BusFault
		6 = UsageFault
		7-10 = Reserved
		11 = SVCall
		12 = Reserved for Debug
		13 = Reserved
		14 = PendSV
		15 = SysTick
		16 = IRQ0.
	*/
	if(lr & 0x00000008)
		debug_printf("CPU was in thread mode\n");
	else debug_printf("CPU was in handler mode\n");

	int offset, i;
	offset = 0;

	debug_printf("CPU status was:\n");
	debug_printf("-  R0: 0x%08lX  R1: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1]); offset += 2;
	debug_printf("-  R2: 0x%08lX  R3: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1]); offset += 2;
	debug_printf("- R12: 0x%08lX\n", stack_ptr[offset++]);
	debug_printf("- LR:   0x%08lX\n", stack_ptr[offset++]);
	debug_printf("- PC:   0x%08lX\n", stack_ptr[offset++]);
	debug_printf("- xPSR: 0x%08lX\n\n", stack_ptr[offset++]);
	if(lr & 0x00000010) {
		debug_printf("FPU status was:\n");
		debug_printf("-  S0: 0x%08lX   S1: 0x%08lX   S2: 0x%08lX   S3: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("-  S4: 0x%08lX   S5: 0x%08lX   S6: 0x%08lX   S7: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("-  S8: 0x%08lX   S9: 0x%08lX  S10: 0x%08lX  S11: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("- S12: 0x%08lX  S13: 0x%08lX  S14: 0x%08lX  S15: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("- FPSCR: 0x%08lX\n", stack_ptr[offset++]);
		offset++; //empty value at end
	}

	debug_printf("Stack was: \n");
	for(i=0; i < 32 && (char*)&(stack_ptr[offset]) < &_estack; i++) {
		if(!((i + 1) % 4) && i)
			debug_printf("\n");
		debug_printf("0x%08lX ", stack_ptr[offset++]);
	}
	debug_printf("\n");


	debug_printf("END of Fault Handler\n");
}

__attribute__((naked)) void Fault_Handler(void)
{
	//On ne veux pas perdre l'�tat des registres, donc pas de C
	//l'attribut naked indique qu'on ne veux pas de prologue / epilogue g�n�r�s par GCC
	__asm volatile
	(
		"TST LR, #4\n"		// Test for MSP or PSP
		"ITE EQ\n"			//If equal
		"MRSEQ R0, MSP\n"	//r0 = msp
		"MRSNE R0, PSP\n"	//else r0 = psp
		"MOV R1, LR\n"
		"B dump_trap_info\n"
	);
}
void HardFault_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));
void MemManage_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));
void BusFault_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));
void UsageFault_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));

void NMI_Handler(void)
{
}

void SVC_Handler(void)
{
	debug_printf("SVC interrupt: unimplemented\n");
}

void DebugMon_Handler(void)
{
	debug_printf("DebugMon: unimplemented\n");
}

void PendSV_Handler(void)
{
	debug_printf("Pending SVC interrupt: unimplemented\n");
}





