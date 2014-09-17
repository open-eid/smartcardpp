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

#ifndef CARDBASE_H
#define CARDBASE_H

#include <cstdlib>
#include "common.h"
#include <algorithm>
#include <time.h>
#include "helperMacro.h"
#include <fstream>
#include "PCSCManager.h"

#ifdef WIN32
	#include <Windows.h>
	#include <WinNT.h>
	#include <Psapi.h>
#else
#include <unistd.h>
#endif

#define NULLWSTR(a) (a == NULL ? L"<NULL>" : a)

#define tagFCP 0x62 //file control parameters
#define tagFMD 0x64 //file management data
#define tagFCI 0x6F //file control information

using std::vector;

/// Exception class for smart card errors, keeps the SW1 SW2 codes


/// Represents basic ISO7816-4 smartcard
/** Represents a basic smart card, with basic ISO7816-4 command set implemented
 Concrete instances of smart cards with appropriate services can be derived from it
 or it can be used directly with basic command set. */
class CardBase
{
	const CardBase operator=(const CardBase &);
private:

public:
	/// File Control Info structure, parsed
	struct FCI
	{
		word	fileID;
		dword	fileLength;
		dword	recCount;
		dword	recMaxLen;
	} LPFCI;

	PCSCManager *mManager;
	/// Selects the Master File on card
	FCI selectMF(bool ignoreFCI = false);
	/// Selects Data File given by two-byte fileID
	int selectDF(int fileID,bool ignoreFCI = false);
	/// Selects Elementary File given by two-byte fileID
	FCI selectEF(int fileID,bool ignoreFCI = false);

	/// Constructor, call connect() to connect card to a reader
	CardBase(PCSCManager *mgr/*, SCardLog scardLog*/);
	/// Constructor, connects the card instance to the reader at index idx
	CardBase(PCSCManager *mManager, unsigned int idx/*, SCardLog scardLog*/);
	/// Constructor, connects the card instance to existing connection
	//CardBase(PCSCManager *mManager);

	~CardBase(void);
	/// connects the card instance to the reader at index idx
	void connect(unsigned int idx);
	/// virtual to be overridden by concrete cards, that can check for ATR or other card specific data
	bool isInReader(unsigned int idx) {UNUSED_ARG(idx);return false;}

	//FIXME: this should be pure virtual
	/// return card RSA key size in bits
	//virtual unsigned int getKeySize() { return 0; }
	unsigned int getKeySize();

	/// set logging stream. set to NULL for no logging ( default )
	void setLogging(std::ostream *logStream);

	/// helper to parse returned TLVs from card
	ByteVec getTag(int identTag,int len,ByteVec &arr);
	
	/// provide CLA for card APDUs
	byte CLAByte() { return 0x00; }

	
	/// Parses a File Control Infromation block from select file commands
	// FIXME: make this pure virtual
	FCI parseFCI(ByteVec fci);

	/// Reads a record from record-based Elementary File
	ByteVec readRecord(int numrec);
	/// Read entire binary Elementary File
	ByteVec readEF(unsigned int fileLen);
	/// Read entire binary Elementary File. extended APDU mode
	ByteVec readEFEx(unsigned int fileLen);
	/// perform a basic APDU command. noreply indicates that command expects no reply data apart from status bytes
	ByteVec execute(ByteVec cmd, int apducase = 0);
	/// perform pin entry command. this is only useful if card manager supports direct pin entry from reader like CTAPI
	void executePinEntry(ByteVec cmd);
	/// perform pin change command. useful if card manager supports direct pin entry
	void executePinChange(ByteVec cmd, size_t oldPinLen,size_t newPinLen);
	/* HACK. Implemented here because needed to be called on connect, which is in CardBase. */
	/// possibly re-identify the card by sending GET VERSION APDU.	
	void reIdentify() {};
};

#endif
