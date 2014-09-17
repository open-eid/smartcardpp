/*
* SMARTCARDPP
* 
* This software is released under either the GNU Library General Public
* License (see LICENSE.LGPL) or the BSD License (see LICENSE.BSD).
* 
* Note that the only valid version of the LGPL license as far as this
* project is concerned is the original GNU Library General Public License
* Version 2.1, February 1999
*
*/

#ifndef SCARDLOG_H
#define SCARDLOG_H
#ifndef WIN32
    #include <stdio.h>
    #include <cstdlib>
    #include <stdarg.h>
    #include <unistd.h>
    #include <PCSC/wintypes.h>
    #include <PCSC/pcsclite.h>
    #include <PCSC/winscard.h>
    #include <arpa/inet.h>
    #include <sys/time.h>
    #include <cstring>
    #include <sys/time.h>
    #include <sys/sysctl.h>
    #ifndef __APPLE__
        #include <reader.h>
    #endif
    #define __FUNC__ __PRETTY_FUNCTION__
#else
    #pragma warning(push)
    #pragma warning(disable:4201)
    #undef UNICODE
    #include <winscard.h>
    #pragma warning(pop)
    #include <time.h>
    #define __FUNC__ __FUNCTION__
#endif
#include <cstdio>
#include <sys/stat.h>
#include "common.h"
class SCardLog
{
private:
	struct timezone 
	{
	  int  tz_minuteswest; /* minutes W of Greenwich */
	  int  tz_dsttime;     /* type of dst correction */
	};
    //char *procName;

	static std::string getParentProcName();
	#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
		#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
	#else
		#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
	#endif

	static FILE *openLog();

public:
	SCardLog();
	~SCardLog();
#ifdef WIN32
	static int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif
	static void writeByteBufferLog(const char *func, int line, unsigned int connectionID, unsigned int transactionID, unsigned char *buff, int buffLength, const char *msg);
	static void writeLog(const char *fmt,...);
	static void writeAPDULog(const char *func, int line, ByteVec apdu, DWORD protocol, bool isCmd, unsigned int connectionID, unsigned int transactionID);
    static void writeMessage(const char *fmt,...);
	static void writeByteVecLog(ByteVec buff, const char *msg);
	static std::string getParentProcFullName();
};

#endif