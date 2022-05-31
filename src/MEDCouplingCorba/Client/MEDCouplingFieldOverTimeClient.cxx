// Copyright (C) 2007-2022  CEA/DEN, EDF R&D
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

#include "MEDCouplingFieldOverTimeClient.hxx"
#include "MEDCouplingMultiFieldsClient.hxx"
#include "MEDCouplingFieldTemplate.hxx"
#include "MEDCouplingDefinitionTime.hxx"
#include "MEDCouplingMeshClient.hxx"
#include "DataArrayDoubleClient.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingMesh.hxx"

#include <vector>

using namespace MEDCoupling;

MEDCouplingFieldOverTimeClient::MEDCouplingFieldOverTimeClient(SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_ptr field):_df(0)
{
  _field_ptr=SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface::_duplicate(field);
  _field_ptr->Register();
}

MEDCouplingFieldOverTimeClient::~MEDCouplingFieldOverTimeClient()
{
  _field_ptr->UnRegister();
  delete _df;
}

MEDCouplingFieldOverTimeClient *MEDCouplingFieldOverTimeClient::New(SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_ptr fieldPtr)
{
  MEDCouplingFieldOverTimeClient *ret=new MEDCouplingFieldOverTimeClient(fieldPtr);
  MEDCouplingMultiFieldsClient::BuildFullMultiFieldsCorbaFetch(ret,fieldPtr);
  return ret;
}

void MEDCouplingFieldOverTimeClient::retrieveFieldTemplates()
{
  SALOME_TYPES::ListOfLong *tinyL=0;
  SALOME_TYPES::ListOfDouble *tinyD=0;
  SALOME_TYPES::ListOfString *tinyS=0;
  //
  CORBA::Long nbOfArrays;
  CORBA::Long nbOfFields;
  CORBA::Long nbOfMeshes=_field_ptr->getMainTinyInfo(tinyL,tinyD,nbOfArrays,nbOfFields);
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
  //
  std::vector<DataArrayDouble *> cppdas(nbOfArrays);
  //
  std::vector<MEDCouplingFieldTemplate *> fts(nbOfFields);
  for(CORBA::Long i=0;i<nbOfFields;i++)
    {
      _field_ptr->getTinyInfo(i,tinyL,tinyD,tinyS);
      int tinyLgth=tinyL->length();
      std::vector<mcIdType> tinyLV(tinyLgth);
      for(int j=0;j<tinyLgth;j++)
        tinyLV[j]=(*tinyL)[j];
      delete tinyL; tinyL=0;
      //
      tinyLgth=tinyD->length();
      std::vector<double> tinyLD(tinyLgth);
      for(int j=0;j<tinyLgth;j++)
        tinyLD[j]=(*tinyD)[j];
      delete tinyD; tinyD=0;
      //
      tinyLgth=tinyS->length();
      std::vector<std::string> tinyLS(tinyLgth);
      for(int j=0;j<tinyLgth;j++)
        tinyLS[j]=(*tinyS)[j];
      delete tinyS; tinyS=0;
      TypeOfField type=(TypeOfField) tinyLV[0];
      MEDCouplingFieldTemplate *f1=MEDCouplingFieldTemplate::New(type);
      //
      DataArrayIdType *array0;
      f1->resizeForUnserialization(tinyLV,array0);
      if(array0)
        {
          _field_ptr->getSerialisationData(i,tinyL);
          tinyLgth=tinyL->length();
          mcIdType *ptr=array0->getPointer();
          for(int j=0;j<tinyLgth;j++)
            ptr[j]=(*tinyL)[j];
          delete tinyL; tinyL=0;
        }
      f1->finishUnserialization(tinyLV,tinyLD,tinyLS);
      fts[i]=f1;
    }
  //last step
  finishUnserialization(mainI,mainD,fts,cppms,cppdas);
  // clean-up
  for(std::vector<MEDCouplingFieldTemplate *>::iterator it=fts.begin();it!=fts.end();it++)
    (*it)->decrRef();
}

std::vector<double> MEDCouplingFieldOverTimeClient::getTimeSteps() const
{
  fetchDefTimeIfNeeded();
  return _df->getHotSpotsTime();
}

void MEDCouplingFieldOverTimeClient::fetchDefTimeIfNeeded() const
{
  if(!_df)
    {
      SALOME_TYPES::ListOfLong *tinyL=0;
      SALOME_TYPES::ListOfDouble *tinyD=0;
      _field_ptr->getTinyInfoAboutTimeDefinition(tinyL,tinyD);
      int tinyLgth=tinyL->length();
      std::vector<int> mainI(tinyLgth);
      for(int i=0;i<tinyLgth;i++)
        mainI[i]=(*tinyL)[i];
      delete tinyL; tinyL=0;
      tinyLgth=tinyD->length();
      std::vector<double> mainD(tinyLgth);
      for(int i=0;i<tinyLgth;i++)
        mainD[i]=(*tinyD)[i];
      delete tinyD; tinyD=0;
      //
      _df=new MEDCouplingDefinitionTime;
      _df->unserialize(mainI,mainD);
    }
}
