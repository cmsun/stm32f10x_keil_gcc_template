#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "Sm_Config.h"

#if defined(DEBUG) || defined(_DEBUG)

#if defined(DEBUG_VIA_USART)
#include "Sm_USART1.h"
Sm_USART1 DebugUSART;
#endif

extern "C" void Sm_printf(const char *format, ...)
{
    char buff[128];
    va_list ap;

    va_start(ap, format);
    vsnprintf(buff, 128, format, ap);
    va_end(ap);

//使用st-link打印debug信息
#if defined(DEBUG_VIA_SWO)
    int len = strlen(buff);
    for(int i = 0; i < len; ++i)
        ITM_SendChar(buff[i]);
#endif

//使用openocd打印debug信息
#if defined(DEBUG_VIA_OPENOCD)
    trace_write(buff, strlen(buff));
#endif

//使用串口打印debug信息
#if defined(DEBUG_VIA_USART)            
    DebugUSART.send((uint8_t *)buff, strlen(buff));
#endif
}

#endif //DEBUG or _DEBUG
