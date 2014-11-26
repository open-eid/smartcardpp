#include "CardTests.h"
#include <time.h>

using namespace std;

CardTests::CardTests(void)
{
	testIntensity = 3;
	constructorsCount = 10;
	selectedCardReader = 999;
	selectedSecondCardReader = 999;
	PIN1 = PinString("0090");
	PIN2 = PinString("01497");
	PUK = PinString("17258403");

	tempPIN1 = PinString("1111");
	tempPIN2 = PinString("22222");
	tempPUK = PinString("12345678");
}

CardTests::~CardTests(void)
{
}

void CardTests::setTestIntesity(int intensity)
{
	this->testIntensity = intensity;
}

int CardTests::getTestIntensity()
{
	return this->testIntensity;
}

bool CardTests::init()
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager();
		tokens = estEIDManager->getTokenList();

		unsigned int i = 0;
		for(i = 0; i < tokens.size(); i++)
		{
			if(tokens[i].second.compare("") == 0)
				continue;
			else
			{
				printf("         Selecting reader: %i %s \r\n", i, tokens[i].first.c_str());
				selectedCardReader = i;
				i++;
				break;
			}
		}

		for(; i < tokens.size(); i++)
		{
			if(tokens[i].second.compare("") == 0)
				continue;
			else
			{
				printf("         Selecting reader: %i %s\r\n", i, tokens[i].first.c_str());
				selectedSecondCardReader = i;
				break;
			}
		}

		if(estEIDManager) delete estEIDManager;
		if(selectedCardReader == 999)
		{
			printf("FAILED TO SELECT FIRST READER\r\n");
			return false;
		}
		if(selectedSecondCardReader == 999)
		{
			printf("FAILED TO SELECT SECOND READER\r\n");
			return false;
		}
		
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	return true;
}

bool CardTests::constructorsTest(void)
{
	try
	{
		printf("\r\n 1\r\n");
		for(int i = 1; i <= testIntensity; i++)
		{
			EstEIDManager *cardNULL = new EstEIDManager();
			delete cardNULL;
			for(int i = 0; i < constructorsCount; i++)
			{
				if(i%100 == 0)
				  printf("\r\n");
				printf(".");
				
				EstEIDManager *card1 = new EstEIDManager();
				delete card1;
			}
			printf("\r\n");
		}
		printf("\r\n          OK\r\n");
		printf(" 2\r\n");
		for(int i = 1; i <= testIntensity; i++)
		{
			EstEIDManager *cardNULL = new EstEIDManager(selectedCardReader);
			EstEIDManager *cardNULL2 = new EstEIDManager(selectedSecondCardReader);
			printf("         Reader name: %s \r\n", cardNULL->getReaderName().c_str());
			printf("         Document number: %s \r\n", cardNULL->readDocumentID().c_str());
			printf("\r\n");
			printf("         Reader name: %s \r\n", cardNULL2->getReaderName().c_str());
			printf("         Document number: %s \r\n", cardNULL2->readDocumentID().c_str());
			delete cardNULL;
			delete cardNULL2;

			for(int i = 0; i < constructorsCount; i++)
			{
				if(i%100 == 0)
				  printf("\r\n");
				printf(".");
				EstEIDManager *card1 = new EstEIDManager(selectedCardReader);
				EstEIDManager *card2 = new EstEIDManager(selectedSecondCardReader);
				delete card1;
				delete card2;
			}
			printf("\r\n");
		}
		printf("\r\n          OK\r\n");
		printf(" 3\r\n");
		for(int i = 1; i <= testIntensity; i++)
		{
			EstEIDManager *cardNULL = new EstEIDManager(tokens[selectedCardReader].first);
			EstEIDManager *cardNULL2 = new EstEIDManager(tokens[selectedSecondCardReader].first);
			printf("         Reader name: %s \r\n", cardNULL->getReaderName().c_str());
			printf("         Document number: %s \r\n", cardNULL->readDocumentID().c_str());
			printf("\r\n");
			printf("         Reader name: %s \r\n", cardNULL2->getReaderName().c_str());
			printf("         Document number: %s \r\n", cardNULL2->readDocumentID().c_str());
			delete cardNULL;
			delete cardNULL2;

			for(int i = 0; i < constructorsCount; i++)
			{
				if(i%100 == 0)
				  printf("\r\n");
				printf(".");
				EstEIDManager *card1 = new EstEIDManager(tokens[selectedCardReader].first);
				EstEIDManager *card2 = new EstEIDManager(tokens[selectedSecondCardReader].first);
				delete card1;
				delete card2;
			}
			printf("\r\n");
		}
		printf("\r\n          OK\r\n");
		printf(" 4");
		
#ifdef WIN32
        for(int i = 1; i <= testIntensity; i++)
		{
			for(int i = 0; i < constructorsCount; i++)
			{
				SCARDCONTEXT scardCtx;
				SCARDHANDLE scard;
				long lReturn = SCARD_S_SUCCESS;
				LPTSTR readers = NULL;
				LPTSTR reader = NULL;
				DWORD readersLen = SCARD_AUTOALLOCATE;
				DWORD dwAP;

				lReturn = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &scardCtx);
				if(lReturn != SCARD_S_SUCCESS)
				{
					printf("SCardEstablishContext failed 0x%08lX\r\n", lReturn);
					return false;
				}

				lReturn = SCardListReaders(scardCtx, NULL, (LPTSTR)&readers, &readersLen);
				switch(lReturn)
				{
				case SCARD_S_SUCCESS:
					reader = readers;
					break;
				case SCARD_E_NO_READERS_AVAILABLE: printf("SCARD_E_NO_READERS_AVAILABLE\r\n"); return false; break;
				default: printf("SCardListReaders failed 0x%08lX\r\n", lReturn); return false; break;
				}
				
				lReturn = SCardConnect(scardCtx, tokens[selectedCardReader].first.c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &scard, &dwAP);
				if(lReturn != SCARD_S_SUCCESS)
				{
					printf("SCardConnect failed 0x%08lX\r\n", lReturn);
					SCardReleaseContext(scardCtx);
					return false;
				}

				if(i%100 == 0)
				  printf("\r\n");
				printf(".");

				EstEIDManager *estEidCard = new EstEIDManager(scardCtx, scard);
				std::string documentid = estEidCard->readDocumentID();
				delete estEidCard;

				if(documentid.length() < 8 && documentid.length() > 9)
				{
					printf("Document number length is invalid: %li\r\n", documentid.length());
					SCardDisconnect(scard, SCARD_RESET_CARD);
					SCardReleaseContext(scardCtx);
					return false;
				}

				lReturn = SCardDisconnect(scard, SCARD_RESET_CARD);
				if(lReturn != SCARD_S_SUCCESS)
				{
					printf("SCardDisconnect failed 0x%08lX\r\n", lReturn);
					SCardReleaseContext(scardCtx);
					return false;
				}

				lReturn = SCardFreeMemory(scardCtx, readers);
				if(lReturn != SCARD_S_SUCCESS)
				{
					printf("SCardFreeMemory failed 0x%08lX\r\n", lReturn);
					return false;
				}

				lReturn = SCardReleaseContext(scardCtx);
				if(lReturn != SCARD_S_SUCCESS)
				{
					printf("SCardReleaseContext failed 0x%08lX\r\n", lReturn);
					return false;
				}
			}
		}
#endif
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		return false;
	}
	return true;
}

