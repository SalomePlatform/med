# Copyright (C) 2007-2025  CEA, EDF
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author : Anthony Geay (CEA/DEN)

import unittest
import CORBA
import MEDCouplingCorbaServantTest_idl
import MEDCouplingCorbaSwigTest
from salome.MEDCouplingClient import *
from salome.kernel import MEDCouplingCorbaServant_idl
import threading

class MEDCouplingCorbaServBasicsTestClt(unittest.TestCase):
    def testBaseCorbaFetching(self):
        self.assertTrue(not CORBA.is_nil(self._objC))
        pass

    def testContentOfFetched1DMesh(self):
        meshPtr=self._objC.get1DMesh();
        self.assertEqual("1DMeshForCorba",meshPtr.getName());
        _mesh_from_distant=MEDCouplingMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        self.assertTrue(isinstance(_mesh_from_distant,MEDCouplingUMesh))
        self.assertTrue(_mesh_from_distant.getSpaceDimension()==3);
        self.assertTrue(_mesh_from_distant.getMeshDimension()==1);
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build1DMesh()
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass

    def testCorbaFetching2D(self):
        meshPtr=self._objC.get2DMesh();
        _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        self.assertTrue(_mesh_from_distant.getSpaceDimension()==2);
        self.assertTrue(_mesh_from_distant.getMeshDimension()==2);
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build2DMesh()
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass
    
    def testContentOfFetched2DMesh(self):
        pass
    
    def testMultiFetchingToTestMemoryManagement(self):
        for i in range(1000):
            meshPtr=self._objC.get2DMesh();
            _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
            meshPtr.UnRegister();
            test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
            meshRef=test.build2DMesh();
            self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
            pass
        pass
    
    def testCorbaFetching3D(self):
        meshPtr=self._objC.get3DMesh();
        _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        self.assertTrue(3==_mesh_from_distant.getSpaceDimension());
        self.assertTrue(3==_mesh_from_distant.getMeshDimension());
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build3DMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass
    
    def testContentOfFetched3DMesh(self):
        pass
    
    def testCorbaFetching3DSurf(self):
        meshPtr=self._objC.get3DSurfMesh();
        _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        self.assertTrue(3==_mesh_from_distant.getSpaceDimension());
        self.assertTrue(2==_mesh_from_distant.getMeshDimension());
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build3DSurfMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass
    
    def testContentOfFetched3DSurfMesh(self):
        pass
    
    def testCorbaFetching0D(self):
        meshPtr=self._objC.get0DMesh();
        _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        self.assertTrue(3==_mesh_from_distant.getSpaceDimension());
        self.assertTrue(0==_mesh_from_distant.getMeshDimension());
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build0DMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass
    
    def testCorbaFetchingM1D(self):
        meshPtr=self._objC.getM1DMesh();
        _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.buildM1DMesh();
        self.assertTrue(-1==_mesh_from_distant.getMeshDimension());
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass

    def testCorbaFetchingExtruded(self):
        meshPtr=self._objC.getExtrudedMesh();
        _mesh_from_distant=MEDCouplingExtrudedMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.buildExtrudedMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12))
        pass

    def testCorbaFetchingCMesh(self):
        meshPtr=self._objC.getCMesh();
        _mesh_from_distant=MEDCouplingCMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.buildCMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12))
        pass

    def testCorbaFetchingIMesh(self):
        meshPtr=self._objC.getIMesh();
        _mesh_from_distant=MEDCouplingIMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.buildIMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12))
        pass

    def testCorbaFetchingCLMesh(self):
        meshPtr=self._objC.getCLMesh();
        _mesh_from_distant=MEDCouplingCurveLinearMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.buildCLMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12))
        pass

    def testCorbaFetching1SGTUMesh(self):
        meshPtr=self._objC.get1SGTUMesh();
        _mesh_from_distant=MEDCoupling1SGTUMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build1SGTUMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12))
        pass

    def testCorbaFetching1DGTUMesh(self):
        meshPtr=self._objC.get1DGTUMesh();
        _mesh_from_distant=MEDCoupling1DGTUMeshClient.New(meshPtr);
        meshPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build1DGTUMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12))
        pass
    
    def testCorbaField2DNTFetching(self):
        fieldPtr=self._objC.getFieldScalarOn2DNT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldScalarOn2DNT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass
    
    def testCorbaField2DNTMultiFetching(self):
        fieldPtr=self._objC.getFieldScalarOn2DNT();
        fieldCpp1=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldCpp2=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.UnRegister();
        self.assertTrue(fieldCpp1.isEqual(fieldCpp2,1.e-12,1.e-15))
        pass

    def corbaField2DNTMFMTThread(self,i,fieldPtr,li):
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        li[i]=fieldCpp
        pass
    
    def testCorbaField2DNTMultiFetchingMT(self):
        li=8*[None]
        th=8*[None]
        fieldPtr=self._objC.getFieldScalarOn2DNT();
        for i in range(8):
            th[i]=threading.Thread(None,self.corbaField2DNTMFMTThread,"Test"+str(i),(i,fieldPtr,li))
            th[i].start()
            pass
        for i in range(8):
            th[i].join()
            pass
        for i in range(8-1):
            self.assertTrue(li[i].isEqual(li[i+1],1.e-12,1.e-15));
            pass
        fieldPtr.UnRegister()
        pass
    
    def testCorbaFieldNode2DNTFetching(self):
        fieldPtr=self._objC.getFieldNodeScalarOn2DNT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldNodeScalarOn2DNT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass
    
    def testCorbaField3DNTFetching(self):
        fieldPtr=self._objC.getFieldScalarOn3DNT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldScalarOn3DNT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass
    
    def testCorbaField3DSurfWTFetching(self):
        fieldPtr=self._objC.getFieldScalarOn3DSurfWT();
        self.assertEqual("toto25",fieldPtr.getName());
        self.assertEqual(["aaa","bbbb","ccccc"],fieldPtr.getInfoOnComponents());
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldScalarOn3DSurfWT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass
    
    def testCorbaField3DSurfCOTIFetching(self):
        fieldPtr=self._objC.getFieldScalarOn3DSurfCOTI();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldScalarOn3DSurfCOTI();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass

    def testCorbaField2DLTFetching(self):
        fieldPtr=self._objC.getFieldScalarOn2DLT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldScalarOn2DLT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass

    def testCorbaFieldGaussPt2DWTFetching(self):
        fieldPtr=self._objC.getFieldGaussPt2DWT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldGaussPt2DWT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass
    
    def testCorbaFieldGaussPtNE2DWTFetching(self):
        fieldPtr=self._objC.getFieldGaussPtNE2DWT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldGaussPtNE2DWT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass

    def testCorbaFieldVectorOnExtrudedWT(self):
        fieldPtr=self._objC.getFieldVectorOnExtrudedWT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldVectorOnExtrudedWT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass

    def testCorbaFieldVectorOnCMeshWT(self):
        fieldPtr=self._objC.getFieldVectorOnCMeshWT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldVectorOnCMeshWT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass

    def testCorbaFieldTemplateCellOn2D(self):
        fieldPtr=self._objC.getFieldTemplateCellOn2D();
        fieldCpp=MEDCouplingFieldTemplateClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldTemplateCellOn2D();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12));
        pass
    
    def testCorbaFieldTemplateNodeOn2D(self):
        fieldPtr=self._objC.getFieldTemplateNodeOn2D();
        fieldCpp=MEDCouplingFieldTemplateClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldTemplateNodeOn2D();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12));
        pass
    
    def testCorbaFieldTemplateGaussPtOn2D(self):
        fieldPtr=self._objC.getFieldTemplateGaussPtOn2D();
        fieldCpp=MEDCouplingFieldTemplateClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldTemplateGaussPtOn2D();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12));
        pass
    
    def testCorbaFieldTemplateGaussNEOn2D(self):
        fieldPtr=self._objC.getFieldTemplateGaussNEOn2D();
        fieldCpp=MEDCouplingFieldTemplateClient.New(fieldPtr);
        fieldPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldTemplateGaussNEOn2D();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12));
        pass

    def testCorbaMultiFields1(self):
        fieldPtr=self._objC.getMultiFields1()
        fieldCpp=MEDCouplingMultiFieldsClient.New(fieldPtr);
        fieldPtr.UnRegister()
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildMultiFields1();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass

    def testCorbaArrayDouble1(self):
        fieldPtr=self._objC.getArrayDouble1()
        self.assertEqual("toto",fieldPtr.getName());
        self.assertEqual(["sss","ppp","ttt"],fieldPtr.getInfoOnComponents());
        fieldCpp=DataArrayDoubleClient.New(fieldPtr)
        fieldPtr.UnRegister()
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildArrayDouble1();
        self.assertTrue(fieldCpp.isEqual(refField,1e-12))
        pass
    
    def testCorbaArrayDouble2(self):
        fieldPtr=self._objC.getArrayDouble2()
        fieldCpp=DataArrayDoubleClient.New(fieldPtr)
        fieldPtr.UnRegister()
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildArrayDouble2();
        self.assertTrue(fieldCpp.isEqual(refField,1e-12))
        pass

    def testCorbaArrayDouble3(self):
        fieldPtr=self._objC.getArrayDouble3()
        fieldCpp=DataArrayDoubleClient.New(fieldPtr)
        fieldPtr.UnRegister()
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildArrayDouble3();
        self.assertTrue(fieldCpp.isEqual(refField,1e-12))
        pass

    def testCorbaArrayInt1(self):
        fieldPtr=self._objC.getArrayInt1()
        fieldCpp=DataArrayIntClient.New(fieldPtr)
        fieldPtr.UnRegister()
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildArrayInt1();
        self.assertTrue(fieldCpp.isEqual(refField))
        pass

    def testCorbaArrayInt2(self):
        fieldPtr=self._objC.getArrayInt2()
        fieldCpp=DataArrayIntClient.New(fieldPtr)
        fieldPtr.UnRegister()
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildArrayInt2();
        self.assertTrue(fieldCpp.isEqual(refField))
        pass

    def testCorbaArrayInt3(self):
        fieldPtr=self._objC.getArrayInt3()
        fieldCpp=DataArrayIntClient.New(fieldPtr)
        fieldPtr.UnRegister()
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildArrayInt3();
        self.assertTrue(fieldCpp.isEqual(refField))
        pass

    def testCorbaFetchingCoords1(self):
        meshPtr=self._objC.get3DSurfMesh();
        cooPtr=meshPtr.getCoords();
        meshPtr.UnRegister();
        cooCpp=DataArrayDoubleClient.New(cooPtr);
        cooPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build3DSurfMesh();
        #
        self.assertTrue(meshRef.getCoords().isEqual(cooCpp,1e-12));
        pass

    def testCorbaMultiFields2(self):
        fotPtr=self._objC.getMultiFields2();
        fotc=MEDCouplingFieldOverTimeClient.New(fotPtr);
        fotPtr.UnRegister();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildMultiFields2()
        #
        self.assertTrue(fotc.isEqual(refField,1.e-12,1.e-15));
        ts=fotc.getTimeSteps();
        self.assertEqual(6,len(ts));
        expected=[0.2,0.7,1.2,1.35,1.7,2.7];
        for i in range(6):
            self.assertAlmostEqual(expected[i],ts[i],12);
        pass
    
    def testShutdownServer(self):
        self._objC.shutdownOrb()
        pass
    
    def setUp(self):
        self._orb=CORBA.ORB_init([''])      
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        f=file(test.buildFileNameForIOR(),"r")
        ior=f.read()
        self._objC=self._orb.string_to_object(ior);
        pass
    pass

unittest.main()
