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

#include "MEDCouplingMeshServant.hxx"
#include "InterpKernelException.hxx"
#include "MEDCouplingMesh.hxx"
#include "MEDCouplingMemArray.hxx"

using namespace MEDCoupling;

MEDCouplingMeshServant::MEDCouplingMeshServant(const MEDCouplingMesh *cppPointerOfMesh):MEDCouplingRefCountServant(cppPointerOfMesh,cppPointerOfMesh)
{
}

char *MEDCouplingMeshServant::getName()
{
  return CORBA::string_dup(getPointer()->getName().c_str());
}

void MEDCouplingMeshServant::getTinyInfo(SALOME_TYPES::ListOfDouble_out da, SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfString_out sa)
{
  da=new SALOME_TYPES::ListOfDouble;
  la=new SALOME_TYPES::ListOfLong;
  std::vector<double> tinyInfoD;
  std::vector<mcIdType> tinyInfoI;
  std::vector<std::string> tinyInfoS;
  getPointer()->getTinySerializationInformation(tinyInfoD,tinyInfoI,tinyInfoS);
  da->length(tinyInfoD.size());
  la->length(tinyInfoI.size());
  for(int i=0;i<(int)tinyInfoD.size();i++)
    (*da)[i]=tinyInfoD[i];
  for(int i=0;i<(int)tinyInfoI.size();i++)
    (*la)[i]=tinyInfoI[i];
  //
  sa=new SALOME_TYPES::ListOfString;
  int nbOfSts=tinyInfoS.size();
  sa->length(nbOfSts);
  for(int i=0;i<nbOfSts;i++)
    (*sa)[i]=CORBA::string_dup(tinyInfoS[i].c_str());
}

void MEDCouplingMeshServant::getSerialisationData(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfDouble_out da)
{
  DataArrayIdType *array1;
  DataArrayDouble *array2;
  getPointer()->serialize(array1,array2);
  la=new SALOME_TYPES::ListOfLong;
  if(array1)
    {
      int lgth=array1->getNbOfElems();
      const mcIdType *data=array1->getConstPointer();
      la->length(lgth);
      for(int i=0;i<lgth;i++)
        (*la)[i]=data[i];
      array1->decrRef();
    }
  //
  da=new SALOME_TYPES::ListOfDouble;
  if(array2)
    {
      int lgth=array2->getNbOfElems();
      const double *data2=array2->getConstPointer();
      da->length(lgth);
      for(int i=0;i<lgth;i++)
        (*da)[i]=data2[i];
      //
      array2->decrRef();
    }
}

CORBA::Boolean MEDCouplingMeshServant::ExportDataAs(const char *format, SALOME::GenericObj_out exporter)
{
  std::string frmCpp(format);
  if(frmCpp=="MEDCorba")
    {
      SALOME_MED::MEDCouplingMeshCorbaInterface_ptr cPtr=_this();
      Register();
      exporter=cPtr;
      return true;
    }
  exporter=SALOME::GenericObj::_nil();
  return false;
}
