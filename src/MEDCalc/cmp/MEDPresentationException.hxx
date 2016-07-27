// Copyright (C) 2011-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef SRC_MEDCALC_CMP_MEDPRESENTATION_EXCEPTION_HXX_
#define SRC_MEDCALC_CMP_MEDPRESENTATION_EXCEPTION_HXX_

#include "MEDCALC.hxx"
#include <iostream>
#include <sstream>
#include <exception>

class MEDCALC_EXPORT MEDPresentationException
  : public std::exception
{
public:

  MEDPresentationException(const char* msg) {
    std::ostringstream oss;
    oss << "Error: " << msg;
    this->_msg = oss.str();
  }

  MEDPresentationException(const std::string& msg) {
      std::ostringstream oss;
      oss << "Error: " << msg;
      this->_msg = oss.str();
    }

  virtual ~MEDPresentationException() throw() {}

  virtual const char* what() const throw() {
    return this->_msg.c_str();
  }

private:

  std::string _msg;

};

#endif /* SRC_MEDCALC_CMP_MEDPRESENTATION_EXCEPTION_HXX_ */
