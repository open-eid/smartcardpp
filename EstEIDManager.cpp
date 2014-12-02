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

#include "EstEIDManager.h"
#include <openssl/x509.h>
#define log() (SCardLog::writeLog("[%i:%i][%s:%d]", getConnectionID(), getTransactionID(), __FUNC__, __LINE__))

byte modEnv1[] = {0x00,0x22,0x41,0xA4,0x02,0x83,0x00};
byte modEnv2[] = {0x00,0x22,0x41,0xB6,0x02,0x83,0x00};
byte modEnv0[] = {0x00,0x22,0x41,0xB8,0x05,0x83,0x03,0x80};

std::string ESTEID_V1_COLD_ATR = "3bfe9400ff80b1fa451f034573744549442076657220312e3043";
std::string ESTEID_V1_WARM_ATR = "3b6e00ff4573744549442076657220312e30";
std::string ESTEID_V1_2007_COLD_ATR = "3bde18ffc080b1fe451f034573744549442076657220312e302b";
std::string ESTEID_V1_2007_WARM_ATR = "3b5e11ff4573744549442076657220312e30";
std::string ESTEID_V1_1_COLD_ATR = "3b6e00004573744549442076657220312e30";
std::string ESTEID_V3_COLD_DEV1_ATR = "3bfe1800008031fe454573744549442076657220312e30a8";
std::string ESTEID_V3_WARM_DEV1_ATR = "3bfe1800008031fe45803180664090a4561b168301900086";
std::string ESTEID_V3_WARM_DEV2_ATR = "3bfe1800008031fe45803180664090a4162a0083019000e1";
std::string ESTEID_V3_WARM_DEV3_ATR = "3bfe1800008031fe45803180664090a4162a00830f9000ef";
std::string ESTEID_V35_COLD_DEV1_ATR = "3bf9180000c00a31fe4553462d3443432d303181";
std::string ESTEID_V35_COLD_DEV2_ATR = "3bf81300008131fe454a434f5076323431b7";
std::string ESTEID_V35_COLD_DEV3_ATR = "3bfa1800008031fe45fe654944202f20504b4903";
std::string ESTEID_V35_WARM_ATR = "3bfe1800008031fe45803180664090a4162a00830f9000ef";

using namespace std;

EstEIDManager::EstEIDManager() : _card_version(VER_INVALID)
{
	try
	{
		mManager = NULL;
		cBase = NULL;
		authCertObj = NULL;
		signCertObj = NULL;
		mManager = new PCSCManager();
		SCardLog::writeLog("[%i:%i][%s:%d] Creating default connection", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
		cBase = new CardBase(mManager);
		this->_card_version = EstEIDManager::VER_INVALID;
		this->extAPDUSupported = false;
		checkPinPadDetection();
        checkExtendedAPDUSupport();
	}
	catch(...)
	{
		SCardLog::writeLog("[%i:%i][%s:%d]", 0, 0, __FUNC__, __LINE__);
	}
}

EstEIDManager::EstEIDManager(unsigned int idx) : _card_version(VER_INVALID)
{
	mManager = NULL;
	cBase = NULL;
	authCertObj = NULL;
	signCertObj = NULL;

	try
	{
		mManager = new PCSCManager();
		SCardLog::writeLog("[%i:%i][%s:%d] Connecting to index %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, idx);
		cBase = new CardBase(mManager);
		mManager->connect(idx);
		this->_card_version = EstEIDManager::VER_INVALID;
		this->extAPDUSupported = false;
		checkPinPadDetection();
        checkExtendedAPDUSupport();
	}
	catch(...)
	{
		SCardLog::writeLog("[%i:%i][%s:%d]", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
	}
}

EstEIDManager::EstEIDManager(std::string readerName) : _card_version(VER_INVALID)
{
	mManager = NULL;
	cBase = NULL;
	authCertObj = NULL;
	signCertObj = NULL;

	mManager = new PCSCManager(readerName);
	SCardLog::writeLog("[%i:%i][%s:%d] Connecting to reader %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, readerName.c_str());
	cBase = new CardBase(mManager);
	mManager->makeConnection(readerName);
	this->extAPDUSupported = false;
	this->_card_version = EstEIDManager::VER_INVALID;
	checkPinPadDetection();
    checkExtendedAPDUSupport();
}

EstEIDManager::EstEIDManager(SCARDCONTEXT scardCTX, SCARDHANDLE scard) : _card_version(VER_INVALID)
{
	mManager = NULL;
	cBase = NULL;
	authCertObj = NULL;
	signCertObj = NULL;
	
	mManager = new PCSCManager(scardCTX, scard);
	SCardLog::writeLog("[%i:%i][%s:%d] Connecting to user context", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
	mManager->connect(scard);
	cBase = new CardBase(mManager);
	
	this->_card_version = EstEIDManager::VER_INVALID;
	this->extAPDUSupported = false;
	checkPinPadDetection();
    checkExtendedAPDUSupport();
}

void EstEIDManager::connect(unsigned int idx)
{
	if (!isInReader(idx))
		throw NoCardInReaderError();
	cBase = new CardBase(mManager);
	mManager->connect(idx);
	SCardLog::writeLog("[%i:%i][%s:%d] Connecting to index %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, idx);
	this->_card_version = EstEIDManager::VER_INVALID;
	this->extAPDUSupported = false;
}

void EstEIDManager::connect(std::string readerName)
{
	mManager->makeConnection(readerName);
	SCardLog::writeLog("[%i:%i][%s:%d] Connecting to reader %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, readerName.c_str());
	this->extAPDUSupported = false;
	this->_card_version = EstEIDManager::VER_INVALID;
}

void EstEIDManager::disconnect()
{
	mManager->endTransaction();
	mManager->deleteConnection(true);
}

EstEIDManager::~EstEIDManager()
{
	SCardLog::writeLog("[%i:%i][%s:%d]", 0, getTransactionID(), __FUNC__, __LINE__);
	if (this->cBase != NULL)
	{
		delete this->cBase;
		this->cBase = NULL;
	}
	if (this->mManager != NULL)
	{
		delete this->mManager;
		this->mManager = NULL;
	}
	if (this->authCertObj != NULL)
	{
		delete authCertObj;
		this->authCertObj = NULL;
	}
	if (this->signCertObj != NULL)
	{
		delete signCertObj;
		this->signCertObj = NULL;
	}

	this->_card_version = EstEIDManager::VER_INVALID;
}


bool EstEIDManager::isInReader(unsigned int)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	try
	{
		string status =  mManager->getReaderState();
		if(status.find("EMPTY") != std::string::npos)
		{
			return false;
		}
		return true;
	}
	catch(NoCardInReaderError)
	{
		return false;
	}
	catch(SCError &e)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] %s 0x%08X", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what(), e.error);
		throw e;
	}
}

unsigned int EstEIDManager::getKeySize()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	if (_card_version == VER_INVALID)
	{
repeat:
		try
		{
			setCardVersion();
		}
		catch(CardResetError &e)
		{
			SCardLog::writeLog("[%i:%i][%s:%d] %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what());
			mManager->endTransaction();
			this->sleep(1);
			mManager->reconnect();
			goto repeat;
		}
	}

	if (_card_version == VER_1_0 || _card_version == VER_1_0_2007 || _card_version == VER_1_1)
		return 1024;
	else
		return 2048;
}

bool EstEIDManager::isSecureConnection()
{
	log();
	if (noPinPad)
		return false;
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	return this->mManager->isPinPad();
}

void EstEIDManager::enterPin(PinType pinType, const PinString &pin, bool forceUnsecure)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	byte cmdEntPin[] = {0x00,0x20,0x00}; // VERIFY
	ByteVec cmd(MAKEVECTOR(cmdEntPin));
	cmd.push_back((byte)pinType);
	if (pin.length() < 4)
	{
		if (pin.length()!= 0 ||!mManager->isPinPad())
		{
			AuthError err(0,0);
			err.m_badinput = true;
			err.desc = "bad pin length";
			throw err;
		}
	}
	else
	{
		cmd.push_back(LOBYTE(pin.length()));
		for (unsigned i = 0; i < pin.length(); i++)
			cmd.push_back(pin[i]);
		forceUnsecure = true; // Use normal command for sending PIN
	}
	try
	{
		if (forceUnsecure || !mManager->isPinPad())
			cBase->execute(cmd); //exec as a normal command
		else
			cBase->executePinEntry(cmd);
	}
	catch(AuthError &ae)
	{
		throw AuthError(ae);
	}
	catch(CardError &e)
	{
		if (e.SW1 == 0x63) {mManager->endTransaction(); throw AuthError(e);}
		if (e.SW1 == 0x69 && e.SW2 == 0x83){ throw AuthError(e.SW1,e.SW2,true);}
		if (e.SW1 == 0x69 && e.SW2 == 0x84)
		{
			AuthError err(0,0);
			err.m_badinput = true;
			err.desc = "Referenced data invalidated";
			throw err;
		}
		throw e;
	}
}

void EstEIDManager::setSecEnv(byte env)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	byte cmdSecEnv1[] = {0x00, 0x22, 0xF3};
	ByteVec cmd(MAKEVECTOR(cmdSecEnv1));
	cmd.push_back(env);
	cBase->execute(cmd);
}

void EstEIDManager::prepareSign_internal(KeyType keyId, const PinString &pin)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	cBase->selectMF(true);
	cBase->selectDF(FILEID_APP,true);

	if (keyId == 0)
		this->enterPin(PIN_AUTH,pin);
	else
		this->enterPin(PIN_SIGN,pin);
}

ByteVec EstEIDManager::sign_internal(AlgType type,KeyType keyId,const ByteVec &hash)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	byte signCmdSig[] = {0x00,0x2A,0x9E,0x9A};
	byte signCmdAut[] = {0x00,0x88,0x00,0x00};
	byte hashHdMD5[] = {0x30,0x20,0x30,0x0C,0x06,0x08,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x02,0x05,0x05,0x00,0x04,0x10};
	byte hashHdSHA1[] = {0x30,0x21,0x30,0x09,0x06,0x05,0x2B,0x0E,0x03,0x02,0x1A,0x05,0x00,0x04,0x14};
	byte hashHdSHA224[] = {0x30,0x2d,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x04,0x05,0x00,0x04,0x1c};
	byte hashHdSHA256[] = {0x30,0x31,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20};
	byte hashHdSHA384[] = {0x30,0x41,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02,0x05,0x00,0x04,0x30};
	byte hashHdSHA512[] = {0x30,0x51,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03,0x05,0x00,0x04,0x40};

	ByteVec cmd, header;
	if (keyId == 0 )
	{
		cmd = MAKEVECTOR(signCmdAut);
	}
	else
	{
		cmd = MAKEVECTOR(signCmdSig);
	}

	setCardVersion();
	switch(type)
	{
		case MD5:
			header = MAKEVECTOR(hashHdMD5);
			break;
		case SHA1:
			header = MAKEVECTOR(hashHdSHA1);
			break;
		case SHA224:
			header = MAKEVECTOR(hashHdSHA224);
			break;
		case SHA256:
			if(_card_version < VER_1_1)
			  throw UnsupportedCardHashCombination();
			header = MAKEVECTOR(hashHdSHA256);
			break;
		case SHA384:
			if(_card_version < VER_3_0)
			  throw UnsupportedCardHashCombination();
			header = MAKEVECTOR(hashHdSHA384);
			break;
		case SHA512:
			if(_card_version < VER_3_0)
			  throw UnsupportedCardHashCombination();
			header = MAKEVECTOR(hashHdSHA512);
			break;
		case SSL:
			break;
		case NOOID:
			break;
		default:
			throw std::runtime_error("cannot calculate SSL signature with OID");
	}
	cmd.push_back((byte) (header.size() + hash.size()));
	cmd.insert(cmd.end(), header.begin(), header.end());
	
	SCardLog::writeByteVecLog(cmd, "Header: ");

	SCardLog::writeLog("[%i:%i][%s:%d] SELECT MF", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
	cBase->selectMF(true);
	SCardLog::writeLog("[%i:%i][%s:%d] SELECT EEEE", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
	cBase->selectDF(FILEID_APP, true);
	setSecEnv(1);

	cmd.insert(cmd.end(), hash.begin(), hash.end());
	
	SCardLog::writeByteVecLog(cmd, "Hash with header: ");
	ByteVec result;
	try
	{
		if(_card_version != VER_3_0)
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Sending APDU case 4", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
			result = cBase->execute(cmd, 4);
		}
		else
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Sending default APDU case", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
			result = cBase->execute(cmd);
			SCardLog::writeLog("[%i:%i][%s:%d]", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
		}
	}
	catch(CardError e)
	{
		SCardLog::writeLog("[%i:%i][%s:%d]", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
		if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x85 ))
			throw AuthError(e);
		throw e;
	}
	catch (...)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Unsupported combination Card+HASH", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
		throw std::runtime_error("Unsupported combination Card+HASH");
	}
	SCardLog::writeLog("[%i:%i][%s:%d]", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);

	return result;
}

