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

#ifndef __MPIMEDCOUPLINGFIELDDOUBLESERVANT_HXX__
#define __MPIMEDCOUPLINGFIELDDOUBLESERVANT_HXX__

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(ParaMEDMEMComponent)
#include "ParaMEDMEMComponent_i.hxx"
#include "ParaMEDCouplingFieldDoubleServant.hxx"

#include <string>

void * th_getdatabympi(void *st);

namespace MEDCoupling
{
  class MPIMEDCouplingFieldDoubleServant : public virtual POA_SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface,
                                           public ParaMEDCouplingFieldDoubleServant
  {
  public:
    MPIMEDCouplingFieldDoubleServant(CORBA::ORB_ptr orb,PortableServer::POA_ptr poa,ParaMEDMEMComponent_i *pcompo,MEDCouplingFieldDouble* field);
    void getDataByMPI(const char* coupling);
    char* getRef();
    // only for coupling one component with itself
    CORBA::LongLong getImplementation() {return (CORBA::LongLong)_field;}
  private:
    ParaMEDMEMComponent_i *_pcompo;
    MEDCouplingFieldDouble* _field;
    ///IOR string of the object in which the vector is created
    std::string _ref;
  };
}
#endif
