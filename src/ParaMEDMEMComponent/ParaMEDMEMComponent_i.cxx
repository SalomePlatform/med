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

#include "ParaMEDMEMComponent_i.hxx"
#include "utilities.h"
#include "Utils_SALOME_Exception.hxx"
using namespace std;
using namespace MEDCoupling;

typedef struct
{
  bool exception;
  string msg;
} except_st;

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

ParaMEDMEMComponent_i::ParaMEDMEMComponent_i()
{
  _interface = new CommInterface;
}

ParaMEDMEMComponent_i::ParaMEDMEMComponent_i(CORBA::ORB_ptr orb,
                                             PortableServer::POA_ptr poa, 
                                             PortableServer::ObjectId * contId, 
                                             const char *instanceName,
                                             const char *interfaceName,
                                             bool regist)
  : Engines_Component_i(orb,poa,contId,instanceName,interfaceName,false,regist)
{
  _interface = new CommInterface;
}

ParaMEDMEMComponent_i::~ParaMEDMEMComponent_i()
{
  MESSAGE("* [" << _numproc << "] ParaMEDMEMComponent destructor");
  delete _interface;
  pthread_mutex_destroy (&m1);
  pthread_mutex_destroy (&m2);
}

void ParaMEDMEMComponent_i::initializeCoupling(const char * coupling, const char * ior)
{
  int gsize, grank;
  except_st *est;
  void *ret_th;
  pthread_t *th = 0;
  ostringstream msg;
  
  pthread_mutex_lock(&m1);
  if(_numproc == 0)
    {
      th = new pthread_t[_nbproc];
      for(int ip=1;ip<_nbproc;ip++)
        {
          thread_st *st = new thread_st;
          st->ip = ip;
          st->tior = _tior;
          st->coupling = coupling;
          st->ior = ior;
          pthread_create(&(th[ip]),NULL,th_initializecoupling,(void*)st);
        }
    }

  try{
    string service = coupling;
    if( service.size() == 0 )
      throw SALOME_Exception("You have to give a service name !");
    
    if( _gcom.find(service) != _gcom.end() )
      {
        msg << "service " << service << " already exists !";
        throw SALOME_Exception(msg.str().c_str());
      }

    // Connection to distributed parallel component
#ifdef HAVE_MPI2
    remoteMPI2Connect(coupling);
#else
    throw SALOME_Exception("You have to use a MPI2 compliant mpi implementation !");
#endif

    MPI_Comm_size( _gcom[coupling], &gsize );
    MPI_Comm_rank( _gcom[coupling], &grank );
    MESSAGE("[" << grank << "] new communicator of " << gsize << " processes");

    // Creation of processors group for ParaMEDMEM
    // source is always the lower processor numbers
    // target is always the upper processor numbers
    if(_numproc==grank)
      {
        _source[coupling] = new MPIProcessorGroup(*_interface,0,_nbproc-1,_gcom[coupling]);
        _target[coupling] = new MPIProcessorGroup(*_interface,_nbproc,gsize-1,_gcom[coupling]);
        _commgroup[coupling] = _source[coupling];
      }
    else
      {
        _source[coupling] = new MPIProcessorGroup(*_interface,0,gsize-_nbproc-1,_gcom[coupling]);
        _target[coupling] = new MPIProcessorGroup(*_interface,gsize-_nbproc,gsize-1,_gcom[coupling]);
        _commgroup[coupling] = _target[coupling];
      }
    _connectto [coupling] = ior;
    _dec[coupling] = NULL;
    _dec_options[coupling] = NULL;
    
  }
  catch(const std::exception &ex)
    {
      MESSAGE(ex.what());
      THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
    }

  pthread_mutex_unlock(&m1);
  if(_numproc == 0)
    {
      for(int ip=1;ip<_nbproc;ip++)
        {
          pthread_join(th[ip],&ret_th);
          est = (except_st*)ret_th;
          if(est->exception)
            {
              msg << "[" << ip << "] " << est->msg;
              THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
            }
          delete est;
        }
      delete[] th;
    }
}

