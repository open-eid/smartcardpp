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

#include "PCSCManager.h"

using std::string;

#ifndef EINVAL
    #define	EINVAL		22
#endif
#define log() (SCardLog::writeLog("[%i:%i][%s:%d]", connectionID, transactionID, __FUNC__, __LINE__))

PCSCManager::PCSCManager()
{
	setConnectionID();
	log();
	this->mOwnContext = true;
	this->transactionID = 0;
	
	this->cIndex = 0;
	this->readerLanguageId = 0x0409;
	this->proto = SCARD_PROTOCOL_T0;
	construct();
	
	SCError::check(SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext), connectionID, transactionID);
	SCardLog::writeLog("[%i:%i][%s:%d] SCardEstablishContext", connectionID, transactionID, __FUNC__, __LINE__);
	ensureReaders();
}

PCSCManager::PCSCManager(std::string readerName)
{
	log();
	this->mOwnContext = true;
	this->transactionID = 0;
	this->cIndex = 0;
	this->readerLanguageId = 0x0409;
	this->proto = SCARD_PROTOCOL_T0;
	construct();
	SCError::check(SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext), connectionID, transactionID);
	SCardLog::writeLog("[%i:%i][%s:%d] SCardEstablishContext", connectionID, transactionID, __FUNC__, __LINE__);
	ensureReaders();
}

PCSCManager::PCSCManager(SCARDCONTEXT existingContext, SCARDHANDLE hScard)
{
	setConnectionID();
	log();
	this->mOwnContext = false;
	this->transactionID = 0;
	this->cIndex = 0;
	this->readerLanguageId = 0x0409;
	construct();
	this->proto = SCARD_PROTOCOL_T0;
	this->hContext = existingContext;
	this->hScard = hScard;
}

void PCSCManager::construct()
{
	log();
}

PCSCManager::~PCSCManager(void)
{
	log();
	if (mOwnContext)
	{
        deleteConnection(true);
		mReaders.clear();
		SCardReleaseContext(this->hContext);
		SCardLog::writeLog("[%i:%i][%s:%d] SCardReleaseContext", connectionID, transactionID, __FUNC__, __LINE__);
	}
}

void PCSCManager::deleteContext(void)
{
	log();
	SCardReleaseContext(this->hContext);
	SCardLog::writeLog("[%i:%i][%s:%d] SCardReleaseContext", connectionID, transactionID, __FUNC__, __LINE__);
}

void PCSCManager::ensureReaders()
{
	log();
	DWORD ccReaders = 0;
	DWORD mReadersSize = 0;
	
	SCardListReaders(hContext,NULL,NULL,&ccReaders);
	if (ccReaders == 0)
	{
		mReaders.clear();
		mReadersSize = 0;
		mReaderStates.clear();
		resetCurrentContext();
		SCardListReaders(hContext, NULL, NULL, &ccReaders);
		SCardLog::writeLog("[%i:%i][%s:%d] SCardListReaders returned %i", connectionID, transactionID, __FUNC__, __LINE__, ccReaders);
		if (ccReaders == 0)
			return;
	}

	mReadersSize = (DWORD)mReaders.size();

	if (ccReaders != mReadersSize)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Readers buffer has changed %i != %i. Refreshing....", connectionID, transactionID, __FUNC__, __LINE__, mReadersSize, ccReaders);
		
		SCError::check(SCardListReaders(hContext,NULL,NULL,&ccReaders), connectionID, transactionID);
		if (ccReaders == 0)
		{
			mReaderStates.clear();
			return;
		}

		mReaderStates.clear();
		mReaders.clear();
		mReaders.resize(ccReaders);
		SCError::check(SCardListReaders(hContext,NULL,&mReaders[0],&ccReaders), connectionID, transactionID);

		char *p = &mReaders[0];
		while(p < &*(--mReaders.end()))
		{
			SCARD_READERSTATE s = {p,NULL,SCARD_STATE_UNAWARE,0,0,{'\0'}};
			mReaderStates.push_back(s);
			p+= string(p).length() + 1;
		}

		SCardLog::writeLog("[%i:%i][%s:%d] Final list size %i", connectionID, transactionID, __FUNC__, __LINE__, mReaderStates.size());

		if (mReaderStates.size() ==  0 )
			throw SCError(SCARD_E_READER_UNAVAILABLE);
	}
	else
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Readers buffer stable: reported = %i, cached = %i", connectionID, transactionID, __FUNC__, __LINE__, ccReaders, mReadersSize);
	}
#ifdef __APPLE__
	for(size_t i = 0; i < mReaderStates.size(); i++)
		SCError::check(SCardGetStatusChange(hContext, 5, &mReaderStates[i], 1), connectionID, transactionID);
#else
	SCError::check(SCardGetStatusChange(hContext, 0, &mReaderStates[0], DWORD(mReaderStates.size())), connectionID, transactionID);
#endif
	if (cIndex >= mReaderStates.size())
		throw std::range_error("ensureReaders: Index out of bounds");
	
	if(strlen(mReaderStates[cIndex].szReader) > 0)
	{
	  readerName = std::string(mReaderStates[cIndex].szReader);
	}
}

