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

#ifndef _smartcard_pp_types_h
#define _smartcard_pp_types_h

#include <vector>
#include <ostream>

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) (void)x
#endif

typedef unsigned int uint;
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned short ushort;
#ifdef _MSC_VER
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
#endif
/// shorthand for std::vector<unsigned char>, used everywhere
typedef	std::vector<byte> ByteVec;
/// ostream printer
std::ostream& operator<<(std::ostream &out,const ByteVec &vec);


#endif
