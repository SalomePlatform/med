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

#ifndef __MEDCOUPLINGMULTIFIELDSSERVANT_HXX__
#define __MEDCOUPLINGMULTIFIELDSSERVANT_HXX__

#include "SALOMEconfig.h"
#ifdef WIN32
#define NOMINMAX
#endif
#include CORBA_SERVER_HEADER(MEDCouplingCorbaServant)
#include "MEDCouplingCorba.hxx"
#include "MEDCouplingFieldServant.hxx"
#include "MEDCouplingMeshServant.hxx"

#include <vector>

namespace MEDCoupling
{
  class RefCountObject;
  class MEDCouplingMultiFields;

  class MEDCOUPLINGCORBA_EXPORT MEDCouplingMultiFieldsServant : public MEDCouplingRefCountServant,
                                                                public virtual POA_SALOME_MED::MEDCouplingMultiFieldsCorbaInterface
  {
  public:
    typedef MEDCouplingMultiFields CppType;
    MEDCouplingMultiFieldsServant(const MEDCouplingMultiFields *cppPointer);
  public:
    const MEDCouplingMultiFields *getPointer() const { return (const MEDCouplingMultiFields *)(_cpp_pointer); }
  protected:
    char *getName();
    SALOME_TYPES::ListOfString *getInfoOnComponents();
    CORBA::Long getMainTinyInfo(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfDouble_out da, CORBA::Long& nbOfArrays, CORBA::Long& nbOfFields);
    // for field templates
    void getTinyInfo(CORBA::Long id, SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfDouble_out da, SALOME_TYPES::ListOfString_out sa);
    void getSerialisationData(CORBA::Long id, SALOME_TYPES::ListOfLong_out la);
    // for arrays
    SALOME_MED::DataArrayDoubleCorbaInterface_ptr getArray(CORBA::Long id);
    // for meshes
    SALOME_MED::MEDCouplingMeshesCorbaInterface *getMeshes();
    SALOME_MED::MEDCouplingMeshCorbaInterface_ptr getMeshWithId(CORBA::Long id);
    //
    CORBA::Boolean ExportDataAs(const char *format, SALOME::GenericObj_out exporter);
  };
}

#endif
