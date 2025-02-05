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

from medcoupling import *
import math
import os

class MEDCouplingCorbaServBasicsTest:
    def build1DMesh(self):
        coords=[ 0.0, 0.3, 0.75, 1.0 ]
        conn=[ 0,1, 1,2, 2,3 ]
        mesh=MEDCouplingUMesh.New("1DMeshForCorba",1);
        mesh.setDescription("build1DMesh");
        mesh.allocateCells(3);
        mesh.setTime(5.6,7,8);
        mesh.setTimeUnit("ms");
        mesh.insertNextCell(NORM_SEG2,2,conn[0:2]);
        mesh.insertNextCell(NORM_SEG2,2,conn[2:4]);
        mesh.insertNextCell(NORM_SEG2,2,conn[4:6]);
        mesh.finishInsertingCells();
        myCoords=DataArrayDouble.New();
        myCoords.setValues(coords,4,1);
        mesh.setCoords(myCoords);
        mesh.changeSpaceDimension(3);
        myCoords=mesh.getCoords();
        myCoords.setInfoOnComponent(0,"X1D [m]");
        myCoords.setInfoOnComponent(1,"Y1D [dm]");
        myCoords.setInfoOnComponent(2,"Z1D [pm]");
        center=[0.,0.,0.]
        vector=[0.,1.,0.]
        mesh.rotate(center,vector,-math.pi/2.);
        return mesh
        
    def build2DMesh(self):
        targetCoords=[-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 ];
        targetConn=[0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4]
        targetMesh=MEDCouplingUMesh.New();
        targetMesh.setMeshDimension(2);
        targetMesh.setName("MyMesh2D");
        targetMesh.setDescription("build2DMesh");
        targetMesh.allocateCells(5);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[0:4]);
        targetMesh.insertNextCell(NORM_TRI3,3,targetConn[4:7]);
        targetMesh.insertNextCell(NORM_TRI3,3,targetConn[7:10]);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[10:14]);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[14:18]);
        targetMesh.finishInsertingCells();
        myCoords=DataArrayDouble.New();
        myCoords.setValues(targetCoords,9,2);
        targetMesh.setCoords(myCoords);
        return targetMesh;
    
    def build3DMesh(self):
        targetCoords=[ 0., 0., 0., 50., 0., 0. , 200., 0., 0.  , 0., 50., 0., 50., 50., 0. , 200., 50., 0.,   0., 200., 0., 50., 200., 0. , 200., 200., 0. ,
                       0., 0., 50., 50., 0., 50. , 200., 0., 50.  , 0., 50., 50., 50., 50., 50. , 200., 50., 50.,   0., 200., 50., 50., 200., 50. , 200., 200., 50. ,
                       0., 0., 200., 50., 0., 200. , 200., 0., 200.  , 0., 50., 200., 50., 50., 200. , 200., 50., 200.,   0., 200., 200., 50., 200., 200. , 200., 200., 200. ];
        targetConn=[0,1,4,3,9,10,13,12, 1,2,5,4,10,11,14,13, 3,4,7,6,12,13,16,15, 4,5,8,7,13,14,17,16,
                    9,10,13,12,18,19,22,21, 10,11,14,13,19,20,23,22, 12,13,16,15,21,22,25,24, 13,14,17,16,22,23,26,25];
        targetMesh=MEDCouplingUMesh.New();
        targetMesh.setMeshDimension(3);
        targetMesh.setName("MyMesh3D");
        targetMesh.setDescription("build3DMesh");
        targetMesh.allocateCells(12);
        for i in range(8):
            targetMesh.insertNextCell(NORM_HEXA8,8,targetConn[8*i:8*(i+1)]);
            pass
        targetMesh.finishInsertingCells();
        myCoords=DataArrayDouble.New();
        myCoords.setValues(targetCoords,27,3);
        targetMesh.setCoords(myCoords)
        myCoords.setName("check in case")
        return targetMesh;

    def build3DSurfMesh(self):
        targetCoords=[-0.3,-0.3,0.5, 0.2,-0.3,1., 0.7,-0.3,1.5, -0.3,0.2,0.5, 0.2,0.2,1., 0.7,0.2,1.5, -0.3,0.7,0.5, 0.2,0.7,1., 0.7,0.7,1.5];
        targetConn=[0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4];
        targetMesh=MEDCouplingUMesh.New();
        targetMesh.setMeshDimension(2);
        targetMesh.setName("MyMesh3DSurf");
        targetMesh.setDescription("build3DSurfMesh");
        targetMesh.allocateCells(5);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[0:4]);
        targetMesh.insertNextCell(NORM_TRI3,3,targetConn[4:7]);
        targetMesh.insertNextCell(NORM_TRI3,3,targetConn[7:10]);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[10:14]);
        targetMesh.insertNextCell(NORM_QUAD4,4,targetConn[14:18]);
        targetMesh.finishInsertingCells();
        myCoords=DataArrayDouble.New();
        myCoords.setValues(targetCoords,9,3);
        targetMesh.setCoords(myCoords);
        myCoords.setInfoOnComponent(0,"X [m]");
        myCoords.setInfoOnComponent(1,"X [dm]");
        myCoords.setInfoOnComponent(2,"X [m]");
        return targetMesh;

    def build0DMesh(self):
        targetCoords=[-0.3,-0.3,0.5, 0.2,-0.3,1., 0.7,-0.3,1.5, -0.3,0.2,0.5, 0.2,0.2,1., 0.7,0.2,1.5, -0.3,0.7,0.5, 0.2,0.7,1., 0.7,0.7,1.5];
        targetMesh=MEDCouplingUMesh.New();
        targetMesh.setMeshDimension(0);
        targetMesh.allocateCells(8);
        targetMesh.setName("Wonderful 0D mesh");
        targetMesh.setDescription("build0DMesh");
        targetConn=[]
        targetMesh.insertNextCell(NORM_POINT1,1,[0]);
        targetMesh.insertNextCell(NORM_POINT1,1,[1]);
        targetMesh.insertNextCell(NORM_POINT1,1,[2]);
        targetMesh.insertNextCell(NORM_POINT1,1,[3]);
        targetMesh.insertNextCell(NORM_POINT1,1,[4]);
        targetMesh.insertNextCell(NORM_POINT1,1,[5]);
        targetMesh.insertNextCell(NORM_POINT1,1,[7]);
        targetMesh.insertNextCell(NORM_POINT1,1,[6]);
        targetMesh.finishInsertingCells();
        myCoords=DataArrayDouble.New();
        myCoords.setValues(targetCoords,9,3);
        targetMesh.setCoords(myCoords);
        myCoords.setInfoOnComponent(0,"X [m]");
        myCoords.setInfoOnComponent(1,"YY [Pm]");
        myCoords.setInfoOnComponent(2,"ZZZ [m]");
        targetMesh.checkConsistencyLight();
        return targetMesh;

    def buildM1DMesh(self):
        meshM1D=MEDCouplingUMesh.New("wonderful -1 D mesh",-1);
        meshM1D.setDescription("buildM1DMesh");
        meshM1D.checkConsistencyLight();
        return meshM1D;

    def buildExtrudedMesh(self):
        m2D=self.build2DMesh();
        m2D.changeSpaceDimension(3);
        m1D=self.build1DMesh();
        retu=m2D.buildExtrudedMesh(m1D,0);
        ret=MEDCouplingMappedExtrudedMesh.New(retu,m2D,2);
        ret.setName("ExtrudedTestForCorbaTest");
        ret.setDescription("buildExtrudedMesh");
        return ret;

    def buildCMesh(self):
        targetMesh=MEDCouplingCMesh.New();
        targetMesh.setTime(2.3,4,5);
        targetMesh.setTimeUnit("us");
        targetMesh.setName("Example of CMesh");
        targetMesh.setDescription("buildCMesh");
        a1=DataArrayDouble.New();
        a1Data=[3.,4.,5.,6.,7.]
        a1.setValues(a1Data,5,1);
        a1.setInfoOnComponent(0,"SmthX");
        a2=DataArrayDouble.New();
        a2Data=[2.78,3.,4.,5.,6.,7.]
        a2.setValues(a2Data,6,1);
        a2.setInfoOnComponent(0,"SmthZ");
        #
        targetMesh.setCoordsAt(0,a1);
        targetMesh.setCoordsAt(1,a2);
        #
        #
        targetMesh.checkConsistencyLight();
        #
        return targetMesh;

    def buildIMesh(self):
        targetMesh=MEDCouplingIMesh.New();
        targetMesh.setTime(2.3,4,5);
        targetMesh.setTimeUnit("us");
        targetMesh.setName("Example of IMesh");
        targetMesh.setDescription("buildIMesh");
        #
        targetMesh.setSpaceDimension(3);
        targetMesh.setNodeStruct([6,7,8]);
        targetMesh.setOrigin([4.25,3.75,-6.125]);
        targetMesh.setDXYZ([0.5,0.375,0.75]);
        targetMesh.setAxisUnit("mm");
        #
        return targetMesh

    def buildCLMesh(self):
        targetMesh=MEDCouplingCurveLinearMesh();
        targetMesh.setTime(2.3,4,5);
        targetMesh.setTimeUnit("us");
        targetMesh.setName("Example of Curve linear mesh");
        targetMesh.setDescription("buildCLMesh");
        a1=DataArrayDouble(3*20,1);
        a1.iota(7.) ; a1.rearrange(3);
        targetMesh.setCoords(a1);
        targetMesh.setNodeGridStructure([4,5]);
        #
        targetMesh.checkConsistencyLight();
        #
        return targetMesh;

    def build1SGTUMesh(self):
        targetMesh=MEDCoupling1SGTUMesh("Mesh1SGT",NORM_QUAD4)
        targetMesh.setTime(2.3,44,-55)
        targetMesh.setTimeUnit("us")
        targetMesh.setDescription("My Description of 1SGTU");
        a1=DataArrayDouble([1.,1.,0.,2.,1.,0.,3.,1.,0.,1.,0.,0.,2.,0.,0.,0.,0.,0.,0.,1.,0.,3.,0.,0.,4.,0.,0.,4.,1.,0.],10,3)
        a1.setInfoOnComponents(["X1 [m]","YY2 [km]","ZZZ3 [km]"])
        targetMesh.setCoords(a1)
        a2=DataArrayInt([6,0,3,5,3,0,1,4,1,2,7,4,8,7,2,9],4*4,1)
        targetMesh.setNodalConnectivity(a2)
        #
        targetMesh.checkConsistencyLight();
        #
        return targetMesh;

    def build1DGTUMesh(self):
        targetMesh=MEDCoupling1DGTUMesh("Mesh1DGT",NORM_POLYGON);
        targetMesh.setTime(2.3,55,-66)
        targetMesh.setTimeUnit("us")
        targetMesh.setDescription("My Description of 1DGTU");
        a1=DataArrayDouble([1.,1.,0.,2.,1.,0.,3.,1.,0.,1.,0.,0.,2.,0.,0.,0.,0.,0.,0.,1.,0.,3.,0.,0.,4.,0.,0.,4.,1.,0.],10,3)
        a1.setInfoOnComponents(["X1 [m]","YY2 [km]","ZZZ3 [km]"])
        targetMesh.setCoords(a1)
        a2=DataArrayInt([6,0,3,5,3,0,1,4,1,2,7,4,8,7,2],15,1)
        a3=DataArrayInt([0,4,8,12,15],5,1)
        targetMesh.setNodalConnectivity(a2,a3)
        #
        targetMesh.checkConsistencyLight();
        #
        return targetMesh;
        
    def buildFieldScalarOn2DNT(self):
        mesh=self.build2DMesh();
        fieldOnCells=MEDCouplingFieldDouble.New(ON_CELLS,NO_TIME);
        fieldOnCells.setTimeUnit("ms");
        fieldOnCells.setName("toto");
        fieldOnCells.setMesh(mesh);
        array=DataArrayDouble.New();
        tmp=mesh.getNumberOfCells()*6*[7.]
        array.setValues(tmp,mesh.getNumberOfCells(),6);
        fieldOnCells.setArray(array)
        fieldOnCells.checkConsistencyLight();
        return fieldOnCells;

    def buildFieldNodeScalarOn2DNT(self):
        mesh=self.build2DMesh();
        fieldOnNodes=MEDCouplingFieldDouble.New(ON_NODES,NO_TIME);
        fieldOnNodes.setName("toto2");
        fieldOnNodes.setTimeUnit("s");
        fieldOnNodes.setDescription("my wonderful field toto2");
        fieldOnNodes.setMesh(mesh);
        array=DataArrayDouble.New();
        tmp=mesh.getNumberOfNodes()*5*[7.1234]
        array.setValues(tmp,mesh.getNumberOfNodes(),5);
        fieldOnNodes.setArray(array);
        fieldOnNodes.checkConsistencyLight();
        return fieldOnNodes;

    def buildFieldScalarOn3DNT(self):
        mesh=self.build3DMesh();
        fieldOnCells=MEDCouplingFieldDouble.New(ON_CELLS,NO_TIME);
        fieldOnCells.setNature(IntensiveMaximum);
        fieldOnCells.setName("toto");
        fieldOnCells.setDescription("my wonderful 3D field toto2");
        fieldOnCells.setMesh(mesh);
        array=DataArrayDouble.New();
        tmp=mesh.getNumberOfCells()*6*[7.]
        array.setValues(tmp,mesh.getNumberOfCells(),6);
        fieldOnCells.setArray(array);
        fieldOnCells.checkConsistencyLight();
        return fieldOnCells;

    def buildFieldScalarOn3DSurfWT(self):
        mesh=self.build3DSurfMesh();
        fieldOnCells=MEDCouplingFieldDouble.New(ON_CELLS,ONE_TIME);
        fieldOnCells.setName("toto25");
        fieldOnCells.setDescription("my wonderful 3D surf field toto25");
        fieldOnCells.setTimeUnit("us");
        fieldOnCells.setMesh(mesh);
        array=DataArrayDouble.New();
        tmp=mesh.getNumberOfCells()*3*[7.]
        array.setValues(tmp,mesh.getNumberOfCells(),3);
        array.setInfoOnComponents(["aaa","bbbb","ccccc"])
        fieldOnCells.setArray(array);
        fieldOnCells.setTime(6.7,1,4);
        fieldOnCells.checkConsistencyLight();
        return fieldOnCells;

    def buildFieldScalarOn3DSurfCOTI(self):
        mesh=self.build3DSurfMesh();
        fieldOnCells=MEDCouplingFieldDouble.New(ON_CELLS,CONST_ON_TIME_INTERVAL);
        fieldOnCells.setName("toto26");
        fieldOnCells.setDescription("my wonderful 3D surf field toto26");
        fieldOnCells.setTimeUnit("us");
        fieldOnCells.setMesh(mesh);
        array=DataArrayDouble.New();
        tmp=mesh.getNumberOfCells()*3*[7.]
        array.setValues(tmp,mesh.getNumberOfCells(),3);
        fieldOnCells.setArray(array);
        fieldOnCells.setStartTime(6.7,1,4);
        fieldOnCells.setEndTime(7.2,2,8);
        fieldOnCells.checkConsistencyLight();
        return fieldOnCells;

    def buildFieldScalarOn2DLT(self):
        mesh=self.build2DMesh()
        fieldOnCells=MEDCouplingFieldDouble.New(ON_CELLS,LINEAR_TIME)
        fieldOnCells.setName("toto27");
        fieldOnCells.setDescription("my wonderful 2D field toto27");
        fieldOnCells.setTimeUnit("ms");
        fieldOnCells.setMesh(mesh);
        array=DataArrayDouble.New();
        arr1=[1.2,1.02,1.002,1.0002, 3.4,3.04,3.004,3.0004, 5.6,5.06,5.006,5.0006, 7.8,7.08,7.008,7.0008, 9.1,9.01,9.001,9.0001]
        array.setValues(arr1,mesh.getNumberOfCells(),4);
        fieldOnCells.setArray(array);
        array=DataArrayDouble.New();
        arr2=[71.2,71.02,71.002,71.0002, 73.4,73.04,73.004,73.0004, 75.6,75.06,75.006,75.0006, 77.8,77.08,77.008,77.0008, 79.1,79.01,79.001,79.0001]
        array.setValues(arr2,mesh.getNumberOfCells(),4);
        fieldOnCells.setEndArray(array)
        fieldOnCells.setStartTime(6.7,25,26);
        fieldOnCells.setEndTime(17.2,125,126);
        fieldOnCells.checkConsistencyLight();
        return fieldOnCells;

    def buildFieldGaussPt2DWT(self):
        _a=0.446948490915965;
        _b=0.091576213509771;
        _p1=0.11169079483905;
        _p2=0.0549758718227661;
        refCoo1=[ 0.,0., 1.,0., 0.,1. ]
        gsCoo1=[ 2*_b-1, 1-4*_b, 2*_b-1, 2.07*_b-1, 1-4*_b,
                 2*_b-1, 1-4*_a, 2*_a-1, 2*_a-1, 1-4*_a, 2*_a-1, 2*_a-1]
        wg1=[ 4*_p2, 4*_p2, 4*_p2, 4*_p1, 4*_p1, 4*_p1 ]
        _refCoo1=refCoo1
        _gsCoo1=gsCoo1
        _wg1=wg1
        m=self.build2DMesh();
        f=MEDCouplingFieldDouble.New(ON_GAUSS_PT,ONE_TIME);
        f.setTime(6.7,1,4);
        f.setMesh(m);
        f.setGaussLocalizationOnType(NORM_TRI3,_refCoo1,_gsCoo1,_wg1);
        refCoo2=[ 0.,0., 1.,0., 1.,1., 0.,1. ]
        _refCoo2=refCoo2
        _gsCoo1=_gsCoo1[0:4]
        _wg1=_wg1[0:2]
        f.setGaussLocalizationOnType(NORM_QUAD4,_refCoo2,_gsCoo1,_wg1);
        array=DataArrayDouble.New();
        ptr=18*2*[None]
        for i in range(18*2):
            ptr[i]=float(i+1);
            pass
        array.setValues(ptr,18,2);
        array.setInfoOnComponent(0,"Power [MW]");
        array.setInfoOnComponent(1,"Density [kg/m^3]");
        f.setArray(array);
        f.setName("MyFirstFieldOnGaussPoint");
        f.setTimeUnit("ms");
        f.setDescription("mmmmmmmmmmmm");
        f.checkConsistencyLight();
        return f;

    def buildFieldGaussPtNE2DWT(self):
        m=self.build2DMesh();
        f=MEDCouplingFieldDouble.New(ON_GAUSS_NE,ONE_TIME);
        f.setTime(6.8,11,8);
        f.setMesh(m);
        f.setTimeUnit("ms");
        f.setName("MyFirstFieldOnNE");
        f.setDescription("MyDescriptionNE");
        array=DataArrayDouble.New();
        ptr=18*2*[None]
        for i in range(18*2):
            ptr[i]=float(i+7)
        array.setValues(ptr,18,2);
        array.setInfoOnComponent(0,"Power [MW]");
        array.setInfoOnComponent(1,"Density [kg/m^3]");
        f.setArray(array);
        #
        f.checkConsistencyLight();
        return f;

    def buildFieldVectorOnExtrudedWT(self):
        ext=self.buildExtrudedMesh();
        #
        f=MEDCouplingFieldDouble.New(ON_CELLS,ONE_TIME);
        f.setTime(6.8,11,8);
        f.setMesh(ext);
        f.setName("MyFieldOnExtruM");
        f.setDescription("desc of MyFiOnExtruM");
        array=DataArrayDouble.New();
        nbOfCells=ext.getNumberOfCells();
        ptr=2*nbOfCells*[None]
        for i in range(nbOfCells*2):
            ptr[i]=float(i/2+7)+float((i%2)*1000);
            pass
        array.setValues(ptr,nbOfCells,2);
        array.setInfoOnComponent(0,"Power [MW]");
        array.setInfoOnComponent(1,"Density [kg/m^3]");
        f.setArray(array);
        #
        f.checkConsistencyLight();
        return f

    def buildFieldVectorOnCMeshWT(self):
        ext=self.buildCMesh();
        #
        f=MEDCouplingFieldDouble.New(ON_CELLS,ONE_TIME);
        f.setTime(6.8,11,8);
        f.setMesh(ext);
        f.setName("MyFieldOnCMesh");
        f.setDescription("desc of MyFiOnCMesh");
        array=DataArrayDouble.New();
        nbOfCells=ext.getNumberOfCells();
        ptr=2*nbOfCells*[None]
        for i in range(nbOfCells*2):
            ptr[i]=float(i/2+7)+float((i%2)*1000);
            pass
        array.setValues(ptr,nbOfCells,2);
        array.setInfoOnComponent(0,"Power [GW]");
        array.setInfoOnComponent(1,"Density [kg/m^3]");
        f.setArray(array);
        #
        f.checkConsistencyLight();
        return f

    def buildFieldTemplateCellOn2D(self):
        f1=self.buildFieldScalarOn2DNT();
        f2=MEDCouplingFieldTemplate.New(f1);
        f2.setNature(NoNature);
        return f2
    
    def buildFieldTemplateNodeOn2D(self):
        f1=self.buildFieldNodeScalarOn2DNT();
        f2=MEDCouplingFieldTemplate.New(f1);
        f2.setNature(IntensiveMaximum);
        return f2
            
    def buildFieldTemplateGaussPtOn2D(self):
        f1=self.buildFieldGaussPt2DWT();
        f2=MEDCouplingFieldTemplate.New(f1);
        f2.setNature(ExtensiveMaximum);
        return f2

    def buildFieldTemplateGaussNEOn2D(self):
        f1=self.buildFieldGaussPtNE2DWT();
        f2=MEDCouplingFieldTemplate.New(f1);
        f2.setNature(ExtensiveConservation);
        return f2

    def buildMultiFields1(self):
        m1=self.build2DMesh();
        m1.setName("m1");
        m2=self.build2DMesh();
        m2.setName("m2");
        vals0=[-0.7,-1.,-2.,-3.,-4.];
        vals1=[0.,1.,2.,3.,4.,0.1,0.2,0.3,0.4];
        vals1_1=[170.,171.,172.,173.,174.,170.1,170.2,170.3,170.4];
        vals2=[5.,6.,7.,8.,9.];
        vals4=[15.,16.,17.,18.,19.];
        d0=DataArrayDouble.New();
        d0.setValues(vals0,5,1);
        d1=DataArrayDouble.New();
        d1.setValues(vals1,9,1);
        d1_1=DataArrayDouble.New();
        d1_1.setValues(vals1_1,9,1);
        d2=DataArrayDouble.New();
        d2.setValues(vals2,5,1);
        d4=DataArrayDouble.New();
        d4.setValues(vals4,5,1);
        d0.setName("d0"); d1.setName("d1"); d1_1.setName("d1_1"); d2.setName("d2"); d4.setName("d4");
        d0.setInfoOnComponent(0,"c1");
        d1.setInfoOnComponent(0,"c6");
        d1_1.setInfoOnComponent(0,"c9");
        d2.setInfoOnComponent(0,"c5");
        d4.setInfoOnComponent(0,"c7");
        f0=MEDCouplingFieldDouble.New(ON_CELLS,ONE_TIME);
        f0.setMesh(m1);
        f0.setArray(d0);
        f0.setTime(0.2,5,6);
        f0.setName("f0");
        f1=MEDCouplingFieldDouble.New(ON_NODES,LINEAR_TIME);
        f1.setMesh(m1);
        f1.setArrays([d1,d1_1]);
        f1.setStartTime(0.7,7,8);
        f1.setEndTime(1.2,9,10);
        f1.setName("f1");
        f2=MEDCouplingFieldDouble.New(ON_CELLS,CONST_ON_TIME_INTERVAL);
        f2.setMesh(m2);
        f2.setArray(d2);
        f2.setTime(1.2,11,12);
        f2.setEndTime(1.5,13,14);
        f2.setName("f2");
        f3=MEDCouplingFieldDouble.New(ON_CELLS,ONE_TIME);
        f3.setMesh(m1);
        f3.setArray(d2);
        f3.setTime(1.7,15,16);
        f3.setName("f3");
        f4=MEDCouplingFieldDouble.New(ON_CELLS,NO_TIME);
        f4.setMesh(m2);
        f4.setArray(d4);
        f4.setName("f4");
        ret=MEDCouplingMultiFields.New([f0,f1,f2,f3,f4]);
        return ret;
    
    def buildArrayDouble1(self):
        ret=DataArrayDouble.New();
        vals=[2.4,3.2,5.6,9.6,47.6,20.4,24.6,278.1,2.01,3.3,2.4,9.4];
        ret.setValues(vals,4,3);
        ret.setName("toto");
        ret.setInfoOnComponent(0,"sss");
        ret.setInfoOnComponent(1,"ppp");
        ret.setInfoOnComponent(2,"ttt");
        return ret;

    def buildArrayDouble2(self):
        ret=DataArrayDouble.New();
        ret.setName("titi");
        return ret;

    def buildArrayDouble3(self):
        ret=DataArrayDouble.New();
        ret.setName("titi");
        ret.alloc(0,3);
        ret.setInfoOnComponent(0,"sss");
        ret.setInfoOnComponent(1,"ppp");
        ret.setInfoOnComponent(2,"ttt");
        return ret;

    def buildArrayInt1(self):
        ret=DataArrayInt.New();
        vals=[2,3,5,9,47,20,24,278,2,3,2,9];
        ret.setValues(vals,4,3);
        ret.setName("toto");
        ret.setInfoOnComponent(0,"sss");
        ret.setInfoOnComponent(1,"ppp");
        ret.setInfoOnComponent(2,"ttt");
        return ret;

    def buildArrayInt2(self):
        ret=DataArrayInt.New();
        ret.setName("titi");
        return ret;

    def buildArrayInt3(self):
        ret=DataArrayInt.New();
        ret.setName("titi");
        ret.alloc(0,3);
        ret.setInfoOnComponent(0,"sss");
        ret.setInfoOnComponent(1,"ppp");
        ret.setInfoOnComponent(2,"ttt");
        return ret;

    def buildMultiFields2(self):
        m1=self.build2DMesh();
        m1.setName("m1");
        m2=self.build2DMesh();
        m2.setName("m2");
        vals0=[-0.7,-1.,-2.,-3.,-4.];
        vals1=[0.,1.,2.,3.,4.,0.1,0.2,0.3,0.4];
        vals1_1=[170.,171.,172.,173.,174.];
        vals2=[5.,6.,7.,8.,9.];
        vals4=[15.,16.,17.,18.,19.];
        d0=DataArrayDouble.New();
        d0.setValues(vals0,5,1);
        d1=DataArrayDouble.New();
        d1.setValues(vals1[:5],5,1);
        d1_1=DataArrayDouble.New();
        d1_1.setValues(vals1_1,5,1);
        d2=DataArrayDouble.New();
        d2.setValues(vals2,5,1);
        d4=DataArrayDouble.New();
        d4.setValues(vals4,5,1);
        d0.setName("d0"); d1.setName("d1"); d1_1.setName("d1_1"); d2.setName("d2"); d4.setName("d4");
        d0.setInfoOnComponent(0,"c1");
        d1.setInfoOnComponent(0,"c6");
        d1_1.setInfoOnComponent(0,"c9");
        d2.setInfoOnComponent(0,"c5");
        d4.setInfoOnComponent(0,"c7");
        f0=MEDCouplingFieldDouble.New(ON_CELLS,ONE_TIME);
        f0.setMesh(m1);
        f0.setArray(d0);
        f0.setTime(0.2,5,6);
        f0.setName("f0");
        f1=MEDCouplingFieldDouble.New(ON_CELLS,LINEAR_TIME);
        f1.setMesh(m1);
        f1.setArrays([d1,d1_1]);
        f1.setStartTime(0.7,7,8);
        f1.setEndTime(1.2,9,10);
        f1.setName("f1");
        f2=MEDCouplingFieldDouble.New(ON_CELLS,CONST_ON_TIME_INTERVAL);
        f2.setMesh(m2);
        f2.setArray(d2);
        f2.setTime(1.2,11,12);
        f2.setEndTime(1.5,13,14);
        f2.setName("f2");
        f3=MEDCouplingFieldDouble.New(ON_CELLS,ONE_TIME);
        f3.setMesh(m1);
        f3.setArray(d2);
        f3.setTime(1.7,15,16);
        f3.setName("f3");
        f4=MEDCouplingFieldDouble.New(ON_CELLS,ONE_TIME);
        f4.setMesh(m2);
        f4.setArray(d4);
        f4.setName("f4");
        f4.setTime(2.7,25,26);
        ret=MEDCouplingFieldOverTime.New([f0,f1,f2,f3,f4]);
        ret.checkConsistencyLight();
        return ret;

    def buildFileNameForIOR(self):
        tmpdir=os.getenv("TMP", "/tmp");
        username="";
        if os.getenv("USERNAME"):
            username = os.getenv("USERNAME")+"_";
        elif os.getenv("USER"):
            username = os.getenv("USER")+"_";
        ret=tmpdir+"/"+username+"entryPointMEDCouplingCorba.ior";
        return ret;
    pass

