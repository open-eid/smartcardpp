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
#ifndef ESTEIDMANAGER_H
#define ESTEIDMANAGER_H


#pragma once
#include "common.h"
#include "helperMacro.h"
#include <algorithm>
#include <functional>
#include <cctype>
#include <iostream>
#include <string>
#include <string.h>
#include "CardBase.h"
#include "PinString.h"
#include "SCError.h"
#include "Certificate.h"

#ifndef WIN32
	#include <PCSC/wintypes.h>
	#include <PCSC/pcsclite.h>
	#include <PCSC/winscard.h>
	#include <arpa/inet.h>
	#include <sys/time.h>
#else
	#include <Windows.h>
	#include "md5.h"
#endif

#define APDU_RETRY_COUNT 10

typedef std::pair<std::string, std::string> Token;

/// Estonian ID card class. Supplies most of the card functions
class EstEIDManager
{
protected:
	

	enum
	{
		FILEID_MASTER = 0x3F00,
		FILEID_APP	  =	0xEEEE,
		FILEID_RETRYCT =	0x0016,
		FILEID_KEYPOINTER  = 0x0033
	};

public:

	enum PinType {
		PUK = 0,
		PIN_AUTH = 1,
		PIN_SIGN = 2
	};

	enum CardVersion {
		VER_INVALID = -1,
		VER_1_0 = 0,
		VER_1_0_2007,
		VER_1_1,
		VER_3_0,
		VER_3_4,
		VER_3_0_UPPED_TO_3_4,
		VER_3_5,
		VER_3_5_1
	};

	enum KeyType {
		AUTH = 0,
		SIGN = 1
	};

	enum AlgType {
		MD5,
		SHA1,
		SHA224,
		SHA256,
		SHA384,
		SHA512,
		SSL,
		NOOID
	};

	enum RecordNames {
		SURNAME = 1,
		FIRSTNAME,
		MIDDLENAME,
		SEX,
		CITIZEN,
		BIRTHDATE,
		ID,
		DOCUMENTID,
		EXPIRY,
		BIRTHPLACE,
		ISSUEDATE,
		RESIDENCEPERMIT,
		COMMENT1,
		COMMENT2,
		COMMENT3,
		COMMENT4
	};

	enum ReaderLanguageID
	{
		ENG = 0x0409,
		EST = 0x0425,
		RUS = 0x0419
	};

private:
	bool noPinPad;
	PCSCManager *mManager;
	CardBase *cBase;
	ByteVec authCert;
	ByteVec signCert;
	Certificate *authCertObj;
	Certificate *signCertObj;
	void prepareSign_internal(KeyType keyId,const PinString &pin);
	ByteVec sign_internal(AlgType type,KeyType keyId, const ByteVec &hash);
	ByteVec RSADecrypt_internal(const ByteVec &cipher);
	ByteVec RSADecrypt_normal(const ByteVec &cryptogram);
	ByteVec RSADecrypt_chaining(const ByteVec &cryptogram);
	ByteVec RSADecrypt_extended(const ByteVec &cryptogram);
	void readPersonalData_internal(std::vector<std::string>& data, int firstRecord,int lastRecord );
	bool validatePin_internal(PinType pinType,const PinString &pin, byte &retriesLeft, bool forceUnsecure = false);
	bool changePin_internal(PinType pinType,const PinString &newPin,const PinString &oldPin,bool useUnblockCommand=false);
	void checkProtocol();
	bool getRetryCounts_internal(byte &puk,byte &pinAuth,byte &pinSign);
	ByteVec readEFAndTruncate(unsigned int fileLen);
	std::string readRecord_internal(int recordID);
	CardVersion _card_version;
	bool extAPDUSupported;
	void reIdentify();
	std::string getFirstNameFromAuthCertificate();
	std::string getSurNameFromAuthCertificate();
	std::string getPersonalCodeFromAuthCertificate();
	std::vector<std::string> split(const std::string &s, char delim);
	void pause(int sec);
	unsigned int getTransactionID();
	unsigned int getConnectionID();
	void checkPinPadDetection();
    void checkExtendedAPDUSupport();
public:
	EstEIDManager();
	EstEIDManager(unsigned int idx);
	EstEIDManager(std::string readerName);
	EstEIDManager(SCARDCONTEXT scardCTX, SCARDHANDLE scard);
	~EstEIDManager();

