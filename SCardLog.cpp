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

#include "SCardLog.h"
#ifdef WIN32
#include <Windows.h>
#endif

SCardLog::SCardLog(){}
SCardLog::~SCardLog(){}

FILE *SCardLog::openLog()
{
	FILE *debugfp = NULL;
#if defined(__APPLE__)
	struct stat buffer;
	std::string fileName;
	if(char *dir = getenv("TMPDIR"))
		fileName = std::string(dir) + "/smartcardpp.log";
  else
    fileName = "/tmp/smartcardpp.log";
	if(stat(fileName.c_str(), &buffer) >= 0)
	{
		debugfp=fopen(fileName.c_str(), "a+");
	}
#elif defined(WIN32)
	if(getenv("SMARTCARDPP_DEBUG")!= NULL)
	{
		WCHAR logfile[MAX_PATH * 4];
		GetTempPathW(sizeof(logfile)/sizeof(WCHAR),logfile);
		_snwprintf(logfile + wcslen(logfile),MAX_PATH,L"smartcardpp.log");
		debugfp=_wfsopen(logfile, L"a+", _SH_DENYNO);
	}
#else
	struct stat buffer;
	std::string fileName = "/tmp/smartcardpp.log";
	if(stat(fileName.c_str(), &buffer) >= 0)
	{
		debugfp=fopen(fileName.c_str(), "a+");
	}
#endif

	return debugfp;
}

void SCardLog::writeLog(const char *fmt,...)
{
	FILE *debugfp = openLog();
	if(debugfp != NULL)
	{
		time_t dbgTime;
		time(&dbgTime);
		struct tm *timeinfo = localtime(&dbgTime);
		struct timeval tim;
		gettimeofday(&tim, NULL);

		va_list vList ;
		va_start(vList,fmt);
		fprintf(debugfp, "%02i.%02i.%i %02i:%02i:%02i.%li [%s] ", timeinfo->tm_mday, 
									  timeinfo->tm_mon+1, 
									  (timeinfo->tm_year)-100, 
									  timeinfo->tm_hour, 
									  timeinfo->tm_min, 
									  timeinfo->tm_sec, 
									  (long)tim.tv_usec,
									  SCardLog::getParentProcName().c_str());
		
		vfprintf(debugfp, fmt, vList);
		fprintf(debugfp, "\n");
		va_end(vList);
		fflush(debugfp);
		fclose(debugfp);
		debugfp = NULL;
	}
}

void SCardLog::writeMessage(const char *fmt,...)
{
	FILE *debugfp = openLog();
	if(debugfp != NULL)
	{
		time_t dbgTime;
		time(&dbgTime);
		struct tm *timeinfo = localtime(&dbgTime);
		struct timeval tim;
		gettimeofday(&tim, NULL);
	        
		va_list vList ;
		char msg[8192] ;
		va_start(vList,fmt);
		vsnprintf(msg, 8192, fmt, vList) ;
		fprintf(debugfp, "%02i.%02i.%i %02i:%02i:%02i.%li [%s] %s\n", timeinfo->tm_mday, timeinfo->tm_mon+1, (timeinfo->tm_year)-100,
				timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, (long)tim.tv_usec, SCardLog::getParentProcName().c_str(), msg);
		fflush(debugfp);
		va_end(vList);
		fclose(debugfp);
		debugfp = NULL;
	}
}

void SCardLog::writeAPDULog(const char *func, int line, ByteVec apdu, DWORD protocol, bool isCmd, unsigned int connectionID, unsigned int transactionID)
{
	FILE *debugfp = openLog();
	if(debugfp != NULL)
	{
		std::stringstream APDU;
		time_t dbgTime;
		time(&dbgTime);
		struct tm *timeinfo = localtime(&dbgTime);
		struct timeval tim;
		gettimeofday(&tim, NULL);

		APDU.str("");
		for (ByteVec::iterator it = apdu.begin(); it < apdu.end(); it++)
		{
			APDU << "0x" << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (int) *it << ", ";
		}

		fprintf(debugfp, "%02i.%02i.%i %02i:%02i:%02i.%li [%s][%i:%i] [%s:%i] [%s] %s %s \n", timeinfo->tm_mday, timeinfo->tm_mon+1, (timeinfo->tm_year)-100,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, (long)tim.tv_usec, SCardLog::getParentProcName().c_str(), connectionID, transactionID, func, line, protocol==SCARD_PROTOCOL_T0 ? "T0" : "T1", isCmd ? "->" : "<-", APDU.str().c_str());
		fflush(debugfp);
		fclose(debugfp);
		debugfp = NULL;
	}
}

