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

#include "CardBase.h"
#define log() (SCardLog::writeLog("[%i:%i][%s:%d]", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__))

CardError::CardError(byte a,byte b):runtime_error("invalid condition on card"),SW1(a),SW2(b)
{
	std::ostringstream buf;
	buf << "CardError:'" << runtime_error::what() << "'" << 
		" SW1:'0x" <<
		std::hex << std::setfill('0') <<
		std::setw(2) << ushort(a) << "'"
		" SW2:'0x" <<
		std::hex << std::setfill('0') <<
		std::setw(2) << ushort(b) << "'"
		;
	desc = buf.str();
}

CardBase::CardBase(PCSCManager *mgr)
{
	this->mManager = mgr;
	log();
}

CardBase::CardBase(PCSCManager *mManager, unsigned int idx)
{
	this->mManager = mManager;
	log();
	connect(idx);
}

void CardBase::connect(unsigned int idx)
{
	log();
	mManager->connect(idx);
	reIdentify();
}

CardBase::~CardBase(void)
{
	log();
}

ByteVec CardBase::getTag(int identTag,int len,ByteVec &arr)
{
	log();
	std::ostringstream oss;
	ByteVec::iterator iTag;
	iTag = find(arr.begin(),arr.end(),identTag);
	if (iTag == arr.end() )
	{
		oss <<  "fci tag not found, tag " << identTag;
		throw CardDataError( oss.str() );
		
	}
	if (len && *(iTag+1) != len)
	{
		oss <<  "fci tag " << identTag << " invalid length, expecting " <<
			len << " got " << int(*(iTag+1));
		throw CardDataError(oss.str());
	}

	return ByteVec(iTag + 2,iTag + 2 + *(iTag + 1));
}

CardBase::FCI CardBase::parseFCI(ByteVec fci)
{
	log();
	ByteVec tag;

	CardBase::FCI tmp;
	tmp.fileID = 0;
	if (fci.size() < 0x0B || (fci[0] != tagFCP && fci[0] != tagFCI)	|| fci.size()-2 != fci[1] )
		throw CardDataError("invalid fci record");

	fci = ByteVec(fci.begin()+ 2, fci.end());

	tag = this->getTag(0x83, 2, fci);
	if (tag.size() != 2)
		throw CardDataError("file name record invalid length, not two bytes");

	tmp.fileID = MAKEWORD(tag[1],tag[0]);

	tag = this->getTag(0x82, 0, fci);
	switch (tag[0] & 0x3F)
	{
		case 0x38: //DF
		case 0x01: //binary
			if (tag.size() != 1)
				throw CardDataError("linear variable file descriptor not 1 bytes long");
			tag = this->getTag(0x85,2,fci);
			tmp.fileLength = MAKEWORD(tag[1],tag[0]);
			break;
		case 0x02:
		//linear variable
		case 0x04:
			if (tag.size() != 5)
				throw CardDataError("linear variable file descriptor not 5 bytes long");
			tmp.recMaxLen	= MAKEWORD( tag[0x03], tag[0x02] );
			tmp.recCount	= tag[0x04];
			tmp.fileLength	= 0;
			break;

		default:
			throw CardDataError("invalid filelen record, unrecognized tag");
		}
	return tmp;
}


