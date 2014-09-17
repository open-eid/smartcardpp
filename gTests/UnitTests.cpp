#include "gtest\gtest.h"
#include <string.h>
#include "EstEIDManager.h"
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include "vld.h"

int testIntensity = 3;
int selectedCardReader = 0;
int selectedSecondCardReader = 1;
PinString PIN1 = PinString("0090");
PinString PIN2 = PinString("01497");
PinString PUK = PinString("17258403");

PinString tempPIN1 = PinString("1111");
PinString tempPIN2 = PinString("22222");
PinString tempPUK = PinString("12345678");

//--gtest_filter=Constructor1Test.Positive

TEST(Constructor1Test, Positive)
{
	EstEIDManager *cardNULL = NULL;
	ASSERT_NO_THROW(cardNULL = new EstEIDManager());
	ASSERT_NO_THROW(delete cardNULL);

	printf("[----------] ");
	for (int i = 0; i < testIntensity; i++)
	{
		EstEIDManager *card1 = NULL;
		ASSERT_NO_THROW(card1 = new EstEIDManager());
		printf("%i ", i);
		if(card1 != NULL) ASSERT_NO_THROW(delete card1);
	}
	printf("\n");
}

TEST(Constructor2Test, Positive)
{
	EstEIDManager *cardNULL = NULL;
	EstEIDManager *cardNULL2 = NULL;

	ASSERT_NO_THROW(cardNULL = new EstEIDManager(0));
	ASSERT_NO_THROW(cardNULL2 = new EstEIDManager(1));
	std::string readerName = "";
	std::string readerName2 = "";
	std::string documentID = "";
	std::string documentID2 = "";
	ASSERT_NO_THROW(readerName = cardNULL->getReaderName());
	ASSERT_NO_THROW(documentID = cardNULL->readDocumentID());
	ASSERT_NO_THROW(readerName2 = cardNULL2->getReaderName());
	ASSERT_NO_THROW(documentID2 = cardNULL2->readDocumentID());

	ASSERT_STRCASENE(readerName.c_str(), readerName2.c_str());
	ASSERT_STRCASENE(documentID.c_str(), documentID2.c_str());

	ASSERT_NO_THROW(delete cardNULL);
	ASSERT_NO_THROW(delete cardNULL2);

	printf("[----------] ");
	for (int i = 0; i < testIntensity; i++)
	{
		EstEIDManager *card1 = NULL;
		EstEIDManager *card2 = NULL;

		ASSERT_NO_THROW(card1 = new EstEIDManager(0));
		ASSERT_NO_THROW(card2 = new EstEIDManager(1));
		printf("%i ", i);
		ASSERT_NO_THROW(delete card1);
		ASSERT_NO_THROW(delete card2);
	}
	printf("\n");
}

TEST(Constructor3Test, Positive)
{
	EstEIDManager *card = NULL;
	ASSERT_NO_THROW(card = new EstEIDManager());
	
	std::vector<std::pair<std::string, std::string> > tokens;

	ASSERT_NO_THROW(tokens = card->getTokenList());
	ASSERT_NO_THROW(delete card);

	EstEIDManager *cardNULL = NULL;
	EstEIDManager *cardNULL2 = NULL;
	std::string readerName = "";
	std::string readerName2 = "";
	std::string documentID = "";
	std::string documentID2 = "";

	ASSERT_NO_THROW(cardNULL = new EstEIDManager(tokens[0].first));
	ASSERT_NO_THROW(cardNULL2 = new EstEIDManager(tokens[1].first));

	ASSERT_NO_THROW(readerName = cardNULL->getReaderName());
	ASSERT_NO_THROW(documentID = cardNULL->readDocumentID());

	ASSERT_NO_THROW(readerName2 = cardNULL2->getReaderName());
	ASSERT_NO_THROW(documentID2 = cardNULL2->readDocumentID());

	ASSERT_STRCASENE(readerName.c_str(), readerName2.c_str());
	ASSERT_STRCASENE(documentID.c_str(), documentID2.c_str());

	ASSERT_NO_THROW(delete cardNULL);
	ASSERT_NO_THROW(delete cardNULL2);

	printf("[----------] ");
	for (int i = 0; i < testIntensity; i++)
	{
		EstEIDManager *card1 = NULL;
		EstEIDManager *card2 = NULL;

		ASSERT_NO_THROW(card1 = new EstEIDManager(tokens[0].first));
		ASSERT_NO_THROW(card2 = new EstEIDManager(tokens[1].first));
		printf("%i ", i);
		ASSERT_NO_THROW(delete card1);
		ASSERT_NO_THROW(delete card2);
	}
	printf("\n");
}

TEST(Constructor4Test, Positive)
{
	EstEIDManager *card = NULL;
	ASSERT_NO_THROW(card = new EstEIDManager());
	std::vector<std::pair<std::string, std::string> > tokens;
	ASSERT_NO_THROW(tokens = card->getTokenList());
	ASSERT_NO_THROW(delete card);

	printf("[----------] ");
	for (int i = 0; i < testIntensity; i++)
	{
		SCARDCONTEXT scardCtx;
		SCARDHANDLE scard;
		long lReturn = SCARD_S_SUCCESS;
		LPTSTR readers = NULL;
		LPTSTR reader = NULL;
		DWORD readersLen = SCARD_AUTOALLOCATE;
		DWORD dwAP;

		ASSERT_EQ(SCARD_S_SUCCESS, SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &scardCtx));
		ASSERT_EQ(SCARD_S_SUCCESS, SCardListReaders(scardCtx, NULL, (LPTSTR)&readers, &readersLen));
		switch (lReturn)
		{
		case SCARD_S_SUCCESS:
			reader = readers;
			break;
		case SCARD_E_NO_READERS_AVAILABLE: printf("SCARD_E_NO_READERS_AVAILABLE\r\n"); break;
		default: printf("SCardListReaders failed 0x%08X\r\n", lReturn); break;
		}
		ASSERT_EQ(SCARD_S_SUCCESS, SCardConnect(scardCtx, tokens[selectedCardReader].first.c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &scard, &dwAP));

		EstEIDManager *estEidCard = NULL;
		std::string documentid;
		ASSERT_NO_THROW(estEidCard = new EstEIDManager(scardCtx, scard));
		printf("%i ", i);
		ASSERT_NO_THROW(documentid = estEidCard->readDocumentID());
		ASSERT_NO_THROW(delete estEidCard);

		if (documentid.length() < 8 && documentid.length() > 9)
		{
			printf("Document number length is invalid: %i\r\n", documentid.length());
			SCardDisconnect(scard, SCARD_RESET_CARD);
			SCardReleaseContext(scardCtx);
		}

		ASSERT_EQ(SCARD_S_SUCCESS, SCardDisconnect(	scard, SCARD_RESET_CARD));
		ASSERT_EQ(SCARD_S_SUCCESS, SCardFreeMemory(scardCtx, readers));
		ASSERT_EQ(SCARD_S_SUCCESS, SCardReleaseContext(scardCtx));
	}
	printf("\n");
}

TEST(isInReader, Positive)
{
	EstEIDManager *mgr = NULL;
	ASSERT_NO_THROW(mgr = new EstEIDManager(0));
	printf("[----------] Testing card state in reader %s \n[----------] ", mgr->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_TRUE(mgr->isInReader(0));
		ASSERT_TRUE(mgr->isInReader(1));
	}
	printf("\n");
	if (mgr != NULL)
	{
		ASSERT_NO_THROW(delete mgr);
		mgr = NULL;
	}

	ASSERT_NO_THROW(mgr = new EstEIDManager(1));
	printf("[----------] Testing card state in reader %s \n[----------] ", mgr->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_TRUE(mgr->isInReader(0));
		ASSERT_TRUE(mgr->isInReader(1));
	}
	printf("\n");
	if (mgr != NULL)
		ASSERT_NO_THROW(delete mgr);
}

TEST(getKeySize, Positive)
{
	EstEIDManager *mgr = NULL;
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;
	unsigned int keySize = 0;
	ASSERT_NO_THROW(mgr = new EstEIDManager(0));

	printf("[----------] Testing key size for reader %s \n[----------] ", mgr->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(cardVersion = mgr->getCardVersion());
		ASSERT_NO_THROW(keySize = mgr->getKeySize());

		ASSERT_NE(EstEIDManager::VER_INVALID, cardVersion);
		if (cardVersion <= EstEIDManager::VER_1_1)
			ASSERT_EQ(1024, keySize);
		else
			ASSERT_EQ(2048, keySize);

		cardVersion = EstEIDManager::VER_INVALID;
		keySize = 0;
	}
	printf("\n");

	if (mgr != NULL)
		ASSERT_NO_THROW(delete mgr);

	ASSERT_NO_THROW(mgr = new EstEIDManager(1));
	printf("[----------] Testing key size for reader %s \n[----------] ", mgr->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(cardVersion = mgr->getCardVersion());
		ASSERT_NO_THROW(keySize = mgr->getKeySize());

		ASSERT_NE(EstEIDManager::VER_INVALID, cardVersion);
		if (cardVersion <= EstEIDManager::VER_1_1)
			ASSERT_EQ(1024, keySize);
		else
			ASSERT_EQ(2048, keySize);

		cardVersion = EstEIDManager::VER_INVALID;
		keySize = 0;
	}
	printf("\n");

	if (mgr != NULL)
		ASSERT_NO_THROW(delete mgr);
}

