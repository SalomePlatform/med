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

#include "MEDCouplingMeshFieldFactoryComponentServ.hxx"
#include "MEDCouplingMeshFieldFactoryComponent.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"
#include "MEDCouplingFieldTemplateServant.hxx"
#include "MEDCouplingMultiFieldsServant.hxx"
#include "MEDCouplingFieldOverTimeServant.hxx"
#include "MEDCouplingExtrudedMeshServant.hxx"
#include "MEDCouplingCurveLinearMeshServant.hxx"
#include "MEDCoupling1SGTUMeshServant.hxx"
#include "MEDCoupling1DGTUMeshServant.hxx"
#include "MEDCouplingCMeshServant.hxx"
#include "MEDCouplingIMeshServant.hxx"
#include "MEDCouplingUMeshServant.hxx"
#include "DataArrayDoubleServant.hxx"
#include "DataArrayIntServant.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingFieldTemplate.hxx"
#include "MEDCouplingMultiFields.hxx"
#include "MEDCouplingFieldOverTime.hxx"
#include "MEDCouplingMappedExtrudedMesh.hxx"
#include "MEDCouplingCurveLinearMesh.hxx"
#include "MEDCoupling1GTUMesh.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingCMesh.hxx"
#include "MEDCouplingIMesh.hxx"

namespace SALOME_TEST
{
  MEDCouplingMeshFieldFactoryComponent::MEDCouplingMeshFieldFactoryComponent(CORBA::ORB_ptr orb):_orb(orb)
  {
  }

  void MEDCouplingMeshFieldFactoryComponent::destroyFactory()
  {
    PortableServer::ObjectId_var oid = _default_POA()->servant_to_id(this);
    _default_POA()->deactivate_object(oid);
    _remove_ref();
  }

