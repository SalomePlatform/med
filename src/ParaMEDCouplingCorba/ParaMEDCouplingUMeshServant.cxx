// Copyright (C) 2007-2021  CEA/DEN, EDF R&D
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

#include "ParaMEDCouplingUMeshServant.hxx"
#include "utilities.h"
using namespace MEDCoupling;

ParaMEDCouplingUMeshServant::ParaMEDCouplingUMeshServant(CORBA::ORB_ptr orb,MEDCouplingUMesh* mesh):MEDCouplingUMeshServant(mesh),MPIObject_i()
{
  Engines::MPIObject_var pobj = POA_SALOME_MED::ParaMEDCouplingUMeshCorbaInterface::_this();
  BCastIOR(orb,pobj,false);
}

void ParaMEDCouplingUMeshServant::Register()
{
  if(_numproc == 0)
    for(int ip=1;ip<_nbproc;ip++)
      {
        SALOME_MED::ParaMEDCouplingUMeshCorbaInterface_var meshPtr=SALOME_MED::ParaMEDCouplingUMeshCorbaInterface::_narrow((*_tior)[ip]);
        meshPtr->Register();
      }
  MEDCouplingUMeshServant::Register();
}

void ParaMEDCouplingUMeshServant::UnRegister()
{
  if(_numproc == 0)
    for(int ip=1;ip<_nbproc;ip++)
      {
        SALOME_MED::ParaMEDCouplingUMeshCorbaInterface_var meshPtr=SALOME_MED::ParaMEDCouplingUMeshCorbaInterface::_narrow((*_tior)[ip]);
        meshPtr->UnRegister();
      }
  MEDCouplingUMeshServant::UnRegister();
}