TEST(readCardIDTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	std::string cardID = "";
	std::string cardID2 = "";

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Testing cardID for reader %s \n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cardID = estEIDManager->readCardID());
		ASSERT_EQ(cardID.length(), 11);
		printf("%i - %s ", i, cardID.c_str());
		cardID.clear();
	}
	printf("\n");
	
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Testing cardID for reader %s \n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cardID2 = estEIDManager->readCardID());
		ASSERT_EQ(cardID2.length(), 11);
		printf("%i - %s ", i, cardID2.c_str());
		cardID2.clear();
	}
	printf("\n");

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(readDocumentIDTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;
	std::string documentID = "";
	std::string documentID2 = "";

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Testing documentID for reader %s \n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(documentID = estEIDManager->readDocumentID());
		ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());

		if (cardVersion <= EstEIDManager::VER_1_1)
			ASSERT_EQ(documentID.length(), 8);
		else
			ASSERT_EQ(documentID.length(), 9);
		printf("%i - %s ", i, documentID.c_str());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Testing cardID for reader %s \n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(documentID2 = estEIDManager->readDocumentID());
		ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());

		if (cardVersion <= EstEIDManager::VER_1_1)
			ASSERT_EQ(documentID2.length(), 8);
		else
			ASSERT_EQ(documentID2.length(), 9);
		printf("%i - %s ", i, documentID2.c_str());
	}
	printf("\n");
	ASSERT_STRCASENE(documentID.c_str(), documentID2.c_str());

	documentID.clear();
	documentID2.clear();

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(readCardNameFirstNameLastTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	std::string cardName = "";
	std::string cardName2 = "";

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Testing cardName (format surname.firstname) for reader %s \n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cardName = estEIDManager->readCardName(false));
		ASSERT_NE(cardName.length(), 0);
		printf("%i - %s ", i, cardName.c_str());
		cardName.clear();
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Testing documentID for reader %s \n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cardName2 = estEIDManager->readCardName(false));
		ASSERT_NE(cardName2.length(), 0);
		printf("%i - %s ", i, cardName2.c_str());
		cardName2.clear();
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(readCardNameFirstNameFirstTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	std::string cardName = "";
	std::string cardName2 = "";

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Testing cardName (format firstname.surname) for reader %s \n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cardName = estEIDManager->readCardName(true));
		ASSERT_NE(cardName.length(), 0);
		printf("%i - %s ", i, cardName.c_str());
		cardName.clear();
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Testing documentID for reader %s \n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cardName2 = estEIDManager->readCardName(true));
		ASSERT_NE(cardName2.length(), 0);
		printf("%i - %s ", i, cardName2.c_str());
		cardName2.clear();
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(readPersonalDataTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	vector<std::string> data;
	vector<std::string> data2;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Testing personal data reading for reader %s \n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		data.clear();
		ASSERT_TRUE(estEIDManager->readPersonalData(data));
		std::string surname = data[1]; ASSERT_NE(surname.length(), 0);
		std::string firstname = data[2]; ASSERT_NE(firstname.length(), 0);
		std::string middlename = data[3];
		std::string sex = data[4]; ASSERT_NE(sex.length(), 0);
		std::string citizen = data[5]; ASSERT_NE(citizen.length(), 0);
		std::string birthdate = data[6]; ASSERT_NE(birthdate.length(), 0);
		std::string id = data[7]; ASSERT_NE(id.length(), 0);
		std::string documentid = data[8]; ASSERT_NE(documentid.length(), 0);
		std::string expiry = data[9]; ASSERT_NE(expiry.length(), 0);
		printf("[----------] %i - %s %s %s %s %s\n", i, surname.c_str(), firstname.c_str(), id.c_str(), documentid.c_str(), expiry.c_str());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Testing personal data reading for reader %s \n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		data2.clear();
		ASSERT_TRUE(estEIDManager->readPersonalData(data2));
		std::string surname = data2[1]; ASSERT_NE(surname.length(), 0);
		std::string firstname = data2[2]; ASSERT_NE(firstname.length(), 0);
		std::string middlename = data2[3];
		std::string sex = data2[4]; ASSERT_NE(sex.length(), 0);
		std::string citizen = data2[5]; ASSERT_NE(citizen.length(), 0);
		std::string birthdate = data2[6]; ASSERT_NE(birthdate.length(), 0);
		std::string id = data2[7]; ASSERT_NE(id.length(), 0);
		std::string documentid = data2[8]; ASSERT_NE(documentid.length(), 0);
		std::string expiry = data2[9]; ASSERT_NE(expiry.length(), 0);
		printf("[----------] %i - %s %s %s %s %s\n", i, surname.c_str(), firstname.c_str(), id.c_str(), documentid.c_str(), expiry.c_str());
	}
	printf("\n");
	ASSERT_STRCASENE(data[8].c_str(), data2[8].c_str());

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(getKeyUsageCountersTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	dword authCounter, signCounter = 0;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Testing usage counters reading for reader %s \n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_TRUE(estEIDManager->getKeyUsageCounters(authCounter, signCounter));
		ASSERT_NE(authCounter, 0);
		ASSERT_NE(signCounter, 0);
		printf("[----------] %i - %i %i \n", i, authCounter, signCounter);
		authCounter, signCounter = 0;
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Testing usage counters reading for reader %s \n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_TRUE(estEIDManager->getKeyUsageCounters(authCounter, signCounter));
		ASSERT_NE(authCounter, 0);
		ASSERT_NE(signCounter, 0);
		printf("[----------] %i - %i %i \n", i, authCounter, signCounter);
		authCounter, signCounter = 0;
	}
	printf("\n");
}

