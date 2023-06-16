// Copyright (C) 2007-2023  CEA/DEN, EDF R&D
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

#ifndef __MEDCOUPLING1GTUMESHSERVANT_HXX__
#define __MEDCOUPLING1GTUMESHSERVANT_HXX__

#include "SALOMEconfig.h"
#ifdef WIN32
#define NOMINMAX
#endif
#include CORBA_SERVER_HEADER(MEDCouplingCorbaServant)
#include "MEDCouplingPointSetServant.hxx"
#include "MEDCouplingCorba.hxx"

namespace MEDCoupling
{
  class MEDCoupling1GTUMesh;

  class MEDCOUPLINGCORBA_EXPORT MEDCoupling1GTUMeshServant : public MEDCouplingPointSetServant, public virtual POA_SALOME_MED::MEDCoupling1GTUMeshCorbaInterface
  {
  public:
    typedef MEDCoupling1GTUMesh CppType;
    MEDCoupling1GTUMeshServant(const MEDCoupling1GTUMesh *cppPointerOfMesh);
  public:
    const MEDCoupling1GTUMesh *getPointer() const { return (const MEDCoupling1GTUMesh *)_cpp_pointer; }
  };
}

#endif