void SCardLog::writeByteVecLog(ByteVec buff, const char *msg)
{
	FILE *debugfp = openLog();
	if(debugfp != NULL)
	{
		std::stringstream buffStream;
		time_t dbgTime;
		time(&dbgTime);
		struct tm *timeinfo = localtime(&dbgTime);
		struct timeval tim;
		gettimeofday(&tim, NULL);

		buffStream.str("");
		for (ByteVec::iterator it = buff.begin(); it < buff.end(); it++)
			buffStream << std::hex << std::setfill('0') << std::setw(2) << (int) *it << " ";

		fprintf(debugfp, "%02i.%02i.%i %02i:%02i:%02i.%li [%s] [%s] %s with size: %i \n", timeinfo->tm_mday, timeinfo->tm_mon+1, (timeinfo->tm_year)-100,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, (long)tim.tv_usec, SCardLog::getParentProcName().c_str(), msg, buffStream.str().c_str(), (int)buff.size());
		fflush(debugfp);
		fclose(debugfp);
		debugfp = NULL;
	}
}

std::string SCardLog::getParentProcName()
{
    std::string procName = "";
#ifdef WIN32
	WCHAR _cname[256 * 4 ] = L"\0";
	HMODULE caller = GetModuleHandle(NULL);
	PWCHAR cname = _cname + 1;
	GetModuleFileNameW(caller,cname,256);

	PWCHAR fl = (PWCHAR)cname + lstrlenW(cname) - 1;
	while ('\\' != *fl)
		fl--;
	fl++;

	char procNameChar[1024];
	memset((void *)procNameChar, 0, sizeof(procNameChar));
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, fl, -1, procNameChar, sizeof(procNameChar), &DefChar, NULL);
	procName.append(procNameChar);
#else
#ifdef __APPLE__
	struct kinfo_proc info;
	size_t length = sizeof(struct kinfo_proc);
	int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
    
	if (sysctl(mib, 4, &info, &length, NULL, 0) < 0)
	{
		    procName.append("Unknown");
	}
	    else
	{
	    procName.append(info.kp_proc.p_comm);
	}
#else
	
	std::stringstream ss;
	ss << getpid() << ":" << pthread_self();
	procName = ss.str();
#endif
#endif
    return procName;
}

std::string SCardLog::getParentProcFullName()
{
	std::string procName = "";
#ifdef WIN32
	WCHAR _cname[256 * 4] = L"\0";
	HMODULE caller = GetModuleHandle(NULL);
	PWCHAR cname = _cname + 1;
	GetModuleFileNameW(caller, cname, 256);

	char procNameChar[1024];
	memset((void *)procNameChar, 0, sizeof(procNameChar));
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, cname, -1, procNameChar, sizeof(procNameChar), &DefChar, NULL);
	procName.append(procNameChar);
#else
#ifdef __APPLE__
	struct kinfo_proc info;
	size_t length = sizeof(struct kinfo_proc);
	int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };

	if (sysctl(mib, 4, &info, &length, NULL, 0) < 0)
	{
		procName.append("Unknown");
	}
	else
	{
		procName.append(info.kp_proc.p_comm);
	}
#else

	std::stringstream ss;
	ss << getpid() << ":" << pthread_self();
	procName = ss.str();
#endif
#endif
	return procName;
}

#ifdef WIN32
int SCardLog::gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag = 0;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    tmpres /= 10;  /*convert into microseconds*/
    /*converting file time to unix epoch*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }

  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}
#endif

void SCardLog::writeByteBufferLog(const char *func, int line, unsigned int connectionID, unsigned int transactionID, unsigned char *buff, int buffLength, const char *msg)
{
	FILE *debugfp = openLog();
	if(debugfp != NULL)
	{
		std::stringstream buffStream;
		time_t dbgTime;
		time(&dbgTime);
		struct tm *timeinfo = localtime(&dbgTime);
		struct timeval tim;
		gettimeofday(&tim, NULL);

		buffStream.str("");
		unsigned char *ptr = buff;
		for(int i = 0; i < buffLength; i++)
			buffStream << std::hex << std::setfill('0') << std::setw(2) << (int)ptr[i] << " ";

		fprintf(debugfp, "%02i.%02i.%i %02i:%02i:%02i.%li [%s] [%s:%i] [%i:%i] [%s] %s with size: %i \n", timeinfo->tm_mday, timeinfo->tm_mon+1, (timeinfo->tm_year)-100,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, (long)tim.tv_usec, SCardLog::getParentProcName().c_str(), func, line, connectionID, transactionID, msg, buffStream.str().c_str(), buffLength);
		fflush(debugfp);
		fclose(debugfp);
		debugfp = NULL;
	}
}

