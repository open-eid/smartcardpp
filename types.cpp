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

#include "common.h"

std::ostream& operator<<(std::ostream &out,const ByteVec &vec) {
  out << "[ ";
  for(ByteVec::const_iterator i = vec.begin();i!=vec.end();i++) 
    out << std::hex << std::setfill('0') << std::setw(2) << (int)*i << " ";
  out << "]";
  return out;
  }