// Called within a transaction
ByteVec EstEIDManager::RSADecrypt_internal(const ByteVec &cryptogram)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	if(_card_version >= VER_3_5 && cryptogram.size() > 65535)
		throw std::runtime_error("Cryptogram is larger that extended APDU max length 0xFFFF!");
	if (_card_version >= VER_3_0 && cryptogram.size() != 256)
		throw std::runtime_error("2048 bit keys but cryptogram is not 256 bytes!");
	if (_card_version < VER_3_0 && cryptogram.size() != 128)
		throw std::runtime_error("cryptogram is not 128 bytes!");

	
	ByteVec result;

	try
	{
		if(extAPDUSupported)
		{
			
			if(mManager->isT1Protocol())
			{
				SCardLog::writeLog("[%i:%i][%s:%d] Decrypting using extended APDU", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
				result = RSADecrypt_extended(cryptogram);
			}
			else
			{
				SCardLog::writeLog("[%i:%i][%s:%d] Decrypting using APDU chaining", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
				result = RSADecrypt_chaining(cryptogram);
			}
		}
		else
		{
			if (_card_version >= VER_3_0)
			{
				SCardLog::writeLog("[%i:%i][%s:%d] Decrypting using APDU chaining", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
				result = RSADecrypt_chaining(cryptogram);
			}
			else
			{
				SCardLog::writeLog("[%i:%i][%s:%d] Decrypting using standart APDU", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
				result = RSADecrypt_normal(cryptogram);
			}
		}
	}
	catch(CardError e)
	{
		mManager->endTransaction();
		if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x88  || e.SW2 == 0x85 || e.SW1 == 0x6B ))
		{
			mManager->endTransaction();
			throw AuthError(e);
		}
		mManager->endTransaction();
		throw e;
	}
	mManager->endTransaction();
	return result;
}

ByteVec EstEIDManager::RSADecrypt_normal(const ByteVec &cryptogram)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	ByteVec result;
	cBase->selectMF(true);
	cBase->selectDF(FILEID_APP,true);
	cBase->selectEF(FILEID_KEYPOINTER,true);
	ByteVec keyRec = cBase->readRecord(1);
	if (keyRec.size() != 0x15)
	{
		mManager->endTransaction();
		throw CardDataError("key ptr len is not 0x15");
	}
	ByteVec cmd(MAKEVECTOR(modEnv0));
	cmd.insert(cmd.end(),&keyRec[0x9],&keyRec[0xB]);

	setSecEnv(6);
	cBase->execute(MAKEVECTOR(modEnv1));
	cBase->execute(MAKEVECTOR(modEnv2));
	cBase->execute(cmd);
	byte decrypt[] = {0x00, 0x2A, 0x80, 0x86, 0x81, 0x00};
	ByteVec decrypt_command(MAKEVECTOR(decrypt));
	decrypt_command.insert(decrypt_command.end(), cryptogram.begin(), cryptogram.end());
	result = cBase->execute(decrypt_command, 4);

	return result;
}

