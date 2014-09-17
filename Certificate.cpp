#include "Certificate.h"


Certificate::Certificate()
{
	CN, C, O, OU, serialNumber, issuerCN, ownerFirstName, ownerSurName = "";
	NotBefore, NotAfter = 0;
}

Certificate::Certificate(ByteVec cert)
{
	CN, C, O, OU, serialNumber, issuerCN, ownerFirstName, ownerSurName = "";
	NotBefore, NotAfter = 0;
	char *value = NULL;
	char name[1024], temp[1024];
	X509_CINF *cert_inf = NULL;
	
	memset((void *)name, '\0', sizeof(name));
	memset((void *)temp, '\0', sizeof(temp));

	const unsigned char *p = &cert[0];
	X509 *x509 = d2i_X509(NULL, &p, (long)cert.size());
	X509_name_st *x509Name = X509_get_subject_name(x509);
	cert_inf = x509->cert_info;

	unsigned int count = X509_NAME_entry_count(x509Name);
	for (unsigned int i = 0; i < count; i++)
	{
		X509_NAME_ENTRY *entry = X509_NAME_get_entry(x509Name, i);
		OBJ_obj2txt(name, sizeof(name), entry->object, 0);

		if (strcmp(name, "commonName") == 0)
		{
			ASN1_STRING_to_UTF8((unsigned char **)&value, entry->value);
			this->CN = value;
		}
		else if (strcmp(name, "countryName") == 0)
		{
			ASN1_STRING_to_UTF8((unsigned char **)&value, entry->value);
			this->C = value;
		}
		else if (strcmp(name, "organizationName") == 0)
		{
			ASN1_STRING_to_UTF8((unsigned char **)&value, entry->value);
			this->O = value;
		}
		else if (strcmp(name, "organizationalUnitName") == 0)
		{
			ASN1_STRING_to_UTF8((unsigned char **)&value, entry->value);
			this->OU = value;
		}
		else if (strcmp(name, "serialNumber") == 0)
		{
			ASN1_STRING_to_UTF8((unsigned char **)&value, entry->value);
			this->ownerPersonalCode = value;
		}
		else if (strcmp(name, "surname") == 0)
		{
			ASN1_STRING_to_UTF8((unsigned char **)&value, entry->value);
			this->ownerSurName = value;
		}
		else if (strcmp(name, "givenName") == 0)
		{
			ASN1_STRING_to_UTF8((unsigned char **)&value, entry->value);
			this->ownerFirstName = value;
		}
	}

	X509_name_st *issuerName = X509_get_issuer_name(x509);
	count = X509_NAME_entry_count(issuerName);
	for (unsigned int i = 0; i < count; i++)
	{
		X509_NAME_ENTRY *entry = X509_NAME_get_entry(issuerName, i);
		OBJ_obj2txt(name, sizeof(name), entry->object, 0);
		if (strcmp(name, "commonName") == 0)
		{
			ASN1_STRING_to_UTF8((unsigned char **)&value, entry->value);
			this->issuerCN = value;
		}
	}

	ASN1_TIME *certTime = X509_get_notBefore(x509);
	
	std::string tempTime((const char *)certTime->data);
	this->NotBefore = timeFromString(tempTime);

	certTime = X509_get_notAfter(x509);
	tempTime.clear();
	tempTime.append((const char *)certTime->data);
	this->NotAfter = timeFromString(tempTime);

	ASN1_INTEGER *serialNumber = X509_get_serialNumber(x509);
	BIGNUM *bnser = ASN1_INTEGER_to_BN(serialNumber, NULL);
	BN_num_bytes(bnser);
	unsigned char outbuf[50];
	memset((void *)outbuf, '\0', sizeof(outbuf));
	BN_bn2bin(bnser, outbuf);
	
	
	std::stringstream APDU;
	APDU.clear();

	for (int i = 0; i < serialNumber->length; i++)
	{
		APDU << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (int)outbuf[i];
	}
	
	this->serialNumber.clear();
	this->serialNumber.append(APDU.str().c_str());

	X509_free(x509);
	if (value != NULL)
		OPENSSL_free(value);
}

std::string Certificate::getCN()
{
	return this->CN;
}

std::string Certificate::getC()
{
	return this->C;
}

std::string Certificate::getO()
{
	return this->O;
}

std::string Certificate::getOU()
{
	return this->OU;
}

std::string Certificate::getSerialNumber()
{
	return this->serialNumber;
}

std::string Certificate::getIssuerCN()
{
	return this->issuerCN;
}

std::string Certificate::getOwnerFirstName()
{
	return this->ownerFirstName;
}

std::string Certificate::getOwnerSurName()
{
	return this->ownerSurName;
}

std::string Certificate::getOwnerPersonalCode()
{
	return this->ownerPersonalCode;
}

time_t Certificate::getNotValidBefore()
{
	return this->NotBefore;
}

time_t Certificate::getNotValidAfter()
{
	return this->NotAfter;
}

void Certificate::setCN(std::string CN)
{
	this->CN = CN;
}

void Certificate::setOwnerFirstName(std::string ownerFirstName)
{
	this->ownerFirstName = ownerFirstName;
}

