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
#ifndef PCSCMANAGER_H
#define PCSCMANAGER_H

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
	
	#include <sys/time.h>
	#ifndef __APPLE__
		#include <reader.h>
		//#define SCARD_E_NO_READERS_AVAILABLE 0x8010002EL
	#else
		#define ERROR_NO_MEDIA_IN_DRIVE          1112L
	#endif
#else
  #pragma warning(push)
  #pragma warning(disable:4201)
  #undef UNICODE
  #include <winscard.h>
  #pragma warning(pop)
  #include <Windows.h>
  #include <time.h>
  #include <WinNT.h>
  #include <Psapi.h>
  #include <stdlib.h>
#endif

#include <string.h>
#include <sys/stat.h>
#include "common.h"
#include "SCError.h"
#include "SCardLog.h"
#include "helperMacro.h"

#ifndef CM_IOCTL_GET_FEATURE_REQUEST

// FIXME: We should use internal-winscard.h from OpenSC project instead
//        This will clean up PCSCManager.h header considerably and
//        allows us to use Mingw32 for building

#include "internal-pcsc22.h"
#endif

#define SS(a) if ((theState & SCARD_STATE_##a ) == SCARD_STATE_##a) stateStr += string(#a) + string("|")

struct ConnectionBase;
struct PCSCConnection;


/// WinSCard/PCSCLite wrapper
/** PCSCManager represents WinSCard subsystem on Windows or PCSCLite libary
  on platforms where it is available. It loads the libraries dynamically to avoid
 linked-in dependencies */
class PCSCManager
{
private:
	bool mOwnContext;
	SCARDCONTEXT hContext;
	SCARDHANDLE hScard;
	std::vector<char > mReaders;
	std::vector<SCARD_READERSTATE> mReaderStates;
	unsigned int transactionID;
	unsigned int connectionID;
	int readerLanguageId;
	uint cIndex;
	
	#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
		#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
	#else
	  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
	#endif

	struct timezone 
	{
	  int  tz_minuteswest; /* minutes W of Greenwich */
	  int  tz_dsttime;     /* type of dst correction */
	};
    
    struct osver {
        int minor;
        int sub;
    } ;
    typedef struct osver osxver;

	void construct(void);
	void ensureReaders();
	void execPinCommand(/*ConnectionBase *c, */bool verify, std::vector<byte> &cmd);
	std::string translateReaderState(DWORD state);
	std::string translateDwEventState(DWORD state);
	void resetCurrentContext();

	DWORD proto;
	DWORD verify_ioctl;
	DWORD verify_ioctl_start;
	DWORD verify_ioctl_finish;
	DWORD modify_ioctl;
	DWORD modify_ioctl_start;
	DWORD modify_ioctl_finish;
	bool display;
	bool pinpad;
	std::string readerName;
	bool mOwnConnection;
	
#ifdef WIN32
	int gettimeofday(struct timeval *tv, struct timezone *tz);
#elif __APPLE__
    
    void macosx_ver(char *darwinversion, osxver *osxversion ) ;
    char *osversionString(void);
#endif

public:
	PCSCManager();
	PCSCManager(std::string readerName);
	PCSCManager(SCARDCONTEXT existingContext, SCARDHANDLE hScard);
	~PCSCManager(void);
	uint getReaderCount(bool forceRefresh = false);
	std::string getReaderName(uint idx);
	std::string getReaderName();
	std::string getReaderState(uint idx);
	std::string getReaderState();
	std::string getATRHex(uint idx);
	std::string getATRHex();
	bool isPinPad(uint index);
	bool isPinPad();
	void connect(uint idx);
	/// connect using an application-supplied connection handle
	void connect(SCARDHANDLE existingHandle);
	void reconnect();
	void reconnect(unsigned long SCARD_PROTO);
	void setReaderLanguageId(int langId);
	int getReaderLanguageId(void);
	int getTransactionId(void);
	uint getConnectionIndex();
	void setConnectionID();
	uint getConnectionID();
	unsigned long getProtocol();
	void makeConnection(uint idx);
	void makeConnection(std::string readerName);
	void deleteConnection(bool reset);
	void beginTransaction();
	void endTransaction(bool forceReset = false);
	void execCommand(std::vector<BYTE> &cmd,std::vector<BYTE> &recv, unsigned int &recvLen);
	void execPinEntryCommand(std::vector<byte> &cmd);
	void execPinChangeCommand(std::vector<byte> &cmd, size_t oldPinLen, size_t newPinLen);
	bool isT1Protocol();
	bool isOwnConnection();
	std::vector<std::string> getReadersList();
    void deleteContext(void);
    void resetCurrentConnection();
};

class CardError: public std::runtime_error
{
	const CardError operator=(const CardError &);
public:
	const byte SW1,SW2;
	std::string desc;
	CardError(byte a,byte b);
	virtual const char * what() const throw() {	return desc.c_str();} 
	virtual ~CardError() throw(){};
};

/// Exception class thrown when unexpected or inconistent data is read from card
class CardDataError: public std::runtime_error
{
public:
	CardDataError( std::string p):runtime_error(p) {}
};

/// Exception class for authentication errors, like wrong PIN input etc.
class AuthError :public CardError
{
public:
	bool m_blocked;
	bool m_badinput;
	bool m_aborted;
	AuthError(byte a,byte b) : CardError(a,b), m_blocked(false),m_badinput(false),m_aborted(false) {};
	AuthError(byte a,byte b,bool block) : CardError(a,b), m_blocked(block),m_badinput(false),m_aborted(false) {};
	AuthError(CardError _base) : CardError(_base) , m_blocked(false),m_badinput(false),m_aborted(false) {}
};

#endif