TEST(getRetryCounts, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte auth, sign, puk = 0x00;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Testing retry counters reading for reader %s \n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->getRetryCounts(puk, auth, sign)));
		ASSERT_EQ(auth, 0x03);
		ASSERT_EQ(sign, 0x03);
		ASSERT_EQ(puk, 0x03);
		printf("[----------] %i - %i %i %i \n", i, auth, sign, puk);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Testing retry counters reading for reader %s \n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->getRetryCounts(puk, auth, sign)));
		ASSERT_EQ(auth, 0x03);
		ASSERT_EQ(sign, 0x03);
		ASSERT_EQ(puk, 0x03);
		printf("[----------] %i - %i %i %i \n", i, auth, sign, puk);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(getAuthCertTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	ByteVec cert;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Reading AUTH certificate from reader %s\n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cert = estEIDManager->getAuthCert());
		ASSERT_GT(cert.size(), 1000);
		printf("%i ", i);
		cert.clear();
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Reading SIGN certificate from reader %s\n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cert = estEIDManager->getAuthCert());
		ASSERT_GT(cert.size(), 1000);
		printf("%i ", i);
		cert.clear();
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(getSignCertTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	ByteVec cert;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Reading SIGN certificate from reader %s\n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cert = estEIDManager->getSignCert());
		ASSERT_GT(cert.size(), 1000);
		printf("%i ", i);
		cert.clear();
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Reading AUTH certificate from reader %s\n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cert = estEIDManager->getSignCert());
		ASSERT_GT(cert.size(), 1000);
		printf("%i ", i);
		cert.clear();
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(validateAuthPIN, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Validate PIN1 on reader %s\n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateAuthPin("", ret)));
		}
		else
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateAuthPin(PIN1, ret)));
		}
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Validate PIN1 on reader %s\n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateAuthPin("", ret)));
		}
		else
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateAuthPin(PIN1, ret)));
		}
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(validateSignPIN, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Validate PIN2 on reader %s\n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateSignPin("", ret)));
		}
		else
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateSignPin(PIN2, ret)));
		}
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Validate PIN2 on reader %s\n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateSignPin("", ret)));
		}
		else
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateSignPin(PIN2, ret)));
		}
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(validatePUK, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Validate PUK on reader %s\n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validatePuk("", ret)));
		}
		else
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validatePuk(PUK, ret)));
		}
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Validate PUK on reader %s\n[----------] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validatePuk("", ret)));
		}
		else
		{
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validatePuk(PUK, ret)));
		}
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(changePIN1, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Change PIN1 on reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("[----------] %i Change PIN1 to temp value. ", i);
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeAuthPin(tempPIN1, PIN1, ret)));
		printf("Restore original PIN1. \n");
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeAuthPin(PIN1, tempPIN1, ret)));
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Change PIN1 on reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("[----------] %i Change PIN1 to temp value. ", i);
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeAuthPin(tempPIN1, PIN1, ret)));
		printf("Restore original PIN1. \n");
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeAuthPin(PIN1, tempPIN1, ret)));
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(changePIN2, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Change PIN2 on reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("[----------] %i Change PIN2 to temp value. ", i);
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeSignPin(tempPIN2, PIN2, ret)));
		printf("Restore original PIN2. \n");
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeSignPin(PIN2, tempPIN2, ret)));
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Change PIN2 on reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("[----------] %i Change PIN2 to temp value. ", i);
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeSignPin(tempPIN2, PIN2, ret)));
		printf("Restore original PIN2. \n");
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeSignPin(PIN2, tempPIN2, ret)));
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(changePUK, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[----------] Change PUK on reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("[----------] %i Change PUK to temp value. ", i);
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changePUK(tempPUK, PUK, ret)));
		printf("Restore original PUK. \n");
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changePUK(PUK, tempPUK, ret)));
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[----------] Change PUK on reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("[----------] %i Change PUK to temp value. ", i);
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changePUK(tempPUK, PUK, ret)));
		printf("Restore original PUK. \n");
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changePUK(PUK, tempPUK, ret)));
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(unblockPIN1, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	bool lReturn = false;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Unblocking PIN1 for reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		for (int y = 0; y < 3; y++)
		{
			printf("[==========] Blocking -> ");
			try
			{
				estEIDManager->validateAuthPin("9999", ret);
			}
			catch (...)
			{
			}
			printf(" OK. ");
		}
		printf(" Unblocking PIN1... %i,", i);
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->unblockAuthPin(tempPIN1, PUK, ret)));
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeAuthPin(PIN1, tempPIN1, ret)));
		printf(" OK.\n");
	}

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
	
	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Unblocking PIN1 for reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		for (int y = 0; y < 3; y++)
		{
			printf("[==========] Blocking -> ");
			try
			{
				estEIDManager->validateAuthPin("9999", ret);
			}
			catch (...)
			{
			}
			printf(" OK. ");
		}
		printf(" Unblocking PIN1... %i,", i);
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->unblockAuthPin(tempPIN1, PUK, ret)));
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeAuthPin(PIN1, tempPIN1, ret)));
		printf(" OK.\n");
	}

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(unblockPIN2, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	bool lReturn = false;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Unblocking PIN2 for reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		for (int y = 0; y < 3; y++)
		{
			printf("[==========] Blocking -> ");
			try
			{
				estEIDManager->validateSignPin("99999", ret);
			}
			catch (...)
			{
			}
			printf(" OK. ");
		}
		printf(" Unblocking PIN2... %i,", i);
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->unblockSignPin(tempPIN2, PUK, ret)));
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeSignPin(PIN2, tempPIN2, ret)));
		printf(" OK.\n");
	}

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
	
	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Unblocking PIN2 for reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		for (int y = 0; y < 3; y++)
		{
			printf("[==========] Blocking -> ");
			try
			{
				estEIDManager->validateSignPin("99999", ret);
			}
			catch (...)
			{
			}
			printf(" OK. ");
		}
		printf(" Unblocking PIN2... %i,", i);
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->unblockSignPin(tempPIN2, PUK, ret)));
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->changeSignPin(PIN2, tempPIN2, ret)));
		printf(" OK.\n");
	}

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(cardChallengeTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	ByteVec challenge1, challenge2;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Reading card challenge from reader %s\n [==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(challenge1 = estEIDManager->cardChallenge());
		ASSERT_NO_THROW(challenge2 = estEIDManager->cardChallenge());
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Reading card challenge from reader %s\n [==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(challenge1 = estEIDManager->cardChallenge());
		ASSERT_NO_THROW(challenge2 = estEIDManager->cardChallenge());
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(readerLanguageId, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	int lang = 0;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Testing language settings for reader %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(lang = estEIDManager->getReaderLanguageId());
		ASSERT_EQ(lang, EstEIDManager::ENG);

		ASSERT_NO_THROW(estEIDManager->setReaderLanguageId(EstEIDManager::RUS));
		ASSERT_NO_THROW(lang = estEIDManager->getReaderLanguageId());
		ASSERT_EQ(lang, EstEIDManager::RUS);

		ASSERT_NO_THROW(estEIDManager->setReaderLanguageId(EstEIDManager::EST));
		ASSERT_NO_THROW(lang = estEIDManager->getReaderLanguageId());
		ASSERT_EQ(lang, EstEIDManager::EST);

		ASSERT_NO_THROW(estEIDManager->setReaderLanguageId(EstEIDManager::ENG));
		ASSERT_NO_THROW(lang = estEIDManager->getReaderLanguageId());
		ASSERT_EQ(lang, EstEIDManager::ENG);
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Testing language settings for reader %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(lang = estEIDManager->getReaderLanguageId());
		ASSERT_EQ(lang, EstEIDManager::ENG);

		ASSERT_NO_THROW(estEIDManager->setReaderLanguageId(EstEIDManager::RUS));
		ASSERT_NO_THROW(lang = estEIDManager->getReaderLanguageId());
		ASSERT_EQ(lang, EstEIDManager::RUS);

		ASSERT_NO_THROW(estEIDManager->setReaderLanguageId(EstEIDManager::EST));
		ASSERT_NO_THROW(lang = estEIDManager->getReaderLanguageId());
		ASSERT_EQ(lang, EstEIDManager::EST);

		ASSERT_NO_THROW(estEIDManager->setReaderLanguageId(EstEIDManager::ENG));
		ASSERT_NO_THROW(lang = estEIDManager->getReaderLanguageId());
		ASSERT_EQ(lang, EstEIDManager::ENG);
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(isPinPad, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	bool lReturn = false;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Testing PinPad for reader %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(lReturn = estEIDManager->isSecureConnection());
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Testing PinPad for reader %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(lReturn = estEIDManager->isSecureConnection());
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(setCardVersion, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Testing set card version for reader %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(estEIDManager->setCardVersion());
		ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
		ASSERT_NE(cardVersion, EstEIDManager::VER_INVALID);
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Testing set card version for reader %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(estEIDManager->setCardVersion());
		ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
		ASSERT_NE(cardVersion, EstEIDManager::VER_INVALID);
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(getCardVersion, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;
	ByteVec ver;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Testing get card version for reader %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
		ASSERT_NE(cardVersion, EstEIDManager::VER_INVALID);
		if (cardVersion == EstEIDManager::VER_1_1)
		{
			ASSERT_NO_THROW(ver = estEIDManager->getApplicationVersion());
			if (ver[0] == 0x01 && ver[1] == 0x01)
				cardVersion = EstEIDManager::VER_1_1;
		}

		ASSERT_NE(cardVersion, EstEIDManager::VER_INVALID);
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Testing get card version for reader %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
		ASSERT_NE(cardVersion, EstEIDManager::VER_INVALID);
		if (cardVersion == EstEIDManager::VER_1_1)
		{
			ASSERT_NO_THROW(ver = estEIDManager->getApplicationVersion());
			if (ver[0] == 0x01 && ver[1] == 0x01)
				cardVersion = EstEIDManager::VER_1_1;
		}

		ASSERT_NE(cardVersion, EstEIDManager::VER_INVALID);
		printf("%i ", i);
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(getMD5KeyContainerName, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	std::string authContainer, signContainer = "";

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Calculating key container names for reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(authContainer = estEIDManager->getMD5KeyContainerName(EstEIDManager::AUTH););
		ASSERT_NO_THROW(signContainer = estEIDManager->getMD5KeyContainerName(EstEIDManager::SIGN));
		printf("[==========] %i AUTH - %s SIGN - %s\n", i, authContainer.c_str(), signContainer.c_str());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Calculating key container names for reader %s\n", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(authContainer = estEIDManager->getMD5KeyContainerName(EstEIDManager::AUTH););
		ASSERT_NO_THROW(signContainer = estEIDManager->getMD5KeyContainerName(EstEIDManager::SIGN));
		printf("[==========] %i AUTH - %s SIGN - %s\n", i, authContainer.c_str(), signContainer.c_str());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(calcSSL_HashOnly_WithAuthCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[20] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45 };
	ByteVec hash(MAKEVECTOR(hashByte));

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Calculating SSL signarute with AUTH key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(estEIDManager->startTransaction());
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_AUTH, "", false));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_AUTH, PIN1, false));
		}
		ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH));

		ASSERT_NO_THROW(estEIDManager->endTransaction());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Calculating SSL signarute with AUTH key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(estEIDManager->startTransaction());
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_AUTH, "", false));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_AUTH, PIN1, false));
		}
		ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH));

		ASSERT_NO_THROW(estEIDManager->endTransaction());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(calcSSL_HashOnly_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[20] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45 };
	ByteVec hash(MAKEVECTOR(hashByte));

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Calculating SSL signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(estEIDManager->startTransaction());
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, "", false));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2, false));
		}
		ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::SIGN));

		ASSERT_NO_THROW(estEIDManager->endTransaction());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Calculating SSL signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(estEIDManager->startTransaction());
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, "", false));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2, false));
		}
		ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::SIGN));

		ASSERT_NO_THROW(estEIDManager->endTransaction());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(calcSSL_WithPIN_WithAuthCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[20] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45 };
	ByteVec hash(MAKEVECTOR(hashByte));

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Calculating SSL signarute with AUTH key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH, PIN1));
		}
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Calculating SSL signarute with AUTH key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH, PIN1));
		}
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(calcSSL_WithPIN_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[20] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45 };
	ByteVec hash(MAKEVECTOR(hashByte));

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	printf("[==========] Calculating SSL signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::SIGN, PIN2));
		}
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Calculating SSL signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::SIGN, PIN2));
		}
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(signSHA1_HashOnly_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[20] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating SHA1 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(estEIDManager->startTransaction());
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
		}
		ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN));
		ASSERT_NO_THROW(estEIDManager->endTransaction());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Calculating SHA1 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(estEIDManager->startTransaction());
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
		}
		ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN));
		ASSERT_NO_THROW(estEIDManager->endTransaction());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(calcSignSHA1_WithPIN_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[20] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating SHA1 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN, PIN2));
		}
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Calculating SHA1 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN, PIN2));
		}
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(signSHA224_HashOnly_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[28] = { 0x22, 0x12, 0x69, 0x96, 0x1d, 0x5a, 0x1f, 0xee, 0x3f, 0x9b,
		0x98, 0xf4, 0x2c, 0x58, 0x72, 0xa0, 0xa9, 0x06, 0x02, 0x98,
		0xab, 0x55, 0xa2, 0xf9, 0x78, 0xed, 0x20, 0xe0 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating SHA224 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(estEIDManager->startTransaction());
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
		}
		ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN));
		ASSERT_NO_THROW(estEIDManager->endTransaction());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Calculating SHA224 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(estEIDManager->startTransaction());
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
		}
		ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN));
		ASSERT_NO_THROW(estEIDManager->endTransaction());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(calcSignSHA224_WithPIN_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[28] = { 0x22, 0x12, 0x69, 0x96, 0x1d, 0x5a, 0x1f, 0xee, 0x3f, 0x9b,
		0x98, 0xf4, 0x2c, 0x58, 0x72, 0xa0, 0xa9, 0x06, 0x02, 0x98,
		0xab, 0x55, 0xa2, 0xf9, 0x78, 0xed, 0x20, 0xe0 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating SHA224 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN, PIN2));
		}
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Calculating SHA224 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN, PIN2));
		}
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(signSHA256_HashOnly_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[32] = { 0x0b, 0x67, 0xfe, 0x78, 0x15, 0x5d, 0x63, 0x22, 0xce, 0xb8,
		0x28, 0x5f, 0xe1, 0x1d, 0x93, 0x45, 0xea, 0xce, 0x9f, 0x8f,
		0x6a, 0xf8, 0x1f, 0xf0, 0x17, 0x87, 0x99, 0x1c, 0x54, 0x3f,
		0x94, 0xf8 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating SHA256 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	//if (cardVersion > EstEIDManager::VER_1_0_2007)
	//{
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);
			ASSERT_NO_THROW(estEIDManager->startTransaction());
			if (estEIDManager->isSecureConnection())
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
			}
			else
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
			}
			if (cardVersion > EstEIDManager::VER_1_0_2007)
				ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN));
			else
				ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN), CardError);
			ASSERT_NO_THROW(estEIDManager->endTransaction());
		}
		printf("\n");
		if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
		printf("[==========] Calculating SHA256 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);
			ASSERT_NO_THROW(estEIDManager->startTransaction());
			if (estEIDManager->isSecureConnection())
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
			}
			else
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
			}
			if (cardVersion > EstEIDManager::VER_1_0_2007)
				ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN));
			else
				ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN), CardError);
			ASSERT_NO_THROW(estEIDManager->endTransaction());
		}
	//}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(calcSignSHA256_WithPIN_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[32] = { 0x0b, 0x67, 0xfe, 0x78, 0x15, 0x5d, 0x63, 0x22, 0xce, 0xb8,
		0x28, 0x5f, 0xe1, 0x1d, 0x93, 0x45, 0xea, 0xce, 0x9f, 0x8f,
		0x6a, 0xf8, 0x1f, 0xf0, 0x17, 0x87, 0x99, 0x1c, 0x54, 0x3f,
		0x94, 0xf8 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating SHA256 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	//if (cardVersion > EstEIDManager::VER_1_0_2007)
	//{
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);
			if (estEIDManager->isSecureConnection())
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, ""));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, ""), CardError);
			}
			else
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, PIN2));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, PIN2), CardError);
			}
		}
		printf("\n");
		if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
		printf("[==========] Calculating SHA256 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);
			if (estEIDManager->isSecureConnection())
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, ""));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, ""), CardError);
			}
			else
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, PIN2));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, PIN2), CardError);
			}
		}
		printf("\n");
	//}

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(signSHA384_HashOnly_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[48] = { 0xa3, 0xb7, 0xb4, 0xe4, 0xa7, 0xbf, 0xfa, 0x39, 0xc2, 0xa7,
		0x6c, 0xf8, 0x76, 0xe6, 0x42, 0xee, 0x57, 0x99, 0x12, 0x67,
		0xe5, 0xdd, 0xdd, 0xc1, 0xd8, 0xa9, 0x45, 0x04, 0xb9, 0x5b,
		0xbd, 0xfe, 0xfe, 0xaf, 0x87, 0xa6, 0x32, 0xa1, 0x7b, 0xc0,
		0xd5, 0x70, 0x7a, 0x57, 0xda, 0xc7, 0x42, 0x60 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating SHA384 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	//if (cardVersion > EstEIDManager::VER_1_0_2007)
	//{
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);
			ASSERT_NO_THROW(estEIDManager->startTransaction());
			if (estEIDManager->isSecureConnection())
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
			}
			else
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
			}
			if (cardVersion > EstEIDManager::VER_1_0_2007)
				ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN));
			else
				ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN), CardError);
			ASSERT_NO_THROW(estEIDManager->endTransaction());
		}
		printf("\n");
		if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
		printf("[==========] Calculating SHA384 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);
			ASSERT_NO_THROW(estEIDManager->startTransaction());
			if (estEIDManager->isSecureConnection())
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
			}
			else
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
			}
	
			if (cardVersion > EstEIDManager::VER_1_0_2007)
				ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN));
			else
				ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN), CardError);
			ASSERT_NO_THROW(estEIDManager->endTransaction());
		}
		printf("\n");
	//}

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(calcSignSHA384_WithPIN_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[48] = { 0xa3, 0xb7, 0xb4, 0xe4, 0xa7, 0xbf, 0xfa, 0x39, 0xc2, 0xa7,
		0x6c, 0xf8, 0x76, 0xe6, 0x42, 0xee, 0x57, 0x99, 0x12, 0x67,
		0xe5, 0xdd, 0xdd, 0xc1, 0xd8, 0xa9, 0x45, 0x04, 0xb9, 0x5b,
		0xbd, 0xfe, 0xfe, 0xaf, 0x87, 0xa6, 0x32, 0xa1, 0x7b, 0xc0,
		0xd5, 0x70, 0x7a, 0x57, 0xda, 0xc7, 0x42, 0x60 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating SHA384 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	//if (cardVersion > EstEIDManager::VER_1_0_2007)
	//{
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);

			if (estEIDManager->isSecureConnection())
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, ""));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, ""), CardError);
			}
			else
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, PIN2));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, PIN2), CardError);
			}
		}
		printf("\n");
		if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
		printf("[==========] Calculating SHA384 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);
			if (estEIDManager->isSecureConnection())
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, ""));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, ""), CardError);
			}
			else
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, PIN2));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, PIN2), CardError);
			}
		}
		printf("\n");
	//}

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(signSHA512_HashOnly_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[48] = { 0xa3, 0xb7, 0xb4, 0xe4, 0xa7, 0xbf, 0xfa, 0x39, 0xc2, 0xa7,
		0x6c, 0xf8, 0x76, 0xe6, 0x42, 0xee, 0x57, 0x99, 0x12, 0x67,
		0xe5, 0xdd, 0xdd, 0xc1, 0xd8, 0xa9, 0x45, 0x04, 0xb9, 0x5b,
		0xbd, 0xfe, 0xfe, 0xaf, 0x87, 0xa6, 0x32, 0xa1, 0x7b, 0xc0,
		0xd5, 0x70, 0x7a, 0x57, 0xda, 0xc7, 0x42, 0x60 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating SHA512 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	//if (cardVersion > EstEIDManager::VER_1_0_2007)
	//{
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);
			ASSERT_NO_THROW(estEIDManager->startTransaction());
			if (estEIDManager->isSecureConnection())
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
			}
			else
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
			}
			if (cardVersion > EstEIDManager::VER_1_0_2007)
				ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN));
			else
				ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN), CardError);

			ASSERT_NO_THROW(estEIDManager->endTransaction());
		}
		printf("\n");
		if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
		printf("[==========] Calculating SHA512 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);
			ASSERT_NO_THROW(estEIDManager->startTransaction());
			if (estEIDManager->isSecureConnection())
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
			}
			else
			{
				ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
			}

			if (cardVersion > EstEIDManager::VER_1_0_2007)
				ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN));
			else
				ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN), CardError);

			ASSERT_NO_THROW(estEIDManager->endTransaction());
		}
		printf("\n");
	//}

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(calcSignSHA512_WithPIN_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[48] = { 0xa3, 0xb7, 0xb4, 0xe4, 0xa7, 0xbf, 0xfa, 0x39, 0xc2, 0xa7,
		0x6c, 0xf8, 0x76, 0xe6, 0x42, 0xee, 0x57, 0x99, 0x12, 0x67,
		0xe5, 0xdd, 0xdd, 0xc1, 0xd8, 0xa9, 0x45, 0x04, 0xb9, 0x5b,
		0xbd, 0xfe, 0xfe, 0xaf, 0x87, 0xa6, 0x32, 0xa1, 0x7b, 0xc0,
		0xd5, 0x70, 0x7a, 0x57, 0xda, 0xc7, 0x42, 0x60 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating SHA512 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	//if (cardVersion > EstEIDManager::VER_1_0_2007)
	//{
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);
			if (estEIDManager->isSecureConnection())
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, ""));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, ""), CardError);
			}
			else
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, PIN2));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, PIN2), CardError);
			}
		}
		printf("\n");
		if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
		printf("[==========] Calculating SHA512 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
		for (int i = 1; i <= testIntensity; i++)
		{
			printf("%i ", i);
			if (estEIDManager->isSecureConnection())
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, ""));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, ""), CardError);
			}
			else
			{
				if (cardVersion > EstEIDManager::VER_1_0_2007)
					ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, PIN2));
				else
					ASSERT_THROW(estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, PIN2), CardError);
			}
		}
		printf("\n");
	//}
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(signMD5_HashOnly_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[20] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating MD5 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(estEIDManager->startTransaction());
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
		}
		ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN));
		ASSERT_NO_THROW(estEIDManager->endTransaction());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Calculating MD5 signarute with SIGN key providing hash only %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		ASSERT_NO_THROW(estEIDManager->startTransaction());
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2));
		}
		ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN));
		ASSERT_NO_THROW(estEIDManager->endTransaction());
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(calcSignMD5_WithPIN_WithSignCert, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[20] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());
	printf("[==========] Calculating MD5 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN, PIN2));
		}
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
	printf("[==========] Calculating MD5 signarute with SIGN key providing hash and PIN %s\n[==========] ", estEIDManager->getReaderName().c_str());
	for (int i = 1; i <= testIntensity; i++)
	{
		printf("%i ", i);
		if (estEIDManager->isSecureConnection())
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN, ""));
		}
		else
		{
			ASSERT_NO_THROW(estEIDManager->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN, PIN2));
		}
	}
	printf("\n");
	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(RSADecrypt, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[20] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());

	for (int i = 1; i <= testIntensity; i++)
	{
		ByteVec authCert;
		printf("[----------] Reading AUTH certificate \n");
		ASSERT_NO_THROW(authCert = estEIDManager->getAuthCert());
		ASSERT_GT(authCert.size(), 1000);

		const int authCertLength = (unsigned int)authCert.size();
		unsigned char *buf;
		ASSERT_NE(authCertLength, 0);

		printf("[----------] Creating X509 & RSA structures \n");

		buf = (unsigned char *)OPENSSL_malloc(authCertLength);

		for (int i = 0; i < authCertLength; i++)
			buf[i] = authCert[i];

		const unsigned char *pBuf = buf;
		X509 *cert;
		cert = d2i_X509(NULL, &pBuf, authCertLength);
		if (cert == NULL)
		{
			printf("[----------] FAILED. Failed to create X509 structure \n");
			OPENSSL_free(buf);
			if (estEIDManager) delete estEIDManager;
		}
		else
		{
			printf("[----------] X509 structure created \n");
		}

		EVP_PKEY *pkey = X509_get_pubkey(cert);
		RSA *rsa = EVP_PKEY_get1_RSA(pkey);

		printf("[----------] Encrypting data with AUTH public key \n");

		unsigned char origData[] = { 0x54, 0x48, 0x49, 0x53, 0x20, 0x53, 0x4F, 0x46, 0x54, 0x57, 0x41, 0x52, 0x45, 0x20, 0x49, 0x53,
			0x20, 0x50, 0x52, 0x4F, 0x56, 0x49, 0x44, 0x45, 0x44, 0x20, 0x42, 0x59, 0x20, 0x41, 0x4E, 0x54,
			0x4F, 0x4E, 0x20, 0x4C, 0x49, 0x54, 0x4F, 0x56, 0x54, 0x53, 0x45, 0x4E, 0x4B, 0x4F };
		const unsigned char *pOrigData = &origData[0];
		unsigned char cryptogram[1024];
		int origDataLen = sizeof(origData);
		memset((void *)cryptogram, 0, sizeof(cryptogram));

		int lReturn = RSA_public_encrypt(origDataLen, pOrigData, &cryptogram[0], rsa, RSA_PKCS1_PADDING);
		ASSERT_NE(lReturn, 0);
		if (lReturn < 0)
		{
			printf("[----------] FAILED. Failed to encrypt data \n");
		}
		else
		{
			printf("[----------] Data successfully encrypted \n");
		}

		printf("[----------] Verifying PIN1 & decrypting data with AUTH private key \n");

		ByteVec cryptoVector(cryptogram, cryptogram + sizeof(cryptogram) / sizeof(unsigned char));
		ASSERT_NE(cryptoVector.size(), 0);
		cryptoVector.resize(lReturn, 0x00);
		byte ret = 0x03;
		estEIDManager->validateAuthPin(PIN1, ret);
		ByteVec decryptedData = estEIDManager->RSADecrypt(cryptoVector);
		ASSERT_NE(decryptedData.size(), 0);
		printf("\n\n");
		for (unsigned int i = 0; i < decryptedData.size(); i++)
		{
			printf("%X == %X; ", decryptedData.at(i), origData[i]);
			if (decryptedData.at(i) != origData[i]) 
			{
				OPENSSL_free(buf);
				printf("[----------] FAILED. Data verification failed \n");
				if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
			}
			else
			{
				printf(" == OK \n");
			}
		}
		OPENSSL_free(buf);
	}

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);

	
}