ByteVec EstEIDManager::RSADecrypt_chaining(const ByteVec &cryptogram)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	ByteVec result;
	cBase->selectMF(true);
	cBase->selectDF(FILEID_APP,true);
	cBase->selectEF(FILEID_KEYPOINTER,true);
	ByteVec keyRec = cBase->readRecord(1);
	if (keyRec.size() != 0x15)
	{
		mManager->endTransaction();
		throw CardDataError("key ptr len is not 0x15");
	}
	ByteVec cmd(MAKEVECTOR(modEnv0));
	cmd.insert(cmd.end(),&keyRec[0x9],&keyRec[0xB]);
	this->setSecEnv(6);
	cBase->execute(MAKEVECTOR(modEnv1));
	cBase->execute(MAKEVECTOR(modEnv2));
	cBase->execute(cmd);
	byte decrypt_chain1[] = {0x10, 0x2A, 0x80, 0x86, 0xFF, 0x00};
	byte decrypt_chain2[] = {0x00, 0x2A, 0x80, 0x86, 0x02};
	ByteVec first_command(MAKEVECTOR(decrypt_chain1));
	ByteVec second_command(MAKEVECTOR(decrypt_chain2));
	first_command.insert(first_command.end(), cryptogram.begin(), cryptogram.end()-2);
	second_command.insert(second_command.end(), cryptogram.end()-2, cryptogram.end());
	cBase->execute(first_command, 2);
	result = cBase->execute(second_command, 4);

	return result;
}

ByteVec EstEIDManager::RSADecrypt_extended(const ByteVec &cryptogram)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	ByteVec result;
	try
	{
		cBase->selectMF(true);
		cBase->selectDF(FILEID_APP,true);
		cBase->selectEF(FILEID_KEYPOINTER,true);
		this->setSecEnv(6);
		int cryptogramLen = (int)cryptogram.size()+1;
		byte decrypt_ex[] = {0x00, 0x2A, 0x80, 0x86, 0x00, HIBYTE(cryptogramLen), LOBYTE(cryptogramLen), 0x00};
		ByteVec command_ex(MAKEVECTOR(decrypt_ex));
		command_ex.insert(command_ex.end(), cryptogram.begin(), cryptogram.end());
		command_ex.insert(command_ex.end(), 0xFF);
		command_ex.insert(command_ex.end(), 0xFF);
		result = cBase->execute(command_ex, 2);
	}
	catch(CardError &e)
	{
		if(e.SW1 == 0x69 && e.SW2 == 0x82)
			throw e;
		else
			throw e;
	}
	catch(std::runtime_error &e)
	{
		throw e;
	}

	return result;
}

void EstEIDManager::readPersonalData_internal(std::vector<std::string>& data,int recStart,int recEnd)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	cBase->selectMF(true);
	cBase->selectDF(FILEID_APP,true);
	CardBase::FCI fileInfo = cBase->selectEF(0x5044);

	if (fileInfo.recCount != 0x10 && fileInfo.recCount != 0x0A && fileInfo.recCount != 0x14)
	{
		throw CardDataError("personal data file does not contain 16 or 10 records");
	}

	data.resize(min(recEnd,0x10) + 1 );
	for (int i=recStart; i <= recEnd; i++)
	{
		ByteVec record;
		try
		{
			record = cBase->readRecord(i);
		}
		catch(...)
		{
			throw;
		}
		string& ret = data[i];
		for (ByteVec::iterator c = record.begin();c!=record.end();c++)
			ret+= *c ;
		if (_card_version == VER_3_0)
			ret.erase(std::find_if(ret.rbegin(), ret.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), ret.end());
	}
}

bool EstEIDManager::validatePin_internal(PinType pinType,const PinString &pin, byte &retriesLeft,bool forceUnsecure)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	try
	{
		UNUSED_ARG(forceUnsecure);
		
		cBase->selectMF(true);
		if (retriesLeft != 0xFA ) { //sorry, thats a bad hack around sloppy interface definition
			retriesLeft = 0;
			cBase->selectEF(FILEID_RETRYCT,true);
			ByteVec data = cBase->readRecord(pinType == PUK ? 3 : pinType );
			ByteVec tag = cBase->getTag(0x90,1,data);
			retriesLeft = tag[0];
			}
		cBase->selectDF(FILEID_APP,true);
		this->enterPin(pinType,pin);
	}
	catch(AuthError &e)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Wrong PIN. Throwing Auth error", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.SW1, e.SW2);
		throw e;
	}
	catch(CardError &e)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] PIN validation has broke. Try again.... SW1: %02x SW2: %02x", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.SW1, e.SW2);
		if(e.SW1 == 0x63)
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Wrong PIN. Throwing", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.SW1, e.SW2);
			throw e;
		}
	}
	return true;
}

bool EstEIDManager::changePin_internal(PinType pinType,const PinString &newPin,const PinString &oldPin,bool useUnblockCmd )
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	byte cmdChangeCmd[] = {0x00,0x24,0x00};
	bool doSecure = false;

	if (useUnblockCmd) cmdChangeCmd[1]= 0x2C;
	ByteVec cmd(MAKEVECTOR(cmdChangeCmd));
	cmd.push_back((byte)pinType);

	size_t oldPinLen,newPinLen;
	if (newPin.length() < 4 || oldPin.length() < 4 )
	{
		if (!mManager->isPinPad() )
		{
			mManager->endTransaction();
			throw std::runtime_error("bad pin length");
		}

		doSecure = true;
	}
	else
	{
		oldPinLen = oldPin.length();
		newPinLen = newPin.length();

		ByteVec catPins;
		catPins.resize(oldPinLen + newPinLen);
		copy(oldPin.begin(), oldPin.end(), catPins.begin());
		copy(newPin.begin(), newPin.end(), catPins.begin() + oldPin.length());
		cmd.push_back(LOBYTE(catPins.size()));
		cmd.insert(cmd.end(),catPins.begin(),catPins.end());
	}

	try
	{
		if (doSecure)
			cBase->executePinChange(cmd,0,0);
		else
			cBase->execute(cmd);
	}
	catch(AuthError &ae)
	{
		mManager->endTransaction();
		throw AuthError(ae);
	}
	catch(CardError &e)
	{
		mManager->endTransaction();
		if (e.SW1 == 0x63)
			throw AuthError(e);
		else if (useUnblockCmd && e.SW1==0x6a && e.SW2 == 0x80 ) //unblocking, repeating old pin
			throw AuthError(e.SW1,e.SW2,true);
		else
			throw e;
	}
	mManager->endTransaction();
	return true;
}

void EstEIDManager::reconnectWithT0()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	if (mManager->isOwnConnection() && mManager->getProtocol() != SCARD_PROTOCOL_T0)
	{
		mManager->reconnect(SCARD_PROTOCOL_T0);
        selectMF();
	}
}

