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

#ifndef __MEDCOUPLINGUMESHSERVANT_HXX__
#define __MEDCOUPLINGUMESHSERVANT_HXX__

#include "SALOMEconfig.h"
#ifdef WIN32
#define NOMINMAX
#endif
#include CORBA_SERVER_HEADER(MEDCouplingCorbaServant)
#include "MEDCouplingPointSetServant.hxx"
#include "MEDCouplingCorba.hxx"

namespace MEDCoupling
{
  class MEDCouplingUMesh;

  class MEDCOUPLINGCORBA_EXPORT MEDCouplingUMeshServant : public MEDCouplingPointSetServant, public virtual POA_SALOME_MED::MEDCouplingUMeshCorbaInterface
  {
  public:
    typedef MEDCouplingUMesh CppType;
    MEDCouplingUMeshServant(const MEDCouplingUMesh *cppPointerOfMesh);
  public:
    const MEDCouplingUMesh *getPointer() const { return (const MEDCouplingUMesh *)_cpp_pointer; }
  };
}

#endif