void ParaMEDMEMComponent_i::terminateCoupling(const char * coupling)
{
  except_st *est;
  void *ret_th;
  pthread_t *th = 0;
  ostringstream msg;

  pthread_mutex_lock(&m2);
  if(_numproc == 0)
    {
      th = new pthread_t[_nbproc];
      for(int ip=1;ip<_nbproc;ip++)
        {
          thread_st *st = new thread_st;
          st->ip = ip;
          st->tior = _tior;
          st->coupling = coupling;
          pthread_create(&(th[ip]),NULL,th_terminatecoupling,(void*)st);
        }
    }

  try{
    string service = coupling;
    if( service.size() == 0 )
      throw SALOME_Exception("You have to give a service name !");

    if( _gcom.find(service) == _gcom.end() )
      {
        msg << "service " << service << " doesn't exist !";
        throw SALOME_Exception(msg.str().c_str());
      }

    // Disconnection to distributed parallel component
#ifdef HAVE_MPI2
    remoteMPI2Disconnect(coupling);
#else
    throw SALOME_Exception("You have to use a MPI2 compliant mpi implementation !");
#endif

    /* Processors groups and DEC destruction */
    delete _source[coupling];
    _source.erase(coupling);
    delete _target[coupling];
    _target.erase(coupling);
    delete _dec[coupling];
    _dec.erase(coupling);
    _commgroup.erase(coupling);
    if(_dec_options[coupling])
      {
        delete _dec_options[coupling];
        _dec_options.erase(coupling);
      }
    _connectto.erase(coupling);
  }
  catch(const std::exception &ex)
    {
      MESSAGE(ex.what());
      THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
    }
  pthread_mutex_unlock(&m2);
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

void ParaMEDMEMComponent_i::setInterpolationOptions(const char * coupling,
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
                                                    const char * splitting_policy)
{
  except_st *est;
  void *ret_th;
  pthread_t *th = 0;
  ostringstream msg;

  if(_numproc == 0)
    {
      th = new pthread_t[_nbproc];
      for(int ip=1;ip<_nbproc;ip++)
        {
          thread_st *st = new thread_st;
          st->ip = ip;
          st->tior = _tior;
          st->coupling = coupling;
          st->print_level = print_level;
          st->intersection_type = intersection_type;
          st->precision = precision;
          st->median_plane = median_plane;
          st->do_rotate = do_rotate;
          st->bounding_box_adjustment = bounding_box_adjustment;
          st->bounding_box_adjustment_abs = bounding_box_adjustment_abs;
          st->max_distance_for_3Dsurf_intersect = max_distance_for_3Dsurf_intersect;
          st->orientation = orientation;
          st->measure_abs = measure_abs;
          st->splitting_policy = splitting_policy;
          pthread_create(&(th[ip]),NULL,th_setinterpolationoptions,(void*)st);
        }
    }

  if(!_dec_options[coupling])
    _dec_options[coupling] = new INTERP_KERNEL::InterpolationOptions;

  bool ret = _dec_options[coupling]->setInterpolationOptions(print_level,
                                                             intersection_type,
                                                             precision,
                                                             median_plane,
                                                             do_rotate,
                                                             bounding_box_adjustment,
                                                             bounding_box_adjustment_abs,
                                                             max_distance_for_3Dsurf_intersect,
                                                             orientation,
                                                             measure_abs,
                                                             splitting_policy );

  if(!ret)
    {
      MESSAGE("Error on setting interpolation options");
      THROW_SALOME_CORBA_EXCEPTION("Error on setting interpolation options",SALOME::INTERNAL_ERROR);
    }
  
  if(_numproc == 0)
    {
      for(int ip=1;ip<_nbproc;ip++)
        {
          pthread_join(th[ip],&ret_th);
          est = (except_st*)ret_th;
          if(est->exception)
            {
              msg << "[" << ip << "] " << est->msg;
              THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
            }
          delete est;
        }
      delete[] th;
    }
}

void ParaMEDMEMComponent_i::_setInputField(SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface_ptr fieldptr, MEDCouplingFieldDouble *field)
{
  int grank;
  except_st *est;
  void *ret_th;
  pthread_t th;
  ostringstream msg;
  string coupling;
  
  std::map<std::string,std::string>::const_iterator it = mapSearchByValue(_connectto, fieldptr->getRef());
  if(it != _connectto.end())
    coupling = (*it).first.c_str();
  else
    throw SALOME_Exception("Reference of remote component doesn't find in connectto map !");

  if(_numproc == 0)
    {
      thread_st *st = new thread_st;
      st->fieldptr = fieldptr;
      st->coupling = coupling;
      pthread_create(&th,NULL,th_getdata,(void*)st);
    }

  if( coupling.size() == 0 )
    throw SALOME_Exception("You have to give a service name !");

  if( _gcom.find(coupling) == _gcom.end() )
    {
      msg << "service " << coupling << " doesn't exist !";
      throw SALOME_Exception(msg.str().c_str());
    }

  if(!_dec[coupling])
    {

      MPI_Comm_rank( _gcom[coupling], &grank );

      // Creating the intersection Data Exchange Channel
      // Processors which received the field are always the second argument of InterpKernelDEC object
      if(_numproc==grank)
        _dec[coupling] = new InterpKernelDEC(*_target[coupling], *_source[coupling]);
      else
        _dec[coupling] = new InterpKernelDEC(*_source[coupling], *_target[coupling]);

      if(_dec_options[coupling])
        _dec[coupling]->copyOptions(*(_dec_options[coupling]));
      
      //Attaching the field to the DEC
      _dec[coupling]->attachLocalField(field);

      // computing the interpolation matrix
      _dec[coupling]->synchronize();

    }
  else
    //Attaching the field to the DEC
    _dec[coupling]->attachLocalField(field);
  
  //Receiving data
  _dec[coupling]->recvData();

  if(_numproc == 0)
    {
      pthread_join(th,&ret_th);
      est = (except_st*)ret_th;
      if(est->exception)
        throw SALOME_Exception(est->msg.c_str());
      delete est;
    }

}

void ParaMEDMEMComponent_i::_getOutputField(const char * coupling, MEDCouplingFieldDouble *field)
{
  int grank;
  string service = coupling;
  ostringstream msg;

  if( service.size() == 0 )
    throw SALOME_Exception("You have to give a service name !");

  if( _gcom.find(service) == _gcom.end() )
    {
      msg << "service " << service << " doesn't exist !";
      throw SALOME_Exception(msg.str().c_str());
    }

  if(!_dec[coupling])
    {

      MPI_Comm_rank( _gcom[coupling], &grank );

      // Creating the intersection Data Exchange Channel
      // Processors which sent the field are always the first argument of InterpKernelDEC object
      if(_numproc==grank)
        _dec[coupling] = new InterpKernelDEC(*_source[coupling], *_target[coupling]);
      else
        _dec[coupling] = new InterpKernelDEC(*_target[coupling], *_source[coupling]);
  
      if(_dec_options[coupling])
        _dec[coupling]->copyOptions(*(_dec_options[coupling]));
      
      //Attaching the field to the DEC
      _dec[coupling]->attachLocalField(field);
    
      // computing the interpolation matrix
      _dec[coupling]->synchronize();
    }
  else
    //Attaching the field to the DEC
    _dec[coupling]->attachLocalField(field);

  //Sending data
  _dec[coupling]->sendData();
}

void ParaMEDMEMComponent_i::_initializeCoupling(SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface_ptr fieldptr)
{
  except_st *est;
  void *ret_th;
  pthread_t *th;
  //this string specifies the coupling
  string coupling;
  //getting IOR string of the remote object
  string rcompo = fieldptr->getRef();
  if(_numproc == 0){
    //getting IOR string of the local object
    CORBA::Object_var my_ref = _poa->servant_to_reference (_thisObj);
    string lcompo = _orb->object_to_string(my_ref);
    //the component does not communicate with itself, a connection is required
    if( rcompo.find(lcompo) == std::string::npos ){
      th = new pthread_t[1];
      //finding the IOR of the remote object in the map
      std::map<std::string,std::string>::const_iterator it = mapSearchByValue(_connectto, rcompo);
      //if it is not found : connecting two objects : this is the first (and the only) connection between these objects
      if (it == _connectto.end()){
        //generating the coupling string : concatenation of two IOR strings
        coupling = lcompo + rcompo;

        //initializing the coupling on the remote object in a thread
        thread_st *st = new thread_st;
        CORBA::Object_var obj = _orb->string_to_object (rcompo.c_str());
        SALOME_MED::ParaMEDMEMComponent_var compo = SALOME_MED::ParaMEDMEMComponent::_narrow(obj);
        st->compo = compo._retn();
        st->coupling = coupling;
        st->ior = lcompo;
        
        pthread_create(&(th[0]),NULL,th_initializecouplingdist,(void*)st);
        
        //initializing the coupling on the local object
        initializeCoupling (coupling.c_str(), rcompo.c_str());
        pthread_join (th[0], &ret_th); 
        est = (except_st*)ret_th;
        if(est->exception)
          THROW_SALOME_CORBA_EXCEPTION(est->msg.c_str(),SALOME::INTERNAL_ERROR);
        delete est;
        delete[] th;
      }
    }
  }
}

std::map<std::string,std::string>::const_iterator ParaMEDMEMComponent_i::mapSearchByValue(std::map<std::string,std::string> & search_map, std::string search_val)
{
  std::map<std::string,std::string>::const_iterator iRet = search_map.end();
  for (std::map<std::string,std::string>::const_iterator iTer = search_map.begin(); iTer != search_map.end(); iTer ++)
    {
      if( iTer->second.find(search_val) != std::string::npos )
        {
          iRet = iTer;
          break;
        }
    }
  return iRet;
}

bool ParaMEDMEMComponent_i::amICoupledWithThisComponent(const char* cref)
{
  std::map<std::string,std::string>::const_iterator it = mapSearchByValue(_connectto, cref);
  if(it != _connectto.end())
    return true;
  else
    return false;
}

void *th_setinterpolationoptions(void *s)
{
  ostringstream msg;
  thread_st *st = (thread_st*)s;
  except_st *est = new except_st;
  est->exception = false;
  try
    {
      SALOME_MED::ParaMEDMEMComponent_var compo=SALOME_MED::ParaMEDMEMComponent::_narrow((*(st->tior))[st->ip]);
      compo->setInterpolationOptions(st->coupling.c_str(),
                                     st->print_level,
                                     st->intersection_type,
                                     st->precision,
                                     st->median_plane,
                                     st->do_rotate,
                                     st->bounding_box_adjustment,
                                     st->bounding_box_adjustment_abs,
                                     st->max_distance_for_3Dsurf_intersect,
                                     st->orientation,
                                     st->measure_abs,
                                     st->splitting_policy);
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

void *th_initializecoupling(void *s)
{
  ostringstream msg;
  thread_st *st = (thread_st*)s;
  except_st *est = new except_st;
  est->exception = false;

  try
    {
      SALOME_MED::ParaMEDMEMComponent_var compo=SALOME_MED::ParaMEDMEMComponent::_narrow((*(st->tior))[st->ip]);
      compo->initializeCoupling(st->coupling.c_str(),st->ior.c_str());
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

void *th_terminatecoupling(void *s)
{
  ostringstream msg;
  thread_st *st = (thread_st*)s;
  except_st *est = new except_st;
  est->exception = false;

  try
    {
      SALOME_MED::ParaMEDMEMComponent_var compo=SALOME_MED::ParaMEDMEMComponent::_narrow((*(st->tior))[st->ip]);
      compo->terminateCoupling(st->coupling.c_str());
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

void *th_getdata(void *s)
{
  ostringstream msg;
  thread_st *st = (thread_st*)s;
  except_st *est = new except_st;
  est->exception = false;

  try
    {
      st->fieldptr->getDataByMPI(st->coupling.c_str());
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

void *th_initializecouplingdist(void *s)
{
  ostringstream msg;
  thread_st *st = (thread_st*)s;
  except_st *est = new except_st;
  est->exception = false;

  try
    {
      st->compo->initializeCoupling(st->coupling.c_str(), st->ior.c_str());
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

void *th_terminatecouplingdist(void *s)
{
  ostringstream msg;
  thread_st *st = (thread_st*)s;
  except_st *est = new except_st;
  est->exception = false;

  try
    {
      st->compo->terminateCoupling(st->coupling.c_str());
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
