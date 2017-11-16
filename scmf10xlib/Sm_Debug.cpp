#include <stdarg.h>
#include <stdio.h>
#include "Sm_Config.h"

#if defined(DEBUG) || defined(_DEBUG)

#if defined(DEBUG_VIA_USART)
#include "Sm_USART1.h"
Sm_USART1 DUSART;
#endif

extern "C" void Sm_printf(const char *format, ...)
{
    char buff[128];
    va_list ap;

    va_start(ap, format);
    int len = vsnprintf(buff, 128, format, ap);
    va_end(ap);

//使用st-link打印debug信息
#if defined(DEBUG_VIA_SWO)
    for(int i = 0; i < len; ++i)
        ITM_SendChar(buff[i]);
#endif

//使用openocd打印debug信息
#if defined(DEBUG_VIA_OPENOCD)
    trace_write(buff, len);
#endif

//使用串口打印debug信息
#if defined(DEBUG_VIA_USART)            
    DUSART.send((uint8_t *)buff, len);
#endif
}

#endif //DEBUG or _DEBUG
