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

#ifndef __PARAMEDMEMCOMPOCOMPONENT_I__
#define __PARAMEDMEMCOMPOCOMPONENT_I__

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(ParaMEDMEMComponent)
#include "MPIObject_i.hxx"
#include "SALOME_Component_i.hxx"
#include "Topology.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "InterpKernelDEC.hxx"
#include "InterpolationOptions.hxx"
#include "MPIProcessorGroup.hxx"
#include "CommInterface.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"
#include "Utils_CorbaException.hxx"
#include <map>

void * th_setinterpolationoptions(void *st);
void * th_initializecoupling(void *st);
void * th_terminatecoupling(void *st);
void * th_getdata(void *st);
void * th_initializecouplingdist (void *st);
void * th_terminatecouplingdist (void *st);

typedef struct
{
  int ip;
  long print_level;
  const char * intersection_type;
  double precision;
  double median_plane;
  bool do_rotate;
  double bounding_box_adjustment;
  double bounding_box_adjustment_abs;
  double max_distance_for_3Dsurf_intersect;
  long orientation;
  bool measure_abs;
  const char * splitting_policy;
  std::string coupling;
  std::string ior;
  Engines::IORTab* tior;
  SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface_ptr fieldptr;
  SALOME_MED::ParaMEDMEMComponent_ptr compo;
} thread_st;

namespace MEDCoupling
{
  class ParaMEDMEMComponent_i : public virtual POA_SALOME_MED::ParaMEDMEMComponent, public Engines_Component_i, public MPIObject_i
  {

  public:
    // Constructor
    ParaMEDMEMComponent_i();
    ParaMEDMEMComponent_i(CORBA::ORB_ptr orb,
                          PortableServer::POA_ptr poa, 
                          PortableServer::ObjectId * contId, 
                          const char *instanceName,
                          const char *interfaceName,
                          bool regist);
    
    // Destructor
    virtual ~ParaMEDMEMComponent_i();
    void setInterpolationOptions(const char * coupling,
                                 CORBA::Long print_level,
                                 const char * intersection_type,
                                 CORBA::Double precision,
                                 CORBA::Double median_plane,
                                 CORBA::Boolean do_rotate,
                                 CORBA::Double bounding_box_adjustment,
                                 CORBA::Double bounding_box_adjustment_abs,
                                 CORBA::Double max_distance_for_3Dsurf_intersect,
                                 CORBA::Long orientation,
                                 CORBA::Boolean measure_abs,
                                 const char * splitting_policy );
    virtual void initializeCoupling(const char * coupling, const char * ior);
    virtual void terminateCoupling(const char * coupling);
    virtual void _getOutputField(const char * coupling, MEDCouplingFieldDouble* field);
    
  protected:
    void _initializeCoupling(SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface_ptr fieldptr);
    void _setInputField(SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface_ptr fieldptr, MEDCouplingFieldDouble* field);
    bool amICoupledWithThisComponent(const char * cref);

  private:
    std::map<std::string,std::string>::const_iterator mapSearchByValue(std::map<std::string,std::string> & search_map, std::string search_val);
    
    CommInterface* _interface;
    std::map<std::string,InterpKernelDEC*> _dec;
    std::map<std::string,MPIProcessorGroup*> _source, _target;
    std::map<std::string,ProcessorGroup*> _commgroup;
    std::map<std::string,INTERP_KERNEL::InterpolationOptions*> _dec_options;
    std::map<std::string,std::string> _connectto;  //IOR of distant objects
  };
}
#endif
