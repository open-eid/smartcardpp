#pragma once
#include "EstEIDManager.h"
#include "helperMacro.h"
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#ifndef WIN32
#include <sys/time.h>
#endif

class CardTests
{
private:

	#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
		#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
	#else
	  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
	#endif
#ifdef WIN32
	struct timezone 
	{
	  int  tz_minuteswest; /* minutes W of Greenwich */
	  int  tz_dsttime;     /* type of dst correction */
	};
#endif
	int testIntensity;
	int constructorsCount;
	unsigned int selectedCardReader;
	unsigned int selectedSecondCardReader;
	std::vector<std::pair<std::string, std::string> > tokens;

	PinString PIN1;
	PinString PIN2;
	PinString PUK;

	PinString tempPIN1;
	PinString tempPIN2;
	PinString tempPUK;
	//EstEidCard *scard;
public:
	CardTests(void);
	~CardTests(void);
	bool init();
	void setTestIntesity(int intensity);
	int getTestIntensity();
	bool constructorsTest(void);
	bool isInReaderTest(void);
	bool getKeySize(void);
	bool readCardIDTest(void);
	bool readDocumentIDTest(void);
	bool readCardNameTest(void);
	bool readCardNameFirstNameFirstTest(void);
	bool readPersonalDataTest(void);
	bool getKeyUsageCountersTest(void);
	bool getAuthCertTest(void);
    bool getAuthCert_With_Context_Reset_Test(void);
	bool getSignCertTest(void);
    bool getSignCert_With_Context_Reset_Test(void);
	bool calcSSL_HashOnly_WithAuthCert(void);
	bool calcSSL_HashOnly_WithSignCert(void);
	bool calcSSL_WithPIN_WithAuthCert(void);
	bool calcSSL_WithPIN_WithSignCert(void);
	bool calcSignSHA1_HashOnly_WithSignCert(void);
	bool calcSignSHA1_WithPIN_WithSignCert(void);
	bool calcSignSHA224_HashOnly_WithSignCert(void);
	bool calcSignSHA224_WithPIN_WithSignCert(void);
	bool calcSignSHA256_HashOnly_WithSignCert(void);
	bool calcSignSHA256_WithPIN_WithSignCert(void);
	bool calcSignSHA384_HashOnly_WithSignCert(void);
	bool calcSignSHA384_WithPIN_WithSignCert(void);
	bool calcSignSHA512_HashOnly_WithSignCert(void);
	bool calcSignSHA512_WithPIN_WithSignCert(void);
	bool calcSignMD5_HashOnly_WithSignCert(void);
	bool calcSignMD5_WithPIN_WithSignCert(void);
	bool validateAuthPIN(void);
	bool validateSignPIN(void);
	bool validatePUK(void);
	bool changePIN1(void);
	bool changePIN2(void);
	bool changePUK(void);
	bool unblockPIN1(void);
	bool unblockPIN2(void);
	bool getRetryCounts(void);
	bool resetAuth(void);
	bool cardChallengeTest(void);
	bool getMD5KeyContainerName(void);
	bool readerLanguageId(void);
	bool isPinPad(void);
	bool setCardVersion(void);
	bool getCardVersion(void);
	bool RSADecrypt(void);
	bool RSADecryptWithPIN1(void);
	bool ReadDataFromMinidriverTest(void);
	bool ReadDataFromAllAvailableReadesTest();
	bool ListAllReadersUtilityStyleTest();
	bool getReaderNameTest();
	bool getReaderStateTest();
	bool getTokenCountTest();
	bool fileSelectOperationsTest();
	bool ListAllReadersPKCS11StyleTest();
	bool reconnectWithT0Test();
	bool reconnectWithT1Test();
	bool ConnectMultiplyToOneIndexTest();
	bool firefoxCardPoolingTest();
	bool isDigiIDTest();
	int gettimeofday(struct timeval *tv, struct timezone *tz);
	long timeval_diff(struct timeval *difference, struct timeval *end_time, struct timeval *start_time);
};
