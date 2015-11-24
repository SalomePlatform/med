// Copyright (C) 2007-2015  CEA/DEN, EDF R&D
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

#include "ParaMEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include "CommInterface.hxx"
#include "ProcessorGroup.hxx"
#include "MPIProcessorGroup.hxx"
#include "Topology.hxx"
#include "OverlapDEC.hxx"
#include "ParaMESH.hxx"
#include "ParaFIELD.hxx"
#include "ComponentTopology.hxx"

#include "MEDCouplingUMesh.hxx"

#include <set>

using namespace std;

#include "MEDCouplingAutoRefCountObjectPtr.hxx"
#include "MEDLoader.hxx"
#include "MEDLoaderBase.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingRemapper.hxx"

using namespace ParaMEDMEM;

typedef  MEDCouplingAutoRefCountObjectPtr<MEDCouplingUMesh> MUMesh;
typedef  MEDCouplingAutoRefCountObjectPtr<MEDCouplingFieldDouble> MFDouble;

//void ParaMEDMEMTest::testOverlapDEC_LMEC_seq()
//{
//  //  T_SC_Trio_src.med  -- "SupportOf_"
//  //  T_SC_Trio_dst.med  -- "SupportOf_T_SC_Trio"
//  //  h_TH_Trio_src.med  -- "SupportOf_"
//  //  h_TH_Trio_dst.med  -- "SupportOf_h_TH_Trio"
//  string rep("/export/home/adrien/support/antoine_LMEC/");
//  string src_mesh_nam(rep + string("T_SC_Trio_src.med"));
//  string tgt_mesh_nam(rep + string("T_SC_Trio_dst.med"));
////  string src_mesh_nam(rep + string("h_TH_Trio_src.med"));
////  string tgt_mesh_nam(rep + string("h_TH_Trio_dst.med"));
//  MUMesh src_mesh=MEDLoader::ReadUMeshFromFile(src_mesh_nam,"SupportOf_",0);
//  MUMesh tgt_mesh=MEDLoader::ReadUMeshFromFile(tgt_mesh_nam,"SupportOf_T_SC_Trio",0);
////  MUMesh tgt_mesh=MEDLoader::ReadUMeshFromFile(tgt_mesh_nam,"SupportOf_h_TH_Trio",0);
//
//  MFDouble srcField = MEDCouplingFieldDouble::New(ON_CELLS, ONE_TIME);
//  srcField->setMesh(src_mesh);
//  DataArrayDouble * dad = DataArrayDouble::New(); dad->alloc(src_mesh->getNumberOfCells(),1);
//  dad->fillWithValue(1.0);
//  srcField->setArray(dad);
//  srcField->setNature(ConservativeVolumic);
//
//  MEDCouplingRemapper remap;
//  remap.setOrientation(2); // always consider surface intersections as absolute areas.
//  remap.prepare(src_mesh, tgt_mesh, "P0P0");
//  MFDouble tgtField = remap.transferField(srcField, 1.0e+300);
//  tgtField->setName("result");
//  string out_nam(rep + string("adrien.med"));
//  MEDLoader::WriteField(out_nam,tgtField, true);
//  cout << "wrote: " << out_nam << "\n";
//  double integ1 = 0.0, integ2 = 0.0;
//  srcField->integral(true, &integ1);
//  tgtField->integral(true, &integ2);
////  tgtField->reprQuickOverview(cout);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(integ1,integ2,1e-8);
//
//  dad->decrRef();
//}
//
//void ParaMEDMEMTest::testOverlapDEC_LMEC_para()
//{
//  using namespace ParaMEDMEM;
//
//  int size;
//  int rank;
//  MPI_Comm_size(MPI_COMM_WORLD,&size);
//  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
//
//  if (size != 1) return ;
//
//  int nproc = 1;
//  std::set<int> procs;
//
//  for (int i=0; i<nproc; i++)
//    procs.insert(i);
//
//  CommInterface interface;
//  OverlapDEC dec(procs);
//
//  ParaMESH* parameshS=0;
//  ParaMESH* parameshT=0;
//  ParaFIELD* parafieldS=0;
//  ParaFIELD* parafieldT=0;
//  MFDouble srcField;
//
//  // **** FILE LOADING
//  //  T_SC_Trio_src.med  -- "SupportOf_"
//  //  T_SC_Trio_dst.med  -- "SupportOf_T_SC_Trio"
//  //  h_TH_Trio_src.med  -- "SupportOf_"
//  //  h_TH_Trio_dst.med  -- "SupportOf_h_TH_Trio"
//  string rep("/export/home/adrien/support/antoine_LMEC/");
//  string src_mesh_nam(rep + string("T_SC_Trio_src.med"));
//  string tgt_mesh_nam(rep + string("T_SC_Trio_dst.med"));
//
//
//  MPI_Barrier(MPI_COMM_WORLD);
//  if(rank==0)
//    {
//    //  string src_mesh_nam(rep + string("h_TH_Trio_src.med"));
//    //  string tgt_mesh_nam(rep + string("h_TH_Trio_dst.med"));
//      MUMesh src_mesh=MEDLoader::ReadUMeshFromFile(src_mesh_nam,"SupportOf_",0);
//      MUMesh tgt_mesh=MEDLoader::ReadUMeshFromFile(tgt_mesh_nam,"SupportOf_T_SC_Trio",0);
//    //  MUMesh tgt_mesh=MEDLoader::ReadUMeshFromFile(tgt_mesh_nam,"SupportOf_h_TH_Trio",0);
//
//      // **** SOURCE
//      srcField = MEDCouplingFieldDouble::New(ON_CELLS, ONE_TIME);
//      srcField->setMesh(src_mesh);
//      DataArrayDouble * dad = DataArrayDouble::New(); dad->alloc(src_mesh->getNumberOfCells(),1);
//      dad->fillWithValue(1.0);
//      srcField->setArray(dad);
//      srcField->setNature(ConservativeVolumic);
//
//      ComponentTopology comptopo;
//      parameshS = new ParaMESH(src_mesh,*dec.getGroup(),"source mesh");
//      parafieldS = new ParaFIELD(ON_CELLS,ONE_TIME,parameshS,comptopo);
//      parafieldS->getField()->setNature(ConservativeVolumic);//IntegralGlobConstraint
//      parafieldS->getField()->setArray(dad);
//
//      // **** TARGET
//      parameshT=new ParaMESH(tgt_mesh,*dec.getGroup(),"target mesh");
//      parafieldT=new ParaFIELD(ON_CELLS,ONE_TIME,parameshT,comptopo);
//      parafieldT->getField()->setNature(ConservativeVolumic);//IntegralGlobConstraint
//      parafieldT->getField()->getArray()->fillWithValue(1.0e300);
////      valsT[0]=7.;
//    }
//  dec.setOrientation(2);
//  dec.attachSourceLocalField(parafieldS);
//  dec.attachTargetLocalField(parafieldT);
//  dec.synchronize();
//  dec.sendRecvData(true);
//  //
//  if(rank==0)
//    {
//      double integ1 = 0.0, integ2 = 0.0;
//      MEDCouplingFieldDouble * tgtField;
//
//      srcField->integral(true, &integ1);
//      tgtField = parafieldT->getField();
////      tgtField->reprQuickOverview(cout);
//      tgtField->integral(true, &integ2);
//      tgtField->setName("result");
//      string out_nam(rep + string("adrien_para.med"));
//      MEDLoader::WriteField(out_nam,tgtField, true);
//      cout << "wrote: " << out_nam << "\n";
//      CPPUNIT_ASSERT_DOUBLES_EQUAL(integ1,integ2,1e-8);
//    }
//  delete parafieldS;
//  delete parafieldT;
//  delete parameshS;
//  delete parameshT;
//
//  MPI_Barrier(MPI_COMM_WORLD);
//}