CardBase::FCI CardBase::selectMF(bool ignoreFCI)
{
	log();
	byte cmdMF[]= {CLAByte(), 0xA4, 0x00, byte(ignoreFCI ? 0x08 : 0x00), 0x00/*0x02,0x3F,0x00*/};
	ByteVec code;
	try
	{
		code = execute( MAKEVECTOR(cmdMF), ignoreFCI ? 2 : 4);
	}
	catch(...)
	{
		mManager->endTransaction(false);
		SCardLog::writeLog("[%i:%i][%s:%d] Communication error. Forsing transaction end", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
		throw;
	}
	if (ignoreFCI) return FCI();
	return parseFCI(code);
}

int CardBase::selectDF(int fileID,bool ignoreFCI)
{
	log();
	byte cmdSelectDF[] = {CLAByte(),0xA4,0x01,byte(ignoreFCI ? 0x08 : 0x04),0x02};
	ByteVec cmd(MAKEVECTOR(cmdSelectDF));
	cmd.push_back(HIBYTE(fileID));
	cmd.push_back(LOBYTE(fileID));
	ByteVec fcp;
	try
	{
		fcp =  execute(cmd, 4);
	}
	catch(...)
	{
		mManager->endTransaction(false);
		SCardLog::writeLog("[%i:%i][%s:%d] Communication error. Forsing transaction end", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
		throw;
	}
	if (ignoreFCI) return 0;
/*	FCI blah = */parseFCI(fcp);
	return 0;
}

CardBase::FCI CardBase::selectEF(int fileID,bool ignoreFCI)
{
	log();
	byte cmdSelectEF[] = {CLAByte(), 0xA4, 0x02, byte(ignoreFCI ? 0x08 : 0x04), 0x02 };
	ByteVec cmd(MAKEVECTOR(cmdSelectEF));
	cmd.push_back(HIBYTE(fileID));
	cmd.push_back(LOBYTE(fileID));
	ByteVec fci;
	try
	{
		fci = execute(cmd, 4);
	}
	catch(...)
	{
		mManager->endTransaction(false);
		SCardLog::writeLog("[%i:%i][%s:%d] Communication error. Forsing transaction end", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
		throw;
	}
	if (ignoreFCI)
		return FCI();
	return parseFCI(fci);
}

#define PACKETSIZE 254

ByteVec CardBase::readEF(unsigned int  fileLen) 
{
	log();
	byte cmdReadEF[] = {CLAByte(),0xB0,0x00,0x00,0x00};
	ByteVec cmd(MAKEVECTOR(cmdReadEF));

	ByteVec read(0);
	dword i=0;
	
	do {
		byte bytes = LOBYTE( i + PACKETSIZE > fileLen ? fileLen - i : PACKETSIZE );
		
		cmd[2] = HIBYTE(i); //offsethi
		cmd[3] = LOBYTE(i); //offsetlo
		cmd[4] = bytes; //count
		ByteVec ret;
		try
		{
			ret = execute(cmd);
		}
        catch(CardResetError &e)
        {
            throw e;
        }
		catch(...)
		{
			mManager->endTransaction(false);
			SCardLog::writeLog("[%i:%i][%s:%d] Communication error. Forsing transaction end", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
			throw;
		}
		if ( bytes != ret.size() ) 
			throw CardDataError("less bytes read from binary file than specified");

		read.insert(read.end(), ret.begin(),ret.end());
		i += PACKETSIZE ;
	} while (i < (fileLen - 1));
	return read;
}

ByteVec CardBase::readEFEx(unsigned int  fileLen) 
{
	log();
	byte cmdReadEF[] = {CLAByte(), 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00};
	ByteVec cmd(MAKEVECTOR(cmdReadEF));

	ByteVec read(0);
	size_t i=0;
	
	do
	{
		cmd[5] = HIBYTE(fileLen); //offsethi
		cmd[6] = LOBYTE(fileLen); //offsetlo
		ByteVec ret;
		try
		{
			ret = execute(cmd);
		}
        catch(CardResetError &e)
        {
            throw e;
        }
		catch(std::runtime_error &e)
		{
			throw e;
		}
		catch(...)
		{
			mManager->endTransaction(false);
			SCardLog::writeLog("[%i:%i][%s:%d] Communication error. Forsing transaction end", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
			throw;
		}
		SCardLog::writeLog("[%i:%i][%s:%d] Expected %i bytes, recieved %i bytes.", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, fileLen, ret.size());
		if ( fileLen != ret.size() )
		{
			
			throw CardDataError("Less bytes read from binary file than specified");
		}

		read.insert(read.end(), ret.begin(),ret.end());
		i += ret.size();
	}
	while (i < (fileLen - 1));
	
	return read;
}

ByteVec CardBase::readRecord(int numrec) 
{
	log();
	byte cmdReadREC[] = {CLAByte(),0xB2,0x00,0x04,0x00}; 
	cmdReadREC[2] = LOBYTE(numrec);
	ByteVec ret;
	try
	{
		ret = execute(MAKEVECTOR(cmdReadREC));
	}
	catch(...)
	{
		mManager->endTransaction(false);
		SCardLog::writeLog("[%i:%i][%s:%d] Communication error. Forsing transaction end", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
		throw;
	}
	return ret;
}

void CardBase::executePinEntry(ByteVec cmd)
{
	log();
	try
	{
		mManager->execPinEntryCommand(cmd);
	}
	catch(...)
	{
		mManager->endTransaction(false);
		SCardLog::writeLog("[%i:%i][%s:%d] Communication error. Forsing transaction end", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
		throw;
	}
}

void CardBase::executePinChange(ByteVec cmd, size_t oldPinLen,size_t newPinLen)
{
	log();
	try
	{
		mManager->execPinChangeCommand(cmd, oldPinLen, newPinLen);
	}
	catch(...)
	{
		mManager->endTransaction(false);
		SCardLog::writeLog("[%i:%i][%s:%d] Communication error. Forsing transaction end", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
		throw;
	}
}

void CardBase::setLogging(std::ostream * /*logStream*/)
{
	log();
}

ByteVec CardBase::execute(ByteVec cmd, int apducase)
{
	log();
	ByteVec RecvBuffer(65537); // 65535 + SW
	byte SW1 = 0x00;
	byte SW2 = 0x00;
	std::string readerState = "";
	int retriesLeft = 10;
	uint realLen = (uint) RecvBuffer.size() ;
	std::stringstream APDU;

	try
	{
		// T=1 needs Le byte for case 4 APDU-s
		if(mManager->getProtocol() == SCARD_PROTOCOL_T1 && apducase == 4)
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Protocol is T1. Pushing back one byte", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
			cmd.push_back((byte) 0x00); // 0x00 == "All data available, up to 256 bytes". Sufficient for our uses.
		}

		SCardLog::writeLog("[%i:%i][%s:%d] Executing APDU", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);

		while((SW1 == 0x00 && SW2 == 0x00) && retriesLeft > 0)
		{
			

			if(retriesLeft < 10)
			{
				SCardLog::writeLog("[%i:%i][%s:%d] Command send failed with status SW1 = 0x00 SW 2 == 0x00. Pausing and then will try again. Retry: %i ", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, 10 - retriesLeft);

				#ifdef WIN32
					Sleep(500);
				#else
					sleep(5);
				#endif

				SCardLog::writeLog("[%i:%i][%s:%d] Reconnecting", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
				//mManager->reconnect();
                
                mManager->resetCurrentConnection();
                SCardLog::writeLog("[%i:%i][%s:%d] Throwing CardResetError", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
                throw CardResetError();
			}
			SCardLog::writeLog("[%i:%i][%s:%d] Executing", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
            SCardLog::writeAPDULog(__FUNC__, __LINE__, cmd, mManager->getProtocol(), true, mManager->getConnectionID(), mManager->getTransactionId());
			mManager->execCommand(cmd, RecvBuffer, realLen);
			SCardLog::writeLog("[%i:%i][%s:%d] Recieved buffer size is: %i bytes", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, realLen);
			if(realLen < 2)
			{
                SCardLog::writeLog("[%i:%i][%s:%d] Recieved buffer size is less then two bytes: %i", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__,  realLen);
                SW1 = 0x00;
                SW2 = 0x00;
			}
            else
            {
                RecvBuffer.resize(realLen);
                SCardLog::writeLog("[%i:%i][%s:%d] Executed", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
                SW1 = RecvBuffer[realLen - 2];
                SW2 = RecvBuffer[realLen - 1];
            }
            
			if(SW1 == 0x00 && SW2 == 0x00)
				SCardLog::writeLog("[%i:%i][%s:%d] 0x%02X 0x%02X", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, SW1, SW2);
			else
				SCardLog::writeAPDULog(__FUNC__, __LINE__, RecvBuffer, mManager->getProtocol(), false, mManager->getConnectionID(), mManager->getTransactionId());

			SCardLog::writeLog("[%i:%i][%s:%d] SW1: %02x SW2: %02x", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, SW1, SW2);

			// chop off SW
			if(realLen > 2)
			  RecvBuffer.resize(realLen - 2);
			SCardLog::writeLog("[%i:%i][%s:%d] Buffer resized", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);

			int retriesCount = 0;
			while(SW1 == 0x61)
			{
				SCardLog::writeLog("[%i:%i][%s:%d] Executing retry command: %i", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, retriesCount);
				RecvBuffer.resize(258);
		
				realLen = (uint) RecvBuffer.size();
				byte cmdRead[]= {CLAByte(),0xC0,0x00,0x00,0x00};
				cmdRead[4] = SW2;
				ByteVec cmdReadVec = MAKEVECTOR(cmdRead);

				SCardLog::writeAPDULog(__FUNC__, __LINE__, cmdReadVec, mManager->getProtocol(), true, mManager->getConnectionID(), mManager->getTransactionId());

				SCardLog::writeLog("[%i:%i][%s:%d] Executing...", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
				mManager->execCommand(cmdReadVec, RecvBuffer, realLen);
				SCardLog::writeLog("[%i:%i][%s:%d] Recieved buffer size is: %i bytes", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, realLen);
				if(realLen < 2)
				{
                    SCardLog::writeLog("[%i:%i][%s:%d] Recieved buffer size is less then two bytes: %i", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__,  realLen);
                    SW1 = 0x00;
                    SW2 = 0x00;
				  //throw CardDataError("Zero-length input from cardmanager");
				}
				RecvBuffer.resize(realLen);

				

				SW1 = RecvBuffer[realLen - 2];
				SW2 = RecvBuffer[realLen - 1];
				if(SW1 == 0x00 && SW2 == 0x00)
					SCardLog::writeLog("[%i:%i][%s:%d] 0x%02X 0x%02X", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, SW1, SW2);
				else
					SCardLog::writeAPDULog(__FUNC__, __LINE__, RecvBuffer, mManager->getProtocol(), false, mManager->getConnectionID(), mManager->getTransactionId());
				if(realLen > 2)
				  RecvBuffer.resize(realLen - 2);
				SCardLog::writeLog("[%i:%i][%s:%d] Retry buffer resized", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);

				retriesCount++;
			}
			SCardLog::writeLog("[%i:%i][%s:%d] Will try again", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
			retriesLeft--;
		}
		if(retriesLeft <= 0)
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Card was reset more than 10 times. Command execution failed.", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
			throw CardDataError("Card is in use by some other process");
		}

		if (!(SW1 == 0x90 && SW2 == 0x00))
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Error: SW1 %02x SW2 %02x", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, SW1, SW2);
			throw CardError(SW1,SW2);
		}
		SCardLog::writeLog("[%i:%i][%s:%d] Rerturning OK", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
	}
	catch(CardError &e)
	{
		mManager->endTransaction(false);
		SCardLog::writeLog("[%i:%i][%s:%d] CardError:  SW1 %02x SW2 %02x", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, e.SW1, e.SW2);
		throw e;
	}
	catch(CardDataError &e)
	{
		mManager->endTransaction(false);
		SCardLog::writeLog("[%i:%i][%s:%d] CardError:  %s", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, e.what());
		throw e;
	}
	catch(CardResetError &e)
	{
		mManager->endTransaction(false);
		SCardLog::writeLog("[%i:%i][%s:%d] %s", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, e.what());
		throw e;
	}
	catch(std::runtime_error &e)
	{
		mManager->endTransaction(false);
		SCardLog::writeLog("[%i:%i][%s:%d] Runtime error: %s", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__, e.what());
		throw e;
	}
	catch(...)
	{
		mManager->endTransaction(false);
		SCardLog::writeLog("[%i:%i][%s:%d] Communication error. Forsing transaction end", mManager->getConnectionID(), mManager->getTransactionId(), __FUNC__, __LINE__);
		throw;
	}
	return RecvBuffer;
}