  void MEDCouplingMeshFieldFactoryComponent::shutdownOrb()
  {
    _orb->shutdown(0);
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get1DMesh()
  {
    MEDCoupling::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::build1DMesh();
    MEDCoupling::MEDCouplingUMeshServant *m=new MEDCoupling::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get2DMesh()
  {
    MEDCoupling::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::build2DMesh();
    MEDCoupling::MEDCouplingUMeshServant *m=new MEDCoupling::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get3DMesh()
  {
    MEDCoupling::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::build3DMesh();
    MEDCoupling::MEDCouplingUMeshServant *m=new MEDCoupling::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get3DSurfMesh()
  {
    MEDCoupling::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::build3DSurfMesh();
    MEDCoupling::MEDCouplingUMeshServant *m=new MEDCoupling::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get0DMesh()
  {
    MEDCoupling::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::build0DMesh();
    MEDCoupling::MEDCouplingUMeshServant *m=new MEDCoupling::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getM1DMesh()
  {
    MEDCoupling::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::buildM1DMesh();
    MEDCoupling::MEDCouplingUMeshServant *m=new MEDCoupling::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getExtrudedMesh()
  {
    MEDCoupling::MEDCouplingUMesh *m2D;
    MEDCoupling::MEDCouplingMappedExtrudedMesh *m1=MEDCouplingCorbaServBasicsTest::buildExtrudedMesh(m2D);
    m2D->decrRef();
    MEDCoupling::MEDCouplingExtrudedMeshServant *m=new MEDCoupling::MEDCouplingExtrudedMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getCMesh()
  {
    MEDCoupling::MEDCouplingCMesh *m1=MEDCouplingCorbaServBasicsTest::buildCMesh();
    MEDCoupling::MEDCouplingCMeshServant *m=new MEDCoupling::MEDCouplingCMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }
  
  SALOME_MED::MEDCouplingIMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getIMesh()
  {
    MEDCoupling::MEDCouplingIMesh *m1=MEDCouplingCorbaServBasicsTest::buildIMesh();
    MEDCoupling::MEDCouplingIMeshServant *m=new MEDCoupling::MEDCouplingIMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingIMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getCLMesh()
  {
    MEDCoupling::MEDCouplingCurveLinearMesh *m1=MEDCouplingCorbaServBasicsTest::buildCLMesh();
    MEDCoupling::MEDCouplingCurveLinearMeshServant *m=new MEDCoupling::MEDCouplingCurveLinearMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get1SGTUMesh()
  {
    MEDCoupling::MEDCoupling1SGTUMesh *m1=MEDCouplingCorbaServBasicsTest::build1SGTUMesh();
    MEDCoupling::MEDCoupling1SGTUMeshServant *m=new MEDCoupling::MEDCoupling1SGTUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get1DGTUMesh()
  {
    MEDCoupling::MEDCoupling1DGTUMesh *m1=MEDCouplingCorbaServBasicsTest::build1DGTUMesh();
    MEDCoupling::MEDCoupling1DGTUMeshServant *m=new MEDCoupling::MEDCoupling1DGTUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldScalarOn2DNT()
  {
    MEDCoupling::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldScalarOn2DNT();
    MEDCoupling::MEDCouplingFieldDoubleServant *m=new MEDCoupling::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldNodeScalarOn2DNT()
  {
    MEDCoupling::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldNodeScalarOn2DNT();
    MEDCoupling::MEDCouplingFieldDoubleServant *m=new MEDCoupling::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldScalarOn3DNT()
  {
    MEDCoupling::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DNT();
    MEDCoupling::MEDCouplingFieldDoubleServant *m=new MEDCoupling::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldScalarOn3DSurfWT()
  {
    MEDCoupling::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DSurfWT();
    MEDCoupling::MEDCouplingFieldDoubleServant *m=new MEDCoupling::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }
  
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldScalarOn3DSurfCOTI()
  {
    MEDCoupling::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DSurfCOTI();
    MEDCoupling::MEDCouplingFieldDoubleServant *m=new MEDCoupling::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldScalarOn2DLT()
  {
    MEDCoupling::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldScalarOn2DLT();
    MEDCoupling::MEDCouplingFieldDoubleServant *m=new MEDCoupling::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldGaussPt2DWT()
  {
    MEDCoupling::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldGaussPt2DWT();
    MEDCoupling::MEDCouplingFieldDoubleServant *m=new MEDCoupling::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldGaussPtNE2DWT()
  {
    MEDCoupling::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldGaussPtNE2DWT();
    MEDCoupling::MEDCouplingFieldDoubleServant *m=new MEDCoupling::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldVectorOnExtrudedWT()
  {
    MEDCoupling::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldVectorOnExtrudedWT();
    MEDCoupling::MEDCouplingFieldDoubleServant *m=new MEDCoupling::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldVectorOnCMeshWT()
  {
    MEDCoupling::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldVectorOnCMeshWT();
    MEDCoupling::MEDCouplingFieldDoubleServant *m=new MEDCoupling::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }
  
  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldTemplateCellOn2D()
  {
    MEDCoupling::MEDCouplingFieldTemplate *field=MEDCouplingCorbaServBasicsTest::buildFieldTemplateCellOn2D();
    MEDCoupling::MEDCouplingFieldTemplateServant *m=new MEDCoupling::MEDCouplingFieldTemplateServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr ret=m->_this();
    return ret;
  }
  
  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldTemplateNodeOn2D()
  {
    MEDCoupling::MEDCouplingFieldTemplate *field=MEDCouplingCorbaServBasicsTest::buildFieldTemplateNodeOn2D();
    MEDCoupling::MEDCouplingFieldTemplateServant *m=new MEDCoupling::MEDCouplingFieldTemplateServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr ret=m->_this();
    return ret;
  }
  
  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldTemplateGaussPtOn2D()
  {
    MEDCoupling::MEDCouplingFieldTemplate *field=MEDCouplingCorbaServBasicsTest::buildFieldTemplateGaussPtOn2D();
    MEDCoupling::MEDCouplingFieldTemplateServant *m=new MEDCoupling::MEDCouplingFieldTemplateServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldTemplateGaussNEOn2D()
  {
    MEDCoupling::MEDCouplingFieldTemplate *field=MEDCouplingCorbaServBasicsTest::buildFieldTemplateGaussNEOn2D();
    MEDCoupling::MEDCouplingFieldTemplateServant *m=new MEDCoupling::MEDCouplingFieldTemplateServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getMultiFields1()
  {
    MEDCoupling::MEDCouplingMultiFields *fields=MEDCouplingCorbaServBasicsTest::buildMultiFields1();
    MEDCoupling::MEDCouplingMultiFieldsServant *s=new MEDCoupling::MEDCouplingMultiFieldsServant(fields);
    fields->decrRef();
    SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_ptr ret=s->_this();
    return ret;
  }

  SALOME_MED::DataArrayDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayDouble1()
  {
    MEDCoupling::DataArrayDouble *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayDouble1();
    MEDCoupling::DataArrayDoubleServant *retServ=new MEDCoupling::DataArrayDoubleServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::DataArrayDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayDouble2()
  {
    MEDCoupling::DataArrayDouble *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayDouble2();
    MEDCoupling::DataArrayDoubleServant *retServ=new MEDCoupling::DataArrayDoubleServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::DataArrayDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayDouble3()
  {
    MEDCoupling::DataArrayDouble *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayDouble3();
    MEDCoupling::DataArrayDoubleServant *retServ=new MEDCoupling::DataArrayDoubleServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::DataArrayIntCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayInt1()
  {
    MEDCoupling::DataArrayInt *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayInt1();
    MEDCoupling::DataArrayIntServant *retServ=new MEDCoupling::DataArrayIntServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::DataArrayIntCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayInt2()
  {
    MEDCoupling::DataArrayInt *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayInt2();
    MEDCoupling::DataArrayIntServant *retServ=new MEDCoupling::DataArrayIntServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::DataArrayIntCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayInt3()
  {
    MEDCoupling::DataArrayInt *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayInt3();
    MEDCoupling::DataArrayIntServant *retServ=new MEDCoupling::DataArrayIntServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getMultiFields2()
  {
    MEDCoupling::MEDCouplingFieldOverTime *fot=MEDCouplingCorbaServBasicsTest::buildMultiFields2();
    MEDCoupling::MEDCouplingFieldOverTimeServant *retServ=new MEDCoupling::MEDCouplingFieldOverTimeServant(fot);
    fot->decrRef();
    return retServ->_this();
  }
}