void EstEIDManager::reconnectWithT1()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	if (mManager->isOwnConnection())
	{
		try
		{
			if(!mManager->isT1Protocol())
			{
				SCardLog::writeLog("[%i:%i][%s:%d] Current protocol T0. Reconnecting.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
				mManager->reconnect(SCARD_PROTOCOL_T1);
                selectMF();
				if(!mManager->isT1Protocol())
					SCardLog::writeLog("[%i:%i][%s:%d] Something wrong. Failed to change protocol to T1", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
				else
					SCardLog::writeLog("[%i:%i][%s:%d] Protocol changed to T1", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
			}
			else
			{
				SCardLog::writeLog("[%i:%i][%s:%d] Protocol allready is T1. Reconnecting.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
				mManager->reconnect(SCARD_PROTOCOL_T1);
				if(!mManager->isT1Protocol())
					SCardLog::writeLog("[%i:%i][%s:%d] Something wrong. unexpected change of protocol from", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
				else
					SCardLog::writeLog("[%i:%i][%s:%d] Protocol changed from T0", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
			}
		}
		catch(...)
		{
			mManager->reconnect();
		}
	}
	else
	{
		mManager->reconnect();
	}
}

void EstEIDManager::checkProtocol()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	setCardVersion();
	
	try
	{
		cBase->selectMF(true);
	}
	catch(CardError &ce)
	{
		if(ce.SW1 == 0x6A && ce.SW2 == 0x83)
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Digi-ID spetsific condition. Reconnect...", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
			mManager->reconnect();
            mManager->beginTransaction();
			return;
		}
		if (ce.SW1 != 0x6A || ce.SW2 != 0x87 )
		{
			throw;
		}
		this->reconnectWithT0();
        mManager->beginTransaction();
	}
	catch(SCError &sce)
	{
		if (sce.error != 0x1 && sce.error != 0x6f7)  //incorrect function, some drivers do that microsoft generic PCSC with OK3621
		{
			throw;
		}
		this->reconnectWithT0();
        mManager->beginTransaction();
	}

	if(this->_card_version >= VER_3_5 && !mManager->isT1Protocol() && mManager->isOwnConnection() && this->extAPDUSupported)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Trying to change protocol to T1.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
		try
		{
			this->reconnectWithT1();
            mManager->beginTransaction();
			SCardLog::writeLog("[%i:%i][%s:%d] Current protocol is T1.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
		}
		catch(...)
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Failed to reconnect with protocol T1. Restoring connection", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
			mManager->reconnect();
            mManager->beginTransaction();
		}
	}
	
	return;
}

//transacted, public methods
string EstEIDManager::readCardID()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Reading card id iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            string tmp = "";
            string ret = "";
            mManager->beginTransaction();
            
            tmp = this->readRecord_internal(DOCUMENTID);
            
            if(tmp.at(0) == 'N')
            {
                _card_version = VER_1_1;
            }
            if(_card_version == VER_1_1)
            {
                ret = getPersonalCodeFromAuthCertificate();
            }
            else
            {
                
                ret = this->readRecord_internal(ID);
                mManager->endTransaction();
            }
            return ret;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

string EstEIDManager::readDocumentID()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Reading document id iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            string ret = "";

            mManager->beginTransaction();
            //checkProtocol();
            
            ret = this->readRecord_internal(DOCUMENTID);
            mManager->endTransaction();
            return ret;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
        
    }
	
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

string EstEIDManager::readRecord_internal(int recordID)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	ByteVec record;
	string documentID = "";
	try
	{
		cBase->selectMF(true);
		cBase->selectDF(FILEID_APP,true);
		CardBase::FCI fileInfo = cBase->selectEF(0x5044);
		
		if (fileInfo.recCount != 0x10 && fileInfo.recCount != 0x0A && fileInfo.recCount != 0x14)
		{
			throw CardDataError("personal data file does not contain 16 or 10 records");
		}

		record = cBase->readRecord(recordID);
		documentID = string(record.begin(), record.end());
	}
	catch(CardError &e)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] CardError %s SW1 %02x SW2 %02x. Terminating transaction.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what(), e.SW1, e.SW2);
		mManager->endTransaction();
		throw e;
	}
	catch(...)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Unexpected exception. Terminating transaction.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
		mManager->endTransaction();
		throw;
	}

	return documentID;
}

string EstEIDManager::readCardName(bool firstNameFirst)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Reading card name iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            string tmp = "";
            string ret = "";
            string firstName = "";
            string surName = "";
            mManager->beginTransaction();
            tmp = this->readRecord_internal(DOCUMENTID);
            
            if(tmp.at(0) == 'N')
            {
                _card_version = VER_1_1;
            }
            if(_card_version == EstEIDManager::VER_1_1)
            {
                firstName = this->getFirstNameFromAuthCertificate();
                surName = this->getSurNameFromAuthCertificate();
            }
            else
            {
                firstName = this->readRecord_internal(FIRSTNAME);
                surName = this->readRecord_internal(SURNAME);
                mManager->endTransaction();
            }
            firstName.erase(firstName.find_last_not_of(' ')+1);
            surName.erase(surName.find_last_not_of(' ')+1);
            if (firstNameFirst)
                ret = firstName + " " + surName;
            else
                ret = surName + " " + firstName;
            
            return ret;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

void EstEIDManager::reIdentify()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	setCardVersion();
	mManager->beginTransaction();
	byte cmdGetVersion[] = {0x00, 0xCA, 0x01, 0x00, 0x02};
	ByteVec cmd(MAKEVECTOR(cmdGetVersion));

	ByteVec result;
	try
	{
		result = cBase->execute(cmd);
	}
	catch(...)
	{
		mManager->endTransaction();
		return;
	}
	
	if (result[0] == 0x01 && result[1] == 0x01)
		_card_version = VER_1_1;
	else if (result[0] == 0x03 && result[1] == 0x00)
		_card_version = VER_3_0;
	mManager->endTransaction();
}

bool EstEIDManager::readPersonalData(std::vector<std::string>& data, int firstRecord,int lastRecord)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Reading personal data iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->beginTransaction();
            checkProtocol();
            readPersonalData_internal(data,firstRecord,lastRecord);
            mManager->endTransaction();
            return true;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
        catch(CardError &e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] CardError %s SW1 %02x SW2 %02x. Terminating transaction.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what(), e.SW1, e.SW2);
            mManager->endTransaction();
            throw e;
        }
        catch(...)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Unexpected exception. Terminating transaction.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
            mManager->endTransaction();
            throw;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
	
}

bool EstEIDManager::getKeyUsageCounters(dword &authKey,dword &signKey)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();

    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Reading usage counters iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->beginTransaction();
            cBase->selectMF(true);
            cBase->selectDF(FILEID_APP,true);
            cBase->selectEF(FILEID_KEYPOINTER,true);
            ByteVec keyRec = cBase->readRecord(1);
            if (keyRec.size() != 0x15)
            {
                mManager->endTransaction();
                throw CardDataError("key ptr len is not 0x15");
            }
            byte UsedKey = 0;
            if (keyRec[0x9]== 0x12 && keyRec[0xA] == 0x00 ) UsedKey++;
            
            CardBase::FCI fileInfo = cBase->selectEF(0x0013);
            if (fileInfo.recCount < 4)
            {
                mManager->endTransaction();
                throw CardDataError("key info file 0x13 should have 4 records");
            }
            
            ByteVec authData = cBase->readRecord(UsedKey + 3);
            ByteVec signData = cBase->readRecord(UsedKey + 1);
            ByteVec aTag = cBase->getTag(0x91,3,authData);
            ByteVec sTag = cBase->getTag(0x91,3,signData);
            
            authKey = 0xFFFFFF - ((aTag[0] << 16) + (aTag[1] << 8) + aTag[2]);
            signKey = 0xFFFFFF - ((sTag[0] << 16) + (sTag[1] << 8) + sTag[2]);
            
            mManager->endTransaction();
            return true;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }

        catch(...)
        {
            mManager->endTransaction();
            throw CardDataError("Failed to read data from card");
        }
    }
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

ByteVec EstEIDManager::readEFAndTruncate(unsigned int fileLen)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	
	ByteVec ret;
	if(this->extAPDUSupported && mManager->isT1Protocol())
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Card version is higher than 3.4. Reading in extended mode", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
		ret = cBase->readEFEx(fileLen);
	}
	else
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Card version is lower than 3.5. Reading in standart mode", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
		ret = cBase->readEF(fileLen);
	}
	if (ret.size() > 128) { //assume ASN sequence encoding with 2-byte length
		size_t realSize = ret[2] * 256 + ret[3] + 4;
		ret.resize(realSize);
	}
	return ret;
}