TEST(RSADecryptWithPIN, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	byte ret = 0x03;
	byte hashByte[20] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45 };
	ByteVec hash(MAKEVECTOR(hashByte));
	EstEIDManager::CardVersion cardVersion = EstEIDManager::VER_INVALID;

	ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
	ASSERT_NO_THROW(cardVersion = estEIDManager->getCardVersion());

	for (int i = 1; i <= testIntensity; i++)
	{
		ByteVec authCert;
		printf("[----------] Reading AUTH certificate \n");
		ASSERT_NO_THROW(authCert = estEIDManager->getAuthCert());
		ASSERT_GT(authCert.size(), 1000);

		const int authCertLength = (unsigned int)authCert.size();
		unsigned char *buf;
		ASSERT_NE(authCertLength, 0);

		printf("[----------] Creating X509 & RSA structures \n");
		buf = (unsigned char *)OPENSSL_malloc(authCertLength);

		for (int i = 0; i < authCertLength; i++)
			buf[i] = authCert[i];

		const unsigned char *pBuf = buf;
		X509 *cert;
		cert = d2i_X509(NULL, &pBuf, authCertLength);
		if (cert == NULL)
		{
			printf("[----------] FAILED. Failed to create X509 structure \n");
			OPENSSL_free(buf);
			if (estEIDManager) delete estEIDManager;
		}
		else
		{
			printf("[----------] X509 structure created \n");
		}

		EVP_PKEY *pkey = X509_get_pubkey(cert);
		RSA *rsa = EVP_PKEY_get1_RSA(pkey);

		printf("[----------] Encrypting data with AUTH public key \n");

		unsigned char origData[] = { 0x54, 0x48, 0x49, 0x53, 0x20, 0x53, 0x4F, 0x46, 0x54, 0x57, 0x41, 0x52, 0x45, 0x20, 0x49, 0x53,
			0x20, 0x50, 0x52, 0x4F, 0x56, 0x49, 0x44, 0x45, 0x44, 0x20, 0x42, 0x59, 0x20, 0x41, 0x4E, 0x54,
			0x4F, 0x4E, 0x20, 0x4C, 0x49, 0x54, 0x4F, 0x56, 0x54, 0x53, 0x45, 0x4E, 0x4B, 0x4F };
		const unsigned char *pOrigData = &origData[0];
		unsigned char cryptogram[1024];
		int origDataLen = sizeof(origData);
		memset((void *)cryptogram, 0, sizeof(cryptogram));

		int lReturn = RSA_public_encrypt(origDataLen, pOrigData, &cryptogram[0], rsa, RSA_PKCS1_PADDING);
		ASSERT_NE(lReturn, 0);
		if (lReturn < 0)
		{
			printf("[----------] FAILED. Failed to encrypt data \n");
		}
		else
		{
			printf("[----------] Data successfully encrypted \n");
		}

		printf("[----------] Verifying PIN1 & decrypting data with AUTH private key \n");

		ByteVec cryptoVector(cryptogram, cryptogram + sizeof(cryptogram) / sizeof(unsigned char));
		ASSERT_NE(cryptoVector.size(), 0);
		cryptoVector.resize(lReturn, 0x00);
		byte ret = 0x03;
		ByteVec decryptedData = estEIDManager->RSADecrypt(cryptoVector, PIN1);
		ASSERT_NE(decryptedData.size(), 0);
		printf("\n\n");
		for (unsigned int i = 0; i < decryptedData.size(); i++)
		{
			printf("%X == %X; ", decryptedData.at(i), origData[i]);
			if (decryptedData.at(i) != origData[i])
			{
				OPENSSL_free(buf);
				printf("[----------] FAILED. Data verification failed ");
				if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
			}
			else
			{
				printf(" == OK \n");
			}
		}
		OPENSSL_free(buf);
	}

	if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
}