std::string PCSCManager::translateDwEventState(DWORD state)
{
	string stateString = "";
	if((state & SCARD_STATE_IGNORE) == SCARD_STATE_IGNORE)
		stateString.append("SCARD_STATE_IGNORE ");
	if((state & SCARD_STATE_CHANGED) == SCARD_STATE_CHANGED)
		stateString.append("SCARD_STATE_CHANGED ");
	if((state & SCARD_STATE_UNKNOWN) == SCARD_STATE_UNKNOWN)
		stateString.append("SCARD_STATE_UNKNOWN ");
	if((state & SCARD_STATE_UNAVAILABLE) == SCARD_STATE_UNAVAILABLE)
		stateString.append("SCARD_STATE_UNAVAILABLE ");
	if((state & SCARD_STATE_EMPTY) == SCARD_STATE_EMPTY)
		stateString.append("SCARD_STATE_EMPTY ");
	if((state & SCARD_STATE_PRESENT) == SCARD_STATE_PRESENT)
		stateString.append("SCARD_STATE_PRESENT ");
	if((state & SCARD_STATE_ATRMATCH) == SCARD_STATE_ATRMATCH)
		stateString.append("SCARD_STATE_ATRMATCH ");
	if((state & SCARD_STATE_EXCLUSIVE) == SCARD_STATE_EXCLUSIVE)
		stateString.append("SCARD_STATE_EXCLUSIVE ");
	if((state & SCARD_STATE_INUSE) == SCARD_STATE_INUSE)
		stateString.append("SCARD_STATE_INUSE ");
	if((state & SCARD_STATE_MUTE) == SCARD_STATE_MUTE)
		stateString.append("SCARD_STATE_MUTE ");
	return stateString;
}

uint PCSCManager::getReaderCount(bool forceRefresh)
{
	log();
	if (forceRefresh)
	{
		try
		{
			ensureReaders();
		}
		catch(SCError &err)
		{
			if (err.error == long(SCARD_E_NO_READERS_AVAILABLE) || err.error == long(SCARD_E_NO_SERVICE))
			{
				return 0;
			}
			else if (err.error == long(SCARD_E_SERVICE_STOPPED))
			{
				resetCurrentContext();
				makeConnection(cIndex);
				ensureReaders();
			}
			else
				throw err;
		}
	}

	for(uint i = 0; i < mReaderStates.size(); i++)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Reader name %s - %s", connectionID, transactionID, __FUNC__, __LINE__, mReaderStates[i].szReader, translateDwEventState(mReaderStates[i].dwEventState).c_str());
	}
	
	return (uint) mReaderStates.size();
}

std::string PCSCManager::getReaderName(uint idx)
{
	log();
	ensureReaders();
	if (idx > mReaderStates.size())
		return "Unknown reader";
	else
		return mReaderStates[idx].szReader;
}


std::string PCSCManager::getReaderName()
{
	ensureReaders();
	return this->readerName;
}


std::string PCSCManager::getReaderState(uint idx)
{
	if(mReaderStates.size() <= idx)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Index is greater than readers structure. Returning UNKNOWN", connectionID, transactionID, __FUNC__, __LINE__);
		return "UNKNOWN";
	}
	else
	{
		std::string readerStatus = translateReaderState(mReaderStates[idx].dwEventState);
		SCardLog::writeLog("[%i:%i][%s:%d] Reader status %s", connectionID, transactionID, __FUNC__, __LINE__, readerStatus.c_str());
		return readerStatus;
	}
}

std::string PCSCManager::translateReaderState(DWORD state)
{
	DWORD theState = state;
	string stateStr = "";
	SS(IGNORE);
	SS(UNKNOWN);
	SS(UNAVAILABLE);
	SS(EMPTY);
	SS(PRESENT);
	SS(ATRMATCH);
	SS(EXCLUSIVE);
	SS(INUSE);
	SS(MUTE);
#ifdef SCARD_STATE_UNPOWERED
	SS(UNPOWERED);
#endif
	if (stateStr.length() > 0)
		stateStr = stateStr.substr(0,stateStr.length()-1);
	return stateStr ;
}

std::string PCSCManager::getReaderState()
{
	return this->getReaderState(this->cIndex);
}

std::string PCSCManager::getATRHex(uint idx)
{
	log();
	ensureReaders();
	std::ostringstream buf;
	buf << "";
	for(uint i=0;i<mReaderStates[idx].cbAtr;i++)
		buf << std::setfill('0') << std::setw(2) <<std::hex << (short) mReaderStates[idx].rgbAtr[i];
	string retval = buf.str();
	SCardLog::writeLog("[%i:%i][%s:%d] Retrieved ATR for connection index %i. %s", connectionID, transactionID, __FUNC__, __LINE__, idx, retval.c_str());
	return retval;
}

void PCSCManager::connect(uint idx)
{
	log();
	if (mReaderStates.size() < idx)
		throw PCSCManagerFailure();
	cIndex = idx;
	makeConnection(idx);
	setConnectionID();

	this->readerName.clear();
	this->readerName.append(mReaderStates[idx].szReader);
}