ByteVec EstEIDManager::getAuthCert()
{
	log();
	if(mManager == NULL)
    {
        
        SCardLog::writeLog("[%i:%i][%s:%d] mManager is NULL. Throwing an exception", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
        throw PCSCManagerFailure();
    }
	ByteVec tmp;
	CardBase::FCI fileInfo = {0, 0, 0, 0};
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Reading auth cert iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            if(this->authCert.empty())
            {
                SCardLog::writeLog("[%i:%i][%s:%d] Certificate is empty. Reading from card.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
                mManager->beginTransaction();
                cBase->selectMF(true);
                cBase->selectDF(FILEID_APP,true);
                fileInfo = cBase->selectEF(0xAACE);
                tmp = readEFAndTruncate(fileInfo.fileLength );
                mManager->endTransaction();
                this->authCert = tmp;
                return this->authCert;
            }
            else
            {
                SCardLog::writeLog("[%i:%i][%s:%d] Found buffered certificate.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
                return this->authCert;
            }
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            this->authCert.clear();
            tmp.clear();
            mManager->resetCurrentConnection();
            continue;
        }
        catch(CardError &e)
        {
            if(e.SW1 == 0x69 && e.SW2 == 0x85)		//If Invalid condition try again once
            {
                SCardLog::writeLog("[%i:%i][%s:%d] Card error. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
                this->signCert.clear();
                tmp.clear();
                mManager->resetCurrentConnection();
                continue;
            }
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    this->authCert.clear();
    tmp.clear();
    throw CardResetError();
	
}

ByteVec EstEIDManager::getSignCert()
{
	log();
	if(mManager == NULL)
    {
        SCardLog::writeLog("[%i:%i][%s:%d] mManager is NULL. Throwing an exception", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
        throw PCSCManagerFailure();
    }
	ByteVec tmp;
	CardBase::FCI fileInfo = {0, 0, 0, 0};
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Reading auth cert iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            if(this->signCert.empty())
            {
                mManager->beginTransaction();
                cBase->selectMF(true);
                cBase->selectDF(FILEID_APP,true);
                fileInfo = cBase->selectEF(0xDDCE);
                tmp = readEFAndTruncate(fileInfo.fileLength );
                mManager->endTransaction();
                this->signCert = tmp;
                return this->signCert;
            }
            else
            {
                SCardLog::writeLog("[%i:%i][%s:%d] Found buffered certificate.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
                return this->signCert;
            }
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            this->signCert.clear();
            tmp.clear();
            mManager->resetCurrentConnection();
            continue;
        }
        catch(CardError &e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card error. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            if(e.SW1 == 0x69 && e.SW2 == 0x85)		//If Invalid condition try again once
            {
                this->signCert.clear();
                tmp.clear();
                mManager->resetCurrentConnection();
                continue;
            }
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    this->signCert.clear();
    tmp.clear();
    throw CardResetError();
}

ByteVec EstEIDManager::sign(const ByteVec &hash, AlgType type, KeyType keyId)
{
	log();
	

    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Signing iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            if(mManager == NULL)
                throw PCSCManagerFailure();
            if(getTransactionID() == 0)
                mManager->beginTransaction();
            ByteVec tmp = sign_internal(type, keyId, hash);
            mManager->endTransaction(false);
            SCardLog::writeLog("[%i:%i][%s:%d] Sign successfull.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
            return tmp;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
	
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

ByteVec EstEIDManager::sign_pkcs11(const ByteVec &hash, AlgType type, KeyType keyId)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	ByteVec tmp = sign_internal(type, keyId, hash);
	return tmp;
}

ByteVec EstEIDManager::sign(const ByteVec &hash, AlgType type, KeyType keyId, const PinString &pin)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        ByteVec tmp;
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Signing iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->beginTransaction();
            prepareSign_internal(keyId,pin);
            tmp = sign_internal(type,keyId,hash);
            mManager->endTransaction(false);
            return tmp;
        }
        catch(CardResetError &e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
        catch(AuthError &e)
        {
            mManager->endTransaction(false);
            throw e;
        }
        catch(CardError &e)
        {
            mManager->endTransaction(false);
            if(e.SW1 == 0x69 && e.SW2 == 0x85)		//If Invalid condition try again once
            {
                tmp.clear();
                prepareSign_internal(keyId,pin);
                tmp = sign_internal(type,keyId,hash);
            }
            throw e;
        }
        
    }
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

ByteVec EstEIDManager::RSADecrypt(const ByteVec &cryptogram)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
	mManager->beginTransaction();
    checkProtocol();
	ByteVec tmp = RSADecrypt_internal(cryptogram);
	mManager->endTransaction(false);
	return tmp;
}


ByteVec EstEIDManager::RSADecrypt(const ByteVec &cipher,const PinString &pin)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Decrypting iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->beginTransaction();
            checkProtocol();
            cBase->selectMF(true);
            cBase->selectDF(FILEID_APP,true);
            enterPin(PIN_AUTH,pin);
            ByteVec tmp = RSADecrypt_internal(cipher);
            mManager->endTransaction(false);
            return tmp;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();

}

bool EstEIDManager::validateAuthPin(const PinString &pin, byte &retriesLeft )
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Verifying Auth PIN iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->beginTransaction();
            bool tmp = validatePin_internal(PIN_AUTH,pin,retriesLeft);
            mManager->endTransaction(false);
            return tmp;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

bool EstEIDManager::validateSignPin(const PinString &pin, byte &retriesLeft )
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Verifying Sign PIN iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->beginTransaction();
            bool tmp = validatePin_internal(PIN_SIGN,pin,retriesLeft);
            mManager->endTransaction(false);
            return tmp;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

bool EstEIDManager::validatePuk(const PinString &pin, byte &retriesLeft )
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Verifying PUK iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->beginTransaction();
            bool tmp = validatePin_internal(PUK,pin,retriesLeft);
            mManager->endTransaction(false);
            return tmp;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

bool EstEIDManager::changeAuthPin(const PinString &newPin,const PinString &oldPin, byte &retriesLeft )
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Changing Auth PIN iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->beginTransaction();
            if (!mManager->isPinPad())
            {
                try
                {
                    validatePin_internal(PIN_AUTH,oldPin,retriesLeft);
                }
                catch(AuthError &e)
                {
                    mManager->endTransaction(false);
                    throw e;
                }
                catch(CardError &e)
                {
                    SCardLog::writeLog("[%i:%i][%s:%d] CardError ", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
                    mManager->endTransaction(false);
                    throw e;
                }
            }
            else
            {
                byte dummy;
                getRetryCounts_internal(dummy,retriesLeft,dummy);
            }
            bool tmp = changePin_internal(PIN_AUTH,newPin,oldPin);
            mManager->endTransaction(false);
            return tmp;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
	
}

bool EstEIDManager::changeSignPin(const PinString &newPin,const PinString &oldPin, byte &retriesLeft )
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Changing Sign PIN iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->beginTransaction();
            if (!mManager->isPinPad())
            {
                try
                {
                    validatePin_internal(PIN_SIGN,oldPin,retriesLeft);
                }
                catch(AuthError &e)
                {
                    mManager->endTransaction(false);
                    throw e;
                }
                catch(CardError &e)
                {
                    SCardLog::writeLog("[%i:%i][%s:%d] CardError ", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
                    mManager->endTransaction(false);
                    throw e;
                }
            }
            else
            {
                byte dummy;
                getRetryCounts_internal(dummy,dummy,retriesLeft);
            }
            bool tmp = changePin_internal(PIN_SIGN,newPin,oldPin);
            mManager->endTransaction(false);
            return tmp;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
	
}

bool EstEIDManager::changePUK(const PinString &newPUK,const PinString &oldPUK, byte &retriesLeft )
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Changing PUK iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->beginTransaction();
            if (!mManager->isPinPad())
            {
                try
                {
                    validatePin_internal(PUK,oldPUK,retriesLeft);
                }
                catch(AuthError &e)
                {
                    mManager->endTransaction(false);
                    throw e;
                }
                catch(CardError &e)
                {
                    SCardLog::writeLog("[%i:%i][%s:%d] CardError ", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
                    mManager->endTransaction(false);
                    throw e;
                }
            }
            else
            {
                byte dummy;
                getRetryCounts_internal(retriesLeft,dummy,dummy);
            }
            bool tmp = changePin_internal(PUK,newPUK,oldPUK);
            mManager->endTransaction(false);
            return tmp;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
	
}

bool EstEIDManager::unblockAuthPin(const PinString &newPin,const PinString &mPUK, byte &retriesLeft )
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Unblocking Auth PIN iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            bool tmp = false;
            mManager->beginTransaction();
            if (!mManager->isPinPad())
                validatePin_internal(PUK,mPUK,retriesLeft);
            
            for(char i='0'; i < '6'; i++)
            {
                try
                { //ENSURE its blocked
                    validatePin_internal(PIN_AUTH,PinString("0000") + i ,retriesLeft,true);
                }
                catch (...)
                {
                }
            }
            tmp = changePin_internal(PIN_AUTH,newPin,mPUK,true);

            mManager->endTransaction(false);
            return tmp;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

bool EstEIDManager::unblockSignPin(const PinString &newPin,const PinString &mPUK, byte &retriesLeft )
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Unblocking Sign PIN iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            bool tmp = false;
            mManager->beginTransaction();
            if (!mManager->isPinPad())
                validatePin_internal(PUK,mPUK,retriesLeft);
            for(char i='0'; i < '6'; i++)
            {
                try
                { //ENSURE its blocked
                    validatePin_internal(PIN_SIGN,PinString("0000") + i ,retriesLeft,true);
                }
                catch (...)
                {
                }
            }
            tmp = changePin_internal(PIN_SIGN,newPin,mPUK,true);
            mManager->endTransaction(false);
            return tmp;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

bool EstEIDManager::getRetryCounts(byte &puk,byte &pinAuth,byte &pinSign)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Reading retry counters iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->beginTransaction();
            bool tmp = this->getRetryCounts_internal(puk,pinAuth,pinSign);
            mManager->endTransaction(false);
            return tmp;
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

bool EstEIDManager::getRetryCounts_internal(byte &puk,byte &pinAuth,byte &pinSign)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	ByteVec data,tag;
	cBase->selectMF(true);
	cBase->selectEF(FILEID_RETRYCT,true);
	data = cBase->readRecord(3);
	tag = cBase->getTag(0x90,1,data);
	puk = tag[0];
	data = cBase->readRecord(PIN_AUTH);
	tag = cBase->getTag(0x90,1,data);
	pinAuth = tag[0];
	data = cBase->readRecord(PIN_SIGN);
	tag = cBase->getTag(0x90,1,data);
	pinSign = tag[0];
	return true;
}


void EstEIDManager::resetAuth()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	try
	{
		mManager->beginTransaction();
		cBase->selectMF(true);
		setSecEnv(0);
		mManager->endTransaction(false);
	}
	catch (...)
	{
		mManager->endTransaction(false);
	}
}

ByteVec EstEIDManager::cardChallenge()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	byte cmdEntPin[] = {0x00,0x84,0x00,0x00,0x08};
	ByteVec cmd(MAKEVECTOR(cmdEntPin));
	ByteVec result;
	try
	{
		result = cBase->execute(cmd);
	}
	catch(CardError e)
	{
		if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x85 )) 
		{
			throw AuthError(e);
		}
		throw e;
	}
	return result;
}

