#pragma import(__use_no_semihosting_swi)  
#include <stdio.h>
#include "stm32f10x.h"
#include "Sm_Debug.h"
  
struct __FILE { int handle; /* Add whatever you need here */ };  
FILE __stdout;  
FILE __stdin;  
      
int fputc(int ch, FILE *f)   
{  
    return ITM_SendChar(ch);  
}  
  
volatile int32_t ITM_RxBuffer;  
int fgetc(FILE *f)  
{  
  while (ITM_CheckChar() != 1) __NOP();  
  int ch = ITM_ReceiveChar();
  /* ITM_SendChar(ch); */
  return ch;  
}  
  
int ferror(FILE *f)  
{  
    /* Your implementation of ferror */  
    return EOF;  
}  
  
void _ttywrch(int ch)  
{  
    fputc(ch, &__stdout);  
}  
  
int __backspace()  
{  
    /* Sm_Debug("%c", '\b'); */
    return 0;  
}  
void _sys_exit(int return_code)  
{  
label:  
    goto label;  /* endless loop */  
}  