void prepareData1(int rank, ProcessorGroup * grp,
                                  MEDCouplingUMesh *& meshS, MEDCouplingUMesh *& meshT,
                                  ParaMESH*& parameshS, ParaMESH*& parameshT,
                                  ParaFIELD*& parafieldS, ParaFIELD*& parafieldT)
{
  if(rank==0)
    {
      const double coordsS[10]={0.,0.,0.5,0.,1.,0.,0.,0.5,0.5,0.5};
      const double coordsT[6]={0.,0.,1.,0.,1.,1.};
      meshS=MEDCouplingUMesh::New();
      meshS->setMeshDimension(2);
      DataArrayDouble *myCoords=DataArrayDouble::New();
      myCoords->alloc(5,2);
      std::copy(coordsS,coordsS+10,myCoords->getPointer());
      meshS->setCoords(myCoords);
      myCoords->decrRef();
      int connS[7]={0,3,4,1, 1,4,2};
      meshS->allocateCells(2);
      meshS->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,connS);
      meshS->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,connS+4);
      meshS->finishInsertingCells();
      ComponentTopology comptopo;
      parameshS=new ParaMESH(meshS, *grp,"source mesh");
      parafieldS=new ParaFIELD(ON_CELLS,NO_TIME,parameshS,comptopo);
      parafieldS->getField()->setNature(ConservativeVolumic);//IntegralGlobConstraint
      double *valsS=parafieldS->getField()->getArray()->getPointer();
      valsS[0]=7.; valsS[1]=8.;
      //
      meshT=MEDCouplingUMesh::New();
      meshT->setMeshDimension(2);
      myCoords=DataArrayDouble::New();
      myCoords->alloc(3,2);
      std::copy(coordsT,coordsT+6,myCoords->getPointer());
      meshT->setCoords(myCoords);
      myCoords->decrRef();
      int connT[3]={0,2,1};
      meshT->allocateCells(1);
      meshT->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,connT);
      meshT->finishInsertingCells();
      parameshT=new ParaMESH(meshT,*grp,"target mesh");
      parafieldT=new ParaFIELD(ON_CELLS,NO_TIME,parameshT,comptopo);
      parafieldT->getField()->setNature(ConservativeVolumic);//IntegralGlobConstraint
      double *valsT=parafieldT->getField()->getArray()->getPointer();
      valsT[0]=7.;
    }
  //
  if(rank==1)
    {
      const double coordsS[10]={1.,0.,0.5,0.5,1.,0.5,0.5,1.,1.,1.};
      const double coordsT[6]={0.,0.,0.5,0.5,0.,1.};
      meshS=MEDCouplingUMesh::New();
      meshS->setMeshDimension(2);
      DataArrayDouble *myCoords=DataArrayDouble::New();
      myCoords->alloc(5,2);
      std::copy(coordsS,coordsS+10,myCoords->getPointer());
      meshS->setCoords(myCoords);
      myCoords->decrRef();
      int connS[7]={0,1,2, 1,3,4,2};
      meshS->allocateCells(2);
      meshS->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,connS);
      meshS->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,connS+3);
      meshS->finishInsertingCells();
      ComponentTopology comptopo;
      parameshS=new ParaMESH(meshS,*grp,"source mesh");
      parafieldS=new ParaFIELD(ON_CELLS,NO_TIME,parameshS,comptopo);
      parafieldS->getField()->setNature(ConservativeVolumic);//IntegralGlobConstraint
      double *valsS=parafieldS->getField()->getArray()->getPointer();
      valsS[0]=9.;
      valsS[1]=11.;
      //
      meshT=MEDCouplingUMesh::New();
      meshT->setMeshDimension(2);
      myCoords=DataArrayDouble::New();
      myCoords->alloc(3,2);
      std::copy(coordsT,coordsT+6,myCoords->getPointer());
      meshT->setCoords(myCoords);
      myCoords->decrRef();
      int connT[3]={0,2,1};
      meshT->allocateCells(1);
      meshT->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,connT);
      meshT->finishInsertingCells();
      parameshT=new ParaMESH(meshT,*grp,"target mesh");
      parafieldT=new ParaFIELD(ON_CELLS,NO_TIME,parameshT,comptopo);
      parafieldT->getField()->setNature(ConservativeVolumic);//IntegralGlobConstraint
      double *valsT=parafieldT->getField()->getArray()->getPointer();
      valsT[0]=8.;
    }
  //
  if(rank==2)
    {
      const double coordsS[8]={0.,0.5, 0.5,0.5, 0.,1., 0.5,1.};
      const double coordsT[6]={0.5,0.5,0.,1.,1.,1.};
      meshS=MEDCouplingUMesh::New();
      meshS->setMeshDimension(2);
      DataArrayDouble *myCoords=DataArrayDouble::New();
      myCoords->alloc(4,2);
      std::copy(coordsS,coordsS+8,myCoords->getPointer());
      meshS->setCoords(myCoords);
      myCoords->decrRef();
      int connS[4]={0,2,3,1};
      meshS->allocateCells(1);
      meshS->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,connS);
      meshS->finishInsertingCells();
      ComponentTopology comptopo;
      parameshS=new ParaMESH(meshS,*grp,"source mesh");
      parafieldS=new ParaFIELD(ON_CELLS,NO_TIME,parameshS,comptopo);
      parafieldS->getField()->setNature(ConservativeVolumic);//IntegralGlobConstraint
      double *valsS=parafieldS->getField()->getArray()->getPointer();
      valsS[0]=10.;
      //
      meshT=MEDCouplingUMesh::New();
      meshT->setMeshDimension(2);
      myCoords=DataArrayDouble::New();
      myCoords->alloc(3,2);
      std::copy(coordsT,coordsT+6,myCoords->getPointer());
      meshT->setCoords(myCoords);
      myCoords->decrRef();
      int connT[3]={0,1,2};
      meshT->allocateCells(1);
      meshT->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,connT);
      meshT->finishInsertingCells();
      parameshT=new ParaMESH(meshT,*grp,"target mesh");
      parafieldT=new ParaFIELD(ON_CELLS,NO_TIME,parameshT,comptopo);
      parafieldT->getField()->setNature(ConservativeVolumic);//IntegralGlobConstraint
      double *valsT=parafieldT->getField()->getArray()->getPointer();
      valsT[0]=9.;
    }

}

