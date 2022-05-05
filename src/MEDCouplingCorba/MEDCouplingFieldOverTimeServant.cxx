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

#include "MEDCouplingFieldOverTimeServant.hxx"
#include "DataArrayDoubleServant.hxx"
#include "MEDCouplingFieldServant.hxx"
#include "MEDCouplingMultiFields.hxx"
#include "MEDCouplingFieldTemplate.hxx"
#include "MEDCouplingMemArray.hxx"

using namespace MEDCoupling;

MEDCouplingFieldOverTimeServant::MEDCouplingFieldOverTimeServant(const MEDCouplingFieldOverTime *cppPointer):MEDCouplingMultiFieldsServant(cppPointer)
{
}

void MEDCouplingFieldOverTimeServant::getTinyInfoAboutTimeDefinition(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfDouble_out da)
{
  MEDCouplingDefinitionTime def=getPointer()->getDefinitionTimeZone();
  std::vector<int> tmp1;
  std::vector<double> tmp2;
  def.getTinySerializationInformation(tmp1,tmp2);
  la=new SALOME_TYPES::ListOfLong;
  la->length(tmp1.size());
  for(int i=0;i<(int)tmp1.size();i++)
    (*la)[i]=tmp1[i];
  da=new SALOME_TYPES::ListOfDouble;
  da->length(tmp2.size());
  for(int i=0;i<(int)tmp2.size();i++)
    (*da)[i]=tmp2[i];
}
