#include "common.h"
#include <string.h>
#include <cstdlib>
#include <openssl/x509.h>
#include <time.h>

#define ALT_E			0x01
#define ALT_O			0x02
#define	LEGAL_ALT(x)		{ if (alt_format & ~(x)) return (0); }
#define TM_YEAR_BASE   1900

static	int conv_num(const char **, int *, int, int);

static const char *day[7] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
	"Friday", "Saturday"
};
static const char *abday[7] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char *mon[12] = {
	"January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December"
};
static const char *abmon[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const char *am_pm[2] = {
	"AM", "PM"
};

class Certificate
{
public:
	Certificate();
	Certificate(ByteVec cert);

	std::string getCN();
	std::string getC();
	std::string getO();
	std::string getOU();
	std::string getSerialNumber();
	std::string getIssuerCN();
	std::string getOwnerFirstName();
	std::string getOwnerSurName();
	std::string getOwnerPersonalCode();
	time_t getNotValidBefore();
	time_t getNotValidAfter();

	void setCN(std::string CN);
	void setOwnerFirstName(std::string ownerFirstName);
	void setOwnerSurName(std::string ownerSurName);
private:

	
	std::string CN;
	std::string C;
	std::string O;
	std::string OU;
	std::string serialNumber;
	std::string issuerCN;
	std::string ownerFirstName;
	std::string ownerSurName;
	std::string ownerPersonalCode;
	time_t NotBefore;
	time_t NotAfter; 

	time_t ASN1_TIME_TO_TIME_T(ASN1_TIME* time);
	time_t timeFromString(std::string date);
	char * strptime(const char *buf, const char *fmt, struct tm *tm);
	
};