/*! Test case from the official doc of the OverlapDEC.
 *  WARNING: bounding boxes are tweaked here to make the case more interesting (i.e. to avoid an all to all exchange
 *  between all procs).
 */
void ParaMEDMEMTest::testOverlapDEC1()
{
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  //  char hostname[256];
  //  printf("(%d) PID %d on localhost ready for attach\n", rank, getpid());
  //  fflush(stdout);

  if (size != 3) return ;

  int nproc = 3;
  std::set<int> procs;

  for (int i=0; i<nproc; i++)
    procs.insert(i);

  CommInterface interface;
  OverlapDEC dec(procs);
  ProcessorGroup * grp = dec.getGroup();
  MEDCouplingUMesh* meshS=0, *meshT=0;
  ParaMESH* parameshS=0, *parameshT=0;
  ParaFIELD* parafieldS=0, *parafieldT=0;

  MPI_Barrier(MPI_COMM_WORLD);
  prepareData1(rank, grp, meshS, meshT, parameshS, parameshT, parafieldS, parafieldT);

  /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   *  HACK ON BOUNDING BOX TO MAKE THIS CASE SIMPLE AND USABLE IN DEBUG
   * Bounding boxes are slightly smaller than should be thus localising the work to be done
   * and avoiding every proc talking to everyone else.
   * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   */
  dec.setBoundingBoxAdjustmentAbs(-1.0e-12);

  dec.attachSourceLocalField(parafieldS);
  dec.attachTargetLocalField(parafieldT);
  dec.synchronize();
  dec.sendRecvData(true);
  //
  if(rank==0)
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(8.75,parafieldT->getField()->getArray()->getIJ(0,0),1e-12);
    }
  if(rank==1)
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(8.5,parafieldT->getField()->getArray()->getIJ(0,0),1e-12);
    }
  if(rank==2)
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(10.5,parafieldT->getField()->getArray()->getIJ(0,0),1e-12);
    }
  delete parafieldS;
  delete parafieldT;
  delete parameshS;
  delete parameshT;
  meshS->decrRef();
  meshT->decrRef();

  MPI_Barrier(MPI_COMM_WORLD);
}