TEST(readDataFromMinidriverTest, Positive)
{
	for (int i = 1; i <= testIntensity; i++)
	{
		SCARDCONTEXT scardCtx;
		SCARDHANDLE scard;
		long lReturn = SCARD_S_SUCCESS;
		LPTSTR readers = NULL;
		LPTSTR reader = NULL;
		DWORD readersLen = SCARD_AUTOALLOCATE;
		DWORD dwAP;

		ASSERT_EQ(SCARD_S_SUCCESS, SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &scardCtx));
		ASSERT_EQ(SCARD_S_SUCCESS, SCardListReaders(scardCtx, NULL, (LPTSTR)&readers, &readersLen));

		switch (lReturn)
		{
		case SCARD_S_SUCCESS:
			reader = readers;
			break;
		case SCARD_E_NO_READERS_AVAILABLE: printf("SCARD_E_NO_READERS_AVAILABLE\r\n"); break;
		default: printf("SCardListReaders failed 0x%08X\r\n", lReturn);  break;
		}


		ASSERT_EQ(SCARD_S_SUCCESS, SCardConnect(scardCtx, reader, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &scard, &dwAP));
		
		EstEIDManager *estEIDManager;
		std::string documentID = "";
		ByteVec authCert, signCert;

		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(scardCtx, scard));
		byte ret = 0x03;
		printf("[----------] Reader %s ", estEIDManager->getReaderName().c_str());
		printf("CP_CARD_GUID ");
		ASSERT_NO_THROW(documentID = estEIDManager->readDocumentID());
		printf("AUTH ");
		ASSERT_NO_THROW(authCert = estEIDManager->getAuthCert());
		printf("SIGN ");
		ASSERT_NO_THROW(signCert = estEIDManager->getSignCert());

		printf("PIN1 ");
		if (estEIDManager->isSecureConnection())
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateAuthPin("", ret)));
		else
		ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateAuthPin(PIN1, ret)));
		
		printf("PIN2 ");
		if (estEIDManager->isSecureConnection())
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateSignPin("", ret)));
		else
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validateSignPin(PIN2, ret)));
		printf("PUK \n");
		if (estEIDManager->isSecureConnection())
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validatePuk("", ret)));
		else
			ASSERT_NO_THROW(ASSERT_TRUE(estEIDManager->validatePuk(PUK, ret)));
		delete estEIDManager;

		ASSERT_EQ(SCARD_S_SUCCESS, SCardDisconnect(scard, SCARD_RESET_CARD));
		ASSERT_EQ(SCARD_S_SUCCESS, SCardFreeMemory(scardCtx, readers));
		ASSERT_EQ(SCARD_S_SUCCESS, SCardReleaseContext(scardCtx));
	}
}

