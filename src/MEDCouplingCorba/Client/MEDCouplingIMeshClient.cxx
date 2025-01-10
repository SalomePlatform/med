// Copyright (C) 2007-2025  CEA, EDF
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

#include "MEDCouplingIMeshClient.hxx"
#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingIMesh.hxx"
#include "MEDCouplingMemArray.hxx"

#include <vector>

using namespace MEDCoupling;

MEDCouplingIMesh *MEDCouplingIMeshClient::New(SALOME_MED::MEDCouplingIMeshCorbaInterface_ptr meshPtr)
{
  MEDCouplingIMesh *ret=MEDCouplingIMesh::New();
  MEDCouplingMeshClient::fillMeshFromCorbaData(ret,meshPtr);
  return ret;
}
