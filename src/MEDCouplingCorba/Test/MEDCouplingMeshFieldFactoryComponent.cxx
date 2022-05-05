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

#include "MEDCouplingMeshFieldFactoryComponent.hxx"
#include "MEDCouplingMappedExtrudedMesh.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingFieldTemplate.hxx"
#include "MEDCouplingMultiFields.hxx"
#include "MEDCouplingFieldOverTime.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCoupling1GTUMesh.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingCMesh.hxx"
#include "MEDCouplingIMesh.hxx"
#include "MEDCouplingCurveLinearMesh.hxx"

#include <cmath>
#include <algorithm>

namespace SALOME_TEST
{
  MEDCoupling::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::build1DMesh()
  {
    double coords[4]={ 0.0, 0.3, 0.75, 1.0 };
    mcIdType conn[2*3]={ 0,1, 1,2, 2,3 };
    MEDCoupling::MEDCouplingUMesh *mesh=MEDCoupling::MEDCouplingUMesh::New("1DMeshForCorba",1);
    mesh->setDescription("build1DMesh");
    mesh->allocateCells(3);
    mesh->setTime(5.6,7,8);
    mesh->setTimeUnit("ms");
    mesh->insertNextCell(INTERP_KERNEL::NORM_SEG2,2,conn);
    mesh->insertNextCell(INTERP_KERNEL::NORM_SEG2,2,conn+2);
    mesh->insertNextCell(INTERP_KERNEL::NORM_SEG2,2,conn+4);
    mesh->finishInsertingCells();
    MEDCoupling::DataArrayDouble *myCoords=MEDCoupling::DataArrayDouble::New();
    myCoords->alloc(4,1);
    std::copy(coords,coords+4,myCoords->getPointer());
    mesh->setCoords(myCoords);
    myCoords->decrRef();
    mesh->changeSpaceDimension(3);
    myCoords=mesh->getCoords();
    myCoords->setInfoOnComponent(0,"X1D [m]");
    myCoords->setInfoOnComponent(1,"Y1D [dm]");
    myCoords->setInfoOnComponent(2,"Z1D [pm]");
    double center[3]={0.,0.,0.};
    double vector[3]={0,1,0};
    mesh->rotate(center,vector,-M_PI/2.);
    return mesh;
  }

