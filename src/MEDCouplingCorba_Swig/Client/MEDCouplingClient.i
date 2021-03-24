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

%module MEDCouplingClient

%{
#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingFieldTemplateClient.hxx"
#include "MEDCouplingMultiFieldsClient.hxx"
#include "MEDCouplingFieldOverTimeClient.hxx"
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCoupling1SGTUMeshClient.hxx"
#include "MEDCoupling1DGTUMeshClient.hxx"
#include "MEDCouplingExtrudedMeshClient.hxx"
#include "MEDCouplingCMeshClient.hxx"
#include "MEDCouplingIMeshClient.hxx"
#include "MEDCouplingCurveLinearMeshClient.hxx"
#include "DataArrayDoubleClient.hxx"
#include "DataArrayIntClient.hxx"

#include <iostream>

using namespace MEDCoupling;
%}

%include "MEDCouplingCommon.i"

%newobject MEDCoupling::MEDCouplingFieldDoubleClient::New;
%newobject MEDCoupling::MEDCouplingFieldTemplateClient::New;
%newobject MEDCoupling::MEDCouplingUMeshClient::New;
%newobject MEDCoupling::MEDCoupling1SGTUMeshClient::New;
%newobject MEDCoupling::MEDCoupling1DGTUMeshClient::New;
%newobject MEDCoupling::MEDCouplingExtrudedMeshClient::New;
%newobject MEDCoupling::MEDCouplingCMeshClient::New;
%newobject MEDCoupling::MEDCouplingIMeshClient::New;
%newobject MEDCoupling::MEDCouplingCurveLinearMeshClient::New;
%newobject MEDCoupling::MEDCouplingMultiFieldsClient::New;
%newobject MEDCoupling::MEDCouplingFieldOverTimeClient::New;
%newobject MEDCoupling::DataArrayDoubleClient::New;
%newobject MEDCoupling::DataArrayIntClient::New;

%nodefaultctor;
%nodefaultdtor;

%{
#if PY_VERSION_HEX < 0x03050000
static char*
Py_EncodeLocale(const wchar_t *text, size_t *error_pos)
{
   return _Py_wchar2char(text, error_pos);
}
#endif
%}

