// Copyright (C) 2007-2024  CEA, EDF
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

#ifndef __PARAMEDCOUPLINGFIELDDOUBLESERVANT_HXX__
#define __PARAMEDCOUPLINGFIELDDOUBLESERVANT_HXX__

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(ParaMEDCouplingCorbaServant)
#include "MPIObject_i.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"

namespace MEDCoupling
{
  class ParaMEDCouplingFieldDoubleServant : public virtual POA_SALOME_MED::ParaMEDCouplingFieldDoubleCorbaInterface,
                                            public MEDCouplingFieldDoubleServant,
                                            public MPIObject_i
  {
  public:
    ParaMEDCouplingFieldDoubleServant(CORBA::ORB_ptr orb, MEDCouplingFieldDouble* field);

    void Register();
    void UnRegister();
  };
}
#endif