void PCSCManager::connect(SCARDHANDLE existingHandle)
{
	log();
#ifdef WIN32
	setConnectionID();
	DWORD cByte = 0;
	byte pbFriendlyName[512];
	memset((void*)pbFriendlyName, '\0', sizeof(pbFriendlyName));

	DWORD tmpProto = 0, sz=sizeof(DWORD);
	SCardLog::writeLog("[%i:%i][%s:%d] SCardGetAttrib", connectionID, transactionID, __FUNC__, __LINE__);
	SCError::check(SCardGetAttrib(existingHandle,SCARD_ATTR_CURRENT_PROTOCOL_TYPE, (LPBYTE)&tmpProto, &sz), connectionID, transactionID);
	if (tmpProto == SCARD_PROTOCOL_UNDEFINED)
		SCError::check(SCardReconnect(this->hScard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_RESET_CARD, &this->proto), connectionID, transactionID);
	else
		this->proto = tmpProto;

	this->readerName = "Unknown";
	long ret = SCardGetAttrib(this->hScard, SCARD_ATTR_DEVICE_FRIENDLY_NAME, NULL, &cByte);
	ret = SCardGetAttrib(this->hScard, SCARD_ATTR_DEVICE_FRIENDLY_NAME, pbFriendlyName, &cByte);
	if (ret == 0)
	{
		if (cByte > 0)
		{
			char *tmp = new char[cByte + 1];
			memcpy(tmp, pbFriendlyName, cByte);
			tmp[cByte] = '\0';
			readerName.clear();
			readerName.append(tmp);
			delete tmp;
		}
	}
	SCardLog::writeLog("[%i:%i][%s:%d] Reader name: %s", connectionID, transactionID, __FUNC__, __LINE__, this->readerName.c_str());
#else
	(void)existingHandle;
#endif
}

//Reconnect to card
void PCSCManager::reconnect()
{
	log();
#ifdef __APPLE__
    DWORD currentProto = SCARD_PROTOCOL_T0;
    SCardLog::writeLog("[%i:%i][%s:%d] SCardDisconnect", connectionID, transactionID, __FUNC__, __LINE__);
    SCardDisconnect(this->hScard, SCARD_RESET_CARD);
    SCardLog::writeLog("[%i:%i][%s:%d] SCardConnect to index %i", connectionID, transactionID, __FUNC__, __LINE__, cIndex);
    SCError::check(SCardConnect(hContext, mReaderStates[cIndex].szReader, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &this->hScard, &currentProto), connectionID, transactionID);
    proto = currentProto;
#else
	SCError::check(SCardReconnect(this->hScard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_RESET_CARD, &this->proto), connectionID, transactionID);
#endif
}

//Reconnect with card and set spetsified SCARD_PROTO
void PCSCManager::reconnect(unsigned long SCARD_PROTO)
{
	log();
#ifdef __APPLE__
    DWORD currentProto = SCARD_PROTOCOL_T0;
    SCardLog::writeLog("[%i:%i][%s:%d] SCardDisconnect", connectionID, transactionID, __FUNC__, __LINE__);
    SCardDisconnect(this->hScard, SCARD_RESET_CARD);
    SCardLog::writeLog("[%i:%i][%s:%d] SCardConnect to index %i", connectionID, transactionID, __FUNC__, __LINE__, cIndex);
    SCError::check(SCardConnect(hContext, mReaderStates[cIndex].szReader, SCARD_SHARE_SHARED, SCARD_PROTO, &this->hScard, &currentProto), connectionID, transactionID);
    proto = currentProto;
#else
	SCError::check(SCardReconnect(this->hScard, SCARD_SHARE_SHARED, SCARD_PROTO, SCARD_RESET_CARD, &this->proto), connectionID, transactionID);
#endif
}

std::string PCSCManager::getATRHex()
{
	log();
	byte atr[33]; // SC_MAX_ATR_SIZE
	DWORD atr_size = sizeof(atr);
	memset(atr, 0, atr_size);
	long lReturn = 1;
	if(!this->hScard)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Card context is NULL", connectionID, transactionID, __FUNC__, __LINE__);
		throw std::runtime_error("Card context is NULL");
	}
	SCardLog::writeLog("[%i:%i][%s:%d] SCardStatus", connectionID, transactionID, __FUNC__, __LINE__);
	
	lReturn = SCardStatus(this->hScard, NULL, NULL, NULL, NULL, atr, &atr_size);
	SCardLog::writeLog("[%i:%i][%s:%d] SCardStatus returned 0x80X", connectionID, transactionID, __FUNC__, __LINE__, lReturn);

	SCError::check(SCardStatus(this->hScard, NULL, NULL, NULL, NULL, atr, &atr_size), connectionID, transactionID);
	SCardLog::writeLog("[%i:%i][%s:%d]", connectionID, transactionID, __FUNC__, __LINE__);
	std::ostringstream buf;
	buf << "";
	for(uint i=0; i<atr_size; i++)
		buf << std::setfill('0') << std::setw(2) <<std::hex << (short) atr[i];
	string hexstring = buf.str();
	SCardLog::writeLog("[%i:%i][%s:%d]", connectionID, transactionID, __FUNC__, __LINE__);
	return hexstring;
}

bool PCSCManager::isPinPad()
{
	log();
	return isPinPad(cIndex);
}

