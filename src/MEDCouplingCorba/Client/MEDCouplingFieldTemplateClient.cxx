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
// Author : Anthony Geay (CEA/DEN)

#include "MEDCouplingFieldTemplateClient.hxx"
#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingMesh.hxx"

#include <vector>

using namespace MEDCoupling;

MEDCouplingFieldTemplate *MEDCouplingFieldTemplateClient::New(SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr fieldPtr)
{
  fieldPtr->Register();
  //
  SALOME_TYPES::ListOfLong *tinyL;
  SALOME_TYPES::ListOfDouble *tinyD;
  SALOME_TYPES::ListOfString *tinyS;
  //1st CORBA call : getting all tiny info of all types (int, double string).
  fieldPtr->getTinyInfo(tinyL,tinyD,tinyS);
  int tinyLgth=tinyL->length();
  std::vector<mcIdType> tinyLV(tinyLgth);
  for(int i=0;i<tinyLgth;i++)
    tinyLV[i]=(*tinyL)[i];
  delete tinyL;
  //
  tinyLgth=tinyD->length();
  std::vector<double> tinyLD(tinyLgth);
  for(int i=0;i<tinyLgth;i++)
    tinyLD[i]=(*tinyD)[i];
  delete tinyD;
  //
  tinyLgth=tinyS->length();
  std::vector<std::string> tinyLS(tinyLgth);
  for(int i=0;i<tinyLgth;i++)
    tinyLS[i]=(*tinyS)[i];
  delete tinyS;
  //
  TypeOfField type=(TypeOfField) tinyLV[0];
  MEDCouplingFieldTemplate *ret=MEDCouplingFieldTemplate::New(type);
  //2nd CORBA call to retrieves the mesh.
  SALOME_MED::MEDCouplingMeshCorbaInterface_ptr meshPtr=fieldPtr->getMesh();
  MEDCouplingMesh *mesh=MEDCouplingMeshClient::New(meshPtr);
  meshPtr->UnRegister();
  CORBA::release(meshPtr);
  ret->setMesh(mesh);
  mesh->decrRef();
  DataArrayIdType *array0;
  ret->resizeForUnserialization(tinyLV,array0);
  SALOME_TYPES::ListOfLong *bigArr0;
  //3rd CORBA invocation to get big content
  fieldPtr->getSerialisationData(bigArr0);
  if(bigArr0->length()!=0)
    {
      mcIdType *pt=array0->getPointer();
      mcIdType lgth=array0->getNbOfElems();
      for(mcIdType i=0;i<lgth;i++)
        pt[i]=(*bigArr0)[i];
    }
  delete bigArr0;
  //
  //notify server that the servant is no more used.
  fieldPtr->UnRegister();
  //
  ret->finishUnserialization(tinyLV,tinyLD,tinyLS);
  //
  return ret;
}
