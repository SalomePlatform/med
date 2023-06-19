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
// Author : Anthony Geay (CEA/DEN)

#ifndef __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENTCLT_HXX__
#define __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENTCLT_HXX__

#include "SALOMEconfig.h"
#ifdef WIN32
#define NOMINMAX
#endif
#include CORBA_CLIENT_HEADER(MEDCouplingCorbaServantTest)

#include <cppunit/extensions/HelperMacros.h>

namespace MEDCoupling
{
  class MEDCouplingUMesh;
}

namespace SALOME_TEST
{
  class MEDCouplingCorbaServBasicsTestClt : public CppUnit::TestFixture
  {
    CPPUNIT_TEST_SUITE(MEDCouplingCorbaServBasicsTestClt);
    CPPUNIT_TEST( checkBaseCorbaFetching );
    CPPUNIT_TEST( checkContentOfFetched1DMesh );
    CPPUNIT_TEST( checkCorbaFetching2D );
    CPPUNIT_TEST( checkContentOfFetched2DMesh );
    CPPUNIT_TEST( checkMultiFetchingToTestMemoryManagement );
    CPPUNIT_TEST( checkCorbaFetching3D );
    CPPUNIT_TEST( checkContentOfFetched3DMesh );
    CPPUNIT_TEST( checkCorbaFetching3DSurf );
    CPPUNIT_TEST( checkContentOfFetched3DSurfMesh );
    CPPUNIT_TEST( checkCorbaFetching0D );
    CPPUNIT_TEST( checkCorbaFetchingM1D );
    CPPUNIT_TEST( checkCorbaFetchingExtruded );
    CPPUNIT_TEST( checkCorbaFetchingCMesh );
    CPPUNIT_TEST( checkCorbaFetchingIMesh );
    CPPUNIT_TEST( checkCorbaFetchingCurveLinearMesh );
    CPPUNIT_TEST( checkCorbaFetching1SGTUMesh );
    CPPUNIT_TEST( checkCorbaFetching1DGTUMesh );
    CPPUNIT_TEST( checkCorbaField2DNTFetching );
    CPPUNIT_TEST( checkCorbaField2DNTMultiFetching );
    CPPUNIT_TEST( checkCorbaField2DNTMultiFetchingMT );
    CPPUNIT_TEST( checkCorbaFieldNode2DNTFetching );
    CPPUNIT_TEST( checkCorbaField3DNTFetching );
    CPPUNIT_TEST( checkCorbaField3DSurfWTFetching );
    CPPUNIT_TEST( checkCorbaField3DSurfCOTIFetching );
    CPPUNIT_TEST( checkCorbaField2DLTFetching );
    CPPUNIT_TEST( checkCorbaFieldGaussPt2DWTFetching );
    CPPUNIT_TEST( checkCorbaFieldGaussPtNE2DWTFetching );
    CPPUNIT_TEST( checkCorbaFieldVectorOnExtrudedWT );
    CPPUNIT_TEST( checkCorbaFieldVectorOnCMeshWT );
    CPPUNIT_TEST( checkCorbaFieldTemplateCellOn2D );
    CPPUNIT_TEST( checkCorbaFieldTemplateNodeOn2D );
    CPPUNIT_TEST( checkCorbaFieldTemplateGaussPtOn2D );
    CPPUNIT_TEST( checkCorbaFieldTemplateGaussNEOn2D );
    CPPUNIT_TEST( checkCorbaMultiFields1 );
    CPPUNIT_TEST( checkCorbaArrayDouble1 );
    CPPUNIT_TEST( checkCorbaArrayDouble2 );
    CPPUNIT_TEST( checkCorbaArrayDouble3 );
    CPPUNIT_TEST( checkCorbaArrayInt1 );
    CPPUNIT_TEST( checkCorbaArrayInt2 );
    CPPUNIT_TEST( checkCorbaArrayInt3 );
    CPPUNIT_TEST( checkCorbaFetchingCoords1 );
    CPPUNIT_TEST( checkCorbaMultiFields2 );
    CPPUNIT_TEST( shutdownServer );
    CPPUNIT_TEST_SUITE_END();
  public:
    void checkBaseCorbaFetching();
    void checkContentOfFetched1DMesh();
    void checkCorbaFetching2D();
    void checkContentOfFetched2DMesh();
    void checkMultiFetchingToTestMemoryManagement();
    void checkCorbaFetching3D();
    void checkContentOfFetched3DMesh();
    void checkCorbaFetching3DSurf();
    void checkContentOfFetched3DSurfMesh();
    void checkCorbaFetching0D();
    void checkCorbaFetchingM1D();
    void checkCorbaFetchingExtruded();
    void checkCorbaFetchingCMesh();
    void checkCorbaFetchingIMesh();
    void checkCorbaFetchingCurveLinearMesh();
    void checkCorbaFetching1SGTUMesh();
    void checkCorbaFetching1DGTUMesh();
    void checkCorbaField2DNTFetching();
    void checkCorbaField2DNTMultiFetching();
    void checkCorbaField2DNTMultiFetchingMT();
    void checkCorbaFieldNode2DNTFetching();
    void checkCorbaField3DNTFetching();
    void checkCorbaField3DSurfWTFetching();
    void checkCorbaField3DSurfCOTIFetching();
    void checkCorbaField2DLTFetching();
    void checkCorbaFieldGaussPt2DWTFetching();
    void checkCorbaFieldGaussPtNE2DWTFetching();
    void checkCorbaFieldVectorOnExtrudedWT();
    void checkCorbaFieldVectorOnCMeshWT();
    void checkCorbaFieldTemplateCellOn2D();
    void checkCorbaFieldTemplateNodeOn2D();
    void checkCorbaFieldTemplateGaussPtOn2D();
    void checkCorbaFieldTemplateGaussNEOn2D();
    void checkCorbaMultiFields1();
    void checkCorbaArrayDouble1();
    void checkCorbaArrayDouble2();
    void checkCorbaArrayDouble3();
    void checkCorbaArrayInt1();
    void checkCorbaArrayInt2();
    void checkCorbaArrayInt3();
    void checkCorbaFetchingCoords1();
    void checkCorbaMultiFields2();
    void shutdownServer();
  private:
    static void *checkCorbaField2DNTMultiFetchingMTStatic(void *stack);
  private:
    static SALOME_TEST::MEDCouplingMeshFieldFactory_ptr _objC;
    static MEDCoupling::MEDCouplingUMesh *_mesh_from_distant;
  public:
    static int _argc;
    static char **_argv;
    static CORBA::ORB_var _orb;
  };
}

#endif
