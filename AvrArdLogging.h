/*
 * AvrArdLogging.h
 *

About
--------------------

A simple logging library for Arduino and other AVR based projects that uses no code 
space if no log level is defined. Automatically stores log strings in PROGMEM.

Uses the Serial class on Arduino platforms and very simple, blocking USART access 
otherwise (room for improvement here).

Uses the AVR `printf()` libraries instead of `Stream.write` for portability and 
better readability of code using logging. Also, repeated use of `Serial.print` would 
also inflate code size quite a bit (same with streaming (`<<`) or cascadable print 
libraries), so `printf()` seemed more suitable for me - especially with today's memory 
sizes on AVR chips / Arduino boards.  
For the `printf()` libraries to be usable with Serial/USART, `stdout` is redirected, 
so one can also just do a `printf()` anywhere in code to output to Serial/USART.

Homepage: <https://github.com/felixstorm/AvrArdLogging>


Usage
--------------------

Add a single line with only `LOG_GLOBALS` on a global level in any one source file 
and initialize logging somewhere in code with `LOG_INIT(baudRate);` before you use it.

Define the desired log level in a global include file or individually in every 
source file before including this file:

	#define LOG_LEVEL 5
	#include "AvrArdLogging.h"

Log Levels are 0-5:

	0 or undefined => off
	1 => errors only (LOG_ERR)
	2 => plus warnings (LOG_WARN)
	3 => plus infos (LOG),
	4 => plus verbose (LOG_VERB)
	5 => plus debug (LOG_DBG)

Alternatively the log level symbol may be defined globally as a command line compiler 
option (e.g. in AtmelStudio 6 in `Project Properties - Toolchain - AVR/GNU C and/or 
C++ Compiler - Symbols` as `LOG_LEVEL=5`).

To log, use `LOG_ERR(fmt, ...)`, `LOG_WARN(fmt, ...)`, `LOG(fmt, ...)`, `LOG_VERB(fmt, 
...)`, `LOG_DBG(fmt, ...)` in your code (syntax like `printf()`, CRLF will be appended 
automatically):

	LOG_ERR("Error connecting to server, http status was %d", httpStatus);

Also, there are `LOGB_ERR(fmt, ...)`, `LOGC_ERR(fmt, ...)`, `LOGE_ERR(fmt, ...)` to 
_B_egin, _C_ontinue and _E_nd a log line (for the other levels respectively):

	LOGB_VERB("Waiting for sensor readings");
	pseudo code loop {
		LOGC_VERB(" - received %d", reading);
	}
	LOGE_VERB(" - done.");

Feel free to adjust my log event layouts to your taste ;-)


printf() Flavors
--------------------
The AVR `printf()` libraries come in three flavors/code sizes: minimal (very basic 
integer and string conversion facilities only), default and with floating point 
support. For details and respective supported `printf()` format string options, see:  
<http://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#gaa3b98c0d17b35642c0f3e4649092b9f1>

* Without any specific linker options, the default `printf()` library will be used.

* To only use the minimal version, undef `vfprintf` and include the `printf_min` library 
in the linker options:  
`-Wl,-u,vfprintf -lprintf_min`  
in AtmelStudio: in `Project Properties - Toolchain - AVR/GNU Linker` - in `General`, 
check "Use vprintf library" and in `Libraries` add `printf_min`)

* To enable floating point support, also undef `vfprintf` and include `printf_flt` 
and `m` (math) libraries:  
`-Wl,-u,vfprintf -lprintf_flt -lm`  
see above for AtmelStudio (Arduino core library must be in library list before `m`, 
otherwise floating point usage will throw linker errors)


Resources, My Inspirations etc.
--------------------
<http://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html>  
<http://playground.arduino.cc/Main/Printf>  
<http://code.google.com/p/arduino-library-dbg>  
<http://www.appelsiini.net/2011/simple-usart-with-avr-libc>  
<http://homepage.hispeed.ch/peterfleury/group__pfleury__uart.html>  


License: GPL
--------------------
<http://www.gnu.org/licenses/gpl.html>

*/


#ifndef AVRARDLOGGING_H_
#define AVRARDLOGGING_H_


#if defined(LOG_LEVEL) && LOG_LEVEL > 0


#ifdef ARDUINO

#define LOG_SER_INIT(baudRate) \
    Serial.begin(baudRate);
#define LOG_SER_PUTC(c) \
    Serial.write(c);

#else

#define LOG_SER_INIT(baudRate) \
    UBRR0 = F_CPU / ((baudRate) * 8l) - 1; \
    UCSR0A = _BV(U2X0); \
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); \
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
#define LOG_SER_PUTC(c) \
    loop_until_bit_is_set(UCSR0A, UDRE0); \
    UDR0 = c;

#endif


#define LOG_GLOBALS \
    static FILE serialOut = {0}; \
    static int serialPutc(char c, FILE *stream) \
    { \
        LOG_SER_PUTC(c); \
        return 0; \
    }