TEST(readDataFromAllAvailableReadersTest, Positive)
{
	for (int i = 1; i <= testIntensity; i++)
	{
		EstEIDManager *estEIDManager = NULL;
		uint readersCount = 0;
		std::vector<std::pair<std::string, std::string> > tokens;

		ASSERT_NO_THROW(estEIDManager = new EstEIDManager());
		ASSERT_NO_THROW(readersCount = estEIDManager->getTokenCount(false));
		tokens = estEIDManager->getTokenList();
		if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
		printf("\n[----------] Reading data from all %i readers\n", readersCount);
		for (uint i = 0; i < readersCount; i++)
		{
			EstEIDManager *estEIDManagerInternal;
			byte ret = 0x03;
			

			if (tokens[i].second.compare("EMPTY") != 0)
			{
				std::string readerName = "";
				std::string documentID = "";
				ByteVec authCert, signCert;

				ASSERT_NO_THROW(estEIDManagerInternal = new EstEIDManager(i));

				ASSERT_NO_THROW(readerName = estEIDManagerInternal->getReaderName());
				ASSERT_NE(readerName.size(), 0);
				printf("[----------] %i. Reader name: %s \n", i, readerName.c_str());

				ASSERT_NO_THROW(documentID = estEIDManagerInternal->readDocumentID());
				ASSERT_NE(documentID.size(), 0);
				printf("[----------]           %i. Document number: %s \n", i, documentID.c_str());

				ASSERT_NO_THROW(authCert = estEIDManagerInternal->getAuthCert());
				ASSERT_NE(authCert.size(), 0);
				printf("[----------]           %i. AUTH certificate length: %i \n", i, authCert.size());
				ASSERT_NO_THROW(signCert = estEIDManagerInternal->getSignCert());
				ASSERT_NE(signCert.size(), 0);
				printf("[----------]           %i. SIGN certificate length: %i \n", i, signCert.size());

				if (estEIDManagerInternal->isSecureConnection() == true)
				{
						printf("[----------]           %i. Validate PIN1 using PINPAD\n", i);
						ASSERT_NO_THROW(ASSERT_TRUE(estEIDManagerInternal->validateAuthPin("", ret)));
					}
					else
					{
						printf("[----------]           %i. Validate PIN1\n", i);
						ASSERT_NO_THROW(ASSERT_TRUE(estEIDManagerInternal->validateAuthPin(PIN1, ret)));
					}
					if (estEIDManagerInternal->isSecureConnection() == true)
					{
						printf("[----------]           %i. Validate PIN2  using PINPAD\n", i);
						ASSERT_NO_THROW(ASSERT_TRUE(estEIDManagerInternal->validateSignPin("", ret)));
					}
					else
					{
						printf("[----------]           %i. Validate PIN2 \n", i);
						ASSERT_NO_THROW(ASSERT_TRUE(estEIDManagerInternal->validateSignPin(PIN2, ret)));
					}

					if (estEIDManagerInternal->isSecureConnection() == true)
					{
						printf("[----------]           %i. Validate PUK  using PINPAD\n", i);
						ASSERT_NO_THROW(ASSERT_TRUE(estEIDManagerInternal->validatePuk("", ret)));
					}
					else
					{
						printf("[----------]           %i. Validate PUK \n", i);
						ASSERT_NO_THROW(ASSERT_TRUE(estEIDManagerInternal->validatePuk(PUK, ret)));
					}
					ASSERT_NO_THROW(delete estEIDManagerInternal);
			}
			else
			{
				printf("[----------] Reader %s is empty. Skipping. \n", tokens[i].first.c_str());
			}
		}
	}
}

TEST(listAllReadersUtilityStyleTest, Positive)
{
	for(int i = 1; i <= testIntensity; i++)
	{
		EstEIDManager *estEIDManager = NULL;
		ASSERT_NO_THROW(estEIDManager = new EstEIDManager());
		std::vector<std::pair<std::string, std::string> > tokens;

		ASSERT_NO_THROW(tokens = estEIDManager->getTokenList());
		ASSERT_NE(tokens.size(), 0);
		for (unsigned int i = 0; i < tokens.size(); i++)
		{
			printf("[----------] Reader \"%s\" has %s token \n", tokens[i].first.c_str(), tokens[i].second.c_str());
		}
		ASSERT_NO_THROW(delete estEIDManager);

		printf("[----------] Connecting to readers by name and reading data %i\n", tokens.size());
		for (unsigned int i = 0; i < tokens.size(); i++)
		{
			if (tokens[i].second.compare("") != 0)
			{
				std::string readerName = "";
				std::string readerState = "";
				std::string documentID = "";

				estEIDManager = new EstEIDManager(tokens[i].first);
				ASSERT_NO_THROW(readerName = estEIDManager->getReaderName());
				ASSERT_NO_THROW(readerState = estEIDManager->getReaderState());
				ASSERT_NO_THROW(documentID = estEIDManager->readDocumentID());

				ASSERT_NE(readerName.size(), 0);
				ASSERT_NE(readerState.size(), 0);
				ASSERT_NE(documentID.size(), 0);

				printf("                    %i. Reader name: %s \n", i, readerName.c_str());
				printf("                    %i. Reader state: %s \n", i, readerState.c_str());
				printf("                    %i. Document number: %s \n", i, documentID.c_str());
				printf("\n");
				if (estEIDManager) ASSERT_NO_THROW(delete estEIDManager);
			}
		}
		printf("\n");
	}
}

TEST(getReaderNameTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	EstEIDManager *estEIDManager2 = NULL;

	printf("[----------] Reading reader names \n");
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
		ASSERT_NO_THROW(estEIDManager2 = new EstEIDManager(1));

		std::string readerName = "";
		std::string readerName2 = "";

		ASSERT_NO_THROW(readerName = estEIDManager->getReaderName());
		ASSERT_NO_THROW(readerName2 = estEIDManager2->getReaderName());

		ASSERT_NE(readerName.size(), 0);
		ASSERT_NE(readerName2.size(), 0);

		printf("                  %i. Reader name: %s \n", i, readerName.c_str());
		printf("                  %i. Reader name: %s \n", i, readerName2.c_str());

		ASSERT_NO_THROW(delete estEIDManager);
		ASSERT_NO_THROW(delete estEIDManager2);
	}
}

TEST(getReaderStateTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	EstEIDManager *estEIDManager2 = NULL;

	printf("[----------] Reading reader states \n");
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
		ASSERT_NO_THROW(estEIDManager2 = new EstEIDManager(1));

		std::string readerStateName = "";
		std::string readerStateName2 = "";

		ASSERT_NO_THROW(readerStateName = estEIDManager->getReaderState());
		ASSERT_NO_THROW(readerStateName2 = estEIDManager2->getReaderState());

		ASSERT_NE(readerStateName.size(), 0);
		ASSERT_NE(readerStateName2.size(), 0);

		printf("                  %i. Reader state: %s \n", i, readerStateName.c_str());
		printf("                  %i. Reader state: %s \n", i, readerStateName2.c_str());

		ASSERT_NO_THROW(delete estEIDManager);
		ASSERT_NO_THROW(delete estEIDManager2);
	}
}

TEST(getTokenCountTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	EstEIDManager *estEIDManager2 = NULL;

	printf("[----------] Counting tokens \n");
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
		ASSERT_NO_THROW(estEIDManager2 = new EstEIDManager(1));

		int readerCount = 0;
		int readerCount2 = 0;

		ASSERT_NO_THROW(readerCount = estEIDManager->getTokenCount(false));
		ASSERT_NO_THROW(readerCount2 = estEIDManager2->getTokenCount(true));

		ASSERT_NE(readerCount, 0);
		ASSERT_NE(readerCount2, 0);

		printf("                  %i. Totally %i tokens availible \n", i, readerCount);
		printf("                  %i. Totally %i tokens availible \n", i, readerCount2);

		ASSERT_NO_THROW(delete estEIDManager);
		ASSERT_NO_THROW(delete estEIDManager2);
	}
}