/*!
 * Same as testOverlapDEC1() but with regular bounding boxes. If you're looking for a nice debug case,
 * testOverlapDEC1() is more appropriate.
 */
void ParaMEDMEMTest::testOverlapDEC2()
{
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  if (size != 3) return ;

  int nproc = 3;
  std::set<int> procs;

  for (int i=0; i<nproc; i++)
    procs.insert(i);

  CommInterface interface;
  OverlapDEC dec(procs);
  ProcessorGroup * grp = dec.getGroup();
  MEDCouplingUMesh* meshS=0, *meshT=0;
  ParaMESH* parameshS=0, *parameshT=0;
  ParaFIELD* parafieldS=0, *parafieldT=0;

  MPI_Barrier(MPI_COMM_WORLD);
  prepareData1(rank, grp, meshS, meshT, parameshS, parameshT, parafieldS, parafieldT);

  /* Normal bounding boxes here: */
  dec.setBoundingBoxAdjustmentAbs(+1.0e-12);

  dec.attachSourceLocalField(parafieldS);
  dec.attachTargetLocalField(parafieldT);
  dec.synchronize();
  dec.sendRecvData(true);
  //
  if(rank==0)
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(8.75,parafieldT->getField()->getArray()->getIJ(0,0),1e-12);
    }
  if(rank==1)
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(8.5,parafieldT->getField()->getArray()->getIJ(0,0),1e-12);
    }
  if(rank==2)
    {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(10.5,parafieldT->getField()->getArray()->getIJ(0,0),1e-12);
    }
  delete parafieldS;
  delete parafieldT;
  delete parameshS;
  delete parameshT;
  meshS->decrRef();
  meshT->decrRef();

  MPI_Barrier(MPI_COMM_WORLD);
}