bool CardTests::isInReaderTest(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n           %i, %s ", i, estEIDManager->getReaderName().c_str());
			estEIDManager->isInReader(selectedCardReader);
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::getKeySize(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          %i,", i);
			unsigned int keySize = estEIDManager->getKeySize();
			printf("%i,", keySize);
			if(keySize == 1024 || keySize == 2048)
				;
			else
			{
				printf("\r\n Invalid key length: %i ", keySize);
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
            
            printf("\r\n          %i,", i);
            keySize = estEIDManager2->getKeySize();
            printf("%i,", keySize);
            if(keySize == 1024 || keySize == 2048)
                ;
            else
            {
                printf("\r\n Invalid key length: %i ", keySize);
                if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
                return false;
            }
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::readCardIDTest(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          %i,", i);
			string cardID = estEIDManager->readCardID();
			printf("%s", cardID.c_str());
			if(cardID.length() == 11)
				;
			else
			{
				printf("\r\n Invalid card id length: %li [%s] ", cardID.length(), cardID.c_str());
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
            
            printf("\r\n          %i,", i);
            string cardID2 = estEIDManager2->readCardID();
            printf("%s", cardID2.c_str());
            if(cardID2.length() == 11)
                ;
            else
            {
                printf("\r\n Invalid card id length: %li [%s] ", cardID2.length(), cardID2.c_str());
                if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
                return false;
            }
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
        if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
        if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::readDocumentIDTest(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          %i,", i);
			string documentID = estEIDManager->readDocumentID();
			printf("%s", documentID.c_str());
			if(documentID.length() == 8 || documentID.length() == 9)
				;
			else
			{
				printf("\r\n Invalid document id length: %li [%s] ", documentID.length(), documentID.c_str());
                if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
            
            printf("\r\n          %i,", i);
            string documentID2 = estEIDManager2->readDocumentID();
            printf("%s", documentID2.c_str());
            if(documentID2.length() == 8 || documentID2.length() == 9)
                ;
            else
            {
                printf("\r\n Invalid document id length: %li [%s] ", documentID2.length(), documentID2.c_str());
                if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
                return false;
            }
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::readCardNameTest(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          %i,", i);
			string cardName = estEIDManager->readCardName(false);
			printf("%s", cardName.c_str());
			if(cardName.length() == 0)
            {
                printf("\r\n Invalid card anme length: %li [%s] ", cardName.length(), cardName.c_str());
                if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
                return false;
            }
            
            printf("\r\n          %i,", i);
            string cardName2 = estEIDManager2->readCardName(false);
            printf("%s", cardName2.c_str());
            if(cardName2.length() == 0)
            {
                printf("\r\n Invalid card anme length: %li [%s] ", cardName2.length(), cardName2.c_str());
                if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
                return false;
            }
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::readCardNameFirstNameFirstTest(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          %i,", i);
			string cardName = estEIDManager->readCardName(true);
			printf("%s", cardName.c_str());
            if(cardName.length() == 0)
            {
                printf("\r\n Invalid card anme length: %li [%s] ", cardName.length(), cardName.c_str());
                if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
                return false;
            }
            
            printf("\r\n          %i,", i);
            string cardName2 = estEIDManager2->readCardName(true);
            printf("%s", cardName2.c_str());
            if(cardName2.length() == 0)
            {
                printf("\r\n Invalid card anme length: %li [%s] ", cardName2.length(), cardName2.c_str());
                if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
                return false;
            }
        }
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::readPersonalDataTest(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			vector<string> data, data2;
			printf("\r\n          %i,", i);
			bool lReturn = estEIDManager->readPersonalData(data);
            bool lReturn2 = estEIDManager2->readPersonalData(data2);
			if(lReturn == false || lReturn2 == false)
			{
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}

			string surname = data[1];
            string surname2 = data2[1];
			if(surname.length() == 0 || surname2.length() == 0)
			{
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
			string firstname = data[2];
            string firstname2 = data2[2];
			if(firstname.length() == 0 || firstname2.length() == 0)
			{
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
            
			string middlename = data[3];
            string middlename2 = data2[3];
            
			string sex = data[4];
            string sex2 = data2[4];
			if(sex.length() == 0 || sex2.length() == 0)
			{
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
			string citizen = data[5];
            string citizen2 = data2[5];
			if(citizen.length() == 0 || citizen2.length() == 0)
			{
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
			string birthdate = data[6];
            string birthdate2 = data2[6];
			if(birthdate.length() == 0 || birthdate2.length() == 0)
			{
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
			string id = data[7];
            string id2 = data2[7];
			if(id.length() == 0 || id2.length() == 0)
			{
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
			string documentid = data[8];
            string documentid2 = data2[8];
			if(documentid.length() == 0 || documentid2.length() == 0)
			{
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
			string expiry = data[9];
            string expiry2 = data2[9];
			if(expiry.length() == 0 || expiry2.length() == 0)
			{
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}

			printf("Owner info: %s %s %s %s %s %s", firstname.c_str(), middlename.c_str(), surname.c_str(), sex.c_str(), citizen.c_str(), birthdate.c_str());
			printf("\r\n          Card info: %s %s %s\r\n", id .c_str(), documentid.c_str(), expiry.c_str());
            
            printf("Owner info: %s %s %s %s %s %s", firstname2.c_str(), middlename2.c_str(), surname2.c_str(), sex2.c_str(), citizen2.c_str(), birthdate2.c_str());
            printf("\r\n          Card info: %s %s %s\r\n", id2.c_str(), documentid2.c_str(), expiry2.c_str());
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::getKeyUsageCountersTest(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          %i,", i);
			dword authCounter, signCounter, authCounter2, signCounter2 = 0;

			estEIDManager->getKeyUsageCounters(authCounter, signCounter);
			printf(" Key usage counters: %ld %ld", authCounter, signCounter);
            
            estEIDManager2->getKeyUsageCounters(authCounter2, signCounter2);
            printf(" Key usage counters: %ld %ld", authCounter2, signCounter2);
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::getAuthCertTest(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          %i,", i);
			ByteVec cert = estEIDManager->getAuthCert();
			
			if(cert.size() < 1000)
			{
				printf(" FAILED. Authentication certificate is too short: %li bytes", cert.size());
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
			else
			{
				printf(" Success. Authentication certificate size: %li bytes", cert.size());
			}
            
            printf("\r\n          %i,", i);
            ByteVec cert2 = estEIDManager2->getAuthCert();
            
            if(cert2.size() < 1000)
            {
                printf(" FAILED. Authentication certificate is too short: %li bytes", cert2.size());
                if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
                return false;
            }
            else
            {
                printf(" Success. Authentication certificate size: %li bytes", cert2.size());
            }
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::getSignCertTest(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          %i,", i);
			ByteVec cert = estEIDManager->getSignCert();
			if(cert.size() < 1000)
			{
				printf(" FAILED. Signature certificate is too short: %li bytes", cert.size());
				if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
				return false;
			}
			else
			{
				printf(" Success. Signature certificate size: %li bytes", cert.size());
			}
            
            printf("\r\n          %i,", i);
            ByteVec cert2 = estEIDManager2->getSignCert();
            if(cert2.size() < 1000)
            {
                printf(" FAILED. Signature certificate is too short: %li bytes", cert2.size());
                if(estEIDManager) delete estEIDManager;
                if(estEIDManager2) delete estEIDManager2;
                return false;
            }
            else
            {
                printf(" Success. Signature certificate size: %li bytes", cert2.size());
            }
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSSL_HashOnly_WithAuthCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

		byte hashByte[20] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45};
		ByteVec hash(MAKEVECTOR(hashByte));
		estEIDManager->startTransaction();
        estEIDManager2->startTransaction();
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating SSL with auth key. Hash only mode... %i,", i);
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN1 ");
				estEIDManager->enterPin(EstEIDManager::PIN_AUTH, "", false);
			}
			else
				estEIDManager->enterPin(EstEIDManager::PIN_AUTH, PIN1, false);
            
			estEIDManager->sign_pkcs11(hash, EstEIDManager::SSL, EstEIDManager::AUTH);

			printf(" Success.");
            
            printf("\r\n          Calculating SSL with auth key. Hash only mode... %i,", i);
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN1 ");
                estEIDManager2->enterPin(EstEIDManager::PIN_AUTH, "", false);
            }
            else
                estEIDManager2->enterPin(EstEIDManager::PIN_AUTH, PIN1, false);
            
            estEIDManager2->sign_pkcs11(hash, EstEIDManager::SSL, EstEIDManager::AUTH);
            
            printf(" Success.");
		}
		estEIDManager->endTransaction();
        estEIDManager2->endTransaction();
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		estEIDManager->endTransaction();
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		estEIDManager->endTransaction();
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSSL_HashOnly_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		byte ret = 0x03;
		byte hashByte[20] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45};
		ByteVec hash(MAKEVECTOR(hashByte));

        estEIDManager->startTransaction();
        estEIDManager2->startTransaction();
        for(int i = 1; i <= testIntensity; i++)
        {
            printf("\r\n          Calculating SSL with sign key. Hash only mode... %i,", i);
            if(estEIDManager->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager->validateSignPin("", ret);
            }
            else
                estEIDManager->validateSignPin(PIN2, ret);

            estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::SIGN);
            printf(" Success.");
            
            printf("\r\n          Calculating SSL with sign key. Hash only mode... %i,", i);
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->validateSignPin("", ret);
            }
            else
                estEIDManager2->validateSignPin(PIN2, ret);
            
            estEIDManager2->sign(hash, EstEIDManager::SSL, EstEIDManager::SIGN);
            printf(" Success.");
        }
        estEIDManager->endTransaction();
        estEIDManager2->endTransaction();
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("SW1: 0x%X SW2: 0x%X ", e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSSL_WithPIN_WithAuthCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		byte hashByte[20] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45};
		ByteVec hash(MAKEVECTOR(hashByte));

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating SSL with auth key. Hash & PIN mode... %i,", i);

			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN1 ");
				ByteVec ssl = estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH, "");
			}
			else
				ByteVec ssl = estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH, PIN1);
			printf(" Success.");
            
            printf("\r\n          Calculating SSL with auth key. Hash & PIN mode... %i,", i);
            
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN1 ");
                ByteVec ssl = estEIDManager2->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH, "");
            }
            else
                ByteVec ssl = estEIDManager2->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH, PIN1);
            printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignSHA1_HashOnly_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

		byte hashByte[20] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45};
		ByteVec hash(MAKEVECTOR(hashByte));

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating signature for SHA1 with sign key. Hash only mode... %i,", i);
			estEIDManager->startTransaction();
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, "");
			}
			else
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2);
			estEIDManager->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN);
			estEIDManager->endTransaction();
			printf(" Success.");
            
            printf("\r\n          Calculating signature for SHA1 with sign key. Hash only mode... %i,", i);
            estEIDManager2->startTransaction();
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->enterPin(EstEIDManager::PIN_SIGN, "");
            }
            else
                estEIDManager2->enterPin(EstEIDManager::PIN_SIGN, PIN2);
            estEIDManager2->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN);
            estEIDManager2->endTransaction();
            printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignSHA1_WithPIN_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
    
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

		byte hashByte[20] = {0x6c, 0x39, 0x71, 0xdb, 0xf5, 0x50, 0xa6, 0x72, 0x92, 0xaa, 0xf5, 0x19, 0x34, 0x6e, 0x84, 0x34, 0xc1, 0x11, 0x7f, 0x9b};
		ByteVec hash(MAKEVECTOR(hashByte));

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating signature for SHA1 with sign key. Hash & PIN mode... %i,", i);
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN, "");
			}
			else
				estEIDManager->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN, PIN2);
			printf(" Success.");
            
            printf("\r\n          Calculating signature for SHA1 with sign key. Hash & PIN mode... %i,", i);
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN, "");
            }
            else
                estEIDManager2->sign(hash, EstEIDManager::SHA1, EstEIDManager::SIGN, PIN2);
            printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
        if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
        if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
    if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignSHA224_HashOnly_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
    
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

		byte hashByte[28] = {0x22, 0x12, 0x69, 0x96, 0x1d, 0x5a, 0x1f, 0xee, 0x3f, 0x9b, 
							 0x98, 0xf4, 0x2c, 0x58, 0x72, 0xa0, 0xa9, 0x06, 0x02, 0x98, 
							 0xab, 0x55, 0xa2, 0xf9, 0x78, 0xed, 0x20, 0xe0};
		ByteVec hash(MAKEVECTOR(hashByte));

		if(estEIDManager->getCardVersion() == EstEIDManager::VER_1_0 || estEIDManager->getCardVersion() == EstEIDManager::VER_1_0_2007)
		{
			printf(" Won`t test.");
			return true;
		}

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating signature for SHA224 with sign key. Hash only mode... %i,", i);
			estEIDManager->startTransaction();
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, "");
			}
			else
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2);

			estEIDManager->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN);
			estEIDManager->endTransaction();
            
            estEIDManager2->startTransaction();
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->enterPin(EstEIDManager::PIN_SIGN, "");
            }
            else
                estEIDManager2->enterPin(EstEIDManager::PIN_SIGN, PIN2);
            
            estEIDManager2->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN);
            estEIDManager2->endTransaction();
			printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignSHA224_WithPIN_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
    
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

		byte hashByte[28] = {0x22, 0x12, 0x69, 0x96, 0x1d, 0x5a, 0x1f, 0xee, 0x3f, 0x9b, 
							 0x98, 0xf4, 0x2c, 0x58, 0x72, 0xa0, 0xa9, 0x06, 0x02, 0x98, 
							 0xab, 0x55, 0xa2, 0xf9, 0x78, 0xed, 0x20, 0xe0};
		ByteVec hash(MAKEVECTOR(hashByte));

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating signature for SHA224 hash with sign key. Hash & PIN mode... %i,", i);
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN, "");
			}
			else
				estEIDManager->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN, PIN2);
			printf(" Success.");
            
            printf("\r\n          Calculating signature for SHA224 hash with sign key. Hash & PIN mode... %i,", i);
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN, "");
            }
            else
                estEIDManager2->sign(hash, EstEIDManager::SHA224, EstEIDManager::SIGN, PIN2);
            printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignSHA256_HashOnly_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
        
        if(estEIDManager->getCardVersion() == EstEIDManager::VER_1_0 || estEIDManager->getCardVersion() == EstEIDManager::VER_1_0_2007)
        {
            printf(" Won`t test.");
            return true;
        }

		byte hashByte[32] = {0x0b, 0x67, 0xfe, 0x78, 0x15, 0x5d, 0x63, 0x22, 0xce, 0xb8, 
							 0x28, 0x5f, 0xe1, 0x1d, 0x93, 0x45, 0xea, 0xce, 0x9f, 0x8f, 
							 0x6a, 0xf8, 0x1f, 0xf0, 0x17, 0x87, 0x99, 0x1c, 0x54, 0x3f, 
							 0x94, 0xf8};
		ByteVec hash(MAKEVECTOR(hashByte));

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating signature for SHA256 with sign key. Hash only mode... %i,", i);
			estEIDManager->startTransaction();
            estEIDManager2->startTransaction();
            
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, "");
			}
			else
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2);

			estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN);
            
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->enterPin(EstEIDManager::PIN_SIGN, "");
            }
            else
                estEIDManager2->enterPin(EstEIDManager::PIN_SIGN, PIN2);
            
            estEIDManager2->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN);
            
            estEIDManager->endTransaction();
			estEIDManager2->endTransaction();
			printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignSHA256_WithPIN_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
        
        if(estEIDManager->getCardVersion() == EstEIDManager::VER_1_0 || estEIDManager->getCardVersion() == EstEIDManager::VER_1_0_2007)
        {
            printf(" Won`t test.");
            return true;
        }

		byte hashByte[32] = {0x0b, 0x67, 0xfe, 0x78, 0x15, 0x5d, 0x63, 0x22, 0xce, 0xb8, 
							 0x28, 0x5f, 0xe1, 0x1d, 0x93, 0x45, 0xea, 0xce, 0x9f, 0x8f, 
							 0x6a, 0xf8, 0x1f, 0xf0, 0x17, 0x87, 0x99, 0x1c, 0x54, 0x3f, 
							 0x94, 0xf8};
		ByteVec hash(MAKEVECTOR(hashByte));

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating signature for SHA256 hash with sign key. Hash & PIN mode... %i,", i);
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, "");
			}
			else
				estEIDManager->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, PIN2);

			printf(" Success.");
            
            printf("\r\n          Calculating signature for SHA256 hash with sign key. Hash & PIN mode... %i,", i);
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, "");
            }
            else
                estEIDManager2->sign(hash, EstEIDManager::SHA256, EstEIDManager::SIGN, PIN2);
            
            printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignSHA384_HashOnly_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

		byte hashByte[48] = {0xa3, 0xb7, 0xb4, 0xe4, 0xa7, 0xbf, 0xfa, 0x39, 0xc2, 0xa7, 
							 0x6c, 0xf8, 0x76, 0xe6, 0x42, 0xee, 0x57, 0x99, 0x12, 0x67, 
							 0xe5, 0xdd, 0xdd, 0xc1, 0xd8, 0xa9, 0x45, 0x04, 0xb9, 0x5b, 
							 0xbd, 0xfe, 0xfe, 0xaf, 0x87, 0xa6, 0x32, 0xa1, 0x7b, 0xc0, 
							 0xd5, 0x70, 0x7a, 0x57, 0xda, 0xc7, 0x42, 0x60};
		ByteVec hash(MAKEVECTOR(hashByte));

		if(estEIDManager->getCardVersion() == EstEIDManager::VER_1_0 || estEIDManager->getCardVersion() == EstEIDManager::VER_1_0_2007)
		{
			printf(" Won`t test.");
			return true;
		}

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating signature for SHA384 with sign key. Hash only mode... %i,", i);
			estEIDManager->startTransaction();
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, "");
			}
			else
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2);

			estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN);
			estEIDManager->endTransaction();
			printf(" Success.");
            
            printf("\r\n          Calculating signature for SHA384 with sign key. Hash only mode... %i,", i);
            estEIDManager2->startTransaction();
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->enterPin(EstEIDManager::PIN_SIGN, "");
            }
            else
                estEIDManager2->enterPin(EstEIDManager::PIN_SIGN, PIN2);
            
            estEIDManager2->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN);
            estEIDManager2->endTransaction();
            printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignSHA384_WithPIN_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
        
        if(estEIDManager->getCardVersion() == EstEIDManager::VER_1_0 || estEIDManager->getCardVersion() == EstEIDManager::VER_1_0_2007)
        {
            printf(" Won`t test.");
            return true;
        }

		byte hashByte[48] = {0xa3, 0xb7, 0xb4, 0xe4, 0xa7, 0xbf, 0xfa, 0x39, 0xc2, 0xa7, 
							 0x6c, 0xf8, 0x76, 0xe6, 0x42, 0xee, 0x57, 0x99, 0x12, 0x67, 
							 0xe5, 0xdd, 0xdd, 0xc1, 0xd8, 0xa9, 0x45, 0x04, 0xb9, 0x5b, 
							 0xbd, 0xfe, 0xfe, 0xaf, 0x87, 0xa6, 0x32, 0xa1, 0x7b, 0xc0, 
							 0xd5, 0x70, 0x7a, 0x57, 0xda, 0xc7, 0x42, 0x60};
		ByteVec hash(MAKEVECTOR(hashByte));

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating signature for SHA384 hash with sign key. Hash & PIN mode... %i,", i);
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, "");
			}
			else
				estEIDManager->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, PIN2);
			printf(" Success.");
            
            printf("\r\n          Calculating signature for SHA384 hash with sign key. Hash & PIN mode... %i,", i);
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, "");
            }
            else
                estEIDManager2->sign(hash, EstEIDManager::SHA384, EstEIDManager::SIGN, PIN2);
            printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignSHA512_HashOnly_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
    
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

		byte hashByte[48] = {0xa3, 0xb7, 0xb4, 0xe4, 0xa7, 0xbf, 0xfa, 0x39, 0xc2, 0xa7, 
							 0x6c, 0xf8, 0x76, 0xe6, 0x42, 0xee, 0x57, 0x99, 0x12, 0x67, 
							 0xe5, 0xdd, 0xdd, 0xc1, 0xd8, 0xa9, 0x45, 0x04, 0xb9, 0x5b, 
							 0xbd, 0xfe, 0xfe, 0xaf, 0x87, 0xa6, 0x32, 0xa1, 0x7b, 0xc0, 
							 0xd5, 0x70, 0x7a, 0x57, 0xda, 0xc7, 0x42, 0x60};
		ByteVec hash(MAKEVECTOR(hashByte));

		if(estEIDManager->getCardVersion() == EstEIDManager::VER_1_0 || estEIDManager->getCardVersion() == EstEIDManager::VER_1_0_2007)
		{
			printf(" Won`t test.");
			return true;
		}

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating signature for SHA512 with sign key. Hash only mode... %i,", i);
			estEIDManager->startTransaction();
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, "");
			}
			else
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2);

			estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN);
			estEIDManager->endTransaction();
			printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignSHA512_WithPIN_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
    
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

		byte hashByte[48] = {0xa3, 0xb7, 0xb4, 0xe4, 0xa7, 0xbf, 0xfa, 0x39, 0xc2, 0xa7, 
							 0x6c, 0xf8, 0x76, 0xe6, 0x42, 0xee, 0x57, 0x99, 0x12, 0x67, 
							 0xe5, 0xdd, 0xdd, 0xc1, 0xd8, 0xa9, 0x45, 0x04, 0xb9, 0x5b, 
							 0xbd, 0xfe, 0xfe, 0xaf, 0x87, 0xa6, 0x32, 0xa1, 0x7b, 0xc0, 
							 0xd5, 0x70, 0x7a, 0x57, 0xda, 0xc7, 0x42, 0x60};
		ByteVec hash(MAKEVECTOR(hashByte));

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating signature for SHA512 hash with sign key. Hash & PIN mode... %i,", i);
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, "");
			}
			else
				estEIDManager->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, PIN2);
			printf(" Success.");
            
            printf("\r\n          Calculating signature for SHA512 hash with sign key. Hash & PIN mode... %i,", i);
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, "");
            }
            else
                estEIDManager2->sign(hash, EstEIDManager::SHA512, EstEIDManager::SIGN, PIN2);
            printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignMD5_HashOnly_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

		byte hashByte[20] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45};
		ByteVec hash(MAKEVECTOR(hashByte));
		if(estEIDManager->getCardVersion() == EstEIDManager::VER_1_0 || estEIDManager->getCardVersion() == EstEIDManager::VER_1_0_2007)
		{
			printf(" Won`t test.");
			return true;
		}

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating MD5 with sign key. Hash only mode... %i,", i);
			estEIDManager->startTransaction();
            estEIDManager2->startTransaction();
            
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, "");
			}
			else
				estEIDManager->enterPin(EstEIDManager::PIN_SIGN, PIN2);

			estEIDManager->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN);
            
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->enterPin(EstEIDManager::PIN_SIGN, "");
            }
            else
                estEIDManager2->enterPin(EstEIDManager::PIN_SIGN, PIN2);
            
            estEIDManager2->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN);
			
            estEIDManager->endTransaction();
            estEIDManager2->endTransaction();
			printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::calcSignMD5_WithPIN_WithSignCert(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
    
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

		byte hashByte[20] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45};
		ByteVec hash(MAKEVECTOR(hashByte));

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Calculating MD5 with sign key. Hash & PIN mode... %i,", i);
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				estEIDManager->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN, "");
			}
			else
				estEIDManager->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN, PIN2);
			printf(" Success.");
            
            printf("\r\n          Calculating MD5 with sign key. Hash & PIN mode... %i,", i);
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                estEIDManager2->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN, "");
            }
            else
                estEIDManager2->sign(hash, EstEIDManager::MD5, EstEIDManager::SIGN, PIN2);
            printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::validateAuthPIN(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
    
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		byte ret = 0x03;

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Validating AUTH PIN... %i,", i);
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN1 ");
				if(!estEIDManager->validateAuthPin("", ret))
                {
                    printf("\r\n          PIN verification failed");
                    if(estEIDManager) delete estEIDManager;
                    if(estEIDManager2) delete estEIDManager2;
                    return false;
                }
			}
			else
			{
				if(!estEIDManager->validateAuthPin(PIN1, ret))
                {
                    printf("\r\n          PIN verification failed");
                    if(estEIDManager) delete estEIDManager;
                    if(estEIDManager2) delete estEIDManager2;
                    return false;
                }
			}
			printf(" Success.");
            
            printf("\r\n          Validating AUTH PIN... %i,", i);
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN1 ");
                if(!estEIDManager2->validateAuthPin("", ret))
                {
                    printf("\r\n          PIN verification failed");
                    if(estEIDManager) delete estEIDManager;
                    if(estEIDManager2) delete estEIDManager2;
                    return false;
                }
            }
            else
            {
                if(!estEIDManager2->validateAuthPin(PIN1, ret))
                {
                    printf("\r\n          PIN verification failed");
                    if(estEIDManager) delete estEIDManager;
                    if(estEIDManager2) delete estEIDManager2;
                    return false;
                }
            }
            printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
        if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
        if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
        if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
    if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::validateSignPIN(void)
{
	EstEIDManager *estEIDManager = NULL;
    EstEIDManager *estEIDManager2 = NULL;
    
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
		byte ret = 0x03;

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Validating SIGN PIN... %i,", i);
			if(estEIDManager->isSecureConnection())
			{
				printf(" PIN2 ");
				if(!estEIDManager->validateSignPin("", ret))
                {
                    printf("\r\n          PIN verification failed");
                    if(estEIDManager) delete estEIDManager;
                    if(estEIDManager2) delete estEIDManager2;
                    return false;
                }
			}
			else
			{
				if(!estEIDManager->validateSignPin(PIN2, ret))
                {
                    printf("\r\n          PIN verification failed");
                    if(estEIDManager) delete estEIDManager;
                    if(estEIDManager2) delete estEIDManager2;
                    return false;
                }
			}
			printf(" Success.");
            
            printf("\r\n          Validating SIGN PIN... %i,", i);
            if(estEIDManager2->isSecureConnection())
            {
                printf(" PIN2 ");
                if(!estEIDManager2->validateSignPin("", ret))
                {
                    printf("\r\n          PIN verification failed");
                    if(estEIDManager) delete estEIDManager;
                    if(estEIDManager2) delete estEIDManager2;
                    return false;
                }
            }
            else
            {
                if(!estEIDManager2->validateSignPin(PIN2, ret))
                {
                    printf("\r\n          PIN verification failed");
                    if(estEIDManager) delete estEIDManager;
                    if(estEIDManager2) delete estEIDManager2;
                    return false;
                }
            }
            printf(" Success.");
		}
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
        if(estEIDManager2) delete estEIDManager2;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
    if(estEIDManager2) delete estEIDManager2;
	return true;
}

