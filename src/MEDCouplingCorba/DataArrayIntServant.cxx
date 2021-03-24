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
// Author : Anthony Geay (CEA/DEN)

#include "DataArrayIntServant.hxx"
#include "MEDCouplingMemArray.hxx"

using namespace MEDCoupling;

DataArrayIntServant::DataArrayIntServant(const DataArrayInt *cppPointer):DataArrayServant(cppPointer)
{
}

void DataArrayIntServant::getTinyInfo(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfString_out sa)
{
  la=new SALOME_TYPES::ListOfLong;
  std::vector<mcIdType> tinyInfo;
  getPointer()->getTinySerializationIntInformation(tinyInfo);
  la->length(tinyInfo.size());
  for(int i=0;i<(int)tinyInfo.size();i++)
    (*la)[i]=tinyInfo[i];
  //
  sa=new SALOME_TYPES::ListOfString;
  std::vector<std::string> tinyInfo3;
  getPointer()->getTinySerializationStrInformation(tinyInfo3);
  sa->length(tinyInfo3.size());
  for(int i=0;i<(int)tinyInfo3.size();i++)
    (*sa)[i]=CORBA::string_dup(tinyInfo3[i].c_str());
}

void DataArrayIntServant::getSerialisationData(SALOME_TYPES::ListOfLong_out da)
{
  da=new SALOME_TYPES::ListOfLong;
  if(getPointer()->isAllocated())
    {
      int nbElem=getPointer()->getNbOfElems();
      const int *data=getPointer()->getConstPointer();
      da->length(nbElem);
      for(int i=0;i<nbElem;i++)
        (*da)[i]=data[i];
    }
  else
    {
      da->length(0);
    }
}

SALOME::StringSeq *DataArrayIntServant::GetExportableFormats()
{
  SALOME::StringSeq *ret=new SALOME::StringSeq;
  ret->length(0);
  return ret;
}

CORBA::Boolean DataArrayIntServant::ExportDataAs(const char * /*format*/, SALOME::GenericObj_out exporter)
{
  exporter=SALOME::GenericObj::_nil();
  return false;
}
