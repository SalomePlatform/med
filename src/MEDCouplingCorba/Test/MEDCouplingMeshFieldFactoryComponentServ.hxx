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

#ifndef __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENTSERV_HXX__
#define __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENTSERV_HXX__

#include <cppunit/extensions/HelperMacros.h>
#include "SALOMEconfig.h"
#ifdef WIN32
#define NOMINMAX
#endif
#include CORBA_SERVER_HEADER(MEDCouplingCorbaServantTest)

namespace MEDCoupling
{
  class MEDCouplingUMesh;
}

namespace SALOME_TEST
{
  class MEDCouplingMeshFieldFactoryComponent : public POA_SALOME_TEST::MEDCouplingMeshFieldFactory
  {
  public:
    MEDCouplingMeshFieldFactoryComponent(CORBA::ORB_ptr orb);
    void destroyFactory();
    void shutdownOrb();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr get1DMesh();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr get2DMesh();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr get3DMesh();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr get3DSurfMesh();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr get0DMesh();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr getM1DMesh();
    SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_ptr getExtrudedMesh();
    SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr getCMesh();
    SALOME_MED::MEDCouplingIMeshCorbaInterface_ptr getIMesh();
    SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_ptr getCLMesh();
    SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface_ptr get1SGTUMesh();
    SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface_ptr get1DGTUMesh();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldScalarOn2DNT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldNodeScalarOn2DNT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldScalarOn3DNT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldScalarOn3DSurfWT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldScalarOn3DSurfCOTI();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldScalarOn2DLT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldGaussPt2DWT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldGaussPtNE2DWT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldVectorOnExtrudedWT();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getFieldVectorOnCMeshWT();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr getFieldTemplateCellOn2D();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr getFieldTemplateNodeOn2D();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr getFieldTemplateGaussPtOn2D();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr getFieldTemplateGaussNEOn2D();
    SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_ptr getMultiFields1();
    SALOME_MED::DataArrayDoubleCorbaInterface_ptr getArrayDouble1();
    SALOME_MED::DataArrayDoubleCorbaInterface_ptr getArrayDouble2();
    SALOME_MED::DataArrayDoubleCorbaInterface_ptr getArrayDouble3();
    SALOME_MED::DataArrayIntCorbaInterface_ptr getArrayInt1();
    SALOME_MED::DataArrayIntCorbaInterface_ptr getArrayInt2();
    SALOME_MED::DataArrayIntCorbaInterface_ptr getArrayInt3();
    SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_ptr getMultiFields2();
  private:
    CORBA::ORB_ptr _orb;
  };
}

#endif