ByteVec EstEIDManager::runCommand( const ByteVec &vec )
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	ByteVec result;

	int retry = 3;
	
	while( retry )
	{
		try
		{
			result = cBase->execute(vec);
			break;
		}
		catch(CardError e)
		{
			if ( retry == 1 )
			{
				if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x85 )) 
				{
					throw AuthError(e);
				}
				throw e;
			}
			else
			{
				reconnectWithT0();
			}
		}
		retry--;
	}
	return result;
}

std::string EstEIDManager::getMD5KeyContainerName(int type)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	std::string keyContainerNameMD5 = "";
#ifdef WIN32
	SCardLog::writeLog("[%i:%i][%s:%d]", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
	std::string keyContainerName = "";
	
	try
	{
		keyContainerName.append(readDocumentID());
		keyContainerName.append("_");
		switch(type)
		{
			case 0: keyContainerName.append("AUT"); break;
			case 1: keyContainerName.append("SIG"); break;
			default: throw std::runtime_error("Invalid key container type.");
		}

		unsigned char outPut[16];
		md5_context md5Ctx;
		md5_starts(&md5Ctx);
		md5_update(&md5Ctx, (unsigned char *)keyContainerName.c_str(), (int)keyContainerName.length());
		md5_finish(&md5Ctx, outPut);
		std::stringstream stream;
		stream.setf(std::ios_base::hex, std::ios::basefield);
		stream.fill(_T('0'));
		for(int i = 0; i < sizeof(outPut); i++)
			stream << std::setw(2) << (unsigned short)outPut[i];
		keyContainerNameMD5 = stream.str();

		SCardLog::writeLog("[%i:%i][%s:%d] Returning container name MD5: %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, keyContainerNameMD5.c_str());
	}
	catch(CardError &e)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Card error: %s, 0x%X, 0x%X", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what(), e.SW1, e.SW2);
		throw e;
	}
	catch(std::runtime_error &re)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Runtime error: %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, re.what());
		throw re;
	}
#else
	(void)type;
#endif
	return keyContainerNameMD5;
}

void EstEIDManager::startTransaction()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	mManager->beginTransaction();
}

void EstEIDManager::endTransaction()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	mManager->endTransaction();
}

void EstEIDManager::setReaderLanguageId(ReaderLanguageID langId)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	mManager->setReaderLanguageId(langId);
}

int EstEIDManager::getReaderLanguageId(void)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	return mManager->getReaderLanguageId();
}

void EstEIDManager::setCardVersion()
{
	log();
	std::string currentATR = "";
	if(mManager->isOwnConnection())
		 currentATR = mManager->getATRHex(mManager->getConnectionIndex());
	else
		currentATR = mManager->getATRHex();

	if(currentATR.compare(ESTEID_V1_COLD_ATR) == 0)
		this->_card_version = VER_1_0;
	else if(currentATR.compare(ESTEID_V1_WARM_ATR) == 0)
		this->_card_version = VER_1_0;
	else if(currentATR.compare(ESTEID_V1_2007_COLD_ATR) == 0)
		this->_card_version = VER_1_0_2007;
	else if(currentATR.compare(ESTEID_V1_2007_WARM_ATR) == 0)
		this->_card_version = VER_1_0_2007;
	else if(currentATR.compare(ESTEID_V1_1_COLD_ATR) == 0)
		this->_card_version = VER_1_1;
	else if(currentATR.compare(ESTEID_V3_COLD_DEV1_ATR) == 0)
		this->_card_version = VER_3_4;
	else if(currentATR.compare(ESTEID_V3_WARM_DEV1_ATR) == 0)
		this->_card_version = VER_3_4;
	else if(currentATR.compare(ESTEID_V3_WARM_DEV2_ATR) == 0)
		this->_card_version = VER_3_4;
	else if(currentATR.compare(ESTEID_V3_WARM_DEV3_ATR) == 0)
		this->_card_version = VER_3_4;
	else if(currentATR.compare(ESTEID_V35_COLD_DEV1_ATR) == 0)
		this->_card_version = VER_3_5;
	else if(currentATR.compare(ESTEID_V35_COLD_DEV2_ATR) == 0)
		this->_card_version = VER_3_5;
	else if(currentATR.compare(ESTEID_V35_COLD_DEV3_ATR) == 0)
		this->_card_version = VER_3_5;
	else if(currentATR.compare(ESTEID_V35_WARM_ATR) == 0)
		this->_card_version = VER_3_5;
	else
		this->_card_version = VER_INVALID;
	SCardLog::writeLog("[%i:%i][%s:%d] Card version: %s, ATR version %s. Detecting extended APDU support", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, getCardName().c_str(), getATRName(currentATR).c_str());
	extAPDUSupported = this->isExtAPDUSupported();
	if(extAPDUSupported == true)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Extended APDU supported", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, getCardName().c_str());
	}
	else
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Extended APDU is not supported", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, getCardName().c_str());
	}

	SCardLog::writeLog("[%i:%i][%s:%d] Card version: %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, this->getCardName().c_str());
}


EstEIDManager::CardVersion EstEIDManager::getCardVersion()
{
	log();
	if(this->_card_version == EstEIDManager::VER_INVALID)
		setCardVersion();
	return this->_card_version;
}

ByteVec EstEIDManager::getApplicationVersion()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	try
	{
		byte apduBytes[] = {0x00, 0xCA, 0x01, 0x00, 0x02};
		ByteVec apdu(MAKEVECTOR(apduBytes));
		ByteVec result;

		result = cBase->execute(apdu);
		if(result.size() <= 0)
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Application information not found", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
			return result;
		}
		else
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Returning application version", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
			return result;
		}
	}
	catch(CardError &e)
	{
		
		if((e.SW1 == 0x6c && e.SW2 == 0x03) || (e.SW1 == 0x6a && e.SW2 == 0x83))
		{
			byte apduBytes[] = {0x00, 0xCA, 0x01, 0x00, 0x03};
			ByteVec apdu(MAKEVECTOR(apduBytes));
			ByteVec result;
			result = cBase->execute(apdu);
			if(result.size() <= 0)
			{
				SCardLog::writeLog("[%i:%i][%s:%d] Application information not found", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
				return result;
			}
			else
			{
				SCardLog::writeLog("[%i:%i][%s:%d] Returning application version", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
				return result; 
			}
		}
		SCardLog::writeLog("[%i:%i][%s:%d] Card error: %s, 0x%X, 0x%X", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what(), e.SW1, e.SW2);
		throw e;
	}
	catch(std::runtime_error &re)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Runtime error: %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, re.what());
		throw re;
	}
}

