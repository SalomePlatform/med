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

#ifndef __MEDCOUPLINGREFCOUNTSERVANT_HXX__
#define __MEDCOUPLINGREFCOUNTSERVANT_HXX__

#include "SALOMEconfig.h"

#include CORBA_SERVER_HEADER(MEDCouplingCorbaServant)
#include "MEDCouplingCorba.hxx"

namespace MEDCoupling
{
  class TimeLabel;
  class RefCountObject;

  class MEDCOUPLINGCORBA_EXPORT MEDCouplingRefCountServant : public virtual POA_SALOME_MED::MEDCouplingRefCountCorbaInterface
  {
  protected:
    MEDCouplingRefCountServant(const RefCountObject *pointer, const TimeLabel *pointer2);
    ~MEDCouplingRefCountServant();
  protected:
    void Register();
    void UnRegister();
    void Destroy(); // obsolete
    CORBA::Long getTimeLabel();
    char *versionStr();
    void versionMajMinRel(CORBA::Long_out maj, CORBA::Long_out minor, CORBA::Long_out releas);
    SALOME::StringSeq *GetExportableFormats();
  protected:
    int _ref_counter;
    const RefCountObject *_cpp_pointer;
    const TimeLabel *_tl_pointer;
  };
}

#endif
