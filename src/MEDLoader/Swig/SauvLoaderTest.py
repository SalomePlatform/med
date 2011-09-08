#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2011  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

from MEDLoader import *
import unittest, os
from MEDLoaderDataForTest import MEDLoaderDataForTest

class SauvLoaderTest(unittest.TestCase):

    def testSauv2Med(self):
        # get a file containing all types of readable piles
        self.assertTrue( os.getenv("MED_ROOT_DIR") )
        sauvFile = os.path.join( os.getenv("MED_ROOT_DIR"), "share","salome",
                                 "resources","med","allPillesTest.sauv")
        self.assertTrue( os.access( sauvFile, os.F_OK))

        # read SAUV and write MED
        medFile = "SauvLoaderTest.med"
        sr=SauvReader.New(sauvFile);
        d2=sr.loadInMEDFileDS();
        d2.write(medFile,0);

        # check 
        self.assertEqual(1,d2.getNumberOfMeshes())
        self.assertEqual(8+97,d2.getNumberOfFields())
        mm = d2.getMeshes()
        m = mm.getMeshAtPos(0)
        self.assertEqual(17,len(m.getGroupsNames()))

        os.remove( medFile )
        pass

    def testMed2Sauv(self):
        # read pointe.med
        self.assertTrue( os.getenv("MED_ROOT_DIR") )
        medFile = os.path.join( os.getenv("MED_ROOT_DIR"), "share","salome",
                                "resources","med","pointe.med")
        self.assertTrue( os.access( medFile, os.F_OK))
        pointeMed = MEDFileData.New( medFile )

        # add 3 faces to pointeMed
        pointeMedMesh = pointeMed.getMeshes().getMeshAtPos(0)
        pointeM1D = MEDCouplingUMesh.New()
        pointeM1D.setCoords( pointeMedMesh.getCoords() )
        pointeM1D.setMeshDimension( 2 )
        pointeM1D.allocateCells( 3 )
        pointeM1D.insertNextCell( NORM_TRI3, 3, [0,1,2])
        pointeM1D.insertNextCell( NORM_TRI3, 3, [0,1,3])
        pointeM1D.insertNextCell( NORM_QUAD4, 4, [10,11,12,13])
        pointeM1D.finishInsertingCells()
        pointeMedMesh.setMeshAtLevel( -1, pointeM1D )
        pointeMed.getMeshes().setMeshAtPos( 0, pointeMedMesh )

        # add a field on 2 faces to pointeMed
        ff1=MEDFileFieldMultiTS.New()
        f1=MEDCouplingFieldDouble.New(ON_GAUSS_NE,ONE_TIME)
        #f1.setMesh( pointeM1D )
        f1.setName("Field on 2 faces")
        d=DataArrayDouble.New()
        d.alloc(3+4,2)
        d.setInfoOnComponent(0,"sigX [MPa]")
        d.setInfoOnComponent(1,"sigY [GPa]")
        d.setValues([311,312,321,322,331,332,411,412,421,422,431,432,441,442],3+4,2)
        f1.setArray(d)
        da=DataArrayInt.New()
        da.setValues([0,2],2,1)
        da.setName("sup2")
        ff1.appendFieldProfile(f1,pointeMedMesh,-1,da)
        pointeMed.getFields().pushField( ff1 )

        #remove fieldnodeint
        pointeFields = pointeMed.getFields()
        for i in range( pointeFields.getNumberOfFields() ):
            if pointeFields.getFieldAtPos(i).getName() == "fieldnodeint":
                pointeFields.destroyFieldAtPos( i )
                break

        # write pointeMed to SAUV
        sauvFile = "SauvLoaderTest.sauv"
        sw=SauvWriter.New();
        sw.setMEDFileDS(pointeMed);
        sw.write(sauvFile);

        # read SAUV and check
        sr=SauvReader.New(sauvFile);
        d2=sr.loadInMEDFileDS();
        self.assertEqual(1,d2.getNumberOfMeshes())
        self.assertEqual(4,d2.getNumberOfFields())
        m = d2.getMeshes().getMeshAtPos(0)
        self.assertEqual("maa1",m.getName())
        self.assertEqual(5,len(m.getGroupsNames()))
        self.assertEqual(3,m.getMeshDimension())
        groups = m.getGroupsNames()
        self.assertTrue( "groupe1" in groups )
        self.assertTrue( "groupe2" in groups )
        self.assertTrue( "groupe3" in groups )
        self.assertTrue( "groupe4" in groups )
        self.assertTrue( "groupe5" in groups )
        self.assertEqual(16,m.getSizeAtLevel(0))
        um0 = m.getGenMeshAtLevel(0)
        self.assertEqual(12, um0.getNumberOfCellsWithType( NORM_TETRA4 ))
        self.assertEqual(2, um0.getNumberOfCellsWithType( NORM_PYRA5 ))
        self.assertEqual(2, um0.getNumberOfCellsWithType( NORM_HEXA8 ))
        um1 = m.getGenMeshAtLevel(-1)
        self.assertEqual(2, um1.getNumberOfCellsWithType( NORM_TRI3 ))
        pointeUM0 = pointeMedMesh.getGenMeshAtLevel(0)
        self.assertTrue(m.getCoords().isEqualWithoutConsideringStr(pointeMedMesh.getCoords(),1e-12))
        self.assertEqual( um0.getMeasureField(0).accumulate(0),
                          pointeUM0.getMeasureField(0).accumulate(0),1e-12)
        # check fields
        # fieldnodedouble
        fieldnodedoubleTS1 = pointeMed.getFields().getFieldWithName("fieldnodedouble")
        fieldnodedoubleTS2 = d2.getFields().getFieldWithName("fieldnodedouble")
        self.assertEqual( fieldnodedoubleTS1.getInfo(), fieldnodedoubleTS2.getInfo())
        self.assertEqual( fieldnodedoubleTS1.getNumberOfTS(), fieldnodedoubleTS2.getNumberOfTS())
        io1 = fieldnodedoubleTS1.getIterations()
        io2 = fieldnodedoubleTS2.getIterations()
        for i in range(fieldnodedoubleTS1.getNumberOfTS() ):
            fnd1 = fieldnodedoubleTS1.getFieldOnMeshAtLevel(ON_NODES, io1[i][0],io1[i][1],pointeUM0)
            fnd2 = fieldnodedoubleTS2.getFieldOnMeshAtLevel(ON_NODES, io2[i][0],io2[i][1],um0)
            self.assertTrue( fnd1.getArray().isEqual( fnd2.getArray(), 1e-12 ))
        # fieldcelldoublevector
        fieldnodedoubleTS1 = pointeMed.getFields().getFieldWithName("fieldcelldoublevector")
        fieldnodedoubleTS2 = d2.getFields().getFieldWithName("fieldcelldoublevector")
        self.assertEqual( fieldnodedoubleTS1.getInfo(), fieldnodedoubleTS2.getInfo())
        self.assertEqual( fieldnodedoubleTS1.getNumberOfTS(), fieldnodedoubleTS2.getNumberOfTS())
        io1 = fieldnodedoubleTS1.getIterations()
        io2 = fieldnodedoubleTS2.getIterations()
        for i in range(fieldnodedoubleTS1.getNumberOfTS() ):
            fnd1 = fieldnodedoubleTS1.getFieldOnMeshAtLevel(ON_CELLS, io1[i][0],io1[i][1],pointeUM0)
            fnd2 = fieldnodedoubleTS2.getFieldOnMeshAtLevel(ON_CELLS, io2[i][0],io2[i][1],um0)
            self.assertAlmostEqual( fnd1.accumulate(0), fnd2.accumulate(0) )
            self.assertAlmostEqual( fnd1.accumulate(1), fnd2.accumulate(1) )
            self.assertAlmostEqual( fnd1.accumulate(2), fnd2.accumulate(2) )
        # Field on 2 faces
        fieldOnFaces = d2.getFields().getFieldWithName(f1.getName())
        io1 = fieldOnFaces.getIterations()
        fof = fieldOnFaces.getFieldOnMeshAtLevel(f1.getTypeOfField(),io1[i][0],io1[i][1],um1)
        self.assertTrue( d.isEqual( fof.getArray(), 1e-12 ))

        os.remove( sauvFile )
        pass
    pass

unittest.main()