std::string EstEIDManager::getCardName()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	std::string cardName = "";
	switch(this->_card_version)
	{
		case EstEIDManager::VER_1_0: cardName = "Estonian National ID Card version 1.0"; break;
		case EstEIDManager::VER_1_0_2007: cardName = "Estonian National ID Card version 1.0 since 2006"; break;
		case EstEIDManager::VER_1_1: cardName = "Estonian National ID Card version 1.1 Digi-ID"; break;
		case EstEIDManager::VER_3_0: cardName = "Estonian National ID Card version 3.0"; break;
		case EstEIDManager::VER_3_4: cardName = "Estonian National ID Card version 3.4"; break;
		case EstEIDManager::VER_3_0_UPPED_TO_3_4: cardName = "Estonian National ID Card version 3.0 updated to version 3.4"; break;
		case EstEIDManager::VER_3_5: cardName = "Estonian National ID Card version 3.5"; break;
		case EstEIDManager::VER_3_5_1: cardName = "Estonian National ID Card version 3.5.1"; break;
		default: cardName = "Unknown smard card";
	}
	return cardName;
}

std::string EstEIDManager::getATRName(std::string atr)
{
	log();
	std::string atrName = "";
	if(atr.compare(ESTEID_V1_COLD_ATR) == 0)
	  atrName = "ESTEID V1.0 COLD ATR";
	else if(atr.compare(ESTEID_V1_WARM_ATR) == 0)
	  atrName = "ESTEID V1.0 WARM ATR";
	else if(atr.compare(ESTEID_V1_2007_COLD_ATR) == 0)
	  atrName = "ESTEID V1.0 2007 COLD ATR";
	else if(atr.compare(ESTEID_V1_2007_WARM_ATR) == 0)
	  atrName = "ESTEID V1.0 2007 WARM ATR";
	else if(atr.compare(ESTEID_V1_1_COLD_ATR) == 0)
	  atrName = "ESTEID V1.1 COLD ATR";
	else if(atr.compare(ESTEID_V3_COLD_DEV1_ATR) == 0)
	  atrName = "ESTEID V3.0 COLD DEV1 ATR";
	else if(atr.compare(ESTEID_V3_WARM_DEV1_ATR) == 0)
	  atrName = "ESTEID V3.0 WARM DEV1 ATR";
	else if(atr.compare(ESTEID_V3_WARM_DEV2_ATR) == 0)
	  atrName = "ESTEID V3.0 WARM DEV2 ATR";
	else if(atr.compare(ESTEID_V3_WARM_DEV3_ATR) == 0)
	  atrName = "ESTEID V3.0 WARM DEV3 ATR";
	else if(atr.compare(ESTEID_V35_COLD_DEV1_ATR) == 0)
	  atrName = "ESTEID V3.5 COLD DEV1 ATR";
	else if(atr.compare(ESTEID_V35_COLD_DEV2_ATR) == 0)
	  atrName = "ESTEID V3.5 COLD DEV2 ATR";
	else if(atr.compare(ESTEID_V35_COLD_DEV3_ATR) == 0)
	  atrName = "ESTEID V3.5 COLD DEV3 ATR";
	else if(atr.compare(ESTEID_V35_WARM_ATR) == 0)
	  atrName = "ESTEID V3.5 WARM ATR";
	else
	  atrName = "Unknown ATR";

	return atrName;
}

void EstEIDManager::sendApplicationIdentifierPreV3()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	try
	{
		//Application identifier for pre V3 cards. (1.0, 1.0 2006, 1.1)
		byte apduBytes[] = {0x00, 0xA4, 0x04, 0x00, 0x10, 0xD2, 0x33, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		ByteVec apdu(MAKEVECTOR(apduBytes));
		ByteVec result;
		if(mManager->isT1Protocol())
			apdu.push_back(0x27);
		result = cBase->execute(apdu);
		return;
	}
	catch(CardError &e)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Card error: %s, 0x%X, 0x%X", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what(), e.SW1, e.SW2);
		throw e;
	}
	catch(std::runtime_error &re)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Runtime error: %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, re.what());
		throw re;
	}
}

void EstEIDManager::sendApplicationIdentifierV3()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	try
	{
		//Application identifier for V3.x cards. (3.0, 3.4, 3.0 updated to 3.4)
		byte apduBytes[] = {0x00, 0xA4, 0x04, 0x00, 0x0E, 0xF0, 0x45, 0x73, 0x74, 0x45, 0x49, 0x44, 0x20, 0x76, 0x65, 0x72, 0x20, 0x31, 0x2E};
		ByteVec apdu(MAKEVECTOR(apduBytes));
		ByteVec result;

		result = cBase->execute(apdu);
		return;
	}
	catch(CardError &e)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Card error: %s, 0x%X, 0x%X", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what(), e.SW1, e.SW2);
		throw e;
	}
	catch(std::runtime_error &re)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Runtime error: %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, re.what());
		throw re;
	}
}

void EstEIDManager::sendApplicationIdentifierV3_5()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	try
	{
		//Application identifier for pre V3 cards. (1.0, 1.0 2006, 1.1)
		byte apduBytes[] = {0x00, 0xA4, 0x04, 0x00, 0x0F, 0xD2, 0x33, 0x00, 0x00, 0x00, 0x45, 0x73, 0x74, 0x45, 0x49, 0x44, 0x20, 0x76, 0x33, 0x35};
		ByteVec apdu(MAKEVECTOR(apduBytes));
		ByteVec result;

		result = cBase->execute(apdu);
		return;
	}
	catch(CardError &e)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Card error: %s, 0x%X, 0x%X", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what(), e.SW1, e.SW2);
		throw e;
	}
	catch(std::runtime_error &re)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Runtime error: %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, re.what());
		throw re;
	}
}

bool EstEIDManager::isExtAPDUSupported()
{
	log();
    
#ifndef __APPLE__
    
    if(!noExtAPDU)
    {
        std::string readerName = mManager->getReaderName();
        if(this->_card_version <= VER_3_0_UPPED_TO_3_4)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card is older than 3.5 %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, this->getCardName().c_str());
            return false;
        }
        else
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card 3.5 or newer and reader name is %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, readerName.c_str());
        }

        if(readerName.find("OMNIKEY CardMan 1021") != std::string::npos)
        {
            return true;
        }
        if(readerName.find("OMNIKEY Smart Card Reader") != std::string::npos)
        {
            return true;
        }
        if(readerName.find("SCM Microsystems Inc. SCR33x USB") != std::string::npos)
        {
            return true;
        }
        if(readerName.find("Gemalto Ezio Shield") != std::string::npos)
        {
            return true;
        }
        if(readerName.find("Oz776") != std::string::npos)
        {
            return true;
        }
        if(readerName.find("Lenovo Integrated Smart Card Reader") != std::string::npos)
        {
            return true;
        }
    }
#endif
    SCardLog::writeLog("[%i:%i][%s:%d] Extended APDU not supported", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
	return false;
}

std::string EstEIDManager::getReaderName()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	std::string readerName;
	readerName.clear();
	readerName = mManager->getReaderName();
	if(readerName.empty())
		throw runtime_error("Reader name could not be read");
	return readerName;
}

uint EstEIDManager::getTokenCount(bool forceRefresh)
{
	if(mManager == NULL)
	{
		try
		{
			this->_card_version = EstEIDManager::VER_INVALID;
			this->extAPDUSupported = false;
            return 0;
		}
		catch(...)
		{
			SCardLog::writeLog("[%i:%i][%s:%d] PCSCManager unavailible. Reporting 0 readers", 0, 0, __FUNC__, __LINE__);
			return 0;
		}
	}

	while (mManager->getTransactionId() != 0)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Card in use wating", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
		sleep(1);
	}
	uint count = this->mManager->getReaderCount(forceRefresh);
	SCardLog::writeLog("[%i:%i][%s:%d] Total tokens %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, count);

	return count;
}

