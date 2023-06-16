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

#include "MEDCouplingFieldTemplateServant.hxx"
#include "MEDCouplingFieldTemplate.hxx"

#include "MEDCouplingMemArray.hxx"

using namespace MEDCoupling;

MEDCouplingFieldTemplateServant::MEDCouplingFieldTemplateServant(const MEDCouplingFieldTemplate *cppPointerOfMesh):MEDCouplingFieldServant(cppPointerOfMesh)
{
}

void MEDCouplingFieldTemplateServant::getTinyInfo(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfDouble_out da, SALOME_TYPES::ListOfString_out sa)
{
  la=new SALOME_TYPES::ListOfLong;
  std::vector<mcIdType> tinyInfo;
  getPointer()->getTinySerializationIntInformation(tinyInfo);
  la->length(tinyInfo.size());
  for(int i=0;i<(int)tinyInfo.size();i++)
    (*la)[i]=tinyInfo[i];
  //
  da=new SALOME_TYPES::ListOfDouble;
  std::vector<double> tinyInfo2;
  getPointer()->getTinySerializationDbleInformation(tinyInfo2);
  da->length(tinyInfo2.size());
  for(int i=0;i<(int)tinyInfo2.size();i++)
    (*da)[i]=tinyInfo2[i];
  //
  sa=new SALOME_TYPES::ListOfString;
  std::vector<std::string> tinyInfo3;
  getPointer()->getTinySerializationStrInformation(tinyInfo3);
  sa->length(tinyInfo3.size());
  for(int i=0;i<(int)tinyInfo3.size();i++)
    (*sa)[i]=CORBA::string_dup(tinyInfo3[i].c_str());
}

void MEDCouplingFieldTemplateServant::getSerialisationData(SALOME_TYPES::ListOfLong_out la)
{
  DataArrayIdType *dataInt;
  getPointer()->serialize(dataInt);
  //
  la=new SALOME_TYPES::ListOfLong;
  if(dataInt)
    {
      mcIdType lgth=dataInt->getNbOfElems();
      const mcIdType *ptr=dataInt->getConstPointer();
      la->length(lgth);
      for(mcIdType i=0;i<lgth;i++)
        (*la)[i]=ptr[i];
    }
  else
    la->length(0);
}

CORBA::Boolean MEDCouplingFieldTemplateServant::ExportDataAs(const char *format, SALOME::GenericObj_out exporter)
{
  std::string frmCpp(format);
  if(frmCpp=="MEDCorba")
    {
      exporter=getMesh();
      return true;
    }
  return false;
}