void Certificate::setOwnerSurName(std::string ownerSurName)
{
	this->ownerSurName = ownerSurName;
}

time_t Certificate::ASN1_TIME_TO_TIME_T(ASN1_TIME* time)
{
	struct tm t;
	const char* str = (const char*)time->data;
	size_t i = 0;

	memset(&t, 0, sizeof(t));

	if (time->type == V_ASN1_UTCTIME) /* two digit year */
	{
		t.tm_year = (str[i++] - '0') * 10 + (str[++i] - '0');
		if (t.tm_year < 70)
			t.tm_year += 100;
	}
	else if (time->type == V_ASN1_GENERALIZEDTIME) /* four digit year */
	{
		t.tm_year = (str[i++] - '0') * 1000 + (str[++i] - '0') * 100 + (str[++i] - '0') * 10 + (str[++i] - '0');
		t.tm_year -= 1900;
	}
	t.tm_mon = ((str[i++] - '0') * 10 + (str[++i] - '0')) - 1; // -1 since January is 0 not 1.
	t.tm_mday = (str[i++] - '0') * 10 + (str[++i] - '0');
	t.tm_hour = (str[i++] - '0') * 10 + (str[++i] - '0');
	t.tm_min = (str[i++] - '0') * 10 + (str[++i] - '0');
	t.tm_sec = (str[i++] - '0') * 10 + (str[++i] - '0');

	/* Note: we did not adjust the time based on time zone information */
	return mktime(&t);
}

time_t Certificate::timeFromString(std::string date)
{
	struct tm tm_struct = { 0 };
	time_t _time = time(0);

	tm_struct.tm_hour = 23;
	tm_struct.tm_min = 59;
	tm_struct.tm_sec = 59;

	if (strptime(date.c_str(), "%y%m%d%H%M%S", &tm_struct) != NULL)
	{
		_time = mktime(&tm_struct);
		if (tm_struct.tm_isdst)
		{
			_time -= 3600;
		}
	}


	return _time;
}

