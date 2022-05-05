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

#ifndef __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENT_HXX__
#define __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENT_HXX__

#include "MCType.hxx"

#include <string>

namespace MEDCoupling
{
  class MEDCouplingUMesh;
  class MEDCouplingFieldDouble;
  class MEDCouplingFieldTemplate;
  class MEDCouplingMappedExtrudedMesh;
  class MEDCoupling1DGTUMesh;
  class MEDCoupling1SGTUMesh;
  class MEDCouplingCMesh;
  class MEDCouplingIMesh;
  class MEDCouplingCurveLinearMesh;
  class DataArrayDouble;
  class DataArrayInt;
  class MEDCouplingMultiFields;
  class MEDCouplingFieldOverTime;
}

namespace SALOME_TEST
{
  class MEDCouplingCorbaServBasicsTest
  {
  public:
    static MEDCoupling::MEDCouplingUMesh *build1DMesh();
    static MEDCoupling::MEDCouplingUMesh *build2DMesh();
    static MEDCoupling::MEDCouplingUMesh *build3DMesh();
    static MEDCoupling::MEDCouplingUMesh *build3DSurfMesh();
    static MEDCoupling::MEDCouplingUMesh *build0DMesh();
    static MEDCoupling::MEDCouplingUMesh *buildM1DMesh();
    static MEDCoupling::MEDCouplingMappedExtrudedMesh *buildExtrudedMesh(MEDCoupling::MEDCouplingUMesh *&m2D);
    static MEDCoupling::MEDCouplingCMesh *buildCMesh();
    static MEDCoupling::MEDCouplingIMesh *buildIMesh();
    static MEDCoupling::MEDCouplingCurveLinearMesh *buildCLMesh();
    static MEDCoupling::MEDCoupling1SGTUMesh *build1SGTUMesh();
    static MEDCoupling::MEDCoupling1DGTUMesh *build1DGTUMesh();
    static MEDCoupling::MEDCouplingFieldDouble *buildFieldScalarOn2DNT();
    static MEDCoupling::MEDCouplingFieldDouble *buildFieldNodeScalarOn2DNT();
    static MEDCoupling::MEDCouplingFieldDouble *buildFieldScalarOn3DNT();
    static MEDCoupling::MEDCouplingFieldDouble *buildFieldScalarOn3DSurfWT();
    static MEDCoupling::MEDCouplingFieldDouble *buildFieldScalarOn3DSurfCOTI();
    static MEDCoupling::MEDCouplingFieldDouble *buildFieldScalarOn2DLT();
    static MEDCoupling::MEDCouplingFieldDouble *buildFieldGaussPt2DWT();
    static MEDCoupling::MEDCouplingFieldDouble *buildFieldGaussPtNE2DWT();
    static MEDCoupling::MEDCouplingFieldDouble *buildFieldVectorOnExtrudedWT();
    static MEDCoupling::MEDCouplingFieldDouble *buildFieldVectorOnCMeshWT();
    static MEDCoupling::MEDCouplingFieldTemplate *buildFieldTemplateCellOn2D();
    static MEDCoupling::MEDCouplingFieldTemplate *buildFieldTemplateNodeOn2D();
    static MEDCoupling::MEDCouplingFieldTemplate *buildFieldTemplateGaussPtOn2D();
    static MEDCoupling::MEDCouplingFieldTemplate *buildFieldTemplateGaussNEOn2D();
    static MEDCoupling::MEDCouplingMultiFields *buildMultiFields1();
    static MEDCoupling::DataArrayDouble *buildArrayDouble1();
    static MEDCoupling::DataArrayDouble *buildArrayDouble2();
    static MEDCoupling::DataArrayDouble *buildArrayDouble3();
    static MEDCoupling::DataArrayInt *buildArrayInt1();
    static MEDCoupling::DataArrayInt *buildArrayInt2();
    static MEDCoupling::DataArrayInt *buildArrayInt3();
    static MEDCoupling::MEDCouplingFieldOverTime *buildMultiFields2();
    static std::string buildFileNameForIOR();
  };
}

#endif
