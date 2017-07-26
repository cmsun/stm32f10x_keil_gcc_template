/*
 *                                 提示：
 * arm-none-eabi-gcc默认scanf、sscanf、printf、sprintf、vsprintf等函数不支持对浮点数的格式化。
 * 如果要支持浮点数的格式化，要修改makefile文件。在arm-none-eabi-gcc命令和arm-none-eabi-g++
 * 命令后面加上-u_printf_float和-u_printf_float，或者加上LDFLAGS+=-u_scanf_float和
 * LDFLAGS+=-u_printf_float。
 * 比如原命令为：arm-none-eabi-g++ -mcpu=cortex-m3 –mthumb ...
 * 修改命令为：arm-none-eabi-g++ -u_printf_float -mcpu=cortex-m3 –mthumb ...
 * 然后重新编译就可以支持printf对浮点数的格式化。但是会增加可执行文件的大小。
 */

#ifndef SM_DEBUG_H
#define SM_DEBUG_H

#ifdef DEBUG

#ifdef __cplusplus
extern "C" {
#endif

extern void abort(void);
void Sm_printf(const char *format, ...);

#ifdef __cplusplus
}
#endif

#define Sm_Debug(format, args...) Sm_printf(format, ##args)
#define Sm_assert_failed(file, line) (Sm_Debug("assert failed: file:%s line:%d\n", file, line), abort())
#define Sm_assert(expr) ((expr) ? (void)0 : Sm_assert_failed(__FILE__, __LINE__))

#else

#define Sm_Debug(format, args...) ((void)0)
#define Sm_assert(expr) ((void)0)

#endif //DEBUG

#endif //SM_DEBUG_H