bool PCSCManager::isPinPad(uint idx)
{
	log();

	DWORD i, feature_len, rcount;
	PCSC_TLV_STRUCTURE *pcsc_tlv;
	BYTE feature_buf[256], rbuf[256];
	
	memset((void *)&feature_buf, 0, sizeof(feature_buf));

	// Is there a pinpad?
	this->verify_ioctl = this->verify_ioctl_start = this->verify_ioctl_finish = 0;
	this->modify_ioctl = this->modify_ioctl_start = this->modify_ioctl_finish = 0;
	this->pinpad = this->display = false;

	SCardLog::writeLog("[%i:%i][%s:%d] -> 0x%X", connectionID, transactionID, __FUNC__, __LINE__, CM_IOCTL_GET_FEATURE_REQUEST);

	SCardLog::writeLog("[%i:%i][%s:%d] SCardControl", connectionID, transactionID, __FUNC__, __LINE__);

	long ret = SCardControl(this->hScard, CM_IOCTL_GET_FEATURE_REQUEST, NULL, 0, feature_buf, sizeof(feature_buf), &feature_len);

	while (ret == SCARD_W_RESET_CARD)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] SCARD_W_RESET_CARD", connectionID, transactionID, __FUNC__, __LINE__);
		reconnect();
		ret = SCardControl(this->hScard, CM_IOCTL_GET_FEATURE_REQUEST, NULL, 0, feature_buf, sizeof(feature_buf), &feature_len);
	}
	
	if(ret != SCARD_S_SUCCESS)
    {
        SCardLog::writeLog("[%i:%i][%s:%d] SCardControl ERROR CODE RECIEVED 0x%08X", connectionID, transactionID, __FUNC__, __LINE__, ret);
        this->pinpad = false;
        return this->pinpad;
    }

	SCardLog::writeByteBufferLog(__FUNC__, __LINE__, this->connectionID, this->transactionID, feature_buf, sizeof(feature_buf), "<-");

	feature_len /= sizeof(PCSC_TLV_STRUCTURE);
	pcsc_tlv = (PCSC_TLV_STRUCTURE *)feature_buf;

	for (i = 0; i < feature_len; i++)
	{
		if (pcsc_tlv[i].tag == FEATURE_VERIFY_PIN_DIRECT)
		{
			this->verify_ioctl = ntohl(pcsc_tlv[i].value);
			SCardLog::writeLog("[%i:%i][%s:%d] %s %s %s", connectionID, transactionID, __FUNC__, __LINE__, "Reader", getReaderName(idx).c_str(), "has reported FEATURE_VERIFY_PIN_DIRECT");
		}
		else if (pcsc_tlv[i].tag == FEATURE_VERIFY_PIN_START)
		{
			this->verify_ioctl_start = ntohl(pcsc_tlv[i].value);
			SCardLog::writeLog("[%i:%i][%s:%d] %s %s %s", connectionID, transactionID, __FUNC__, __LINE__, "Reader", getReaderName(idx).c_str(), "has reported FEATURE_VERIFY_PIN_START");
		}
		else if (pcsc_tlv[i].tag == FEATURE_VERIFY_PIN_FINISH) 
		{
			this->verify_ioctl_finish = ntohl(pcsc_tlv[i].value);
			SCardLog::writeLog("[%i:%i][%s:%d] %s %s %s", connectionID, transactionID, __FUNC__, __LINE__, "Reader", getReaderName(idx).c_str(), "has reported FEATURE_VERIFY_PIN_FINISH");
		}
		else if (pcsc_tlv[i].tag == FEATURE_MODIFY_PIN_DIRECT) 
		{
			this->modify_ioctl = ntohl(pcsc_tlv[i].value);
			SCardLog::writeLog("[%i:%i][%s:%d] %s %s %s", connectionID, transactionID, __FUNC__, __LINE__, "Reader", getReaderName(idx).c_str(), "has reported FEATURE_MODIFY_PIN_DIRECT");
		}
		else if (pcsc_tlv[i].tag == FEATURE_MODIFY_PIN_START) 
		{
			this->modify_ioctl_start = ntohl(pcsc_tlv[i].value);
			SCardLog::writeLog("[%i:%i][%s:%d] %s %s %s", connectionID, transactionID, __FUNC__, __LINE__, "Reader", getReaderName(idx).c_str(), "has reported FEATURE_MODIFY_PIN_START");
		}
		else if (pcsc_tlv[i].tag == FEATURE_MODIFY_PIN_FINISH) 
		{
			this->modify_ioctl_finish = ntohl(pcsc_tlv[i].value);
			SCardLog::writeLog("[%i:%i][%s:%d] %s %s %s", connectionID, transactionID, __FUNC__, __LINE__, "Reader", getReaderName(idx).c_str(), "has reported FEATURE_MODIFY_PIN_FINISH");
		}
		else if (pcsc_tlv[i].tag == FEATURE_IFD_PIN_PROPERTIES) 
		{
			SCardLog::writeLog("[%i:%i][%s:%d] %s %s %s", connectionID, transactionID, __FUNC__, __LINE__, "Reader", getReaderName(idx).c_str(), "has reported FEATURE_IFD_PIN_PROPERTIES");
			SCardLog::writeLog("[%i:%i][%s:%d] SCardControl", connectionID, transactionID, __FUNC__, __LINE__);
			if (SCardControl(this->hScard,  ntohl(pcsc_tlv[i].value), NULL, 0, rbuf, sizeof(rbuf), &rcount) == SCARD_S_SUCCESS)
			{
				PIN_PROPERTIES_STRUCTURE *caps = (PIN_PROPERTIES_STRUCTURE *)rbuf;
				if (caps->wLcdLayout > 0)
				{
					SCardLog::writeLog("[%i:%i][%s:%d] %s %s %s", connectionID, transactionID, __FUNC__, __LINE__, "Reader", getReaderName(idx).c_str(), "has reported LCD Display");
					this->display=true;
				}
			}
		}
	}

	if((this->verify_ioctl || (this->verify_ioctl_start && this->verify_ioctl_finish)) && (this->modify_ioctl || (this->modify_ioctl_start && this->modify_ioctl_finish)))
		this->pinpad = true;

	if(this->pinpad)
		SCardLog::writeLog("[%i:%i][%s:%d] %s %s %s", connectionID, transactionID, __FUNC__, __LINE__, "Reader", getReaderName(idx).c_str(), "has reported PINPAD");
	else
		SCardLog::writeLog("[%i:%i][%s:%d] %s %s %s", connectionID, transactionID, __FUNC__, __LINE__, "Reader", getReaderName(idx).c_str(), "has reported no PINPAD");

	return this->pinpad;
}