bool CardTests::validatePUK(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
		byte ret = 0x03;

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Validating PUK... %i,", i);
			if(estEIDManager->isSecureConnection())
			{
				printf(" PUK ");
				estEIDManager->validatePuk(PUK, ret);
			}
			else
			{
				estEIDManager->validatePuk(PUK, ret);
			}
			printf(" Success.");
		}
		return true;
	}
	catch(AuthError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	return false;
}

bool CardTests::changePIN1(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
		byte ret = 0x03;

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Changing PIN1... %i,", i);
			bool lReturn = estEIDManager->changeAuthPin(tempPIN1, PIN1, ret);
			if(lReturn != true)
			{
				printf(" EstEIDManager::changeAuthPin returned false.");
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			else
			{
				if(!estEIDManager->changeAuthPin(PIN1, tempPIN1, ret))
                {
                    printf(" EstEIDManager::changePUK returned false.");
                    if(estEIDManager) delete estEIDManager;
                    return false;
                }
			}
			printf(" Success.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::changePIN2(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
		byte ret = 0x03;

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Changing PIN2... %i,", i);
			bool lReturn = estEIDManager->changeSignPin(tempPIN2, PIN2, ret);
			if(lReturn != true)
			{
				printf(" EstEIDManager::changeSignPin returned false.");
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			else
			{
				if(!estEIDManager->changeSignPin(PIN2, tempPIN2, ret))
                {
                    printf(" EstEIDManager::changePUK returned false.");
                    if(estEIDManager) delete estEIDManager;
                    return false;
                }
			}
			printf(" Success.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::changePUK(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
		byte ret = 0x03;

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Changing PUK... %i,", i);
			bool lReturn = estEIDManager->changePUK(tempPUK, PUK, ret);
			if(lReturn != true)
			{
				printf(" EstEIDManager::changePUK returned false.");
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			else
			{
				if(!estEIDManager->changePUK(PUK, tempPUK, ret))
                {
                    printf(" EstEIDManager::changePUK returned false.");
                    if(estEIDManager) delete estEIDManager;
                    return false;
                }
			}
			printf(" Success.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::getRetryCounts(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Reading retry counters... %i,", i);
			byte auth, sign, puk = 0x00;
			bool lReturn = estEIDManager->getRetryCounts(puk, auth, sign);
			if(lReturn == false)
			{
				printf(" EstEIDManager::getRetryCounts returned false.");
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			else
			{
				if(auth != 0x03)
				{
					printf(" EstEIDManager::getRetryCounts auth value is invlid: 0x%02X .", auth);
					if(estEIDManager) delete estEIDManager;
					return false;
				}
				if(sign != 0x03)
				{
					printf(" EstEIDManager::getRetryCounts sign value is invlid: 0x%02X .", sign);
					if(estEIDManager) delete estEIDManager;
					return false;
				}
				if(puk != 0x03)
				{
					printf(" EstEIDManager::getRetryCounts puk value is invlid: 0x%02X .", puk);
					if(estEIDManager) delete estEIDManager;
					return false;
				}
				printf(" EstEIDManager::getRetryCounts auth: 0x%02X, sign: 0x%02X, puk: 0x%02X.", auth, sign, puk);
			}
			printf(" Success.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::resetAuth(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);

		printf(" Reader %s.", estEIDManager->getReaderName().c_str());
		if(estEIDManager->getCardVersion() == EstEIDManager::VER_1_0 ||
			estEIDManager->getCardVersion() == EstEIDManager::VER_1_0_2007)
		{
			for(int i = 1; i <= testIntensity; i++)
			{
				printf("\r\n          Reseting authentication... %i,", i);
				byte ret = 0x03;
				byte hashByte[20] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x40, 0x49, 0x49, 0x47, 0x33, 0x12, 0x45, 0x23, 0x56, 0x45};
				ByteVec hash(MAKEVECTOR(hashByte));

				printf("\r\n          Calculating SSL with auth key. %i,", i);
				bool lReturn = estEIDManager->validateAuthPin(PIN1, ret);
				printf(" PIN1 validated ");
				if(lReturn == true)
				{
					ByteVec ssl = estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH);
					printf(" OK ");
				}
				printf(" Reseting. ");
				estEIDManager->resetAuth();
				try
				{
					printf(" Verify ");
					ByteVec ssl = estEIDManager->sign(hash, EstEIDManager::SSL, EstEIDManager::AUTH);
					printf(" FAILED.  ");
					return false;
				}
				catch(...)
				{
					printf(" OK.  ");
				}
				
				printf(" Success.");
			}
		}
		else
		{
			printf(" Unsupported by card. Won`t test.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}



bool CardTests::cardChallengeTest(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
        
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Reading challenges... %i, \r\n", i);
			
			ByteVec challenge1 = estEIDManager->cardChallenge();
			ByteVec challenge2 = estEIDManager->cardChallenge();

			if(challenge1.size() != 8)
			{
				printf("\r\n          Challenge 1 length is invalid\r\n");
				return false;
			}
			if(challenge2.size() != 8)
			{
				printf("\r\n          Challenge 2 length is invalid\r\n");
				return false;
			}
			if(challenge1.size() != challenge2.size())
			{
				printf("\r\n          Challenges have different length\r\n");
			}

			printf("          ");
			for(size_t i = 0; i < challenge1.size(); i++)
			{
				printf("0x%02X ", challenge1.at(i));
			}
			printf("\r\n");
			printf("          ");
			for(size_t i = 0; i < challenge2.size(); i++)
			{
				printf("0x%02X ", challenge2.at(i));
			}
			printf("\r\n");

			printf(" Success.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}


bool CardTests::getMD5KeyContainerName(void)
{
#ifdef WIN32
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Reading MD5 Key Container Name... %i,", i);
			std::string authContainer = estEIDManager->getMD5KeyContainerName(EstEIDManager::AUTH);
			std::string signContainer = estEIDManager->getMD5KeyContainerName(EstEIDManager::SIGN);
			if(authContainer.size() != 32)
			{
				printf("\r\n          AUTH key container name has invalid length\r\n");
				return false;
			}
			if(signContainer.size() != 32)
			{
				printf("\r\n          SIGN key container name has invalid length\r\n");
				return false;
			}
			if(authContainer.compare(signContainer) == 0)
			{
				printf("\r\n          AUTH & SIGN container names are equal\r\n");
				return false;
			}
			printf(" Success.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
#endif
	return true;
}



bool CardTests::readerLanguageId(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		

		for(int i = 1; i <= testIntensity; i++)
		{
			estEIDManager = new EstEIDManager(selectedCardReader);
			printf("\r\n          Cheking language id default value...");
			int lang = estEIDManager->getReaderLanguageId();
			if(lang != EstEIDManager::ENG)
			{
				printf("\r\n          Language ID is invalid %i\r\n", lang);
				return false;
			}

			printf("\r\n          Cheking if language id can be set to RUS...");
			estEIDManager->setReaderLanguageId(EstEIDManager::RUS);
			if(estEIDManager->getReaderLanguageId() != EstEIDManager::RUS)
			{
				printf("\r\n          Language ID is in invalid state %i expected %i\r\n", lang, EstEIDManager::RUS);
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			printf("\r\n          Cheking if language id can be set to ENG...");
			estEIDManager->setReaderLanguageId(EstEIDManager::ENG);
			if(estEIDManager->getReaderLanguageId() != EstEIDManager::ENG)
			{
				printf("\r\n          Language ID is in invalid state %i expected %i\r\n", lang, EstEIDManager::ENG);
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			printf("\r\n          Cheking if language id can be set to EST...");
			estEIDManager->setReaderLanguageId(EstEIDManager::EST);
			if(estEIDManager->getReaderLanguageId() != EstEIDManager::EST)
			{
				printf("\r\n          Language ID is in invalid state %i expected %i\r\n", lang, EstEIDManager::EST);
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			printf(" Success.");
			if(estEIDManager) delete estEIDManager;
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	return true;
}


bool CardTests::isPinPad(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Checking for PINPAD... %i,", i);
			bool lReturn = estEIDManager->isSecureConnection();
			if(lReturn != true)
			{
				printf(" NO PINPAD.");
			}
			else
			{
				printf(" PINPAD PRESENT.");
			}
			printf(" Success.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::setCardVersion(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Setting setCardVersion");
			estEIDManager->setCardVersion();
			printf(" %s... %i,", estEIDManager->getCardName().c_str(), i);
			if(estEIDManager->getCardVersion() == EstEIDManager::VER_INVALID)
			{
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			estEIDManager->setCardVersion();
			printf(" %s... %i,", estEIDManager->getCardName().c_str(), i);
			if(estEIDManager->getCardVersion() == EstEIDManager::VER_INVALID)
			{
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			printf(" Success.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::getCardVersion(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Checking cardCardVersion");
			EstEIDManager::CardVersion scardVersion = estEIDManager->getCardVersion();
			printf(" %s... %i,", estEIDManager->getCardName().c_str(), i);
			scardVersion = EstEIDManager::VER_1_1;
			if(scardVersion == EstEIDManager::VER_1_1)
			{
				ByteVec ver = estEIDManager->getApplicationVersion();
				if(ver[0] == 0x01 && ver[1] == 0x01)
					scardVersion = EstEIDManager::VER_1_1;
			}



			if(scardVersion == EstEIDManager::VER_INVALID)
			{
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			printf(" Success.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::RSADecrypt(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);

		for(int i = 1; i <= testIntensity; i++)
		{
			//Read certificate
			printf("\r\n                           Reading AUTH certificate. ");
			ByteVec authCert = estEIDManager->getAuthCert();
			if(authCert.size() < 1000)
			{
				printf(" FAILED. Authentication certificate is too short: %li bytes\r\n", authCert.size());
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			else
			{
				printf(" Authentication certificate size: %li bytes\r\n", authCert.size());
			}

			printf("                           Creating X509 & RSA structures ");
			const int authCertLength = (unsigned int)authCert.size();
			unsigned char *buf;
			
			buf = (unsigned char *)OPENSSL_malloc(authCertLength);

			for(int i = 0; i < authCertLength; i++)
				buf[i] = authCert[i];

			const unsigned char *pBuf = buf;
			X509 *cert;
			cert = d2i_X509(NULL, &pBuf, authCertLength);
			if(cert == NULL)
			{
				printf(" FAILED. Failed to create X509 structure \r\n");
				OPENSSL_free(buf);
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			else
			{
				printf(" X509 structure created \r\n");
			}

			EVP_PKEY *pkey = X509_get_pubkey(cert);
			RSA *rsa = EVP_PKEY_get1_RSA(pkey);
			
			printf("                           Encrypting data with AUTH public key ");

			unsigned char origData[] = {0x54, 0x48, 0x49, 0x53, 0x20, 0x53, 0x4F, 0x46, 0x54, 0x57, 0x41, 0x52, 0x45, 0x20, 0x49, 0x53,
										0x20, 0x50, 0x52, 0x4F, 0x56, 0x49, 0x44, 0x45, 0x44, 0x20, 0x42, 0x59, 0x20, 0x41, 0x4E, 0x54,
										0x4F, 0x4E, 0x20, 0x4C, 0x49, 0x54, 0x4F, 0x56, 0x54, 0x53, 0x45, 0x4E, 0x4B, 0x4F};
			const unsigned char *pOrigData = &origData[0];
			unsigned char cryptogram[1024];
			int origDataLen = sizeof(origData);
			memset((void *)cryptogram, 0, sizeof(cryptogram));

			int lReturn = RSA_public_encrypt(origDataLen, pOrigData, &cryptogram[0], rsa, RSA_PKCS1_PADDING);
			if(lReturn < 0)
			{
				printf(" FAILED. Failed to encrypt data \r\n");
			}
			else
			{
				printf(" Data successfully encrypted \r\n");
			}

			printf("                           Verifying PIN1 & decrypting data with AUTH private key ");

			ByteVec cryptoVector(cryptogram, cryptogram + sizeof(cryptogram) / sizeof(unsigned char));
			cryptoVector.resize(lReturn, 0x00);
			byte ret = 0x03;
            estEIDManager->startTransaction();
			estEIDManager->validateAuthPin(PIN1, ret);
			ByteVec decryptedData = estEIDManager->RSADecrypt(cryptoVector);
            estEIDManager->endTransaction();
			printf("\r\n\r\n");
			for(unsigned int i = 0; i < decryptedData.size(); i++)
			{
				printf("%X == %X; ", decryptedData.at(i), origData[i]);
				if(decryptedData.at(i) != origData[i])
				{
					OPENSSL_free(buf);
					printf("                              FAILED. Data verification failed ");
					if(estEIDManager) delete estEIDManager;
					return false;
				}
				else
				{
					printf(" == OK ");
				}
			}
			OPENSSL_free(buf);
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::RSADecryptWithPIN1(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);

		//Read certificate
		printf("\r\n                           Reading AUTH certificate. ");
		ByteVec authCert = estEIDManager->getAuthCert();
		if(authCert.size() < 1000)
		{
			printf(" FAILED. Authentication certificate is too short: %li bytes\r\n", authCert.size());
			if(estEIDManager) delete estEIDManager;
			return false;
		}
		else
		{
			printf(" Authentication certificate size: %li bytes\r\n", authCert.size());
		}

		printf("                           Creating X509 & RSA structures ");
		const int authCertLength = (unsigned int)authCert.size();
		unsigned char *buf;
		
		buf = (unsigned char *)OPENSSL_malloc(authCertLength);

		for(int i = 0; i < authCertLength; i++)
			buf[i] = authCert[i];

		const unsigned char *pBuf = buf;
		X509 *cert;
		cert = d2i_X509(NULL, &pBuf, authCertLength);
		if(cert == NULL)
		{
			printf(" FAILED. Failed to create X509 structure \r\n");
			OPENSSL_free(buf);
			if(estEIDManager) delete estEIDManager;
			return false;
		}
		else
		{
			printf(" X509 structure created \r\n");
		}

		EVP_PKEY *pkey = X509_get_pubkey(cert);
		RSA *rsa = EVP_PKEY_get1_RSA(pkey);
		
		printf("                           Encrypting data with AUTH public key ");

		unsigned char origData[] = {0x54, 0x48, 0x49, 0x53, 0x20, 0x53, 0x4F, 0x46, 0x54, 0x57, 0x41, 0x52, 0x45, 0x20, 0x49, 0x53,
									0x20, 0x50, 0x52, 0x4F, 0x56, 0x49, 0x44, 0x45, 0x44, 0x20, 0x42, 0x59, 0x20, 0x41, 0x4E, 0x54,
									0x4F, 0x4E, 0x20, 0x4C, 0x49, 0x54, 0x4F, 0x56, 0x54, 0x53, 0x45, 0x4E, 0x4B, 0x4F};
		const unsigned char *pOrigData = &origData[0];
		unsigned char cryptogram[1024];
		int origDataLen = sizeof(origData);
		memset((void *)cryptogram, 0, sizeof(cryptogram));

		int lReturn = RSA_public_encrypt(origDataLen, pOrigData, &cryptogram[0], rsa, RSA_PKCS1_PADDING);
		if(lReturn < 0)
		{
			printf(" FAILED. Failed to encrypt data \r\n");
		}
		else
		{
			printf(" Data successfully encrypted \r\n");
		}

		printf("                           Verifying PIN1 & decrypting data with AUTH private key ");

		ByteVec cryptoVector(cryptogram, cryptogram + sizeof(cryptogram) / sizeof(unsigned char));
		cryptoVector.resize(lReturn, 0x00);
		ByteVec decryptedData = estEIDManager->RSADecrypt(cryptoVector, PIN1);

		printf("\r\n\r\n");
		for(unsigned int i = 0; i < decryptedData.size(); i++)
		{
			printf("%X == %X; ", decryptedData.at(i), origData[i]);
			if(decryptedData.at(i) != origData[i])
			{
				OPENSSL_free(buf);
				printf("                              FAILED. Data verification failed ");
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			else
			{
				printf(" == OK ");
			}
		}

		OPENSSL_free(buf);
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::unblockPIN1(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
		byte ret = 0x03;

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Blocking ");
			for(int y = 0; y < 3; y++)
			{
				try
				{
					estEIDManager->validateAuthPin("9999", ret);
				}
				catch(...)
				{}
			}

			printf(" OK. ");
			printf("          Unblocking PIN1... %i,", i);
			bool lReturn = estEIDManager->unblockAuthPin(tempPIN1, PUK, ret);
			if(lReturn != true)
			{
				printf(" EstEIDManager::unblockAuthPin returned false.");
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			else
			{
				if(!estEIDManager->changeAuthPin(PIN1, tempPIN1, ret))
                {
                    printf(" EstEIDManager::unblockAuthPin returned false.");
                    if(estEIDManager) delete estEIDManager;
                    return false;
                }
			}
			printf(" Success.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::unblockPIN2(void)
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
		byte ret = 0x03;

		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          Blocking ");
			for(int y = 0; y < 3; y++)
			{
				try
				{
					estEIDManager->validateSignPin("99999", ret);
				}
				catch(...)
				{}
			}

			printf(" OK. ");
			printf("          Unblocking PIN2... %i,", i);
			bool lReturn = estEIDManager->unblockSignPin(tempPIN2, PUK, ret);
			if(lReturn != true)
			{
				printf(" EstEIDManager::unblockSignPin returned false.");
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			else
			{
				if(!estEIDManager->changeSignPin(PIN2, tempPIN2, ret))
                {
                    printf(" EstEIDManager::unblockAuthPin returned false.");
                    if(estEIDManager) delete estEIDManager;
                    return false;
                }
			}
			printf(" Success.");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::ReadDataFromMinidriverTest(void)
{
#ifdef WIN32
		for(int i = 1; i <= testIntensity; i++)
		{
			SCARDCONTEXT scardCtx;
			SCARDHANDLE scard;
			long lReturn = SCARD_S_SUCCESS;
			LPTSTR readers = NULL;
			LPTSTR reader = NULL;
			DWORD readersLen = SCARD_AUTOALLOCATE;
			DWORD dwAP;

			lReturn = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &scardCtx);
			if(lReturn != SCARD_S_SUCCESS)
			{
				printf("SCardEstablishContext failed 0x%08lX\r\n", lReturn);
				return false;
			}

			lReturn = SCardListReaders(scardCtx, NULL, (LPTSTR)&readers, &readersLen);

			switch(lReturn)
			{
			case SCARD_S_SUCCESS:
				reader = readers;
				break;
			case SCARD_E_NO_READERS_AVAILABLE: printf("SCARD_E_NO_READERS_AVAILABLE\r\n"); return false; break;
			default: printf("SCardListReaders failed 0x%08lX\r\n", lReturn); return false; break;
			}
				
			lReturn = SCardConnect(scardCtx, tokens[selectedCardReader].first.c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &scard, &dwAP);
			if(lReturn != SCARD_S_SUCCESS)
			{
				printf("SCardConnect failed 0x%08lX\r\n", lReturn);
				SCardReleaseContext(scardCtx);
				return false;
			}
			printf("\r\n");
			EstEIDManager *estEIDManager;
			try
			{
				estEIDManager = new EstEIDManager(scardCtx, scard);
				byte ret = 0x03;
				printf("          Reader %s \r\n", estEIDManager->getReaderName().c_str());
				printf("          CP_CARD_GUID \r\n");
				std::string documentID = estEIDManager->readDocumentID();
				printf("          AUTH \r\n");
				ByteVec authCert = estEIDManager->getAuthCert();
				printf("          SIGN \r\n");
				ByteVec signCert = estEIDManager->getSignCert();
				printf("          PIN1 \r\n");
				if(estEIDManager->isSecureConnection())
				{
					estEIDManager->validateAuthPin("", ret);
				}
					estEIDManager->validateAuthPin(PIN1, ret);

				printf("          PIN2 \r\n");
				if(estEIDManager->isSecureConnection())
				{
					estEIDManager->validateSignPin("", ret);
				}
				else
					estEIDManager->validateSignPin(PIN2, ret);

				printf("          PUK \r\n");
				if(estEIDManager->isSecureConnection())
				{
					estEIDManager->validatePuk("", ret);
				}
				else
					estEIDManager->validatePuk(PUK, ret);
				
				delete estEIDManager;
			}
			catch(CardError &e)
			{
				printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			catch(runtime_error &r)
			{
				printf("%s ", r.what());
				if(estEIDManager) delete estEIDManager;
				return false;
			}

			lReturn = SCardDisconnect(scard, SCARD_RESET_CARD);
			if(lReturn != SCARD_S_SUCCESS)
			{
				printf("SCardDisconnect failed 0x%08lX\r\n", lReturn);
				SCardReleaseContext(scardCtx);
				return false;
			}

			lReturn = SCardFreeMemory(scardCtx, readers);
			if(lReturn != SCARD_S_SUCCESS)
			{
				printf("SCardFreeMemory failed 0x%08lX\r\n", lReturn);
				return false;
			}

			lReturn = SCardReleaseContext(scardCtx);
			if(lReturn != SCARD_S_SUCCESS)
			{
				printf("SCardReleaseContext failed 0x%08lX\r\n", lReturn);
				return false;
			}
		}
#endif
	return true;
}

bool CardTests::ReadDataFromAllAvailableReadesTest()
{
	for(int i = 1; i <= testIntensity; i++)
	{
		printf("\r\n %i. ", i);
		EstEIDManager *estEIDManager = NULL;
		try
		{
			estEIDManager = new EstEIDManager();
			uint readersCount = estEIDManager->getTokenCount(false);
			if(estEIDManager) delete estEIDManager;
			printf("\r\n         Reading data from all %i readers \r\n", readersCount);
			for(uint i = 0; i < readersCount; i++)
			{
				EstEIDManager *estEIDManagerInternal;
				byte ret = 0x03;
				if(tokens[i].second.compare("EMPTY") != 0)
				{
					try
					{
						estEIDManagerInternal = new EstEIDManager(i);
						printf("         %i. Reader name: %s \r\n", i, estEIDManagerInternal->getReaderName().c_str());
						printf("                  %i. Document number: %s \r\n", i, estEIDManagerInternal->readDocumentID().c_str());
						printf("                  %i. AUTH certificate length: %li \r\n", i, estEIDManagerInternal->getAuthCert().size());
						printf("                  %i. SIGN certificate length: %li \r\n", i, estEIDManagerInternal->getSignCert().size());
						if(estEIDManagerInternal->isSecureConnection() == true)
						{
							printf("                  %i. Validate PIN1 using PINPAD\r\n", i);
							estEIDManagerInternal->validateAuthPin("", ret);
						}
						else
						{
							printf("                  %i. Validate PIN1 \r\n", i);
							estEIDManagerInternal->validateAuthPin(PIN1, ret);
						}

						if(estEIDManagerInternal->isSecureConnection() == true)
						{
							printf("                  %i. Validate PIN2  using PINPAD\r\n", i);
							estEIDManagerInternal->validateSignPin("", ret);
						}
						else
						{
							printf("                  %i. Validate PIN2 \r\n", i);
							estEIDManagerInternal->validateSignPin(PIN2, ret);
						}

						if(estEIDManagerInternal->isSecureConnection() == true)
						{
							printf("                  %i. Validate PUK  using PINPAD\r\n", i);
							estEIDManagerInternal->validatePuk("", ret);
						}
						else
						{
							printf("                  %i. Validate PUK \r\n", i);
							estEIDManagerInternal->validatePuk(PUK, ret);
						}
						printf("\r\n");
						delete estEIDManagerInternal;
					}
					catch(...)
					{
						delete estEIDManagerInternal;
						printf("Internal error");
						return false;
					}
				}
				else
				{
					printf("         Reader %s is empty. Skipping. \r\n", tokens[i].first.c_str());
				}
			}
		}
		catch(AuthError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;
			return false;
		}
		catch(CardError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;		return false;
		}
		catch(runtime_error &r)
		{
			printf("%s ", r.what());
			if(estEIDManager) delete estEIDManager;
			return false;
		}
	}
	return true;
}

bool CardTests::ListAllReadersUtilityStyleTest()
{
	//for(int i = 1; i <= testIntensity; i++)
	//{
		//printf("\r\n %i. ", i);
		EstEIDManager *estEIDManager = NULL;
		try
		{
			for(int y = 0; y < constructorsCount; y++) //Used to test Utility card pooling
			{
				estEIDManager = new EstEIDManager();
				std::vector<std::pair<std::string, std::string> > tokens;
				
				try
				{
				  tokens = estEIDManager->getTokenList();
				}
				catch(NoReadersAvailible &e)
				{
				  printf("\r\n          %s", e.what());
#ifdef WIN32
				  Sleep(1000);
#else
				  sleep(1);
#endif
				  continue;
				}

				for(unsigned int i = 0; i < tokens.size(); i++)
				{
					printf("\r\n%i         Reader \"%s\" has %s token", y, tokens[i].first.c_str(), tokens[i].second.c_str());
				}
				delete estEIDManager;

				printf("\r\n\r\n          Connecting to readers by name and reading data %li\r\n", tokens.size());
				for(unsigned int i = 0; i < tokens.size(); i++)
				{
					if(tokens[i].second.compare("") != 0)
					{
						try
						{
							estEIDManager = new EstEIDManager(tokens[i].first);
							printf("          %i. Reader name: %s \r\n", i, estEIDManager->getReaderName().c_str());
							printf("          %i. Reader state: %s \r\n", i, estEIDManager->getReaderState().c_str());
							printf("          %i. Document number: %s \r\n", i, estEIDManager->readDocumentID().c_str());
							printf("\r\n");
							if(estEIDManager) delete estEIDManager;
						}
						catch(...)
						{
							if(estEIDManager) delete estEIDManager;
							printf("Internal error");
							return false;
						}
					}
				}
				printf("\r\n");
	#ifdef WIN32
				Sleep(1000);
	#else
				sleep(1);
	#endif
			}
		}
		catch(AuthError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;
			return false;
		}
		catch(CardError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;		return false;
		}
		catch(runtime_error &r)
		{
			printf("%s ", r.what());
			if(estEIDManager) delete estEIDManager;
			return false;
		}
	//}
	return true;
}

bool CardTests::getReaderNameTest()
{
	for(int i = 1; i <= testIntensity; i++)
	{
		printf("\r\n %i. ", i);
		EstEIDManager *estEIDManager = NULL;
		EstEIDManager *estEIDManager2 = NULL;
		try
		{
			printf("\r\n          Reading reading reader names \r\n");
			for(int i = 1; i <= testIntensity; i++)
			{
				estEIDManager = new EstEIDManager(selectedCardReader);
				estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

				printf("         %i. Reader name: %s \r\n", i, estEIDManager->getReaderName().c_str());
				printf("         %i. Reader name: %s \r\n", i, estEIDManager2->getReaderName().c_str());

				delete estEIDManager;
				delete estEIDManager2;
			}
		}
		catch(AuthError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;
			if(estEIDManager2) delete estEIDManager2;
			return false;
		}
		catch(CardError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;		return false;
		}
		catch(runtime_error &r)
		{
			printf("%s ", r.what());
			if(estEIDManager) delete estEIDManager;
			if(estEIDManager2) delete estEIDManager2;
			return false;
		}
	}
	return true;
}

bool CardTests::getReaderStateTest()
{
	for(int i = 1; i <= testIntensity; i++)
	{
		printf("\r\n %i. ", i);
		EstEIDManager *estEIDManager = NULL;
		EstEIDManager *estEIDManager2 = NULL;
		try
		{
			printf("\r\n          Reading reading reader states \r\n");
			for(int i = 1; i <= testIntensity; i++)
			{
				estEIDManager = new EstEIDManager(selectedCardReader);
				estEIDManager2 = new EstEIDManager(selectedSecondCardReader);
				
				printf("         %i. Reader name: %s \r\n", i, estEIDManager->getReaderState().c_str());
				printf("         %i. Reader name: %s \r\n", i, estEIDManager2->getReaderState().c_str());

				delete estEIDManager;
				delete estEIDManager2;
			}
		}
		catch(AuthError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;
			if(estEIDManager2) delete estEIDManager2;
			return false;
		}
		catch(CardError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;
			if(estEIDManager2) delete estEIDManager2;
			return false;
		}
		catch(runtime_error &r)
		{
			printf("%s ", r.what());
			if(estEIDManager) delete estEIDManager;
			if(estEIDManager2) delete estEIDManager2;
			return false;
		}
	}
	return true;
}

bool CardTests::getTokenCountTest()
{
	for(int i = 1; i <= testIntensity; i++)
	{
		printf("\r\n %i. ", i);
		EstEIDManager *estEIDManager = NULL;
		try
		{
			printf("\r\n          Counting tokens \r\n");
			for(int i = 1; i <= testIntensity; i++)
			{
				estEIDManager = new EstEIDManager();
				
				printf("         %i. Totally %i tokens availible \r\n", i, estEIDManager->getTokenCount(false));
				printf("         %i. Totally %i tokens availible \r\n", i, estEIDManager->getTokenCount(true));

				delete estEIDManager;
			}
		}
		catch(AuthError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;
			return false;
		}
		catch(CardError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;		return false;
		}
		catch(runtime_error &r)
		{
			printf("%s ", r.what());
			if(estEIDManager) delete estEIDManager;
			return false;
		}
	}
	return true;
}

bool CardTests::fileSelectOperationsTest()
{
	for(int i = 1; i <= testIntensity; i++)
	{
		printf("\r\n %i. ", i);
		EstEIDManager *estEIDManager = NULL;
		EstEIDManager *estEIDManager2 = NULL;
		try
		{
			printf("\r\n          Testing select file operations \r\n");
			for(int i = 1; i <= testIntensity; i++)
			{
				estEIDManager = new EstEIDManager(selectedCardReader);
				estEIDManager2 = new EstEIDManager(selectedSecondCardReader);

				estEIDManager->selectMF();
				estEIDManager->selectDF(0xEEEE);
				estEIDManager->selectEF(0x0033);
				estEIDManager->setSecEnv(3);

				printf("\r\n          OK");

				delete estEIDManager;
				delete estEIDManager2;
			}
		}
		catch(AuthError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;
			if(estEIDManager2) delete estEIDManager2;
			return false;
		}
		catch(CardError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;
			if(estEIDManager2) delete estEIDManager2;
			return false;
		}
		catch(runtime_error &r)
		{
			printf("%s ", r.what());
			if(estEIDManager) delete estEIDManager;
			if(estEIDManager2) delete estEIDManager2;
			return false;
		}
	}
	return true;
}

bool CardTests::ListAllReadersPKCS11StyleTest()
{
	for(int i = 1; i <= testIntensity; i++)
	{
		printf("\r\n %i. ", i);
		EstEIDManager *estEIDManager = NULL;
		try
		{
			for(int i = 1; i <= testIntensity; i++)
			{
				estEIDManager = new EstEIDManager();
				unsigned int tokenCount = estEIDManager->getTokenCount(false);
				delete estEIDManager;
				printf("\r\n          Found %i readers \r\n\r\n", tokenCount);
					
				for(unsigned int i = 0; i < tokenCount; i++)
				{
					try
					{
					  estEIDManager = new EstEIDManager(i);
					  if(estEIDManager->isInReader(i) == true)
					  {
							printf("\r\n          Card is in reader %s", estEIDManager->getReaderName().c_str());
					  }
					  else
					  {
						  printf("\r\n          No card in reader %s", estEIDManager->getReaderName().c_str());
					  }
					  delete estEIDManager;
					}
					catch(SCError)
					{
						printf("\r\n          Error connecting to card.");
					}
				}
				printf("\r\n");
	#ifdef WIN32
				Sleep(100);
	#else
				sleep(1);
	#endif
			}
		}
		catch(AuthError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;
			return false;
		}
		catch(CardError &e)
		{
			printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
			if(estEIDManager) delete estEIDManager;		return false;
		}
		catch(runtime_error &r)
		{
			printf("%s ", r.what());
			if(estEIDManager) delete estEIDManager;
			return false;
		}
	}
	return true;
}

bool CardTests::reconnectWithT0Test()
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          %i,", i);
			string documentID = estEIDManager->readDocumentID();
			printf("%s", documentID.c_str());
			if(documentID.length() == 8 || documentID.length() == 9)
				;
			else
			{
				printf("\r\n Invalid document id length: %li [%s] ", documentID.length(), documentID.c_str());
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			printf(" Reconnect with T0 ");
			estEIDManager->reconnectWithT0();
			documentID = estEIDManager->readDocumentID();
			printf("%s", documentID.c_str());
			if(documentID.length() == 8 || documentID.length() == 9)
				;
			else
			{
				printf("\r\n Invalid document id length: %li [%s] ", documentID.length(), documentID.c_str());
				if(estEIDManager) delete estEIDManager;
				return false;
			}
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::reconnectWithT1Test()
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          %i,", i);
			string documentID = estEIDManager->readDocumentID();
			printf("%s", documentID.c_str());
			if(documentID.length() == 8 || documentID.length() == 9)
				;
			else
			{
				printf("\r\n Invalid document id length: %li [%s] ", documentID.length(), documentID.c_str());
				if(estEIDManager) delete estEIDManager;
				return false;
			}
			printf(" Reconnect with T1 ");
			estEIDManager->reconnectWithT1();
			documentID = estEIDManager->readDocumentID();
			printf("%s", documentID.c_str());
			if(documentID.length() == 8 || documentID.length() == 9)
				;
			else
			{
				printf("\r\n Invalid document id length: %li [%s] ", documentID.length(), documentID.c_str());
				if(estEIDManager) delete estEIDManager;
				return false;
			}
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::ConnectMultiplyToOneIndexTest()
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		
		for(int i = 1; i <= testIntensity; i++)
		{

			estEIDManager = new EstEIDManager();
			estEIDManager->getTokenCount(true);
			
			EstEIDManager *estEidManager1 = new EstEIDManager(selectedCardReader);
			EstEIDManager *estEidManager2 = new EstEIDManager(selectedCardReader);
			EstEIDManager *estEidManager3 = new EstEIDManager(selectedCardReader);
			EstEIDManager *estEidManager4 = new EstEIDManager(selectedCardReader);
			EstEIDManager *estEidManager5 = new EstEIDManager(selectedCardReader);

			printf("\r\n          1. %s 2. %s 3. %s 4. %s 5. %s", estEidManager1->readDocumentID().c_str(), 
													estEidManager2->readDocumentID().c_str(),
													estEidManager3->readDocumentID().c_str(),
													estEidManager4->readDocumentID().c_str(),
													estEidManager5->readDocumentID().c_str());

			delete estEidManager1;
			delete estEidManager2;
			delete estEidManager3;
			delete estEidManager4;
			delete estEidManager5;
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

bool CardTests::firefoxCardPoolingTest()
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		for(int i = 1; i <= testIntensity; i++)
		{
			for(int y = 0; y < constructorsCount; y++)
			{
				estEIDManager = new EstEIDManager();
				uint count = estEIDManager->getTokenCount(true);
				delete estEIDManager;
				for(int z = 0; z < count; z++)
				{
					EstEIDManager *m = new EstEIDManager(z);
					printf("\n %i Reader: %s, %s", y, m->getReaderName().c_str(), m->isInReader(z) ? "PRESENT" : "EMPTY");
					delete m;
				}
			}
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	return true;
}

bool CardTests::isDigiIDTest()
{
	EstEIDManager *estEIDManager = NULL;
	try
	{
		estEIDManager = new EstEIDManager(selectedCardReader);
		for(int i = 1; i <= testIntensity; i++)
		{
			printf("\r\n          %i,", i);
			string name = estEIDManager->readCardName();
			if(estEIDManager->isDigiID() == true)
				printf("Card is DIGI-ID");
			else
				printf("Card is not DIGI-ID");
		}
	}
	catch(CardError &e)
	{
		printf("%s SW1: 0x%X SW2: 0x%X ", e.what(), e.SW1, e.SW2);
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	catch(runtime_error &r)
	{
		printf("%s ", r.what());
		if(estEIDManager) delete estEIDManager;
		return false;
	}
	if(estEIDManager) delete estEIDManager;
	return true;
}

#ifdef WIN32

int CardTests::gettimeofday(struct timeval *tv, struct timezone *tz)
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
#else
#include <sys/time.h>
int CardTests::gettimeofday(struct timeval *tv, struct timezone *tz)
{
  tv->tv_sec = 0;
  tv->tv_usec = 0;
  
  return 0;
}
#endif

long CardTests::timeval_diff(struct timeval *difference, struct timeval *end_time, struct timeval *start_time)
{
	struct timeval temp_diff;

	if(difference==NULL)
	{
		difference=&temp_diff;
	}

	difference->tv_sec =end_time->tv_sec -start_time->tv_sec ;
	difference->tv_usec=end_time->tv_usec-start_time->tv_usec;

	/* Using while instead of if below makes the code slightly more robust. */

	while(difference->tv_usec<0)
	{
		difference->tv_usec+=1000000;
		difference->tv_sec -=1;
	}

	return 1000000LL*difference->tv_sec+difference->tv_usec;
}