TEST(fileSelectOperationsTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	EstEIDManager *estEIDManager2 = NULL;

	printf("[----------] Executing custom APDUs ");
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
		ASSERT_NO_THROW(estEIDManager2 = new EstEIDManager(1));

		printf("MF ");
		ASSERT_NO_THROW(estEIDManager->selectMF());
		printf("EEEE ");
		ASSERT_NO_THROW(estEIDManager->selectDF(0xEEEE));
		printf("0033 ");
		ASSERT_NO_THROW(estEIDManager->selectEF(0x0033));
		printf("SE 3 ");
		ASSERT_NO_THROW(estEIDManager->setSecEnv(3));

		ASSERT_NO_THROW(delete estEIDManager);
		ASSERT_NO_THROW(delete estEIDManager2);
	}
	printf("OK\n");
}

TEST(listAllReadersPKCS11StyleTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	unsigned int tokenCount = 0;

	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(estEIDManager = new EstEIDManager());
		ASSERT_NO_THROW(tokenCount = estEIDManager->getTokenCount(false));
		ASSERT_NO_THROW(delete estEIDManager);
		printf("[----------] Found %i readers \n", tokenCount);

		for (unsigned int i = 0; i < tokenCount; i++)
		{
			std::string readerName = "";
			ASSERT_NO_THROW(estEIDManager = new EstEIDManager(i));
			ASSERT_NO_THROW(readerName = estEIDManager->getReaderName());
			if (estEIDManager->isInReader(i) == true)
			{
				printf("                    Card is in reader %s \n", readerName.c_str());
			}
			else
			{
				printf("                    No card in reader %s \n", readerName.c_str());
			}
			ASSERT_NO_THROW(delete estEIDManager);
		}
	}
}

TEST(reconnectWithT0Test, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	std::string documentID = "";
	std::string documentID2 = "";

	for (int i = 1; i <= testIntensity; i++)
	{
		printf("[----------] Testing reconnection with T0 \n");
		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));

		ASSERT_NO_THROW(documentID = estEIDManager->readDocumentID());
		ASSERT_NE(documentID.size(), 0);
		ASSERT_NO_THROW(estEIDManager->reconnectWithT0());
		ASSERT_NO_THROW(documentID2 = estEIDManager->readDocumentID());
		ASSERT_NE(documentID2.size(), 0);
		ASSERT_STRCASEEQ(documentID.c_str(), documentID2.c_str());
		
		ASSERT_NO_THROW(delete estEIDManager);
	}
}

TEST(reconnectWithT1Test, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	std::string documentID = "";
	std::string documentID2 = "";

	for (int i = 1; i <= testIntensity; i++)
	{
		printf("[----------] Testing reconnection with T1 \n");
		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));

		ASSERT_NO_THROW(documentID = estEIDManager->readDocumentID());
		ASSERT_NE(documentID.size(), 0);
		ASSERT_NO_THROW(estEIDManager->reconnectWithT1());
		ASSERT_NO_THROW(documentID2 = estEIDManager->readDocumentID());
		ASSERT_NE(documentID2.size(), 0);
		ASSERT_STRCASEEQ(documentID.c_str(), documentID2.c_str());

		ASSERT_NO_THROW(delete estEIDManager);
	}
}

TEST(connectMultiplyToOneIndexTest, Positive)
{
	EstEIDManager *estEidManager1 = NULL;
	EstEIDManager *estEidManager2 = NULL;
	EstEIDManager *estEidManager3 = NULL;
	EstEIDManager *estEidManager4 = NULL;
	EstEIDManager *estEidManager5 = NULL;
	std::string documentID1 = "";
	std::string documentID2 = "";
	std::string documentID3 = "";
	std::string documentID4 = "";
	std::string documentID5 = "";

	for (int i = 1; i <= testIntensity; i++)
	{
		printf("[----------] Testing multiply connections to single token \n");
		ASSERT_NO_THROW(estEidManager1 = new EstEIDManager(0));
		ASSERT_NO_THROW(estEidManager2 = new EstEIDManager(0));
		ASSERT_NO_THROW(estEidManager3 = new EstEIDManager(0));
		ASSERT_NO_THROW(estEidManager4 = new EstEIDManager(0));
		ASSERT_NO_THROW(estEidManager5 = new EstEIDManager(0));

		ASSERT_NO_THROW(documentID1 = estEidManager1->readDocumentID());
		ASSERT_NO_THROW(documentID2 = estEidManager2->readDocumentID());
		ASSERT_NO_THROW(documentID3 = estEidManager3->readDocumentID());
		ASSERT_NO_THROW(documentID4 = estEidManager4->readDocumentID());
		ASSERT_NO_THROW(documentID5 = estEidManager5->readDocumentID());

		ASSERT_NE(documentID1.size(), 0);
		ASSERT_NE(documentID2.size(), 0);
		ASSERT_NE(documentID3.size(), 0);
		ASSERT_NE(documentID4.size(), 0);
		ASSERT_NE(documentID5.size(), 0);

		ASSERT_STRCASEEQ(documentID1.c_str(), documentID2.c_str());
		ASSERT_STRCASEEQ(documentID1.c_str(), documentID3.c_str());
		ASSERT_STRCASEEQ(documentID1.c_str(), documentID4.c_str());
		ASSERT_STRCASEEQ(documentID1.c_str(), documentID5.c_str());

		ASSERT_NO_THROW(delete estEidManager1);
		ASSERT_NO_THROW(delete estEidManager2);
		ASSERT_NO_THROW(delete estEidManager3);
		ASSERT_NO_THROW(delete estEidManager4);
		ASSERT_NO_THROW(delete estEidManager5);
	}
}

TEST(firefoxCardPoolingTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	std::string documentID = "";
	std::string documentID2 = "";
	uint count = 0;

	printf("[----------] Testing Firefox pooling \n");
	for (int i = 1; i <= 100; i++)
	{
		ASSERT_NO_THROW(estEIDManager = new EstEIDManager());
		ASSERT_NO_THROW(count = estEIDManager->getTokenCount(true));
		ASSERT_NE(count, 0);
		ASSERT_NO_THROW(delete estEIDManager);

		for (uint z = 0; z < count; z++)
		{
			EstEIDManager *m = NULL;
			ASSERT_NO_THROW(m = new EstEIDManager(z));
			printf("[----------] Reader: %s, %s \n", m->getReaderName().c_str(), m->isInReader(z) ? "PRESENT" : "EMPTY");
			ASSERT_NO_THROW(delete m);
		}
	}
}

TEST(isDigiIDTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	std::string name = "";
	bool isDigiID = false;

	printf("[----------] Checking if card is a DigiID card \n");
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
		ASSERT_NO_THROW(name = estEIDManager->readCardName());
		ASSERT_NE(name.size(), 0);
		ASSERT_NO_THROW(isDigiID = estEIDManager->isDigiID());
		if (isDigiID == true)
			printf("[----------] Card is DIGI-ID %s \n", name.c_str());
		else
			printf("[----------] Card is not DIGI-ID %s\n", name.c_str());
		ASSERT_NO_THROW(delete estEIDManager);

		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
		ASSERT_NO_THROW(name = estEIDManager->readCardName());
		ASSERT_NE(name.size(), 0);
		ASSERT_NO_THROW(isDigiID = estEIDManager->isDigiID());
		if (isDigiID == true)
			printf("[----------] Card is DIGI-ID %s \n", name.c_str());
		else
			printf("[----------] Card is not DIGI-ID %s\n", name.c_str());
		ASSERT_NO_THROW(delete estEIDManager);
	}
}

