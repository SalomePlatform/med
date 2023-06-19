// Copyright (C) 2007-2023  CEA, EDF
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
// Author : Anthony Geay (CEA/DEN)

#ifndef __MEDCOUPLINGCMESHSERVANT_HXX__
#define __MEDCOUPLINGCMESHSERVANT_HXX__

#include "SALOMEconfig.h"

#include CORBA_SERVER_HEADER(MEDCouplingCorbaServant)
#include "MEDCouplingStructuredMeshServant.hxx"
#include "MEDCouplingCorba.hxx"

namespace MEDCoupling
{
  class MEDCouplingCMesh;

  class MEDCOUPLINGCORBA_EXPORT MEDCouplingCMeshServant : MEDCouplingStructuredMeshServant, public virtual POA_SALOME_MED::MEDCouplingCMeshCorbaInterface
  {
  public:
    typedef MEDCouplingCMesh CppType;
    MEDCouplingCMeshServant(const MEDCouplingCMesh *cppPointerOfMesh);
    ~MEDCouplingCMeshServant();
  public:
    const MEDCouplingCMesh *getPointer() const { return (const MEDCouplingCMesh *)(_cpp_pointer); }
  };
}

#endif