void PCSCManager::makeConnection(uint idx)
{
	log();
	DWORD currentProto = SCARD_PROTOCOL_T0;
	transactionID = 0;
	switch(this->proto)
	{
		case 0x00000000: SCardLog::writeLog("[%i:%i][%s:%d] Selected protocol: SCARD_PROTOCOL_UNDEFINED", connectionID, transactionID, __FUNC__, __LINE__); break;
		case SCARD_PROTOCOL_T0: SCardLog::writeLog("[%i:%i][%s:%d] Selected protocol: SCARD_PROTOCOL_T0", connectionID, transactionID, __FUNC__, __LINE__); break;
		case SCARD_PROTOCOL_T1: SCardLog::writeLog("[%i:%i][%s:%d] Selected protocol: SCARD_PROTOCOL_T1", connectionID, transactionID, __FUNC__, __LINE__); break;
		case SCARD_PROTOCOL_RAW: SCardLog::writeLog("[%i:%i][%s:%d] Selected protocol: SCARD_PROTOCOL_RAW", connectionID, transactionID, __FUNC__, __LINE__); break;
		default: SCardLog::writeLog("[%i:%i][%s:%d] Selected protocol unknown", connectionID, transactionID, __FUNC__, __LINE__);
	}
	SCardLog::writeLog("[%i:%i][%s:%d] SCardConnect", connectionID, transactionID, __FUNC__, __LINE__);
	if(idx >= mReaderStates.size())
		throw std::runtime_error("Index out of bounds");
	SCError::check(SCardConnect(hContext, mReaderStates[idx].szReader, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &this->hScard, &currentProto), connectionID, transactionID);

	proto = currentProto;
}

void PCSCManager::makeConnection(std::string readerName)
{
	log();
	DWORD currentProto = SCARD_PROTOCOL_T0;
	transactionID = 0;
	switch(this->proto)
	{
		case 0x00000000: SCardLog::writeLog("[%i:%i][%s:%d] Selected protocol: SCARD_PROTOCOL_UNDEFINED", connectionID, transactionID, __FUNC__, __LINE__); break;
		case SCARD_PROTOCOL_T0: SCardLog::writeLog("[%i:%i][%s:%d] Selected protocol: SCARD_PROTOCOL_T0", connectionID, transactionID, __FUNC__, __LINE__); break;
		case SCARD_PROTOCOL_T1: SCardLog::writeLog("[%i:%i][%s:%d] Selected protocol: SCARD_PROTOCOL_T1", connectionID, transactionID, __FUNC__, __LINE__); break;
		case SCARD_PROTOCOL_RAW: SCardLog::writeLog("[%i:%i][%s:%d] Selected protocol: SCARD_PROTOCOL_RAW", connectionID, transactionID, __FUNC__, __LINE__); break;
		default: SCardLog::writeLog("[%i:%i][%s:%d] Selected protocol unknown", connectionID, transactionID, __FUNC__, __LINE__);
	}
	unsigned int i = 0;
	do
	{
		if(readerName.compare(mReaderStates[i].szReader) == 0)
			break;
		i++;
	}
	while(i < mReaderStates.size());

	if(i <= mReaderStates.size())
		this->cIndex = i;

	this->readerName = readerName;
		
	SCardLog::writeLog("[%i:%i][%s:%d] SCardConnect", connectionID, transactionID, __FUNC__, __LINE__);
	SCError::check(SCardConnect(this->hContext, readerName.c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &this->hScard, &currentProto), connectionID, transactionID);
	proto = currentProto;
}

void PCSCManager::deleteConnection(bool reset)
{
	log();
	connectionID = 0;
	//scardlog.setConnectionID(0);
	/* FIXME: Don't check the return code with SCError, which will throw if something happens.
	 * deleteConnection is called from a destructor what means throwing is not allowed.
	 * Fix when removing ConnectionBase */
	//(*pSCardDisconnect)((( PCSCConnection *)c)->hScard,SCARD_RESET_CARD);
	SCardLog::writeLog("[%i:%i][%s:%d] SCardDisconnect", connectionID, transactionID, __FUNC__, __LINE__);
	SCardDisconnect(this->hScard, reset ? SCARD_RESET_CARD : SCARD_LEAVE_CARD);
}

