AvrArdLogging
====================


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
*B*egin, *C*ontinue and *E*nd a log line (for the other levels respectively):

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

Without any specific linker options, the default `printf()` library is used.

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


Links, My Inspirations etc.
--------------------
<http://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html>
<http://playground.arduino.cc/Main/Printf>
<http://code.google.com/p/arduino-library-dbg>
<http://www.appelsiini.net/2011/simple-usart-with-avr-libc>
<http://homepage.hispeed.ch/peterfleury/group__pfleury__uart.html>


License: GPL
--------------------
<http://www.gnu.org/licenses/gpl.html>
