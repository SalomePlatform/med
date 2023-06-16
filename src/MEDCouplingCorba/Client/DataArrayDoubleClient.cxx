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

#include "DataArrayDoubleClient.hxx"
#include "MEDCouplingMemArray.hxx"

#include <vector>

using namespace MEDCoupling;

DataArrayDouble *DataArrayDoubleClient::New(SALOME_MED::DataArrayDoubleCorbaInterface_ptr dadPtr)
{
  SALOME_TYPES::ListOfLong *tinyL=0;
  SALOME_TYPES::ListOfDouble *tinyD=0;
  SALOME_TYPES::ListOfString *tinyS=0;
  //
  dadPtr->Register();
  dadPtr->getTinyInfo(tinyL,tinyS);
  DataArrayDouble *ret=DataArrayDouble::New();
  int tinyLgth=tinyL->length();
  std::vector<mcIdType> v1(tinyLgth);
  for(int j=0;j<tinyLgth;j++)
    v1[j]=(*tinyL)[j];
  delete tinyL; tinyL=0;
  //
  tinyLgth=tinyS->length();
  std::vector<std::string> v3(tinyLgth);
  for(int j=0;j<tinyLgth;j++)
    v3[j]=(*tinyS)[j];
  delete tinyS; tinyS=0;
  if(ret->resizeForUnserialization(v1))
    {
      double *pt=ret->getPointer();
      dadPtr->getSerialisationData(tinyD);
      int lgth=tinyD->length();
      for(int j=0;j<lgth;j++)
        pt[j]=(*tinyD)[j];
      delete tinyD; tinyD=0;
    }
  dadPtr->UnRegister();
  ret->finishUnserialization(v1,v3);
  return ret;
}