void PCSCManager::beginTransaction()
{
	log();
	if(transactionID == 0)
	{
		struct timeval tim;
		gettimeofday(&tim, NULL);
		this->transactionID = tim.tv_usec+rand();
		SCardLog::writeLog("[%i:%i][%s:%d] SCardBeginTransaction", connectionID, transactionID, __FUNC__, __LINE__);
		SCError::check(SCardBeginTransaction(this->hScard), connectionID, transactionID);
	}
}

void PCSCManager::endTransaction(bool forceReset)
{
	log();
	if(this->transactionID == 0)
		return;
	
	
	if (forceReset)
	{ //workaround for reader driver bug
		BYTE _rdrBuf[1024];
		LPSTR reader = (LPSTR) _rdrBuf;
		DWORD rdrLen = sizeof(reader);
		DWORD state,proto,result,active;
		BYTE atr[1024];
		DWORD atrLen = sizeof(atr);
		SCardLog::writeLog("[%i:%i][%s:%d] SCardStatus", connectionID, transactionID, __FUNC__, __LINE__);
		result = SCardStatus(this->hScard,reader,&rdrLen,&state,&proto,atr,&atrLen);
		if (result == SCARD_W_RESET_CARD)
		{
			// FIXME: This will allow re-connecting with T1 even when T0 is forced
			SCardLog::writeLog("[%i:%i][%s:%d] SCardReconnect", connectionID, transactionID, __FUNC__, __LINE__);
			SCardReconnect(this->hScard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, SCARD_RESET_CARD, &active);
			SCardLog::writeLog("[%i:%i][%s:%d] SCardStatus", connectionID, transactionID, __FUNC__, __LINE__);
			SCardStatus(this->hScard, reader, &rdrLen, &state, &proto, atr, &atrLen);
		}
	}
	transactionID = 0;
	SCardLog::writeLog("[%i:%i][%s:%d] SCardEndTransaction", connectionID, transactionID, __FUNC__, __LINE__);
	SCardEndTransaction(this->hScard, forceReset ? SCARD_RESET_CARD : SCARD_LEAVE_CARD);
}

void PCSCManager::execCommand(std::vector<BYTE> &cmd,std::vector<BYTE> &recv, uint &recvLen)
{
	log();
	const SCARD_IO_REQUEST _MT0 = {1,8};
	const SCARD_IO_REQUEST _MT1 = {2,8};

	DWORD ret = recvLen;
	DWORD proto = getProtocol();
	SCardLog::writeLog("[%i:%i][%s:%d] SCardTransmit 0x%08X", connectionID, transactionID, __FUNC__, __LINE__, proto);

	SCError::check(SCardTransmit(this->hScard, proto == SCARD_PROTOCOL_T0 ? &_MT0 : &_MT1, &cmd[0], (DWORD)cmd.size(), NULL, &recv[0] , &ret), connectionID, transactionID);
	SCardLog::writeLog("[%i:%i][%s:%d] SCardTransmit OK", connectionID, transactionID, __FUNC__, __LINE__);
	recvLen = (uint)(ret);
}

#define SET_ESTEID_PINFORMAT(a) \
	a->bTimerOut = 30; \
	a->bTimerOut2 = 30; \
	a->bmFormatString = 0x02; /* Ascii */ \
	a->bmPINBlockString = 0x00; \
	a->bmPINLengthFormat = 0x00; \
	a->bEntryValidationCondition = 0x02; /* Keypress only */ \
	/* FIXME: Min/Max pin length should be passed to us from CardBase */ \
	a->wPINMaxExtraDigit = (4 << 8 ) + 12; /* little endian! */ \
	/* Ignore language and T=1 parameters. */ \
	a->wLangId = 0x0000; \
	a->bTeoPrologue[0] = 0x00; \
	a->bTeoPrologue[1] = 0x00; \
	a->bTeoPrologue[2] = 0x00;


