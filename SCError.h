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
#ifndef SCERROR_H
#define SCERROR_H

#include "SCardLog.h"

#define ERROR_NO_MEDIA_IN_DRIVE          1112L

/// Exception class for smartcard subsystem errors
/** Smartcard subsystem errors, like reader busy etc. Currently these are only
 thrown for PCSCManager, but CTAPI should derive its own from here and throw them
 as well */
class SCError : public std::runtime_error
{
private:
    SCardLog scardlog;
	const SCError operator=(const SCError &);
protected:
	std::string desc;
public:
	const long error; //SC Api returns longs
	SCError(long err);
	virtual ~SCError() throw() {};
	virtual const char * what() const throw() { return desc.c_str();} 
	static void check(long err, int cid, int tid);
};

class NoCardInReaderError: public std::runtime_error
{	
public:
    NoCardInReaderError() : std::runtime_error("No card in specified reader"){}
};

class CardResetError: public std::runtime_error
{
public:
    CardResetError() : std::runtime_error("The specified reader is not currently available for use."){}
};

class NoReadersAvailible: public std::runtime_error
{
public:
    NoReadersAvailible() : std::runtime_error("Cannot find a smart card reader."){}
};

class PCSCManagerFailure: public std::runtime_error
{
public:
    PCSCManagerFailure() : std::runtime_error("PCSCManager failure"){}
};

class UnsupportedCardHashCombination: public std::runtime_error
{
public:
    UnsupportedCardHashCombination() : std::runtime_error("Card does not support provided hash type"){}
};

#endif
