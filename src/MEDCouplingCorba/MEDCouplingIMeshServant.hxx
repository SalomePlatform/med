// Copyright (C) 2007-2024  CEA, EDF
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
// Author : Anthony Geay

#ifndef __MEDCOUPLINGIMESHSERVANT_HXX__
#define __MEDCOUPLINGIMESHSERVANT_HXX__

#include "SALOMEconfig.h"

#include CORBA_SERVER_HEADER(MEDCouplingCorbaServant)
#include "MEDCouplingStructuredMeshServant.hxx"
#include "MEDCouplingCorba.hxx"

namespace MEDCoupling
{
  class MEDCouplingIMesh;

  class MEDCOUPLINGCORBA_EXPORT MEDCouplingIMeshServant : MEDCouplingStructuredMeshServant, public virtual POA_SALOME_MED::MEDCouplingIMeshCorbaInterface
  {
  public:
    typedef MEDCouplingIMesh CppType;
    MEDCouplingIMeshServant(const MEDCouplingIMesh *cppPointerOfMesh);
    ~MEDCouplingIMeshServant();
  public:
    const MEDCouplingIMesh *getPointer() const { return (const MEDCouplingIMesh *)(_cpp_pointer); }
  };
}

#endif
