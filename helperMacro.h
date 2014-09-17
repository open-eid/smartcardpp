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

#if !defined(_WINDOWS) && !defined(HELPERMACROS_H)
#define MAKEWORD(a, b)      ((word)(((byte)(((dword)(a)) & 0xff)) | ((word)((byte)(((dword)(b)) & 0xff))) << 8))
#define LOBYTE(w)           ((byte)(((dword)(w)) & 0xff))
#define HIBYTE(w)           ((byte)((((dword)(w)) >> 8) & 0xff))
#endif
#ifndef HELPERMACROS_H
#define HELPERMACROS_H
#define LENOF(a) (sizeof(a)/sizeof(*(a)))
#define MAKEVECTOR(a) ByteVec(a,a + LENOF(a) )
#endif