  MEDCoupling::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::build2DMesh()
  {
    double targetCoords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
    mcIdType targetConn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
    MEDCoupling::MEDCouplingUMesh *targetMesh=MEDCoupling::MEDCouplingUMesh::New();
    targetMesh->setMeshDimension(2);
    targetMesh->setName("MyMesh2D");
    targetMesh->setDescription("build2DMesh");
    targetMesh->allocateCells(5);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,targetConn+4);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,targetConn+7);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn+10);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn+14);
    targetMesh->finishInsertingCells();
    MEDCoupling::DataArrayDouble *myCoords=MEDCoupling::DataArrayDouble::New();
    myCoords->alloc(9,2);
    std::copy(targetCoords,targetCoords+18,myCoords->getPointer());
    targetMesh->setCoords(myCoords);
    myCoords->decrRef();
    return targetMesh;
  }

  MEDCoupling::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::build3DMesh()
  {
    double targetCoords[81]={ 0., 0., 0., 50., 0., 0. , 200., 0., 0.  , 0., 50., 0., 50., 50., 0. , 200., 50., 0.,   0., 200., 0., 50., 200., 0. , 200., 200., 0. ,
                              0., 0., 50., 50., 0., 50. , 200., 0., 50.  , 0., 50., 50., 50., 50., 50. , 200., 50., 50.,   0., 200., 50., 50., 200., 50. , 200., 200., 50. ,
                              0., 0., 200., 50., 0., 200. , 200., 0., 200.  , 0., 50., 200., 50., 50., 200. , 200., 50., 200.,   0., 200., 200., 50., 200., 200. , 200., 200., 200. };
    mcIdType targetConn[64]={0,1,4,3,9,10,13,12, 1,2,5,4,10,11,14,13, 3,4,7,6,12,13,16,15, 4,5,8,7,13,14,17,16,
                        9,10,13,12,18,19,22,21, 10,11,14,13,19,20,23,22, 12,13,16,15,21,22,25,24, 13,14,17,16,22,23,26,25};
    MEDCoupling::MEDCouplingUMesh *targetMesh=MEDCoupling::MEDCouplingUMesh::New();
    targetMesh->setMeshDimension(3);
    targetMesh->setName("MyMesh3D");
    targetMesh->setDescription("build3DMesh");
    targetMesh->allocateCells(12);
    for(int i=0;i<8;i++)
      targetMesh->insertNextCell(INTERP_KERNEL::NORM_HEXA8,8,targetConn+8*i);
    targetMesh->finishInsertingCells();
    MEDCoupling::DataArrayDouble *myCoords=MEDCoupling::DataArrayDouble::New();
    myCoords->alloc(27,3);
    std::copy(targetCoords,targetCoords+81,myCoords->getPointer());
    targetMesh->setCoords(myCoords);
    myCoords->setName("check in case");
    myCoords->decrRef();
    return targetMesh;
  }

  MEDCoupling::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::build3DSurfMesh()
  {
    double targetCoords[27]={-0.3,-0.3,0.5, 0.2,-0.3,1., 0.7,-0.3,1.5, -0.3,0.2,0.5, 0.2,0.2,1., 0.7,0.2,1.5, -0.3,0.7,0.5, 0.2,0.7,1., 0.7,0.7,1.5};
    mcIdType targetConn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
    MEDCoupling::MEDCouplingUMesh *targetMesh=MEDCoupling::MEDCouplingUMesh::New();
    targetMesh->setMeshDimension(2);
    targetMesh->setName("MyMesh3DSurf");
    targetMesh->setDescription("build3DSurfMesh");
    targetMesh->allocateCells(5);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,targetConn+4);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,targetConn+7);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn+10);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn+14);
    targetMesh->finishInsertingCells();
    MEDCoupling::DataArrayDouble *myCoords=MEDCoupling::DataArrayDouble::New();
    myCoords->alloc(9,3);
    std::copy(targetCoords,targetCoords+27,myCoords->getPointer());
    targetMesh->setCoords(myCoords);
    myCoords->setInfoOnComponent(0,"X [m]");
    myCoords->setInfoOnComponent(1,"X [dm]");
    myCoords->setInfoOnComponent(2,"X [m]");
    myCoords->decrRef();
    return targetMesh;
  }

  MEDCoupling::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::build0DMesh()
  {
    double targetCoords[27]={-0.3,-0.3,0.5, 0.2,-0.3,1., 0.7,-0.3,1.5, -0.3,0.2,0.5, 0.2,0.2,1., 0.7,0.2,1.5, -0.3,0.7,0.5, 0.2,0.7,1., 0.7,0.7,1.5};
    const mcIdType targetConn[]={0,1,2,3,4,5,7,6};
    MEDCoupling::MEDCouplingUMesh *targetMesh=MEDCoupling::MEDCouplingUMesh::New();
    targetMesh->setMeshDimension(0);
    targetMesh->allocateCells(8);
    targetMesh->setName("Wonderful 0D mesh");
    targetMesh->setDescription("build0DMesh");
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+1);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+2);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+3);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+4);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+5);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+6);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+7);
    targetMesh->finishInsertingCells();
    MEDCoupling::DataArrayDouble *myCoords=MEDCoupling::DataArrayDouble::New();
    myCoords->alloc(9,3);
    std::copy(targetCoords,targetCoords+27,myCoords->getPointer());
    targetMesh->setCoords(myCoords);
    myCoords->setInfoOnComponent(0,"X [m]");
    myCoords->setInfoOnComponent(1,"YY [Pm]");
    myCoords->setInfoOnComponent(2,"ZZZ [m]");
    myCoords->decrRef();
    //
    targetMesh->checkConsistencyLight();
    return targetMesh;
  }

  MEDCoupling::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::buildM1DMesh()
  {
    MEDCoupling::MEDCouplingUMesh *meshM1D=MEDCoupling::MEDCouplingUMesh::New("wonderful -1 D mesh",-1);
    meshM1D->setDescription("buildM1DMesh");
    meshM1D->checkConsistencyLight();
    return meshM1D;
  }

  MEDCoupling::MEDCouplingMappedExtrudedMesh *MEDCouplingCorbaServBasicsTest::buildExtrudedMesh(MEDCoupling::MEDCouplingUMesh *&m2D)
  {
    m2D=build2DMesh();
    m2D->changeSpaceDimension(3);
    MEDCoupling::MEDCouplingUMesh *m1D=build1DMesh();
    MEDCoupling::MEDCouplingUMesh *retu=m2D->buildExtrudedMesh(m1D,0);
    m1D->decrRef();
    MEDCoupling::MEDCouplingMappedExtrudedMesh *ret=MEDCoupling::MEDCouplingMappedExtrudedMesh::New(retu,m2D,2);
    ret->setName("ExtrudedTestForCorbaTest");
    ret->setDescription("buildExtrudedMesh");
    retu->decrRef();
    return ret;
  }

  MEDCoupling::MEDCouplingCMesh *MEDCouplingCorbaServBasicsTest::buildCMesh()
  {
    MEDCoupling::MEDCouplingCMesh *targetMesh=MEDCoupling::MEDCouplingCMesh::New();
    targetMesh->setTime(2.3,4,5);
    targetMesh->setTimeUnit("us");
    targetMesh->setName("Example of CMesh");
    targetMesh->setDescription("buildCMesh");
    MEDCoupling::DataArrayDouble *a1=MEDCoupling::DataArrayDouble::New();
    a1->alloc(5,1);
    a1->setInfoOnComponent(0,"SmthX");
    const double a1Data[5]={3.,4.,5.,6.,7.};
    std::copy(a1Data,a1Data+5,a1->getPointer());
    MEDCoupling::DataArrayDouble *a2=MEDCoupling::DataArrayDouble::New();
    a2->alloc(6,1);
    a2->setInfoOnComponent(0,"SmthZ");
    const double a2Data[6]={2.78,3.,4.,5.,6.,7.};
    std::copy(a2Data,a2Data+6,a2->getPointer());
    //
    targetMesh->setCoordsAt(0,a1);
    targetMesh->setCoordsAt(1,a2);
    //
    a1->decrRef();
    a2->decrRef();
    //
    targetMesh->checkConsistencyLight();
    //
    return targetMesh;
  }
    

  MEDCoupling::MEDCouplingIMesh *MEDCouplingCorbaServBasicsTest::buildIMesh()
  {
    MEDCoupling::MEDCouplingIMesh *targetMesh(MEDCoupling::MEDCouplingIMesh::New());
    targetMesh->setTime(2.3,4,5);
    targetMesh->setTimeUnit("us");
    targetMesh->setName("Example of IMesh");
    targetMesh->setDescription("buildIMesh");
    mcIdType ns[3]={6,7,8};
    double orig[3]={4.25,3.75,-6.125};
    double inter[3]={0.5,0.375,0.75};
    targetMesh->setSpaceDimension(3);
    targetMesh->setNodeStruct(ns,ns+3);
    targetMesh->setOrigin(orig,orig+3);
    targetMesh->setDXYZ(inter,inter+3);
    targetMesh->setAxisUnit("mm");
    return targetMesh;
  }

  MEDCoupling::MEDCouplingCurveLinearMesh *MEDCouplingCorbaServBasicsTest::buildCLMesh()
  {
    MEDCoupling::MEDCouplingCurveLinearMesh *targetMesh=MEDCoupling::MEDCouplingCurveLinearMesh::New();
    targetMesh->setTime(2.3,4,5);
    targetMesh->setTimeUnit("us");
    targetMesh->setName("Example of Curve linear mesh");
    targetMesh->setDescription("buildCLMesh");
    MEDCoupling::DataArrayDouble *a1=MEDCoupling::DataArrayDouble::New();
    a1->alloc(3*20,1);
    a1->iota(7.);
    a1->rearrange(3);
    targetMesh->setCoords(a1);
    a1->decrRef();
    mcIdType structure[2]={4,5};
    targetMesh->setNodeGridStructure(structure,structure+2);
    //
    targetMesh->checkConsistencyLight();
    //
    return targetMesh;
  }

  MEDCoupling::MEDCoupling1SGTUMesh *MEDCouplingCorbaServBasicsTest::build1SGTUMesh()
  {
    MEDCoupling::MEDCoupling1SGTUMesh *targetMesh=MEDCoupling::MEDCoupling1SGTUMesh::New("Mesh1SGT",INTERP_KERNEL::NORM_QUAD4);
    targetMesh->setTime(2.3,44,-55);
    targetMesh->setTimeUnit("us");
    targetMesh->setDescription("My Description of 1SGTU");
    MEDCoupling::DataArrayDouble *a1=MEDCoupling::DataArrayDouble::New(); a1->alloc(10,3); a1->setInfoOnComponent(0,"X1 [m]");  a1->setInfoOnComponent(1,"YY2 [km]"); a1->setInfoOnComponent(2,"ZZZ3 [km]");
    const double coords[30]={1.,1.,0.,2.,1.,0.,3.,1.,0.,1.,0.,0.,2.,0.,0.,0.,0.,0.,0.,1.,0.,3.,0.,0.,4.,0.,0.,4.,1.,0.};
    const mcIdType conn[16]={6,0,3,5,3,0,1,4,1,2,7,4,8,7,2,9};
    std::copy(coords,coords+30,a1->getPointer()); targetMesh->setCoords(a1); a1->decrRef();
    MEDCoupling::DataArrayIdType *a2(MEDCoupling::DataArrayIdType::New()); a2->alloc(4*4,1);
    std::copy(conn,conn+16,a2->getPointer());
    targetMesh->setNodalConnectivity(a2); a2->decrRef();
    //
    targetMesh->checkConsistencyLight();
    //
    return targetMesh;
  }
  
  MEDCoupling::MEDCoupling1DGTUMesh *MEDCouplingCorbaServBasicsTest::build1DGTUMesh()
  {
    MEDCoupling::MEDCoupling1DGTUMesh *targetMesh=MEDCoupling::MEDCoupling1DGTUMesh::New("Mesh1DGT",INTERP_KERNEL::NORM_POLYGON);
    targetMesh->setTime(2.3,55,-66);
    targetMesh->setTimeUnit("us");
    targetMesh->setDescription("My Description of 1DGTU");
    MEDCoupling::DataArrayDouble *a1=MEDCoupling::DataArrayDouble::New(); a1->alloc(10,3); a1->setInfoOnComponent(0,"X1 [m]");  a1->setInfoOnComponent(1,"YY2 [km]"); a1->setInfoOnComponent(2,"ZZZ3 [km]");
    const double coords[30]={1.,1.,0.,2.,1.,0.,3.,1.,0.,1.,0.,0.,2.,0.,0.,0.,0.,0.,0.,1.,0.,3.,0.,0.,4.,0.,0.,4.,1.,0.};
    const mcIdType conn[15]={6,0,3,5,3,0,1,4,1,2,7,4,8,7,2};
    const mcIdType conni[5]={0,4,8,12,15};
    std::copy(coords,coords+30,a1->getPointer()); targetMesh->setCoords(a1); a1->decrRef();
    MEDCoupling::DataArrayIdType *a2(MEDCoupling::DataArrayIdType::New()); a2->alloc(15,1);
    std::copy(conn,conn+15,a2->getPointer());
    MEDCoupling::DataArrayIdType *a3(MEDCoupling::DataArrayIdType::New()); a3->alloc(5,1);
    std::copy(conni,conni+5,a3->getPointer());
    targetMesh->setNodalConnectivity(a2,a3); a2->decrRef(); a3->decrRef();
    //
    targetMesh->checkConsistencyLight();
    //
    return targetMesh;
  }

  MEDCoupling::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldScalarOn2DNT()
  {
    MEDCoupling::MEDCouplingUMesh *mesh=build2DMesh();
    MEDCoupling::MEDCouplingFieldDouble *fieldOnCells=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::NO_TIME);
    fieldOnCells->setTimeUnit("ms");
    fieldOnCells->setName("toto");
    fieldOnCells->setMesh(mesh);
    MEDCoupling::DataArrayDouble *array=MEDCoupling::DataArrayDouble::New();
    array->alloc(mesh->getNumberOfCells(),6);
    fieldOnCells->setArray(array);
    double *tmp=array->getPointer();
    array->decrRef();
    std::fill(tmp,tmp+mesh->getNumberOfCells()*6,7.);
    mesh->decrRef();
    fieldOnCells->checkConsistencyLight();
    return fieldOnCells;
  }

  MEDCoupling::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldNodeScalarOn2DNT()
  {
    MEDCoupling::MEDCouplingUMesh *mesh=build2DMesh();
    MEDCoupling::MEDCouplingFieldDouble *fieldOnNodes=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_NODES,MEDCoupling::NO_TIME);
    fieldOnNodes->setName("toto2");
    fieldOnNodes->setTimeUnit("s");
    fieldOnNodes->setDescription("my wonderful field toto2");
    fieldOnNodes->setMesh(mesh);
    MEDCoupling::DataArrayDouble *array=MEDCoupling::DataArrayDouble::New();
    array->alloc(mesh->getNumberOfNodes(),5);
    fieldOnNodes->setArray(array);
    double *tmp=array->getPointer();
    array->decrRef();
    std::fill(tmp,tmp+mesh->getNumberOfNodes()*5,7.1234);
    mesh->decrRef();
    fieldOnNodes->checkConsistencyLight();
    return fieldOnNodes;
  }

  MEDCoupling::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DNT()
  {
    MEDCoupling::MEDCouplingUMesh *mesh=build3DMesh();
    MEDCoupling::MEDCouplingFieldDouble *fieldOnCells=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::NO_TIME);
    fieldOnCells->setNature(MEDCoupling::IntensiveMaximum);
    fieldOnCells->setName("toto");
    fieldOnCells->setDescription("my wonderful 3D field toto2");
    fieldOnCells->setMesh(mesh);
    MEDCoupling::DataArrayDouble *array=MEDCoupling::DataArrayDouble::New();
    array->alloc(mesh->getNumberOfCells(),6);
    fieldOnCells->setArray(array);
    double *tmp=array->getPointer();
    array->decrRef();
    std::fill(tmp,tmp+mesh->getNumberOfCells()*6,7.);
    mesh->decrRef();
    fieldOnCells->checkConsistencyLight();
    return fieldOnCells;
  }

  MEDCoupling::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DSurfWT()
  {
    MEDCoupling::MEDCouplingUMesh *mesh=build3DSurfMesh();
    MEDCoupling::MEDCouplingFieldDouble *fieldOnCells=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::ONE_TIME);
    fieldOnCells->setName("toto25");
    fieldOnCells->setDescription("my wonderful 3D surf field toto25");
    fieldOnCells->setTimeUnit("us");
    fieldOnCells->setMesh(mesh);
    MEDCoupling::DataArrayDouble *array=MEDCoupling::DataArrayDouble::New();
    array->alloc(mesh->getNumberOfCells(),3);
    array->setInfoOnComponent(0,"aaa"); array->setInfoOnComponent(1,"bbbb"); array->setInfoOnComponent(2,"ccccc");
    fieldOnCells->setArray(array);
    double *tmp=array->getPointer();
    array->decrRef();
    std::fill(tmp,tmp+mesh->getNumberOfCells()*3,7.);
    mesh->decrRef();
    fieldOnCells->setTime(6.7,1,4);
    fieldOnCells->checkConsistencyLight();
    return fieldOnCells;
  }

  MEDCoupling::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DSurfCOTI()
  {
    MEDCoupling::MEDCouplingUMesh *mesh=build3DSurfMesh();
    MEDCoupling::MEDCouplingFieldDouble *fieldOnCells=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::CONST_ON_TIME_INTERVAL);
    fieldOnCells->setName("toto26");
    fieldOnCells->setDescription("my wonderful 3D surf field toto26");
    fieldOnCells->setTimeUnit("us");
    fieldOnCells->setMesh(mesh);
    MEDCoupling::DataArrayDouble *array=MEDCoupling::DataArrayDouble::New();
    array->alloc(mesh->getNumberOfCells(),3);
    fieldOnCells->setArray(array);
    double *tmp=array->getPointer();
    array->decrRef();
    std::fill(tmp,tmp+mesh->getNumberOfCells()*3,7.);
    mesh->decrRef();
    fieldOnCells->setStartTime(6.7,1,4);
    fieldOnCells->setEndTime(7.2,2,8);
    fieldOnCells->checkConsistencyLight();
    return fieldOnCells;
  }

  MEDCoupling::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldScalarOn2DLT()
  {
    MEDCoupling::MEDCouplingUMesh *mesh=build2DMesh();
    MEDCoupling::MEDCouplingFieldDouble *fieldOnCells=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::LINEAR_TIME);
    fieldOnCells->setName("toto27");
    fieldOnCells->setDescription("my wonderful 2D field toto27");
    fieldOnCells->setTimeUnit("ms");
    fieldOnCells->setMesh(mesh);
    MEDCoupling::DataArrayDouble *array1=MEDCoupling::DataArrayDouble::New();
    array1->alloc(mesh->getNumberOfCells(),4);
    fieldOnCells->setArray(array1);
    double *tmp=array1->getPointer();
    array1->decrRef();
    const double arr1[20]={1.2,1.02,1.002,1.0002, 3.4,3.04,3.004,3.0004, 5.6,5.06,5.006,5.0006, 7.8,7.08,7.008,7.0008, 9.1,9.01,9.001,9.0001};
    std::copy(arr1,arr1+20,tmp);
    MEDCoupling::DataArrayDouble *array2=MEDCoupling::DataArrayDouble::New();
    array2->alloc(mesh->getNumberOfCells(),4);
    fieldOnCells->setEndArray(array2);
    tmp=array2->getPointer();
    array2->decrRef();
    mesh->decrRef();
    const double arr2[20]={71.2,71.02,71.002,71.0002, 73.4,73.04,73.004,73.0004, 75.6,75.06,75.006,75.0006, 77.8,77.08,77.008,77.0008, 79.1,79.01,79.001,79.0001};
    std::copy(arr2,arr2+20,tmp);
    fieldOnCells->setStartTime(6.7,25,26);
    fieldOnCells->setEndTime(17.2,125,126);
    fieldOnCells->checkConsistencyLight();
    return fieldOnCells;
  }

  MEDCoupling::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldGaussPt2DWT()
  {
    const double _a=0.446948490915965;
    const double _b=0.091576213509771;
    const double _p1=0.11169079483905;
    const double _p2=0.0549758718227661;
    const double refCoo1[6]={ 0.,0., 1.,0., 0.,1. };
    const double gsCoo1[12]={ 2*_b-1, 1-4*_b, 2*_b-1, 2.07*_b-1, 1-4*_b,
                              2*_b-1, 1-4*_a, 2*_a-1, 2*_a-1, 1-4*_a, 2*_a-1, 2*_a-1 };
    const double wg1[6]={ 4*_p2, 4*_p2, 4*_p2, 4*_p1, 4*_p1, 4*_p1 };
    std::vector<double> _refCoo1(refCoo1,refCoo1+6);
    std::vector<double> _gsCoo1(gsCoo1,gsCoo1+12);
    std::vector<double> _wg1(wg1,wg1+6);
    MEDCoupling::MEDCouplingUMesh *m=build2DMesh();
    MEDCoupling::MEDCouplingFieldDouble *f=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_GAUSS_PT,MEDCoupling::ONE_TIME);
    f->setTime(6.7,1,4);
    f->setMesh(m);
    m->decrRef();
    f->setGaussLocalizationOnType(INTERP_KERNEL::NORM_TRI3,_refCoo1,_gsCoo1,_wg1);
    const double refCoo2[8]={ 0.,0., 1.,0., 1.,1., 0.,1. };
    std::vector<double> _refCoo2(refCoo2,refCoo2+8);
    _gsCoo1.resize(4); _wg1.resize(2);
    f->setGaussLocalizationOnType(INTERP_KERNEL::NORM_QUAD4,_refCoo2,_gsCoo1,_wg1);
    MEDCoupling::DataArrayDouble *array=MEDCoupling::DataArrayDouble::New();
    array->alloc(18,2);
    array->setInfoOnComponent(0,"Power [MW]");
    array->setInfoOnComponent(1,"Density [kg/m^3]");
    double *ptr=array->getPointer();
    for(int i=0;i<18*2;i++)
      ptr[i]=(double)(i+1);
    f->setArray(array);
    f->setName("MyFirstFieldOnGaussPoint");
    f->setTimeUnit("ms");
    f->setDescription("mmmmmmmmmmmm");
    array->decrRef();
    f->checkConsistencyLight();
    return f;
  }

  MEDCoupling::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldGaussPtNE2DWT()
  {
    MEDCoupling::MEDCouplingUMesh *m=build2DMesh();
    MEDCoupling::MEDCouplingFieldDouble *f=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_GAUSS_NE,MEDCoupling::ONE_TIME);
    f->setTime(6.8,11,8);
    f->setMesh(m);
    f->setTimeUnit("ms");
    f->setName("MyFirstFieldOnNE");
    f->setDescription("MyDescriptionNE");
    MEDCoupling::DataArrayDouble *array=MEDCoupling::DataArrayDouble::New();
    array->alloc(18,2);
    array->setInfoOnComponent(0,"Power [MW]");
    array->setInfoOnComponent(1,"Density [kg/m^3]");
    double *ptr=array->getPointer();
    for(int i=0;i<18*2;i++)
      ptr[i]=(double)(i+7);
    f->setArray(array);
    array->decrRef();
    //
    f->checkConsistencyLight();
    m->decrRef();
    return f;
  }

  MEDCoupling::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldVectorOnExtrudedWT()
  {
    MEDCoupling::MEDCouplingUMesh *m2D=0;
    MEDCoupling::MEDCouplingMappedExtrudedMesh *ext=buildExtrudedMesh(m2D);
    //
    MEDCoupling::MEDCouplingFieldDouble *f=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::ONE_TIME);
    f->setTime(6.8,11,8);
    f->setMesh(ext);
    f->setName("MyFieldOnExtruM");
    f->setDescription("desc of MyFiOnExtruM");
    MEDCoupling::DataArrayDouble *array=MEDCoupling::DataArrayDouble::New();
    int nbOfCells=ext->getNumberOfCells();
    array->alloc(nbOfCells,2);
    array->setInfoOnComponent(0,"Power [MW]");
    array->setInfoOnComponent(1,"Density [kg/m^3]");
    double *ptr=array->getPointer();
    for(int i=0;i<nbOfCells*2;i++)
      ptr[i]=(double)(i/2+7)+(double)((i%2)*1000);
    f->setArray(array);
    array->decrRef();
    //
    f->checkConsistencyLight();
    //
    m2D->decrRef();
    ext->decrRef();
    return f;
  }

  MEDCoupling::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldVectorOnCMeshWT()
  {
    MEDCoupling::MEDCouplingCMesh *m=buildCMesh();
    MEDCoupling::MEDCouplingFieldDouble *f=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::ONE_TIME);
    f->setTime(6.8,11,8);
    f->setMesh(m);
    m->decrRef();
    f->setName("MyFieldOnCMesh");
    f->setDescription("desc of MyFiOnCMesh");
    MEDCoupling::DataArrayDouble *array=MEDCoupling::DataArrayDouble::New();
    int nbOfCells=m->getNumberOfCells();
    array->alloc(nbOfCells,2);
    array->setInfoOnComponent(0,"Power [GW]");
    array->setInfoOnComponent(1,"Density [kg/m^3]");
    double *ptr=array->getPointer();
    for(int i=0;i<nbOfCells*2;i++)
      ptr[i]=(double)(i/2+7)+(double)((i%2)*1000);
    f->setArray(array);
    array->decrRef();
    //
    f->checkConsistencyLight();
    //
    return f;
  }

  MEDCoupling::MEDCouplingFieldTemplate *MEDCouplingCorbaServBasicsTest::buildFieldTemplateCellOn2D()
  {
    MEDCoupling::MEDCouplingFieldDouble *f1=buildFieldScalarOn2DNT();
    MEDCoupling::MEDCouplingFieldTemplate *f2=MEDCoupling::MEDCouplingFieldTemplate::New(*f1);
    f2->setNature(MEDCoupling::NoNature);
    f1->decrRef();
    return f2;
  }

  MEDCoupling::MEDCouplingFieldTemplate *MEDCouplingCorbaServBasicsTest::buildFieldTemplateNodeOn2D()
  {
    MEDCoupling::MEDCouplingFieldDouble *f1=buildFieldNodeScalarOn2DNT();
    MEDCoupling::MEDCouplingFieldTemplate *f2=MEDCoupling::MEDCouplingFieldTemplate::New(*f1);
    f2->setNature(MEDCoupling::IntensiveMaximum);
    f1->decrRef();
    return f2;
  }

  MEDCoupling::MEDCouplingFieldTemplate *MEDCouplingCorbaServBasicsTest::buildFieldTemplateGaussPtOn2D()
  {
    MEDCoupling::MEDCouplingFieldDouble *f1=buildFieldGaussPt2DWT();
    MEDCoupling::MEDCouplingFieldTemplate *f2=MEDCoupling::MEDCouplingFieldTemplate::New(*f1);
    f2->setNature(MEDCoupling::ExtensiveMaximum);
    f1->decrRef();
    return f2;
  }

  MEDCoupling::MEDCouplingFieldTemplate *MEDCouplingCorbaServBasicsTest::buildFieldTemplateGaussNEOn2D()
  {
    MEDCoupling::MEDCouplingFieldDouble *f1=buildFieldGaussPtNE2DWT();
    MEDCoupling::MEDCouplingFieldTemplate *f2=MEDCoupling::MEDCouplingFieldTemplate::New(*f1);
    f2->setNature(MEDCoupling::ExtensiveConservation);
    f1->decrRef();
    return f2;
  }

  MEDCoupling::MEDCouplingMultiFields *MEDCouplingCorbaServBasicsTest::buildMultiFields1()
  {
    MEDCoupling::MEDCouplingUMesh *m1=build2DMesh();
    m1->setName("m1");
    MEDCoupling::MEDCouplingUMesh *m2=build2DMesh();
    m2->setName("m2");
    const double vals0[]={-0.7,-1.,-2.,-3.,-4.};
    const double vals1[]={0.,1.,2.,3.,4.,0.1,0.2,0.3,0.4};
    const double vals1_1[]={170.,171.,172.,173.,174.,170.1,170.2,170.3,170.4};
    const double vals2[]={5.,6.,7.,8.,9.};
    const double vals4[]={15.,16.,17.,18.,19.};
    //
    MEDCoupling::DataArrayDouble *d0=MEDCoupling::DataArrayDouble::New(); d0->alloc(5,1); std::copy(vals0,vals0+5,d0->getPointer());
    MEDCoupling::DataArrayDouble *d1=MEDCoupling::DataArrayDouble::New(); d1->alloc(9,1); std::copy(vals1,vals1+9,d1->getPointer());
    MEDCoupling::DataArrayDouble *d1_1=MEDCoupling::DataArrayDouble::New(); d1_1->alloc(9,1); std::copy(vals1_1,vals1_1+9,d1_1->getPointer());
    MEDCoupling::DataArrayDouble *d2=MEDCoupling::DataArrayDouble::New(); d2->alloc(5,1); std::copy(vals2,vals2+5,d2->getPointer());
    MEDCoupling::DataArrayDouble *d4=MEDCoupling::DataArrayDouble::New(); d4->alloc(5,1); std::copy(vals4,vals4+5,d4->getPointer());
    //
    d0->setName("d0"); d1->setName("d1"); d1_1->setName("d1_1"); d2->setName("d2"); d4->setName("d4");
    d0->setInfoOnComponent(0,"c1");
    d1->setInfoOnComponent(0,"c6");
    d1_1->setInfoOnComponent(0,"c9");
    d2->setInfoOnComponent(0,"c5");
    d4->setInfoOnComponent(0,"c7");
    //
    MEDCoupling::MEDCouplingFieldDouble *f0=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::ONE_TIME);
    f0->setMesh(m1);
    f0->setArray(d0);
    f0->setTime(0.2,5,6);
    f0->setName("f0");
    MEDCoupling::MEDCouplingFieldDouble *f1=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_NODES,MEDCoupling::LINEAR_TIME);
    f1->setMesh(m1);
    std::vector<MEDCoupling::DataArrayDouble *> d1s(2); d1s[0]=d1; d1s[1]=d1_1;
    f1->setArrays(d1s);
    f1->setStartTime(0.7,7,8);
    f1->setEndTime(1.2,9,10);
    f1->setName("f1");
    MEDCoupling::MEDCouplingFieldDouble *f2=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::CONST_ON_TIME_INTERVAL);
    f2->setMesh(m2);
    f2->setArray(d2);
    f2->setTime(1.2,11,12);
    f2->setEndTime(1.5,13,14);
    f2->setName("f2");
    MEDCoupling::MEDCouplingFieldDouble *f3=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::ONE_TIME);
    f3->setMesh(m1);
    f3->setArray(d2);
    f3->setTime(1.7,15,16);
    f3->setName("f3");
    MEDCoupling::MEDCouplingFieldDouble *f4=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::NO_TIME);
    f4->setMesh(m2);
    f4->setArray(d4);
    f4->setName("f4");
    //
    std::vector<MEDCoupling::MEDCouplingFieldDouble *> fs(5);
    fs[0]=f0; fs[1]=f1; fs[2]=f2; fs[3]=f3; fs[4]=f4;
    MEDCoupling::MEDCouplingMultiFields *ret=MEDCoupling::MEDCouplingMultiFields::New(fs);
    //
    m1->decrRef();
    m2->decrRef();
    d0->decrRef();
    d1->decrRef();
    d1_1->decrRef();
    d2->decrRef();
    d4->decrRef();
    f0->decrRef();
    f1->decrRef();
    f2->decrRef();
    f3->decrRef();
    f4->decrRef();
    //
    return ret;
  }

  MEDCoupling::DataArrayDouble *MEDCouplingCorbaServBasicsTest::buildArrayDouble1()
  {
    MEDCoupling::DataArrayDouble *ret=MEDCoupling::DataArrayDouble::New();
    ret->alloc(4,3);
    const double vals[12]={2.4,3.2,5.6,9.6,47.6,20.4,24.6,278.1,2.01,3.3,2.4,9.4};
    std::copy(vals,vals+12,ret->getPointer());
    ret->setName("toto");
    ret->setInfoOnComponent(0,"sss");
    ret->setInfoOnComponent(1,"ppp");
    ret->setInfoOnComponent(2,"ttt");
    return ret;
  }

  MEDCoupling::DataArrayDouble *MEDCouplingCorbaServBasicsTest::buildArrayDouble2()
  {
    MEDCoupling::DataArrayDouble *ret=MEDCoupling::DataArrayDouble::New();
    ret->setName("titi");
    return ret;
  }

  MEDCoupling::DataArrayDouble *MEDCouplingCorbaServBasicsTest::buildArrayDouble3()
  {
    MEDCoupling::DataArrayDouble *ret=MEDCoupling::DataArrayDouble::New();
    ret->setName("titi");
    ret->alloc(0,3);
    ret->setInfoOnComponent(0,"sss");
    ret->setInfoOnComponent(1,"ppp");
    ret->setInfoOnComponent(2,"ttt");
    return ret;
  }

  MEDCoupling::DataArrayInt *MEDCouplingCorbaServBasicsTest::buildArrayInt1()
  {
    MEDCoupling::DataArrayInt *ret=MEDCoupling::DataArrayInt::New();
    ret->alloc(4,3);
    const int vals[12]={2,3,5,9,47,20,24,278,2,3,2,9};
    std::copy(vals,vals+12,ret->getPointer());
    ret->setName("toto");
    ret->setInfoOnComponent(0,"sss");
    ret->setInfoOnComponent(1,"ppp");
    ret->setInfoOnComponent(2,"ttt");
    return ret;
  }

  MEDCoupling::DataArrayInt *MEDCouplingCorbaServBasicsTest::buildArrayInt2()
  {
    MEDCoupling::DataArrayInt *ret=MEDCoupling::DataArrayInt::New();
    ret->setName("titi");
    return ret;
  }

  MEDCoupling::DataArrayInt *MEDCouplingCorbaServBasicsTest::buildArrayInt3()
  {
    MEDCoupling::DataArrayInt *ret=MEDCoupling::DataArrayInt::New();
    ret->setName("titi");
    ret->alloc(0,3);
    ret->setInfoOnComponent(0,"sss");
    ret->setInfoOnComponent(1,"ppp");
    ret->setInfoOnComponent(2,"ttt");
    return ret;
  }

  MEDCoupling::MEDCouplingFieldOverTime *MEDCouplingCorbaServBasicsTest::buildMultiFields2()
  {
    MEDCoupling::MEDCouplingUMesh *m1=build2DMesh();
    m1->setName("m1");
    MEDCoupling::MEDCouplingUMesh *m2=build2DMesh();
    m2->setName("m2");
    const double vals0[]={-0.7,-1.,-2.,-3.,-4.};
    const double vals1[]={0.,1.,2.,3.,4.};
    const double vals1_1[]={170.,171.,172.,173.,174.};
    const double vals2[]={5.,6.,7.,8.,9.};
    const double vals4[]={15.,16.,17.,18.,19.};
    //
    MEDCoupling::DataArrayDouble *d0=MEDCoupling::DataArrayDouble::New(); d0->alloc(5,1); std::copy(vals0,vals0+5,d0->getPointer());
    MEDCoupling::DataArrayDouble *d1=MEDCoupling::DataArrayDouble::New(); d1->alloc(5,1); std::copy(vals1,vals1+5,d1->getPointer());
    MEDCoupling::DataArrayDouble *d1_1=MEDCoupling::DataArrayDouble::New(); d1_1->alloc(5,1); std::copy(vals1_1,vals1_1+5,d1_1->getPointer());
    MEDCoupling::DataArrayDouble *d2=MEDCoupling::DataArrayDouble::New(); d2->alloc(5,1); std::copy(vals2,vals2+5,d2->getPointer());
    MEDCoupling::DataArrayDouble *d4=MEDCoupling::DataArrayDouble::New(); d4->alloc(5,1); std::copy(vals4,vals4+5,d4->getPointer());
    //
    d0->setName("d0"); d1->setName("d1"); d1_1->setName("d1_1"); d2->setName("d2"); d4->setName("d4");
    d0->setInfoOnComponent(0,"c1");
    d1->setInfoOnComponent(0,"c6");
    d1_1->setInfoOnComponent(0,"c9");
    d2->setInfoOnComponent(0,"c5");
    d4->setInfoOnComponent(0,"c7");
    //
    MEDCoupling::MEDCouplingFieldDouble *f0=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::ONE_TIME);
    f0->setMesh(m1);
    f0->setArray(d0);
    f0->setTime(0.2,5,6);
    f0->setName("f0");
    MEDCoupling::MEDCouplingFieldDouble *f1=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::LINEAR_TIME);
    f1->setMesh(m1);
    std::vector<MEDCoupling::DataArrayDouble *> d1s(2); d1s[0]=d1; d1s[1]=d1_1;
    f1->setArrays(d1s);
    f1->setStartTime(0.7,7,8);
    f1->setEndTime(1.2,9,10);
    f1->setName("f1");
    MEDCoupling::MEDCouplingFieldDouble *f2=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::CONST_ON_TIME_INTERVAL);
    f2->setMesh(m2);
    f2->setArray(d2);
    f2->setTime(1.2,11,12);
    f2->setEndTime(1.5,13,14);
    f2->setName("f2");
    MEDCoupling::MEDCouplingFieldDouble *f3=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::ONE_TIME);
    f3->setMesh(m1);
    f3->setArray(d2);
    f3->setTime(1.7,15,16);
    f3->setName("f3");
    MEDCoupling::MEDCouplingFieldDouble *f4=MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::ONE_TIME);
    f4->setMesh(m2);
    f4->setArray(d4);
    f4->setName("f4");
    f4->setTime(2.7,25,26);
    //
    std::vector<MEDCoupling::MEDCouplingFieldDouble *> fs(5);
    fs[0]=f0; fs[1]=f1; fs[2]=f2; fs[3]=f3; fs[4]=f4;
    MEDCoupling::MEDCouplingFieldOverTime *ret=MEDCoupling::MEDCouplingFieldOverTime::New(fs);
    ret->checkConsistencyLight();
    //
    m1->decrRef();
    m2->decrRef();
    d0->decrRef();
    d1->decrRef();
    d1_1->decrRef();
    d2->decrRef();
    d4->decrRef();
    f0->decrRef();
    f1->decrRef();
    f2->decrRef();
    f3->decrRef();
    f4->decrRef();
    //
    return ret;
  }

  std::string MEDCouplingCorbaServBasicsTest::buildFileNameForIOR()
  {
    std::string tmpdir;
    if (getenv("TMP"))
      tmpdir = getenv("TMP");
    if (tmpdir == "")
      tmpdir = "/tmp";

    std::string username;
    if ( getenv("USERNAME") )
      username = std::string(getenv("USERNAME"))+"_";
    else if ( getenv("USER") )
      username = std::string(getenv("USER"))+"_";

    std::string ret = tmpdir+"/"+username+"entryPointMEDCouplingCorba.ior";
    return ret;
  }
}