#define LOG_INIT(baudRate) \
    do { \
        LOG_SER_INIT(baudRate); \
        fdev_setup_stream(&serialOut, serialPutc, NULL, _FDEV_SETUP_WRITE); \
        stdout = &serialOut; \
    } while(0)

#define LOG_ERR_HEADER "\r\n********** ERROR **********\r\n"
#define LOG_ERR_FOOTER "\r\n***************************\r\n\r\n"
#define LOGB_ERR(fmt, ...) (printf_P(PSTR(LOG_ERR_HEADER fmt), ## __VA_ARGS__))
#define LOGC_ERR(fmt, ...) (printf_P(PSTR(fmt), ## __VA_ARGS__))
#define LOGE_ERR(fmt, ...) (printf_P(PSTR(fmt LOG_ERR_FOOTER), ## __VA_ARGS__))
#define LOG_ERR(fmt, ...) (printf_P(PSTR(LOG_ERR_HEADER fmt LOG_ERR_FOOTER), ## __VA_ARGS__))


#else

#define LOG_GLOBALS
#define LOG_INIT(baud) ((void)0)

#define LOGB_ERR(fmt, ...) ((void)0)
#define LOGC_ERR(fmt, ...) ((void)0)
#define LOGE_ERR(fmt, ...) ((void)0)
#define LOG_ERR(fmt, ...) ((void)0)

#endif


#if defined(LOG_LEVEL) && LOG_LEVEL > 1

#define LOG_WARN_HEADER "*** WARNING: "
#define LOG_WARN_FOOTER "\r\n"
#define LOGB_WARN(fmt, ...) (printf_P(PSTR(LOG_WARN_HEADER fmt), ## __VA_ARGS__))
#define LOGC_WARN(fmt, ...) (printf_P(PSTR(fmt), ## __VA_ARGS__))
#define LOGE_WARN(fmt, ...) (printf_P(PSTR(fmt LOG_WARN_FOOTER), ## __VA_ARGS__))
#define LOG_WARN(fmt, ...) (printf_P(PSTR(LOG_WARN_HEADER fmt LOG_WARN_FOOTER), ## __VA_ARGS__))

#else

#define LOGB_WARN(fmt, ...) ((void)0)
#define LOGC_WARN(fmt, ...) ((void)0)
#define LOGE_WARN(fmt, ...) ((void)0)
#define LOG_WARN(fmt, ...) ((void)0)

#endif


#if defined(LOG_LEVEL) && LOG_LEVEL > 2

#define LOGB(fmt, ...) (printf_P(PSTR(fmt), ## __VA_ARGS__))
#define LOGC(fmt, ...) (printf_P(PSTR(fmt), ## __VA_ARGS__))
#define LOGE(fmt, ...) (printf_P(PSTR(fmt "\r\n"), ## __VA_ARGS__))
#define LOG(fmt, ...) (printf_P(PSTR(fmt "\r\n"), ## __VA_ARGS__))

#else

#define LOGB(fmt, ...) ((void)0)
#define LOGC(fmt, ...) ((void)0)
#define LOGE(fmt, ...) ((void)0)
#define LOG(fmt, ...) ((void)0)

#endif


#if defined(LOG_LEVEL) && LOG_LEVEL > 3

#define LOGB_VERB(fmt, ...) (printf_P(PSTR(fmt), ## __VA_ARGS__))
#define LOGC_VERB(fmt, ...) (printf_P(PSTR(fmt), ## __VA_ARGS__))
#define LOGE_VERB(fmt, ...) (printf_P(PSTR(fmt "\r\n"), ## __VA_ARGS__))
#define LOG_VERB(fmt, ...) (printf_P(PSTR(fmt "\r\n"), ## __VA_ARGS__))

#else

#define LOGB_VERB(fmt, ...) ((void)0)
#define LOGC_VERB(fmt, ...) ((void)0)
#define LOGE_VERB(fmt, ...) ((void)0)
#define LOG_VERB(fmt, ...) ((void)0)

#endif


#if defined(LOG_LEVEL) && LOG_LEVEL > 4

#define LOGB_DBG(fmt, ...) (printf_P(PSTR(fmt), ## __VA_ARGS__))
#define LOGC_DBG(fmt, ...) (printf_P(PSTR(fmt), ## __VA_ARGS__))
#define LOGE_DBG(fmt, ...) (printf_P(PSTR(fmt "\r\n"), ## __VA_ARGS__))
#define LOG_DBG(fmt, ...) (printf_P(PSTR(fmt "\r\n"), ## __VA_ARGS__))

#else

#define LOGB_DBG(fmt, ...) ((void)0)
#define LOGC_DBG(fmt, ...) ((void)0)
#define LOGE_DBG(fmt, ...) ((void)0)
#define LOG_DBG(fmt, ...) ((void)0)

#endif


#endif /* AVRARDLOGGING_H_ */