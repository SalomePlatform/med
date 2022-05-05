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

#include "MPIMEDCouplingFieldDoubleServant.hxx"
#include "utilities.h"
using namespace std;
using namespace MEDCoupling;

typedef struct
{
  bool exception;
  string msg;
} except_st;

MPIMEDCouplingFieldDoubleServant::MPIMEDCouplingFieldDoubleServant(CORBA::ORB_ptr orb,PortableServer::POA_ptr poa,ParaMEDMEMComponent_i *pcompo,MEDCouplingFieldDouble* field):ParaMEDCouplingFieldDoubleServant(orb,field)
{
  _pcompo = pcompo;
  _field = field;

  CORBA::Object_var my_ref = poa->servant_to_reference (pcompo);
  SALOME_MED::ParaMEDMEMComponent_var compo = SALOME_MED::ParaMEDMEMComponent::_narrow(my_ref);
  _ref = orb->object_to_string(compo);

  Engines::MPIObject_var pobj = POA_SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_this();
  BCastIOR(orb,pobj,false);
}

void MPIMEDCouplingFieldDoubleServant::getDataByMPI(const char* coupling)
{
  except_st *est;
  void *ret_th;
  pthread_t *th = 0;

  if(_numproc == 0)
    {
      th = new pthread_t[_nbproc];
      for(int ip=1;ip<_nbproc;ip++)
        {
          thread_st *st = new thread_st;
          st->ip = ip;
          st->tior = _tior;
          st->coupling = coupling;
          pthread_create(&(th[ip]),NULL,th_getdatabympi,(void*)st);
        }
    }

  try
    {
      _pcompo->_getOutputField(coupling,_field);
    }
  catch(const std::exception &ex)
    {
      MESSAGE(ex.what());
      THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
    }
    
  if(_numproc == 0)
    {
      for(int ip=1;ip<_nbproc;ip++)
        {
          pthread_join(th[ip],&ret_th);
          est = (except_st*)ret_th;
          if(est->exception)
            {
              ostringstream msg;
              msg << "[" << ip << "] " << est->msg;
              THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
            }
          delete est;
        }
      delete[] th;
    }
}

void *th_getdatabympi(void *s)
{
  ostringstream msg;
  thread_st *st = (thread_st*)s;
  except_st *est = new except_st;
  est->exception = false;

  try
    {
      SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface_var fieldPtr=SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_narrow((*(st->tior))[st->ip]);
      fieldPtr->getDataByMPI(st->coupling.c_str());
    }
  catch(const SALOME::SALOME_Exception &ex)
    {
      est->exception = true;
      est->msg = ex.details.text;
    }
  catch(const CORBA::Exception &ex)
    {
      est->exception = true;
      msg << "CORBA::Exception: " << ex;
      est->msg = msg.str();
    }
  delete st;
  return((void*)est);
}

char *MPIMEDCouplingFieldDoubleServant::getRef()
{
  return CORBA::string_dup(_ref.c_str());
}