std::string EstEIDManager::getFirstNameFromAuthCertificate()
{
	log();
	string firstName = "";
	getAuthCert();
	const unsigned char *p = &this->authCert[0];
	X509 *x509 = d2i_X509(NULL, &p, (long)this->authCert.size());
	X509_name_st *x509Name = X509_get_subject_name(x509);
	unsigned int count = X509_NAME_entry_count(x509Name);
	for (unsigned int i = 0; i < count; i++)
	  {
		char *value;
		char name[1024];
		X509_NAME_ENTRY *entry = X509_NAME_get_entry(x509Name, i);

		OBJ_obj2txt(name, sizeof(name), entry->object, 0);

		if(strcmp(name, "commonName") == 0)
		{
			ASN1_STRING_to_UTF8((unsigned char **) &value, entry->value);
			string cn(value);
			std::vector<std::string> x = split(cn, ',');
			if(x.size() != 3)
				break;
			else
				firstName = x[1];
		}
	  }
	return firstName;
}

std::string EstEIDManager::getSurNameFromAuthCertificate()
{
	log();
	string surName = "";
	getAuthCert();
	const unsigned char *p = &this->authCert[0];
	X509 *x509 = d2i_X509(NULL, &p, (long)this->authCert.size());
	X509_name_st *x509Name = X509_get_subject_name(x509);

	unsigned int count = X509_NAME_entry_count(x509Name);
	for (unsigned int i = 0; i < count; i++)
	{
		char *value;
		char name[1024];
		X509_NAME_ENTRY *entry = X509_NAME_get_entry(x509Name, i);
		OBJ_obj2txt(name, sizeof(name), entry->object, 0);

		if(strcmp(name, "commonName") == 0)
		{
			ASN1_STRING_to_UTF8((unsigned char **) &value, entry->value);
			string cn(value);
			std::vector<std::string> x = split(cn, ',');
			if(x.size() != 3)
				break;
			else
				surName = x[0];
		}
	  }
	return surName;
}

std::string EstEIDManager::getPersonalCodeFromAuthCertificate()
{
	log();
	string personalCode = "";
	getAuthCert();
	const unsigned char *p = &this->authCert[0];
	X509 *x509 = d2i_X509(NULL, &p, (long)this->authCert.size());
	X509_name_st *x509Name = X509_get_subject_name(x509);

	unsigned int count = X509_NAME_entry_count(x509Name);
	for (unsigned int i = 0; i < count; i++)
	{
		char *value;
		char name[1024];
		X509_NAME_ENTRY *entry = X509_NAME_get_entry(x509Name, i);
		OBJ_obj2txt(name, sizeof(name), entry->object, 0);

		if(strcmp(name, "commonName") == 0)
		{
			ASN1_STRING_to_UTF8((unsigned char **) &value, entry->value);
			string cn(value);
			std::vector<std::string> x = split(cn, ',');
			if(x.size() != 3)
				break;
			else
				personalCode = x[2];
		}
	  }
	return personalCode;
}

std::vector<std::string> EstEIDManager::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
	{
        elems.push_back(item);
    }
    return elems;
}

CardBase::FCI EstEIDManager::selectMF(bool ignoreFCI)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Selecting MF iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            return cBase->selectMF(ignoreFCI);
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
}

int EstEIDManager::selectDF(int fileID,bool ignoreFCI)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Selecting DF iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            return cBase->selectDF(fileID, ignoreFCI);
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
	
}

CardBase::FCI EstEIDManager::selectEF(int fileID,bool ignoreFCI)
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
    
    for(int i = 0; i < APDU_RETRY_COUNT; i++)
    {
        try
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Selecting EF iteraction %i.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            return cBase->selectEF(fileID, ignoreFCI);
        }
        catch (CardResetError e)
        {
            SCardLog::writeLog("[%i:%i][%s:%d] Card was reset. Will retry %i", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, i);
            mManager->resetCurrentConnection();
            continue;
        }
    }
    
    SCardLog::writeLog("[%i:%i][%s:%d] APDU retry limit reached. Throwing CardResetError.", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
    throw CardResetError();
	
}

std::string EstEIDManager::getReaderState()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();
	return mManager->getReaderState();
}

std::vector<Token> EstEIDManager::getTokenList()
{
	log();
	if(mManager == NULL)
	  throw PCSCManagerFailure();

	std::vector<std::string> readers;
	try
	{
		readers = mManager->getReadersList();
	}
	catch(NoReadersAvailible &e)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what());
		throw e;
	}
	std::vector<Token> tokens;
	
	for(unsigned int i = 0; i < readers.size(); i++)
	{
		
		try
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Connecting to reader: %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, readers[i].c_str());
			
			mManager->makeConnection((readers[i]));
			mManager->beginTransaction();
			checkProtocol();
			tokens.push_back(make_pair(readers[i], this->readRecord_internal(DOCUMENTID)));
			mManager->endTransaction();
			mManager->deleteConnection(true);
		}
		catch(NoCardInReaderError &e)
		{
			SCardLog::writeLog("[%i:%i][%s:%d] %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what());
			tokens.push_back(make_pair(readers[i], ""));
			continue;
		}
		catch(SCError &e)
		{
			if(e.error == SCARD_W_REMOVED_CARD)
			{
				SCardLog::writeLog("[%i:%i][%s:%d] No card in reader: %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, readers[i].c_str());
				tokens.push_back(make_pair(readers[i], ""));
				if(mManager != NULL)
				continue;
			}
			else
			{
				SCardLog::writeLog("[%i:%i][%s:%d] No card in reader: %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, readers[i].c_str());
				tokens.push_back(make_pair(readers[i], ""));
				if(mManager != NULL)
				continue;
			}
		}
		catch(runtime_error &e)
		{
			SCardLog::writeLog("[%i:%i][%s:%d] Runtime error: %s %s", getConnectionID(), getTransactionID(), __FUNC__, __LINE__, e.what(), readers[i].c_str());
			tokens.push_back(make_pair(readers[i], ""));
			if(mManager != NULL)
			continue;
		}
	}
	mManager->endTransaction();
	return tokens;
}

unsigned int EstEIDManager::getTransactionID()
{
	if(mManager == NULL)
		return 0;
	else
		return mManager->getTransactionId();
}

unsigned int EstEIDManager::getConnectionID()
{
	if(mManager == NULL)
		return 0;
	else
		return mManager->getConnectionID();
}

void EstEIDManager::sleep(int sec)
{
	log();
#ifdef WIN32
	Sleep(sec*1000);
#else
	sleep(sec);
#endif
}

bool EstEIDManager::isDigiID() //Must be used ONLY with readCardName function. If used with other functions Digi-ID detection may be invalid
{
	log();
	if(_card_version == VER_1_1)
		return true;
	else
		return false;
}

Certificate *EstEIDManager::getCertificateObject(KeyType keyId)
{
	if (keyId == AUTH)
	{
		authCertObj = new Certificate(getAuthCert());
		return authCertObj;
	}
	else if (keyId == SIGN)
	{
		signCertObj = new Certificate(getSignCert());
		return signCertObj;
	}
	else
	{
		return NULL;
	}
}

void EstEIDManager::checkPinPadDetection()
{
	if (getenv("SMARTCARDPP_NOPINPAD") != NULL)
	{
		SCardLog::writeLog("[%i:%i][%s:%d] Running in no pin pad mode", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
		noPinPad = true;
	}
	else
	{
		noPinPad = false;
	}
}

void EstEIDManager::checkExtendedAPDUSupport()
{
    if (getenv("SMARTCARDPP_EXTAPDU") == NULL)
    {
        SCardLog::writeLog("[%i:%i][%s:%d] Extended APDU support turned off", getConnectionID(), getTransactionID(), __FUNC__, __LINE__);
        noExtAPDU = true;
    }
    else
    {
        noExtAPDU = false;
    }
}