void PCSCManager::execPinCommand(bool verify, std::vector<byte> &cmd)
{
	log();

	/* Force T=0 for PinPad commands. T=1 is terminally broken.
         * Actually the same check is done in makeConnection
         * so this is redundant.
         */
	if(getProtocol() != SCARD_PROTOCOL_T0 && this->mOwnConnection)
	{
		reconnect();
	}
	
	BYTE sbuf[256], rbuf[256];
	DWORD ioctl = 0, ioctl2 = 0, rlen=sizeof(rbuf), count = 0;
	size_t offset = 0;
    
	// build PC/SC block. FIXME: hardcoded for EstEID!!!
	if (verify)
	{
		PIN_VERIFY_STRUCTURE *pin_verify  = (PIN_VERIFY_STRUCTURE *)sbuf;
		SET_ESTEID_PINFORMAT(pin_verify);

		if(this->verify_ioctl_start)
		{
			ioctl = this->verify_ioctl_start;
			ioctl2 = this->verify_ioctl_finish;
		}
		else
		{
			ioctl = this->verify_ioctl;
		}

		/* Show default message on display */
		pin_verify->bNumberMessage = this->display ? 0xFF: 0x00;
		pin_verify->bMsgIndex = 0x00;

		pin_verify->wLangId = (uint16_t)this->readerLanguageId;

		/* Set proper command sizes (APDU header + placeholder for Lc) */
		pin_verify->ulDataLength = (uint32_t)cmd.size() + 1;
		count = sizeof(PIN_VERIFY_STRUCTURE) + pin_verify->ulDataLength - 1;
		offset = (byte *)(pin_verify->abData) - (byte *)pin_verify;

	}
	else
	{
		PIN_MODIFY_STRUCTURE *pin_modify  = (PIN_MODIFY_STRUCTURE *)sbuf;
		SET_ESTEID_PINFORMAT(pin_modify);

		if(this->modify_ioctl_start)
		{
			ioctl = this->modify_ioctl_start;
			ioctl2 = this->modify_ioctl_finish;
		}
		else
		{
			ioctl = this->modify_ioctl;
		}

		pin_modify->bInsertionOffsetOld = 0x00;
		pin_modify->bInsertionOffsetNew = 0x00;
		pin_modify->bConfirmPIN = 0x03;

		/* Default messages */
		pin_modify->bNumberMessage = this->display ? 0x03 : 0x00;
		pin_modify->bMsgIndex1 = 0x00;
		pin_modify->bMsgIndex2 = 0x01;
		pin_modify->bMsgIndex3 = 0x02;
		pin_modify->wLangId = (uint16_t)this->readerLanguageId;
		/* Set proper command sizes (APDU header + placeholder for Lc) */
		pin_modify->ulDataLength = (uint32_t)cmd.size() + 1;
		count = sizeof(PIN_MODIFY_STRUCTURE) + pin_modify->ulDataLength - 1;
		offset = (byte *)pin_modify->abData - (byte *)pin_modify;
	}

	/* Copy APDU itself */
	for (size_t i = 0; i < cmd.size(); i++)
		sbuf[offset + i] = cmd[i];
    
    /* Add Lc byte placeholder to APDU (will be recalculated by the IFD handler driver) */
    sbuf[offset + cmd.size() + 1] = 0x00;


	ByteVec tmp(MAKEVECTOR(sbuf));
	SCardLog::writeAPDULog(__FUNC__, __LINE__, tmp, getProtocol(), true, getConnectionIndex(), getTransactionId());

	SCardLog::writeLog("[%i:%i][%s:%d] SCardControl", connectionID, transactionID, __FUNC__, __LINE__);
	SCError::check(SCardControl(this->hScard, ioctl, sbuf, count, rbuf, sizeof(rbuf), &rlen), connectionID, transactionID);
	// finish a two phase operation
	if (ioctl2)
	{
		rlen = sizeof(rbuf);
		SCardLog::writeLog("[%i:%i][%s:%d] SCardControl", connectionID, transactionID, __FUNC__, __LINE__);
		SCError::check(SCardControl(this->hScard, ioctl2, NULL, 0, rbuf, sizeof(rbuf), &rlen), connectionID, transactionID);
	}
	ByteVec tmp2(MAKEVECTOR(rbuf));
	SCardLog::writeAPDULog(__FUNC__, __LINE__, tmp2, getProtocol(), false, getConnectionIndex(), getTransactionId());
	byte SW1 = rbuf[rlen - 2];
	byte SW2 = rbuf[rlen - 1];
	
	SCardLog::writeLog("[%i:%i][%s:%d] SW1 %02x SW2 %02x", connectionID, transactionID, __FUNC__, __LINE__, SW1, SW2);
	
	if (SW1 != 0x90)
	{
                // 0x00 - Timeout (SCM)
                // 0x01 - Cancel pressed (OK, SCM)
		AuthError err(SW1,SW2);
		if (SW1==0x64 && SW2 == 0x00)
		{
			err.m_timeout = true;
			throw err;
		}
		if (SW1==0x64 && SW2 == 0x01)
		{
			err.m_aborted = true;
			throw err;
		}
		if ( ( SW1==0x63 && SW2==0xC0 ) || //pin retry count 0
			( SW1==0x69 && SW2==0x83 ) ) //blocked
		{
			err.m_blocked = true;
			throw err;
		}
		if ( SW1==0x63 || //wrong pin
			( SW1==0x64 && SW2==0x02 ) || //password mistmatch
			( SW1==0x64 && SW2==0x03 ) ) //pin too short
			throw AuthError(SW1,SW2);

		throw CardError(SW1, SW2);
	}
}

void PCSCManager::execPinEntryCommand(std::vector<byte> &cmd)
{
	log();
	execPinCommand(true, cmd);
}

void PCSCManager::execPinChangeCommand(std::vector<byte> &cmd, size_t, size_t)
{
	log();
	execPinCommand(false, cmd);
}

bool PCSCManager::isT1Protocol()
{
	log();
	return getProtocol() == SCARD_PROTOCOL_T1;
}

void PCSCManager::setReaderLanguageId(int langId)
{
	log();
	this->readerLanguageId = langId;
}
int PCSCManager::getReaderLanguageId(void)
{
	log();
	return this->readerLanguageId;
}

int PCSCManager::getTransactionId(void)
{
    if(!transactionID)
        return 0;
    else
        return transactionID;
}

uint PCSCManager::getConnectionIndex()
{
	log();
	return cIndex;
}

void PCSCManager::setConnectionID()
{
	log();
	struct timeval tim;
	gettimeofday(&tim, NULL);
	connectionID = tim.tv_usec+rand();
}

