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

#include "MEDCouplingMultiFieldsClient.hxx"
#include "MEDCouplingFieldTemplate.hxx"
#include "MEDCouplingMeshClient.hxx"
#include "DataArrayDoubleClient.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingMesh.hxx"

#include <vector>

using namespace MEDCoupling;

MEDCouplingMultiFields *MEDCouplingMultiFieldsClient::New(SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_ptr fieldPtr)
{
  MEDCouplingMultiFields *ret=MEDCouplingMultiFields::New();
  fieldPtr->Register();
  BuildFullMultiFieldsCorbaFetch(ret,fieldPtr);
  //notify server that the servant is no more used.
  fieldPtr->UnRegister();
  return ret;
}

void MEDCouplingMultiFieldsClient::BuildFullMultiFieldsCorbaFetch(MEDCouplingMultiFields *ret,SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_ptr fieldPtr)
{
  //
  SALOME_TYPES::ListOfLong *tinyL=0;
  SALOME_TYPES::ListOfDouble *tinyD=0;
  SALOME_TYPES::ListOfString *tinyS=0;
  //
  CORBA::Long nbOfArrays;
  CORBA::Long nbOfFields;
  CORBA::Long nbOfMeshes=fieldPtr->getMainTinyInfo(tinyL,tinyD,nbOfArrays,nbOfFields);
  int tinyLgth=tinyL->length();
  std::vector<mcIdType> mainI(tinyLgth);
  for(int i=0;i<tinyLgth;i++)
    mainI[i]=(*tinyL)[i];
  delete tinyL; tinyL=0;
  tinyLgth=tinyD->length();
  std::vector<double> mainD(tinyLgth);
  for(int i=0;i<tinyLgth;i++)
    mainD[i]=(*tinyD)[i];
  delete tinyD; tinyD=0;
  //
  std::vector<MEDCouplingMesh *> cppms(nbOfMeshes);
  SALOME_MED::MEDCouplingMeshesCorbaInterface *meshes=fieldPtr->getMeshes();
  for(CORBA::Long i=0;i<nbOfMeshes;i++)
    {
      cppms[i]=MEDCouplingMeshClient::New((*meshes)[i]);
      (*meshes)[i]->UnRegister();
    }
  delete meshes;
  //
  std::vector<DataArrayDouble *> cppdas(nbOfArrays);
  for(CORBA::Long i=0;i<nbOfArrays;i++)
    {
      SALOME_MED::DataArrayDoubleCorbaInterface_ptr daPtr=fieldPtr->getArray(i);
      cppdas[i]=DataArrayDoubleClient::New(daPtr);
      daPtr->UnRegister();
      CORBA::release(daPtr);
    }
  //
  std::vector<MEDCouplingFieldTemplate *> fts(nbOfFields);
  for(CORBA::Long i=0;i<nbOfFields;i++)
    {
      fieldPtr->getTinyInfo(i,tinyL,tinyD,tinyS);
      int tinyLgth2=tinyL->length();
      std::vector<mcIdType> tinyLV(tinyLgth2);
      for(int j=0;j<tinyLgth2;j++)
        tinyLV[j]=(*tinyL)[j];
      delete tinyL; tinyL=0;
      //
      tinyLgth2=tinyD->length();
      std::vector<double> tinyLD(tinyLgth2);
      for(int j=0;j<tinyLgth2;j++)
        tinyLD[j]=(*tinyD)[j];
      delete tinyD; tinyD=0;
      //
      tinyLgth2=tinyS->length();
      std::vector<std::string> tinyLS(tinyLgth2);
      for(int j=0;j<tinyLgth2;j++)
        tinyLS[j]=(*tinyS)[j];
      delete tinyS; tinyS=0;
      TypeOfField type=(TypeOfField) tinyLV[0];
      MEDCouplingFieldTemplate *f1=MEDCouplingFieldTemplate::New(type);
      //
      DataArrayIdType *array0;
      f1->resizeForUnserialization(tinyLV,array0);
      if(array0)
        {
          fieldPtr->getSerialisationData(i,tinyL);
          tinyLgth2=tinyL->length();
          mcIdType *ptr=array0->getPointer();
          for(int j=0;j<tinyLgth2;j++)
            ptr[j]=(*tinyL)[j];
          delete tinyL; tinyL=0;
        }
      f1->finishUnserialization(tinyLV,tinyLD,tinyLS);
      fts[i]=f1;
    }
  //last step
  ret->finishUnserialization(mainI,mainD,fts,cppms,cppdas);
  // clean-up
  for(std::vector<MEDCouplingFieldTemplate *>::iterator it=fts.begin();it!=fts.end();it++)
    (*it)->decrRef();
  for(std::vector<MEDCouplingMesh *>::iterator it=cppms.begin();it!=cppms.end();it++)
    (*it)->decrRef();
  for(std::vector<DataArrayDouble *>::iterator it=cppdas.begin();it!=cppdas.end();it++)
    (*it)->decrRef();
}