char * Certificate::strptime(const char *buf, const char *fmt, struct tm *tm)
{
	char c;
	const char *bp;
	size_t len = 0;
	int alt_format, i, split_year = 0;

	bp = buf;

	while ((c = *fmt) != '\0') {
		/* Clear `alternate' modifier prior to new conversion. */
		alt_format = 0;

		/* Eat up white-space. */
		if (isspace(c)) {
			while (isspace(*bp))
				bp++;

			fmt++;
			continue;
		}

		if ((c = *fmt++) != '%')
			goto literal;


	again:		switch (c = *fmt++) {
	case '%':	/* "%%" is converted to "%". */
		literal :
			if (c != *bp++)
				return (0);
		break;

		/*
		* "Alternative" modifiers. Just set the appropriate flag
		* and start over again.
		*/
	case 'E':	/* "%E?" alternative conversion modifier. */
		LEGAL_ALT(0);
		alt_format |= ALT_E;
		goto again;

	case 'O':	/* "%O?" alternative conversion modifier. */
		LEGAL_ALT(0);
		alt_format |= ALT_O;
		goto again;

		/*
		* "Complex" conversion rules, implemented through recursion.
		*/
	case 'c':	/* Date and time, using the locale's format. */
		LEGAL_ALT(ALT_E);
		if (!(bp = strptime(bp, "%x %X", tm)))
			return (0);
		break;

	case 'D':	/* The date as "%m/%d/%y". */
		LEGAL_ALT(0);
		if (!(bp = strptime(bp, "%m/%d/%y", tm)))
			return (0);
		break;

	case 'R':	/* The time as "%H:%M". */
		LEGAL_ALT(0);
		if (!(bp = strptime(bp, "%H:%M", tm)))
			return (0);
		break;

	case 'r':	/* The time in 12-hour clock representation. */
		LEGAL_ALT(0);
		if (!(bp = strptime(bp, "%I:%M:%S %p", tm)))
			return (0);
		break;

	case 'T':	/* The time as "%H:%M:%S". */
		LEGAL_ALT(0);
		if (!(bp = strptime(bp, "%H:%M:%S", tm)))
			return (0);
		break;

	case 'X':	/* The time, using the locale's format. */
		LEGAL_ALT(ALT_E);
		if (!(bp = strptime(bp, "%H:%M:%S", tm)))
			return (0);
		break;

	case 'x':	/* The date, using the locale's format. */
		LEGAL_ALT(ALT_E);
		if (!(bp = strptime(bp, "%m/%d/%y", tm)))
			return (0);
		break;

		/*
		* "Elementary" conversion rules.
		*/
	case 'A':	/* The day of week, using the locale's form. */
	case 'a':
		LEGAL_ALT(0);
		for (i = 0; i < 7; i++) {
			/* Full name. */
			len = strlen(day[i]);
			//if (strncasecmp(day[i], bp, len) == 0)
			if (strncmp(day[i], bp, len) == 0)
				break;

			/* Abbreviated name. */
			len = strlen(abday[i]);
			//if (strncasecmp(abday[i], bp, len) == 0)
			if (strncmp(abday[i], bp, len) == 0)
				break;
		}

		/* Nothing matched. */
		if (i == 7)
			return (0);

		tm->tm_wday = i;
		bp += len;
		break;

	case 'B':	/* The month, using the locale's form. */
	case 'b':
	case 'h':
		LEGAL_ALT(0);
		for (i = 0; i < 12; i++) {
			/* Full name. */
			len = strlen(mon[i]);
			//if (strncasecmp(mon[i], bp, len) == 0)
			if (strncmp(mon[i], bp, len) == 0)
				break;

			/* Abbreviated name. */
			len = strlen(abmon[i]);
			//if (strncasecmp(abmon[i], bp, len) == 0)
			if (strncmp(abmon[i], bp, len) == 0)
				break;
		}

		/* Nothing matched. */
		if (i == 12)
			return (0);

		tm->tm_mon = i;
		bp += len;
		break;

	case 'C':	/* The century number. */
		LEGAL_ALT(ALT_E);
		if (!(conv_num(&bp, &i, 0, 99)))
			return (0);

		if (split_year) {
			tm->tm_year = (tm->tm_year % 100) + (i * 100);
		}
		else {
			tm->tm_year = i * 100;
			split_year = 1;
		}
		break;

	case 'd':	/* The day of month. */
	case 'e':
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
			return (0);
		break;

	case 'k':	/* The hour (24-hour clock representation). */
		LEGAL_ALT(0);
		/* FALLTHROUGH */
	case 'H':
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
			return (0);
		break;

	case 'l':	/* The hour (12-hour clock representation). */
		LEGAL_ALT(0);
		/* FALLTHROUGH */
	case 'I':
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
			return (0);
		if (tm->tm_hour == 12)
			tm->tm_hour = 0;
		break;

	case 'j':	/* The day of year. */
		LEGAL_ALT(0);
		if (!(conv_num(&bp, &i, 1, 366)))
			return (0);
		tm->tm_yday = i - 1;
		break;

	case 'M':	/* The minute. */
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
			return (0);
		break;

	case 'm':	/* The month. */
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &i, 1, 12)))
			return (0);
		tm->tm_mon = i - 1;
		break;

	case 'p':	/* The locale's equivalent of AM/PM. */
		LEGAL_ALT(0);
		/* AM? */
		//if (strcasecmp(am_pm[0], bp) == 0) {
		if (strcmp(am_pm[0], bp) == 0) {
			if (tm->tm_hour > 11)
				return (0);

			bp += strlen(am_pm[0]);
			break;
		}
		/* PM? */
		//else if (strcasecmp(am_pm[1], bp) == 0) {
		else if (strcmp(am_pm[1], bp) == 0) {
			if (tm->tm_hour > 11)
				return (0);

			tm->tm_hour += 12;
			bp += strlen(am_pm[1]);
			break;
		}

		/* Nothing matched. */
		return (0);

	case 'S':	/* The seconds. */
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
			return (0);
		break;

	case 'U':	/* The week of year, beginning on sunday. */
	case 'W':	/* The week of year, beginning on monday. */
		LEGAL_ALT(ALT_O);
		/*
		* XXX This is bogus, as we can not assume any valid
		* information present in the tm structure at this
		* point to calculate a real value, so just check the
		* range for now.
		*/
		if (!(conv_num(&bp, &i, 0, 53)))
			return (0);
		break;

	case 'w':	/* The day of week, beginning on sunday. */
		LEGAL_ALT(ALT_O);
		if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
			return (0);
		break;

	case 'Y':	/* The year. */
		LEGAL_ALT(ALT_E);
		if (!(conv_num(&bp, &i, 0, 9999)))
			return (0);

		tm->tm_year = i - TM_YEAR_BASE;
		break;

	case 'y':	/* The year within 100 years of the epoch. */
		LEGAL_ALT(ALT_E | ALT_O);
		if (!(conv_num(&bp, &i, 0, 99)))
			return (0);

		if (split_year) {
			tm->tm_year = ((tm->tm_year / 100) * 100) + i;
			break;
		}
		split_year = 1;
		if (i <= 68)
			tm->tm_year = i + 2000 - TM_YEAR_BASE;
		else
			tm->tm_year = i + 1900 - TM_YEAR_BASE;
		break;

		/*
		* Miscellaneous conversions.
		*/
	case 'n':	/* Any kind of white-space. */
	case 't':
		LEGAL_ALT(0);
		while (isspace(*bp))
			bp++;
		break;


	default:	/* Unknown/unsupported conversion. */
		return (0);
	}


	}

	/* LINTED functional specification */
	return ((char *)bp);
}

static int
conv_num(const char **buf, int *dest, int llim, int ulim)
{
	int result = 0;

	/* The limit also determines the number of valid digits. */
	int rulim = ulim;

	if (**buf < '0' || **buf > '9')
		return (0);

	do {
		result *= 10;
		result += *(*buf)++ - '0';
		rulim /= 10;
	} while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

	if (result < llim || result > ulim)
		return (0);

	*dest = result;
	return (1);
}