TEST(getCertificateObjectTest, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	std::string name = "";
	bool isDigiID = false;
	Certificate *authCert0 = NULL;
	Certificate *signCert0 = NULL;
	Certificate *authCert1 = NULL;
	Certificate *signCert1 = NULL;

	printf("[----------] Retrieving auth certificate object\n");
	for (int i = 1; i <= testIntensity; i++)
	{
		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(0));
		ASSERT_NO_THROW(authCert0 = estEIDManager->getCertificateObject(EstEIDManager::KeyType::AUTH));
		ASSERT_NO_THROW(signCert0 = estEIDManager->getCertificateObject(EstEIDManager::KeyType::SIGN));

		struct tm timeinfoNotValidBefore;
		struct tm timeinfoNotValidAfter;
		time_t rawtimeNotValidBefore = authCert0->getNotValidBefore();
		time_t rawTimeNotValidAfter = authCert0->getNotValidAfter();
		localtime_s(&timeinfoNotValidBefore, &rawtimeNotValidBefore);
		localtime_s(&timeinfoNotValidAfter, &rawTimeNotValidAfter);

		printf("[----------] Authentication certificate info\n");
		printf("            [----------] Owner firstname: %s\n", authCert0->getOwnerFirstName().c_str());
		printf("            [----------] Owner Surname: %s\n", authCert0->getOwnerSurName().c_str());
		printf("            [----------] Owner Personal code: %s\n", authCert0->getOwnerPersonalCode().c_str());
		printf("            [----------] Not valid before: %lld [%02i.%02i.%i %02i:%02i]\n", authCert0->getNotValidBefore(), 
																							timeinfoNotValidBefore.tm_mday, 
																							timeinfoNotValidBefore.tm_mon + 1, 
																							(timeinfoNotValidBefore.tm_year) + 1900, 
																							timeinfoNotValidBefore.tm_hour, 
																							timeinfoNotValidBefore.tm_min);
		printf("            [----------] Not valid after: %lld [%02i.%02i.%i %02i:%02i]\n", authCert0->getNotValidAfter(), 
																							timeinfoNotValidAfter.tm_mday,
																							timeinfoNotValidAfter.tm_mon + 1,
																							(timeinfoNotValidAfter.tm_year) + 1900,
																							timeinfoNotValidAfter.tm_hour,
																							timeinfoNotValidAfter.tm_min);
		printf("            [----------] CN: %s\n", authCert0->getCN().c_str());
		printf("            [----------] C: %s\n", authCert0->getC().c_str());
		printf("            [----------] O: %s\n", authCert0->getO().c_str());
		printf("            [----------] OU: %s\n", authCert0->getOU().c_str());
		printf("            [----------] SerialNumber: %s\n", authCert0->getSerialNumber().c_str());
		printf("            [----------] Issuer CN: %s\n", authCert0->getIssuerCN().c_str());

		rawtimeNotValidBefore = signCert0->getNotValidBefore();
		rawTimeNotValidAfter = signCert0->getNotValidAfter();
		localtime_s(&timeinfoNotValidBefore, &rawtimeNotValidBefore);
		localtime_s(&timeinfoNotValidAfter, &rawTimeNotValidAfter);

		printf("[----------] Signature certificate info\n");
		printf("            [----------] Owner firstname: %s\n", signCert0->getOwnerFirstName().c_str());
		printf("            [----------] Owner Surname: %s\n", signCert0->getOwnerSurName().c_str());
		printf("            [----------] Owner Personal code: %s\n", signCert0->getOwnerPersonalCode().c_str());
		printf("            [----------] Not valid before: %lld [%02i.%02i.%i %02i:%02i]\n", signCert0->getNotValidBefore(),
																							timeinfoNotValidBefore.tm_mday,
																							timeinfoNotValidBefore.tm_mon + 1,
																							(timeinfoNotValidBefore.tm_year) + 1900,
																							timeinfoNotValidBefore.tm_hour,
																							timeinfoNotValidBefore.tm_min);
		printf("            [----------] Not valid after: %lld [%02i.%02i.%i %02i:%02i]\n", signCert0->getNotValidAfter(),
																							timeinfoNotValidAfter.tm_mday,
																							timeinfoNotValidAfter.tm_mon + 1,
																							(timeinfoNotValidAfter.tm_year) + 1900,
																							timeinfoNotValidAfter.tm_hour,
																							timeinfoNotValidAfter.tm_min);
		printf("            [----------] CN: %s\n", signCert0->getCN().c_str());
		printf("            [----------] C: %s\n", signCert0->getC().c_str());
		printf("            [----------] O: %s\n", signCert0->getO().c_str());
		printf("            [----------] OU: %s\n", signCert0->getOU().c_str());
		printf("            [----------] SerialNumber: %s\n", signCert0->getSerialNumber().c_str());
		printf("            [----------] Issuer CN: %s\n", signCert0->getIssuerCN().c_str());

		ASSERT_STRNE("", authCert0->getOwnerFirstName().c_str());
		ASSERT_STRNE("", authCert0->getOwnerSurName().c_str());
		ASSERT_STRNE("", authCert0->getOwnerPersonalCode().c_str());
		ASSERT_NE (0, authCert0->getNotValidBefore());
		ASSERT_NE(0, authCert0->getNotValidAfter());
		ASSERT_STRNE("", authCert0->getCN().c_str());
		ASSERT_STRNE("", authCert0->getC().c_str());
		ASSERT_STRNE("", authCert0->getO().c_str());
		ASSERT_STRNE("", authCert0->getOU().c_str());
		ASSERT_STRNE("", authCert0->getSerialNumber().c_str());
		ASSERT_STRNE("", authCert0->getIssuerCN().c_str());

		ASSERT_STRNE("", signCert0->getOwnerFirstName().c_str());
		ASSERT_STRNE("", signCert0->getOwnerSurName().c_str());
		ASSERT_STRNE("", signCert0->getOwnerPersonalCode().c_str());
		ASSERT_NE(0, signCert0->getNotValidBefore());
		ASSERT_NE(0, signCert0->getNotValidAfter());
		ASSERT_STRNE("", signCert0->getCN().c_str());
		ASSERT_STRNE("", signCert0->getC().c_str());
		ASSERT_STRNE("", signCert0->getO().c_str());
		ASSERT_STRNE("", signCert0->getOU().c_str());
		ASSERT_STRNE("", signCert0->getSerialNumber().c_str());
		ASSERT_STRNE("", signCert0->getIssuerCN().c_str());

		ASSERT_NO_THROW(delete estEIDManager);

		ASSERT_NO_THROW(estEIDManager = new EstEIDManager(1));
		ASSERT_NO_THROW(authCert1 = estEIDManager->getCertificateObject(EstEIDManager::KeyType::AUTH));
		ASSERT_NO_THROW(signCert1 = estEIDManager->getCertificateObject(EstEIDManager::KeyType::SIGN));

		rawtimeNotValidBefore = authCert1->getNotValidBefore();
		rawTimeNotValidAfter = authCert1->getNotValidAfter();
		localtime_s(&timeinfoNotValidBefore, &rawtimeNotValidBefore);
		localtime_s(&timeinfoNotValidAfter, &rawTimeNotValidAfter);

		printf("[----------] Authentication certificate info\n");
		printf("            [----------] Owner firstname: %s\n", authCert1->getOwnerFirstName().c_str());
		printf("            [----------] Owner Surname: %s\n", authCert1->getOwnerSurName().c_str());
		printf("            [----------] Owner Personal code: %s\n", authCert1->getOwnerPersonalCode().c_str());
		printf("            [----------] Not valid before: %lld [%02i.%02i.%i %02i:%02i]\n", authCert1->getNotValidBefore(),
																							timeinfoNotValidBefore.tm_mday,
																							timeinfoNotValidBefore.tm_mon + 1,
																							(timeinfoNotValidBefore.tm_year) + 1900,
																							timeinfoNotValidBefore.tm_hour,
																							timeinfoNotValidBefore.tm_min);
		printf("            [----------] Not valid after: %lld [%02i.%02i.%i %02i:%02i]\n", authCert1->getNotValidAfter(),
																							timeinfoNotValidAfter.tm_mday,
																							timeinfoNotValidAfter.tm_mon + 1,
																							(timeinfoNotValidAfter.tm_year) + 1900,
																							timeinfoNotValidAfter.tm_hour,
																							timeinfoNotValidAfter.tm_min);
		printf("            [----------] CN: %s\n", authCert1->getCN().c_str());
		printf("            [----------] C: %s\n", authCert1->getC().c_str());
		printf("            [----------] O: %s\n", authCert1->getO().c_str());
		printf("            [----------] OU: %s\n", authCert1->getOU().c_str());
		printf("            [----------] SerialNumber: %s\n", authCert1->getSerialNumber().c_str());
		printf("            [----------] Issuer CN: %s\n", authCert1->getIssuerCN().c_str());

		rawtimeNotValidBefore = signCert1->getNotValidBefore();
		rawTimeNotValidAfter = signCert1->getNotValidAfter();
		localtime_s(&timeinfoNotValidBefore, &rawtimeNotValidBefore);
		localtime_s(&timeinfoNotValidAfter, &rawTimeNotValidAfter);

		printf("[----------] Signature certificate info\n");
		printf("            [----------] Owner firstname: %s\n", signCert1->getOwnerFirstName().c_str());
		printf("            [----------] Owner Surname: %s\n", signCert1->getOwnerSurName().c_str());
		printf("            [----------] Owner Personal code: %s\n", signCert1->getOwnerPersonalCode().c_str());
		printf("            [----------] Not valid before: %lld [%02i.%02i.%i %02i:%02i]\n", signCert1->getNotValidBefore(),
																							timeinfoNotValidBefore.tm_mday,
																							timeinfoNotValidBefore.tm_mon + 1,
																							(timeinfoNotValidBefore.tm_year) + 1900,
																							timeinfoNotValidBefore.tm_hour,
																							timeinfoNotValidBefore.tm_min);
		printf("            [----------] Not valid after: %lld [%02i.%02i.%i %02i:%02i]\n", signCert1->getNotValidAfter(),
																							timeinfoNotValidAfter.tm_mday,
																							timeinfoNotValidAfter.tm_mon + 1,
																							(timeinfoNotValidAfter.tm_year) + 1900,
																							timeinfoNotValidAfter.tm_hour,
																							timeinfoNotValidAfter.tm_min);
		printf("            [----------] CN: %s\n", signCert1->getCN().c_str());
		printf("            [----------] C: %s\n", signCert1->getC().c_str());
		printf("            [----------] O: %s\n", signCert1->getO().c_str());
		printf("            [----------] OU: %s\n", signCert1->getOU().c_str());
		printf("            [----------] SerialNumber: %s\n", signCert1->getSerialNumber().c_str());
		printf("            [----------] Issuer CN: %s\n", signCert1->getIssuerCN().c_str());

		

		ASSERT_NO_THROW(delete estEIDManager);
	}
}

TEST(connect, Positive)
{
	EstEIDManager *estEIDManager = NULL;
	ASSERT_NO_THROW(estEIDManager = new EstEIDManager());

	for (int i = 1; i <= testIntensity; i++)
	{
		estEIDManager->connect(selectedCardReader);
		printf("            [----------] CN: %s\n", estEIDManager->readCardName().c_str());
		estEIDManager->disconnect();

		estEIDManager->connect(selectedCardReader + 1);
		printf("            [----------] CN: %s\n", estEIDManager->readCardName().c_str());
		estEIDManager->disconnect();
	}

	ASSERT_NO_THROW(delete estEIDManager);
}