namespace MEDCoupling
{
  class MEDCouplingFieldDoubleClient
  {
  public:
    %extend
      {
        static MEDCouplingFieldDouble *New(PyObject *fieldPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorField=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",fieldPtr);
          if(!iorField)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingFieldDoubleClient.New appears to differ from CORBA reference ! Expecting a FieldDouble CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorField), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var fieldPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_var fieldPtrCppC=SALOME_MED::MEDCouplingFieldDoubleCorbaInterface::_narrow(fieldPtrCpp);
          if(CORBA::is_nil(fieldPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingFieldDoubleCorbaInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorField);
          MEDCouplingFieldDouble *ret=MEDCouplingFieldDoubleClient::New(fieldPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingFieldTemplateClient
  {
  public:
    %extend
      {
        static MEDCouplingFieldTemplate *New(PyObject *fieldPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorField=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",fieldPtr);
          if(!iorField)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingFieldTemplateClient.New appears to differ from CORBA reference ! Expecting a FieldTemplate CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorField), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var fieldPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_var fieldPtrCppC=SALOME_MED::MEDCouplingFieldTemplateCorbaInterface::_narrow(fieldPtrCpp);
          if(CORBA::is_nil(fieldPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingFieldTemplateCorbaInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorField);
          MEDCouplingFieldTemplate *ret=MEDCouplingFieldTemplateClient::New(fieldPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingMultiFieldsClient
  {
  public:
    %extend
      {
        static MEDCouplingMultiFields *New(PyObject *fieldPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorField=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",fieldPtr);
          if(!iorField)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingFieldTemplateClient.New appears to differ from CORBA reference ! Expecting a MultiFields CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorField), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var fieldPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_var fieldPtrCppC=SALOME_MED::MEDCouplingMultiFieldsCorbaInterface::_narrow(fieldPtrCpp);
          if(CORBA::is_nil(fieldPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingMultiFieldsCorbaInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorField);
          MEDCouplingMultiFields *ret=MEDCouplingMultiFieldsClient::New(fieldPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingFieldOverTimeClient : public MEDCouplingFieldOverTime
  {
  public:
    std::vector<double> getTimeSteps() const;
    %extend
      {
        static MEDCouplingFieldOverTimeClient *New(PyObject *fieldPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorField=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",fieldPtr);
          if(!iorField)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingFieldOverTimeClient.New appears to differ from CORBA reference ! Expecting a FieldOverTime CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorField), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var fieldPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_var fieldPtrCppC=SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface::_narrow(fieldPtrCpp);
          if(CORBA::is_nil(fieldPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingFieldOverTimeCorbaInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorField);
          MEDCouplingFieldOverTimeClient *ret=MEDCouplingFieldOverTimeClient::New(fieldPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingMeshClient.New appears to differ from CORBA reference ! Expecting a MeshCorbaInterface CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorMesh), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingUMeshCorbaInterface_var meshPtrCppC1=SALOME_MED::MEDCouplingUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC1))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCouplingUMeshClient::New(meshPtrCppC1);
            }
          SALOME_MED::MEDCouplingCMeshCorbaInterface_var meshPtrCppC2=SALOME_MED::MEDCouplingCMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC2))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCouplingCMeshClient::New(meshPtrCppC2);
            }
          SALOME_MED::MEDCouplingIMeshCorbaInterface_var meshPtrCppC7=SALOME_MED::MEDCouplingIMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC7))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCouplingIMeshClient::New(meshPtrCppC7);
            }
          SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_var meshPtrCppC3=SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC3))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCouplingCurveLinearMeshClient::New(meshPtrCppC3);
            }
          SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface_var meshPtrCppC4=SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC4))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCoupling1SGTUMeshClient::New(meshPtrCppC4);
            }
          SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface_var meshPtrCppC5=SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC5))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCoupling1DGTUMeshClient::New(meshPtrCppC5);
            }
          SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_var meshPtrCppC6=SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC6))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCouplingExtrudedMeshClient::New(meshPtrCppC6);
            }
          throw INTERP_KERNEL::Exception("Error input corba pointer is not a managed mesh type !");
        } 
      }
  };

  class MEDCouplingUMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingUMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingUMeshClient.New appears to differ from CORBA reference ! Expecting a UMeshCorbaInterface CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorMesh), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingUMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCouplingUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingUMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCouplingUMesh *ret=MEDCouplingUMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCoupling1SGTUMeshClient
  {
  public:
    %extend
      {
        static MEDCoupling1SGTUMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCoupling1SGTUMeshClient.New appears to differ from CORBA reference ! Expecting a 1SGTUMeshCorbaInterface CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorMesh), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCoupling1SGTUMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCoupling1SGTUMesh *ret=MEDCoupling1SGTUMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCoupling1DGTUMeshClient
  {
  public:
    %extend
      {
        static MEDCoupling1DGTUMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCoupling1DGTUMeshClient.New appears to differ from CORBA reference ! Expecting a 1DGTUMeshCorbaInterface CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorMesh), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCoupling1DGTUMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCoupling1DGTUMesh *ret=MEDCoupling1DGTUMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingExtrudedMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingMappedExtrudedMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingExtrudedMeshClient.New appears to differ from CORBA reference ! Expecting an ExtrudedMeshCorbaInterface CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorMesh), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingExtrudedMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCouplingMappedExtrudedMesh *ret=MEDCouplingExtrudedMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingCMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingCMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingCMeshClient.New appears to differ from CORBA reference ! Expecting a CMeshCorbaInterface CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorMesh), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingCMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCouplingCMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingCMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCouplingCMesh *ret=MEDCouplingCMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingIMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingIMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingIMeshClient.New appears to differ from CORBA reference ! Expecting a IMeshCorbaInterface CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorMesh), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingIMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCouplingIMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingIMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCouplingIMesh *ret=MEDCouplingIMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingCurveLinearMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingCurveLinearMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingCurveLinearMeshClient.New appears to differ from CORBA reference ! Expecting a CurveLinearMeshCorbaInterface CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorMesh), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingCurveLinearMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCouplingCurveLinearMesh *ret=MEDCouplingCurveLinearMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class DataArrayDoubleClient
  {
  public:
    %extend
      {
        static DataArrayDouble *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of DataArrayDoubleClient.New appears to differ from CORBA reference ! Expecting a DataArrayDoubleCorbaInterface CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorMesh), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::DataArrayDoubleCorbaInterface_var meshPtrCppC=SALOME_MED::DataArrayDoubleCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.DataArrayDoubleInterface_ptr ");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          DataArrayDouble *ret=DataArrayDoubleClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class DataArrayIntClient
  {
  public:
    %extend
      {
        static DataArrayInt *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of DataArrayIntClient.New appears to differ from CORBA reference ! Expecting a DataArrayIntCorbaInterface CORBA reference !");
          char *ior=Py_EncodeLocale(PyUnicode_AS_UNICODE(iorMesh), NULL);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::DataArrayIntCorbaInterface_var meshPtrCppC=SALOME_MED::DataArrayIntCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.DataArrayIntInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          DataArrayInt *ret=DataArrayIntClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };
}

%pythoncode %{
def MEDCouplingDataArrayDoubleIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDouble____iadd___(self, self, *args)
def MEDCouplingDataArrayDoubleIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDouble____isub___(self, self, *args)
def MEDCouplingDataArrayDoubleImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDouble____imul___(self, self, *args)
def MEDCouplingDataArrayDoubleIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDouble____idiv___(self, self, *args)
def MEDCouplingDataArrayDoubleIpow(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDouble____ipow___(self, self, *args)
def MEDCouplingFieldDoubleIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.MEDCouplingFieldDouble____iadd___(self, self, *args)
def MEDCouplingFieldDoubleIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.MEDCouplingFieldDouble____isub___(self, self, *args)
def MEDCouplingFieldDoubleImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.MEDCouplingFieldDouble____imul___(self, self, *args)
def MEDCouplingFieldDoubleIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.MEDCouplingFieldDouble____idiv___(self, self, *args)
def MEDCouplingFieldDoubleIpow(self,*args):
    import _MEDCoupling
    return _MEDCoupling.MEDCouplingFieldDouble____ipow___(self, self, *args)
def MEDCouplingDataArrayInt32Iadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____iadd___(self, self, *args)
def MEDCouplingDataArrayInt32Isub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____isub___(self, self, *args)
def MEDCouplingDataArrayInt32Imul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____imul___(self, self, *args)
def MEDCouplingDataArrayInt32Idiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____idiv___(self, self, *args)
def MEDCouplingDataArrayInt32Imod(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____imod___(self, self, *args)
def MEDCouplingDataArrayInt32Ipow(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____ipow___(self, self, *args)
def MEDCouplingDataArrayInt64Iadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____iadd___(self, self, *args)
def MEDCouplingDataArrayInt64Isub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____isub___(self, self, *args)
def MEDCouplingDataArrayInt64Imul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____imul___(self, self, *args)
def MEDCouplingDataArrayInt64Idiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____idiv___(self, self, *args)
def MEDCouplingDataArrayInt64Imod(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____imod___(self, self, *args)
def MEDCouplingDataArrayInt64Ipow(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____ipow___(self, self, *args)
def MEDCouplingDataArrayFloatIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayFloat____iadd___(self, self, *args)
def MEDCouplingDataArrayFloatIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayFloat____isub___(self, self, *args)
def MEDCouplingDataArrayFloatImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayFloat____imul___(self, self, *args)
def MEDCouplingDataArrayFloatIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayFloat____idiv___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDoubleTuple____iadd___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDoubleTuple____isub___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDoubleTuple____imul___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDoubleTuple____idiv___(self, self, *args)
def MEDCouplingDataArrayInt32TupleIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32Tuple____iadd___(self, self, *args)
def MEDCouplingDataArrayInt32TupleIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32Tuple____isub___(self, self, *args)
def MEDCouplingDataArrayInt32TupleImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32Tuple____imul___(self, self, *args)
def MEDCouplingDataArrayInt32TupleIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32Tuple____idiv___(self, self, *args)
def MEDCouplingDataArrayInt32TupleImod(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32Tuple____imod___(self, self, *args)
def MEDCouplingDataArrayInt64TupleIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64Tuple____iadd___(self, self, *args)
def MEDCouplingDataArrayInt64TupleIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64Tuple____isub___(self, self, *args)
def MEDCouplingDataArrayInt64TupleImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64Tuple____imul___(self, self, *args)
def MEDCouplingDataArrayInt64TupleIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64Tuple____idiv___(self, self, *args)
def MEDCouplingDataArrayInt64TupleImod(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64Tuple____imod___(self, self, *args)
def MEDCouplingDenseMatrixIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DenseMatrix____iadd___(self, self, *args)
def MEDCouplingDenseMatrixIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DenseMatrix____isub___(self, self, *args)
%}

%include "MEDCouplingFinalize.i"