uint PCSCManager::getConnectionID()
{
	return connectionID;
}

#ifdef WIN32
int PCSCManager::gettimeofday(struct timeval *tv, struct timezone *tz)
{
	log();
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

bool PCSCManager::isOwnConnection()
{
	log();
	return this->mOwnContext;
}

unsigned long PCSCManager::getProtocol()
{
	log();
	if (this->proto == 0)
	{
		log();
		throw PCSCManagerFailure();
	}
	if(this->proto == SCARD_PROTOCOL_T0)
	{
		log();
		return SCARD_PROTOCOL_T0;
	}
	else
	{
		log();
		return SCARD_PROTOCOL_T1;
	}
}

std::vector<std::string> PCSCManager::getReadersList()
{
	log();
	std::vector<std::string> readersList;
	this->ensureReaders();
	for(unsigned int i = 0; i < mReaderStates.size(); i++)
	{
		readersList.push_back(mReaderStates[i].szReader);
	}
	return readersList;
}

void PCSCManager::resetCurrentContext()
{
	log();
	if (mOwnContext)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] SCardReleaseContext", connectionID, transactionID, __FUNC__, __LINE__);
		SCardReleaseContext(this->hContext);
	}
	SCardLog::writeLog("[%i:%i][%s:%d] SCardEstablishContext", connectionID, transactionID, __FUNC__, __LINE__);
	SCError::check(SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext), connectionID, transactionID);
}

void PCSCManager::resetCurrentConnection()
{
    log();
#ifdef __APPLE__
    
    osxver foundver;
    char *osverstr = NULL;
    osverstr = osversionString();
    macosx_ver(osverstr, &foundver);
    
    SCardLog::writeLog("[%i:%i][%s:%d] Running on MacOS 10.%d,%d\n", connectionID, transactionID, __FUNC__, __LINE__, foundver.minor, foundver.sub);
    if(foundver.minor > 9)
    {
        deleteConnection(true);
        if (mOwnContext)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] SCardReleaseContext", connectionID, transactionID, __FUNC__, __LINE__);
            SCardReleaseContext(this->hContext);
            
            SCardLog::writeLog("[%i:%i][%s:%d] SCardEstablishContext", connectionID, transactionID, __FUNC__, __LINE__);
            SCError::check(SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext), connectionID, transactionID);
        }
        
        connect(this->cIndex);
        beginTransaction();
    }
    else
    {
        reconnect();
    }
    
#else
    reconnect();
#endif
}

#ifdef __APPLE__
char *PCSCManager::osversionString(void)
{
    int mib[2];
    size_t len;
    char *kernelVersion=NULL;
    mib[0] = CTL_KERN;
    mib[1] = KERN_OSRELEASE;
    
    if (sysctl(mib, 2, NULL, &len, NULL, 0) < 0 ) {
        fprintf(stderr,"%s: Error during sysctl probe call!\n",__PRETTY_FUNCTION__ );
        fflush(stdout);
        exit(4) ;
    }
    
    kernelVersion = (char *)malloc(len );
    if (kernelVersion == NULL ) {
        fprintf(stderr,"%s: Error during malloc!\n",__PRETTY_FUNCTION__ );
        fflush(stdout);
        exit(4) ;
    }
    if (sysctl(mib, 2, kernelVersion, &len, NULL, 0) < 0 ) {
        fprintf(stderr,"%s: Error during sysctl get verstring call!\n",__PRETTY_FUNCTION__ );
        fflush(stdout);
        exit(4) ;
    }
    
    return kernelVersion ;
}

void PCSCManager::macosx_ver(char *darwinversion, osxver *osxversion )
{
    /*
     From the book Mac Os X and IOS Internals:
     In version 10.1.1, Darwin (the core OS) was renumbered from v1.4.1 to 5.1,
     and since then has followed the OS X numbers consistently by being four
     numbers ahead of the minor version, and aligning its own minor with the
     sub-version.
     */
    
    int errno;
    char firstelm[2]= {0,0},secElm[2]={0,0};
    
    if (strlen(darwinversion) < 5 ) {
        fprintf(stderr,"%s: %s Can't possibly be a version string. Exiting\n",__PRETTY_FUNCTION__,darwinversion);
        fflush(stdout);
        exit(2);
    }
    char *s=darwinversion,*t=firstelm,*curdot=strchr(darwinversion,'.' );
    
    while ( s != curdot )
        *t++ = *s++;
    t=secElm ;
    curdot=strchr(++s,'.' );
    while ( s != curdot )
        *t++ = *s++;
    int maj=0, min=0;
    maj= (int)strtol(firstelm, (char **)NULL, 10);
    if ( maj == 0 && errno != EINVAL ) {
        fprintf(stderr,"%s Error during conversion of version string\n",__PRETTY_FUNCTION__);
        fflush(stdout);
        exit(4);
    }
    
    min=(int)strtol(secElm, (char **)NULL, 10);
    
    if ( min  == 0 && errno == EINVAL ) {
        fprintf(stderr,"%s: Error during conversion of version string\n",__PRETTY_FUNCTION__);
        fflush(stdout);
        exit(4);
    }
    osxversion->minor=maj-4;
    osxversion->sub=min;
}

#endif