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

#include "MEDCouplingFieldServant.hxx"
#include "MEDCouplingMeshServant.hxx"
#include "MEDCouplingUMeshServant.hxx"
#include "MEDCouplingCMeshServant.hxx"
#include "MEDCouplingIMeshServant.hxx"
#include "MEDCouplingCurveLinearMeshServant.hxx"
#include "MEDCouplingExtrudedMeshServant.hxx"
#include "MEDCoupling1SGTUMeshServant.hxx"
#include "MEDCoupling1DGTUMeshServant.hxx"
#include "MEDCouplingField.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingCMesh.hxx"
#include "MEDCouplingIMesh.hxx"
#include "MEDCouplingCurveLinearMesh.hxx"
#include "MEDCouplingMappedExtrudedMesh.hxx"
#include "MEDCoupling1GTUMesh.hxx"

using namespace MEDCoupling;

MEDCouplingFieldServant::MEDCouplingFieldServant(const MEDCouplingField *cppPointerOfMesh):MEDCouplingRefCountServant(cppPointerOfMesh,cppPointerOfMesh)
{
}

SALOME_MED::MEDCouplingMeshCorbaInterface_ptr MEDCouplingFieldServant::BuildCorbaRefFromCppPointer(const MEDCouplingMesh *mesh)
{
  const MEDCouplingUMesh *uMesh=dynamic_cast<const MEDCouplingUMesh *>(mesh);
  if(uMesh)
    {
      MEDCouplingUMeshServant *retServ=new MEDCouplingUMeshServant(uMesh);
      SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=retServ->_this();//let this line even if it seems fool
      return ret;
    }
  const MEDCouplingCMesh *cMesh=dynamic_cast<const MEDCouplingCMesh *>(mesh);
  if(cMesh)
    {
      MEDCouplingCMeshServant *retServ=new MEDCouplingCMeshServant(cMesh);
      SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr ret=retServ->_this();//let this line even if it seems fool
      return ret;
    }
  const MEDCouplingIMesh *iMesh=dynamic_cast<const MEDCouplingIMesh *>(mesh);
  if(iMesh)
    {
      MEDCouplingIMeshServant *retServ=new MEDCouplingIMeshServant(iMesh);
      SALOME_MED::MEDCouplingIMeshCorbaInterface_ptr ret=retServ->_this();//let this line even if it seems fool
      return ret;
    }
  const MEDCouplingMappedExtrudedMesh *eMesh=dynamic_cast<const MEDCouplingMappedExtrudedMesh *>(mesh);
  if(eMesh)
    {
      MEDCouplingExtrudedMeshServant *retServ=new MEDCouplingExtrudedMeshServant(eMesh);
      return retServ->_this();
    }
  const MEDCouplingCurveLinearMesh *clMesh=dynamic_cast<const MEDCouplingCurveLinearMesh *>(mesh);
  if(clMesh)
    {
      MEDCouplingCurveLinearMeshServant *retServ=new MEDCouplingCurveLinearMeshServant(clMesh);
      SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_ptr ret=retServ->_this();//let this line even if it seems fool
      return ret;
    }
  const MEDCoupling1SGTUMesh *u0Mesh=dynamic_cast<const MEDCoupling1SGTUMesh *>(mesh);
  if(u0Mesh)
    {
      MEDCoupling1SGTUMeshServant *retServ=new MEDCoupling1SGTUMeshServant(u0Mesh);
      SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface_ptr ret=retServ->_this();//let this line even if it seems fool
      return ret;
    }
  const MEDCoupling1DGTUMesh *u1Mesh=dynamic_cast<const MEDCoupling1DGTUMesh *>(mesh);
  if(u1Mesh)
    {
      MEDCoupling1DGTUMeshServant *retServ=new MEDCoupling1DGTUMeshServant(u1Mesh);
      SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface_ptr ret=retServ->_this();//let this line even if it seems fool
      return ret;
    }
  throw INTERP_KERNEL::Exception("Not dealt mesh type !");
}

SALOME_MED::MEDCouplingMeshCorbaInterface_ptr MEDCouplingFieldServant::getMesh()
{
  const MEDCouplingMesh *mesh(getPointer()->getMesh());
  return BuildCorbaRefFromCppPointer(mesh);
}