def testMesh():
    tab4=[1, 2, 8, 7, 2, 3, 9, 8, 3,
          4, 10, 9, 4, 5, 11, 10, 5,
          0, 6, 11, 0, 1, 7, 6 ]
    nbOfNodes=12
    nbOfCells=6
    coords=[ 0.024155, 0.04183768725682622, -0.305, 0.04831000000000001, -1.015761910347357e-17,
             -0.305, 0.09662000000000001, -1.832979297858306e-18, -0.305, 0.120775, 0.04183768725682623,
             -0.305, 0.09662000000000001, 0.08367537451365245, -0.305, 0.04831000000000001, 0.08367537451365246,
             -0.305, 0.024155, 0.04183768725682622, -0.2863, 0.04831000000000001, -1.015761910347357e-17, -0.2863, 
             0.09662000000000001, -1.832979297858306e-18, -0.2863, 0.120775, 0.04183768725682623, -0.2863, 0.09662000000000001,
             0.08367537451365245, -0.2863, 0.04831000000000001, 0.08367537451365246, -0.2863 ]
    mesh=MEDCouplingUMesh.New()
    mesh.setMeshDimension(2)
    mesh.allocateCells(8);
    mesh.setName("mesh1")
    for i in range(nbOfCells):
        mesh.insertNextCell(NORM_QUAD4,4,tab4[4*i:4*(i+1)]);
        pass
    mesh.finishInsertingCells()
    myCoords=DataArrayDouble.New()
    myCoords.setValues(coords,nbOfNodes,3);
    mesh.setCoords(myCoords);
    mesh.checkConsistencyLight();
    myFalseConn=DataArrayInt.New()
    myFalseConn.setValues(tab4,6,4)
    return mesh

def testField():
    #
    mesh=testMesh()
    nbOfCells=mesh.getNumberOfCells()
    field=MEDCouplingFieldDouble.New(ON_CELLS)
    field.setMesh(mesh)
    field.setNature(ExtensiveMaximum)
    myCoords=DataArrayDouble.New()
    sampleTab=[]
    for i in range(nbOfCells*9):
        sampleTab.append(float(i))
        myCoords.setValues(sampleTab,nbOfCells,9);
        pass
    field.setArray(myCoords)
    return field