	void connect(unsigned int idx);
	void connect(std::string readerName);
	void disconnect();

	bool isInReader(unsigned int idx);
	bool isSecureConnection();
	void reconnectWithT0();
	void reconnectWithT1();

	/// performs PIN code verification
	void enterPin(PinType pinType, const PinString &pin, bool forceUnsecure = false);
	/// Returns the key size in bits
	unsigned int getKeySize();
	/// Reads the card holder identification code from personal data file
	std::string readCardID();
	/// Reads the card holder card number from personal data file
	std::string readDocumentID();
	/// Reads the card holder name from personal data file, bool flag flips display order
	std::string readCardName(bool firstNameFirst = false);
	/// Reads entire or parts of personal data file from firstRecord to LastRecord
	bool readPersonalData(std::vector<std::string>& data, int firstRecord=SURNAME, int lastRecord=EXPIRY);
	/// gets accumulated key usage counters from the card
	bool getKeyUsageCounters(dword &authKey,dword &signKey);
	/// gets PIN entry retry counts for all three PINs
	bool getRetryCounts(byte &puk,byte &pinAuth,byte &pinSign);
	/// retrieve Authentication certificate
	ByteVec getAuthCert();
	/// retrieve Signature certificate
	ByteVec getSignCert();
	/// decrypt RSA bytes, from 1024 bit/128 byte input vector, using authentication key
	ByteVec RSADecrypt(const ByteVec &cipher);
	/// decrypt RSA with authentication key, with pin supplied
	ByteVec RSADecrypt(const ByteVec &cipher, const PinString &pin);
	

	/// enter and validate authentication PIN. AuthError will be thrown if invalid
	bool validateAuthPin(const PinString &pin, byte &retriesLeft);
	/// enter and validate signature PIN
	bool validateSignPin(const PinString &pin, byte &retriesLeft);
	/// enter and validate PUK code
	bool validatePuk(const PinString &puk, byte &retriesLeft);

	/// change authentication PIN. For secure pin entry, specify pin lengths in "04" format, i.e. two-byte decimal string
	bool changeAuthPin(const PinString &newPin, const PinString &oldPin, byte &retriesLeft);
	/// change signature PIN
	bool changeSignPin(const PinString &newPin, const PinString &oldPin, byte &retriesLeft);
	/// change PUK
	bool changePUK(const PinString &newPUK, const PinString &oldPUK, byte &retriesLeft);
	/// unblock signature PIN using PUK. if correct PUK is supplied, the PIN will be first blocked and then unblocked
	bool unblockAuthPin(const PinString &newPin, const PinString &PUK, byte &retriesLeft);
	/// unblock signature PIN
	bool unblockSignPin(const PinString &newPin, const PinString &PUK, byte &retriesLeft);
	
	/// set security environment for the card. This does not need to be called directly, normally
	void setSecEnv(byte env);
	/// reset authentication, so next crypto operations will require new pin entry
	void resetAuth();
	/// get random challenge number
	ByteVec cardChallenge();
	/// execute a command (transmit an APDU)
	ByteVec runCommand(const ByteVec &vec);

	std::string getMD5KeyContainerName(int type);
	void startTransaction();
	void endTransaction();
	void setReaderLanguageId(ReaderLanguageID langId);
	int getReaderLanguageId(void);

	ByteVec sign(const ByteVec &hash, AlgType type, KeyType keyId);
	ByteVec sign(const ByteVec &hash, AlgType type, KeyType keyId, const PinString &pin);
	ByteVec sign_pkcs11(const ByteVec &hash, AlgType type, KeyType keyId);

	void setCardVersion();
	EstEIDManager::CardVersion getCardVersion();
	ByteVec getApplicationVersion();
	std::string getCardName();
	std::string getATRName(std::string atr);
	void sendApplicationIdentifierPreV3();
	void sendApplicationIdentifierV3();
	void sendApplicationIdentifierV3_5();
	std::string getReaderName();
	std::string getReaderState();
	uint getTokenCount(bool forceRefresh);
	CardBase::FCI selectMF(bool ignoreFCI = false);
	int selectDF(int fileID,bool ignoreFCI = false);
	CardBase::FCI selectEF(int fileID,bool ignoreFCI = false);
	std::vector<Token> getTokenList();
	bool isDigiID();
	Certificate *getCertificateObject(KeyType keyId);
};

#endif
