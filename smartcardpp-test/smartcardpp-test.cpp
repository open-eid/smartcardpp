#include "CardTests.h"

int main()
{
	bool overalTestResult = false;
	CardTests *cardTests = new CardTests();
	struct timeval totalStartTim;
	struct timeval totalEndTim;
	struct timeval totalDiff;
	struct timeval startTim;
	struct timeval endTim;
	struct timeval diff;
    
    int totalTests = 0;
    int failedTests = 0;
    int okTests = 0;

	cardTests->init();

	cardTests->gettimeofday(&totalStartTim, NULL);
	//Run constructors test

	printf("0. Running test: constructorsTest ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool constructorsTestResult = cardTests->constructorsTest();
	if(constructorsTestResult == true)
	{
		overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);
	//Run isInReaderTest

	printf("1. Running test: isInReaderTest ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool isInReaderTestResult = cardTests->isInReaderTest();
	if(isInReaderTestResult == true)
	{
		overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);
	//Run GetKeySize

	printf("2. Running test: getKeySize ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool getKeySizeTestResult = cardTests->getKeySize();
	if(getKeySizeTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);
	//Run readCardIDTest

	printf("3. Running test: readCardIDTest ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool readCardIDTestResult = cardTests->readCardIDTest();
	if(readCardIDTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	//Run readDocumentIDTest

	printf("4. Running test: readDocumentIDTest ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool readDocumentIDTestResult = cardTests->readDocumentIDTest();
	if(readDocumentIDTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	//Run readCardNameTest

	printf("5. Running test: readCardNameTest ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool readCardNameTestResult = cardTests->readCardNameTest();
	if(readCardNameTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	//Run readCardNameTest

	printf("6. Running test: readCardNameFirstNameFirstTest ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool readCardNameFirstNameFirstTestResult = cardTests->readCardNameFirstNameFirstTest();
	if(readCardNameFirstNameFirstTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);
	
	//Run readPersonalDataTest

	printf("7. Running test: readPersonalDataTest ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool readPersonalDataTestResult = cardTests->readPersonalDataTest();
	if(readPersonalDataTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	//Run getKeyUsageCountersTest

	printf("8. Running test: getKeyUsageCountersTest ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool getKeyUsageCountersTestResult = cardTests->getKeyUsageCountersTest();
	if(getKeyUsageCountersTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	//Run getAuthCertTestTest

	printf("9. Running test: getAuthCertTest ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool getAuthCertTestResult = cardTests->getAuthCertTest();
	if(getAuthCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);
    
    //Run getAuthCert_With_Context_Reset_Test
    
    printf("9.1. Running test: getAuthCert_With_Context_Reset_Test ");
    cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
    bool getAuthCert_With_Context_Reset_TestResult = cardTests->getAuthCert_With_Context_Reset_Test();
    if(getAuthCert_With_Context_Reset_TestResult == true)
    {
        if(overalTestResult != false)
            overalTestResult = true;
        okTests++;
        printf("   OK\n");
    }
    else
    {
        overalTestResult = false;
        failedTests++;
        printf("   FAILED\n");
    }
    cardTests->gettimeofday(&endTim, NULL);
    cardTests->timeval_diff(&diff, &endTim, &startTim);
    printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	//Run getSignCertTest

	printf("10. Running test: getSignCertTest ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool getSignCertTestResult = cardTests->getSignCertTest();
	if(getSignCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);
    
    //Run getSignCert_With_Context_Reset_Test
    
    printf("10.1. Running test: getSignCert_With_Context_Reset_Test ");
    cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
    bool getSignCert_With_Context_Reset_TestResult = cardTests->getSignCert_With_Context_Reset_Test();
    if(getSignCert_With_Context_Reset_TestResult == true)
    {
        if(overalTestResult != false)
            overalTestResult = true;
        okTests++;
        printf("   OK\n");
    }
    else
    {
        overalTestResult = false;
        failedTests++;
        printf("   FAILED\n");
    }
    cardTests->gettimeofday(&endTim, NULL);
    cardTests->timeval_diff(&diff, &endTim, &startTim);
    printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("11. Running test: calcSSL_HashOnly_WithAuthCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSSL_HashOnly_WithAuthCertTestResult = cardTests->calcSSL_HashOnly_WithAuthCert();
	if(calcSSL_HashOnly_WithAuthCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);


	printf("12. Running test: calcSSL_HashOnly_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSSL_HashOnly_WithSignCertTestResult = cardTests->calcSSL_HashOnly_WithSignCert();
	if(calcSSL_HashOnly_WithSignCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("13. Running test: calcSSL_WithPIN_WithAuthCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSSL_WithPIN_WithAuthCertTestResult = cardTests->calcSSL_WithPIN_WithAuthCert();
	if(calcSSL_WithPIN_WithAuthCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("14. Running test: calcSignSHA1_HashOnly_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignSHA1_HashOnly_WithSignCertTestResult = cardTests->calcSignSHA1_HashOnly_WithSignCert();
	if(calcSignSHA1_HashOnly_WithSignCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("15. Running test: calcSignSHA1_WithPIN_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignSHA1_WithPIN_WithSignCertTestResult = cardTests->calcSignSHA1_WithPIN_WithSignCert();
	if(calcSignSHA1_WithPIN_WithSignCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("16. Running test: calcSignMD5_HashOnly_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignMD5_HashOnly_WithSignCertTestResult = cardTests->calcSignMD5_HashOnly_WithSignCert();
	if(calcSignMD5_HashOnly_WithSignCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("17. Running test: calcSignMD5_WithPIN_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignMD5_WithPIN_WithSignCertTestResult = cardTests->calcSignMD5_WithPIN_WithSignCert();
	if(calcSignMD5_WithPIN_WithSignCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("18. Running test: validateAuthPIN ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool validateAuthPINTestResult = cardTests->validateAuthPIN();
	if(validateAuthPINTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("19. Running test: validateSignPIN ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool validateSignPINTestResult = cardTests->validateSignPIN();
	if(validateSignPINTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("20. Running test: validatePUK ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool validatePUKTestResult = cardTests->validatePUK();
	if(validatePUKTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("21. Running test: calcSignSHA224_HashOnly_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignSHA224_HashOnly_WithSignCertResult = cardTests->calcSignSHA224_HashOnly_WithSignCert();
	if(calcSignSHA224_HashOnly_WithSignCertResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("22. Running test: calcSignSHA224_WithPIN_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignSHA224_WithPIN_WithSignCertTestResult = cardTests->calcSignSHA224_WithPIN_WithSignCert();
	if(calcSignSHA224_WithPIN_WithSignCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("23. Running test: calcSignSHA256_HashOnly_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignSHA256_HashOnly_WithSignCertResult = cardTests->calcSignSHA256_HashOnly_WithSignCert();
	if(calcSignSHA256_HashOnly_WithSignCertResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("24. Running test: calcSignSHA256_WithPIN_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignSHA256_WithPIN_WithSignCertTestResult = cardTests->calcSignSHA256_WithPIN_WithSignCert();
	if(calcSignSHA256_WithPIN_WithSignCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("25. Running test: calcSignSHA384_HashOnly_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignSHA384_HashOnly_WithSignCertResult = cardTests->calcSignSHA384_HashOnly_WithSignCert();
	if(calcSignSHA384_HashOnly_WithSignCertResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("26. Running test: calcSignSHA384_WithPIN_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignSHA384_WithPIN_WithSignCertTestResult = cardTests->calcSignSHA384_WithPIN_WithSignCert();
	if(calcSignSHA384_WithPIN_WithSignCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("27. Running test: calcSignSHA512_HashOnly_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignSHA512_HashOnly_WithSignCertResult = cardTests->calcSignSHA512_HashOnly_WithSignCert();
	if(calcSignSHA512_HashOnly_WithSignCertResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("28. Running test: calcSignSHA512_WithPIN_WithSignCert ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool calcSignSHA512_WithPIN_WithSignCertTestResult = cardTests->calcSignSHA512_WithPIN_WithSignCert();
	if(calcSignSHA512_WithPIN_WithSignCertTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("29. Running test: changePIN1 ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool changePIN1TestResult = cardTests->changePIN1();
	if(changePIN1TestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("30. Running test: changePIN2 ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool changePIN2TestResult = cardTests->changePIN2();
	if(changePIN2TestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("31. Running test: changePUK ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool changePUKTestResult = cardTests->changePUK();
	if(changePUKTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);
	
	printf("32. Running test: unblockPIN1 ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool unblockPIN1TestResult = cardTests->unblockPIN1();
	if(unblockPIN1TestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);
	
	printf("33. Running test: unblockPIN2 ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool unblockPIN2TestResult = cardTests->unblockPIN2();
	if(unblockPIN2TestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);
	
	printf("34. Running test: getRetryCounts ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool getRetryCountsTestResult = cardTests->getRetryCounts();
	if(getRetryCountsTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("35. Running test: resetAuth ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool resetAuthTestResult = cardTests->resetAuth();
	if(resetAuthTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);
	
	printf("36. Running test: cardChallenge ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool cardChallengeTestResult = cardTests->cardChallengeTest();
	if(cardChallengeTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("37. Running test: getMD5KeyContainerName ");
    cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool getMD5KeyContainerNameTestResult = cardTests->getMD5KeyContainerName();
	if(getMD5KeyContainerNameTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
    cardTests->gettimeofday(&endTim, NULL);
    cardTests->timeval_diff(&diff, &endTim, &startTim);
    printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("38. Running test: readerLanguageId ");
    cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool readerLanguageIdNameTestResult = cardTests->readerLanguageId();
	if(readerLanguageIdNameTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
    cardTests->gettimeofday(&endTim, NULL);
    cardTests->timeval_diff(&diff, &endTim, &startTim);
    printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("39. Running test: isPinPad ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool isPinPadTestResult = cardTests->isPinPad();
	if(isPinPadTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("40. Running test: setCardVersion ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool setCardVersionTestResult = cardTests->setCardVersion();
	if(setCardVersionTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("41. Running test: getCardVersion ");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool getCardVersionTestResult = cardTests->getCardVersion();
	if(getCardVersionTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("42. Running test: RSADecrypt (This test may fail. Does not support mutlitransactions.)");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool RSADecryptTestResult = cardTests->RSADecrypt();
	if(RSADecryptTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("43. Running test: RSADecrypt with PIN1");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool RSADecryptWithPIN1TestResult = cardTests->RSADecryptWithPIN1();
	if(RSADecryptWithPIN1TestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("44. Running test: Read data from minidriver");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool ReadDataFromMinidriverTestResult = cardTests->ReadDataFromMinidriverTest();
	if(ReadDataFromMinidriverTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("45. Running test: Read data from all available readers test");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool ReadDataFromAllAvailableReadesTestResult = cardTests->ReadDataFromAllAvailableReadesTest();
	if(ReadDataFromAllAvailableReadesTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("46. Running test: List all readers, document numbers and reader states using utility algorythm");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool ListAllReadersUtilityStyleTestResult = cardTests->ListAllReadersUtilityStyleTest();
	if(ListAllReadersUtilityStyleTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("47. Running test: Reading reader names");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool getReaderNameTestResult = cardTests->getReaderNameTest();
	if(getReaderNameTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("48. Running test: Reading reader states");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool getReaderStateResult = cardTests->getReaderStateTest();
	if(getReaderStateResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("49. Running test: Get token count");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool getTokenCountTestResult = cardTests->getTokenCountTest();
	if(getTokenCountTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("50. Running test: Testing select file operations");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool fileSelectOperationsTestResult = cardTests->fileSelectOperationsTest();
	if(fileSelectOperationsTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);
	
	printf("51. Running test: Read data from all readers PKCS11 style");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool ListAllReadersPKCS11StyleTestResult = cardTests->ListAllReadersPKCS11StyleTest();
	if(ListAllReadersPKCS11StyleTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("52. Running test: Test reconnect with T0");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool reconnectWithT0TestResult = cardTests->reconnectWithT0Test();
	if(reconnectWithT0TestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("53. Running test: Test reconnect with T1");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool reconnectWithT1TestResult = cardTests->reconnectWithT1Test();
	if(reconnectWithT1TestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("54. Running test: Multiply connections to single slot");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool ConnectMultiplyToOneIndexTestResult = cardTests->ConnectMultiplyToOneIndexTest();
	if(ConnectMultiplyToOneIndexTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("55. Running test: Firefox card pooling");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool firefoxCardPoolingTestResult = cardTests->firefoxCardPoolingTest();
	if(firefoxCardPoolingTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("56. Running test: isDigiID");
	cardTests->gettimeofday(&startTim, NULL);
    totalTests++;
	bool isDigiIDTestResult = cardTests->isDigiIDTest();
	if(isDigiIDTestResult == true)
	{
		if(overalTestResult != false)
			overalTestResult = true;
        okTests++;
		printf("   OK\n");
	}
	else
	{
		overalTestResult = false;
        failedTests++;
		printf("   FAILED\n");
	}
	cardTests->gettimeofday(&endTim, NULL);
	cardTests->timeval_diff(&diff, &endTim, &startTim);
	printf("          Test took %li.%li seconds\n", diff.tv_sec, (long)diff.tv_usec);

	printf("\n\n--------------------------------------------------------------\n");

	cardTests->gettimeofday(&totalEndTim, NULL);
	cardTests->timeval_diff(&totalDiff, &totalEndTim, &totalStartTim);
    printf("Test in total: %i, successfull tests: %i, error: %i \n", totalTests, okTests, failedTests);
	printf("Test suite took %li.%li seconds\n", totalDiff.tv_sec, (long)totalDiff.tv_usec);
	printf("--------------------------------------------------------------\n");

	delete cardTests;
	if(overalTestResult == true)
	{
		printf("Test result: OK\n");
		return 0;
	}
	else
	{
		printf("Test result: FAILED\n");
		return 1;
	}
}