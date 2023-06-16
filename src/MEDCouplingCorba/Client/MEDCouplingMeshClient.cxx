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

#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCoupling1SGTUMeshClient.hxx"
#include "MEDCoupling1DGTUMeshClient.hxx"
#include "MEDCouplingExtrudedMeshClient.hxx"
#include "MEDCouplingMappedExtrudedMesh.hxx"
#include "MEDCouplingCMeshClient.hxx"
#include "MEDCouplingCurveLinearMeshClient.hxx"
#include "MEDCouplingIMeshClient.hxx"
#include "MEDCouplingCMesh.hxx"
#include "MEDCouplingCurveLinearMesh.hxx"
#include "MEDCouplingIMesh.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCoupling1GTUMesh.hxx"

#include <vector>

using namespace MEDCoupling;

MEDCouplingMesh *MEDCouplingMeshClient::New(SALOME_MED::MEDCouplingMeshCorbaInterface_ptr meshPtr)
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr umeshPtr=SALOME_MED::MEDCouplingUMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(umeshPtr))
    {
      MEDCouplingMesh *ret=MEDCouplingUMeshClient::New(umeshPtr);
      CORBA::release(umeshPtr);
      return ret;
    }
  SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_ptr emeshPtr=SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(emeshPtr))
    {
      MEDCouplingMesh *ret=MEDCouplingExtrudedMeshClient::New(emeshPtr);
      CORBA::release(emeshPtr);
      return ret;
    }
  SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr cmeshPtr=SALOME_MED::MEDCouplingCMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(cmeshPtr))
    {
      MEDCouplingMesh *ret=MEDCouplingCMeshClient::New(cmeshPtr);
      CORBA::release(cmeshPtr);
      return ret;
    }
  SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_ptr clmeshPtr=SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(clmeshPtr))
    {
      MEDCouplingMesh *ret=MEDCouplingCurveLinearMeshClient::New(clmeshPtr);
      CORBA::release(clmeshPtr);
      return ret;
    }
  SALOME_MED::MEDCouplingIMeshCorbaInterface_ptr imeshPtr=SALOME_MED::MEDCouplingIMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(imeshPtr))
    {
      MEDCouplingMesh *ret=MEDCouplingIMeshClient::New(imeshPtr);
      CORBA::release(imeshPtr);
      return ret;
    }
  SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface_ptr umeshPtr0=SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(umeshPtr0))
    {
      MEDCouplingMesh *ret=MEDCoupling1SGTUMeshClient::New(umeshPtr0);
      CORBA::release(umeshPtr0);
      return ret;
    }
  SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface_ptr umeshPtr1=SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(umeshPtr1))
    {
      MEDCouplingMesh *ret=MEDCoupling1DGTUMeshClient::New(umeshPtr1);
      CORBA::release(umeshPtr1);
      return ret;
    }
  return 0;
}

void MEDCouplingMeshClient::fillMeshFromCorbaData(MEDCouplingMesh *meshCpp, SALOME_MED::MEDCouplingMeshCorbaInterface_ptr meshPtr)
{
  meshPtr->Register();
  //1st call to getTinyInfo to get tiny array of key integers value
  //to correctly resize local copy of distant instance addressed by 'meshPtr'
  //1st value of returned array is the type of instance. Thanks to
  //CORBA and its type-check no use of this value is necessary.
  SALOME_TYPES::ListOfDouble *tinyD;
  SALOME_TYPES::ListOfLong *tinyI;
  SALOME_TYPES::ListOfString *tinyS;
  meshPtr->getTinyInfo(tinyD,tinyI,tinyS);
  int tinyLgth=tinyI->length();
  std::vector<mcIdType> tinyV(tinyLgth);
  for(int i=0;i<tinyLgth;i++)
    tinyV[i]=(*tinyI)[i];
  int tinyLgth2=tinyD->length();
  std::vector<double> tinyV2(tinyLgth2);
  for(int i=0;i<tinyLgth2;i++)
    tinyV2[i]=(*tinyD)[i];
  std::vector<std::string> sts(tinyS->length());
  for(int i=0;i<(int)sts.size();i++)
    sts[i]=(*tinyS)[i];
  delete tinyS;
  delete tinyI;
  delete tinyD;
  DataArrayIdType* a1=DataArrayIdType::New();
  DataArrayDouble* a2=DataArrayDouble::New();
  //thanks to the entry point tinyV get from the 1st CORBA invocation,
  //resizing a1,a2 and sts.
  std::vector<std::string> uselessVector;
  //vector 'uselessVector' is useless thanks to CORBA that , contrary to MPI, does not need to allocate right length of arrays before invocation
  meshCpp->resizeForUnserialization(tinyV,a1,a2,uselessVector);
  SALOME_TYPES::ListOfLong *a1Corba;
  SALOME_TYPES::ListOfDouble *a2Corba;
  meshPtr->getSerialisationData(a1Corba,a2Corba);
  int myLgth=a1Corba->length();
  mcIdType *ptToFill=a1->getPointer();
  for(int i=0;i<myLgth;i++)
    ptToFill[i]=(*a1Corba)[i];
  delete a1Corba;
  myLgth=a2Corba->length();
  double *ptToFill2=a2->getPointer();
  for(int i=0;i<myLgth;i++)
    ptToFill2[i]=(*a2Corba)[i];
  delete a2Corba;
  //
  meshCpp->unserialization(tinyV2,tinyV,a1,a2,sts);
  a1->decrRef();
  a2->decrRef();
  //
  meshPtr->UnRegister();
}
