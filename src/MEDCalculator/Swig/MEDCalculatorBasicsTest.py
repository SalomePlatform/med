# Copyright (C) 2007-2021  CEA/DEN, EDF R&D
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

from MEDCalculator import *
import unittest

#self.assertEqual(int(expected2[i]),a4.getIJ(0,i));
#self.assertTrue(a2.getInfoOnComponent(2)=="ffff");
#self.assertRaises(Exception,a2.keepSelectedComponents,arr5V);
#self.assertAlmostEqual(2.3,t,13);

class MEDCalculatorBasicsTest(unittest.TestCase):
    def test1(self):
        e=MEDCalculatorBrowserLiteStruct("hfile1.med")
        f=e.getField(0)
        Power=MEDCalculatorDBFieldReal(f)
        Power2=Power[:,:,2:4]+Power[:,:,3:5]
        Power3=Power[:,:,2:4]+Power(":",":","3:5")
        self.assertTrue(Power2.isEqual(Power3,1e-12,1e-12))
        Power2=2+Power2
        self.assertTrue(not Power2.isEqual(Power3,1e-12,1e-12))
        Power2=Power2-2
        self.assertTrue(Power2.isEqual(Power3,1e-10,1e-10))
        Power2=2*Power2
        self.assertTrue(not Power2.isEqual(Power3,1e-12,1e-12))
        Power2=Power2/2
        self.assertTrue(Power2.isEqual(Power3,1e-12,1e-12))
        Power2=Power2*2
        Power2.setName('Power2')
        Power2.write("hfile2.med",True)
        e=MEDCalculatorBrowserLiteStruct("hfile2.med")
        f=e.getField(0)
        Power4=MEDCalculatorDBFieldReal(f)
        self.assertTrue(Power2.isEqual(Power4,1e-12,1e-12))
        pass
    
    def test2(self):
        e=MEDCalculatorBrowserLiteStruct("hfile1.med")
        f=e.getField(0)
        Power=MEDCalculatorDBFieldReal(f)
        v=Power.getValues()
        self.assertEqual(10,len(v));
        for i in range(10):
            v1=v[i]
            self.assertEqual(35,len(v1))
            l=0
            for j in range(5):
                for k in range(7):
                    self.assertAlmostEqual((i+1)*100.+(j+1)*10.+k+1,v1[l],12);
                    l+=1
                    pass
                pass
            pass
        p1=Power[2:4,:,:]
        v=p1.getValues()
        self.assertEqual(2,len(v));
        for i in range(2):
            v1=v[i]
            self.assertEqual(35,len(v1))
            l=0
            for j in range(5):
                for k in range(7):
                    self.assertAlmostEqual((i+3)*100.+(j+1)*10.+k+1,v1[l],12);
                    l+=1
                    pass
                pass
            pass
        p2=Power[3:7,:,2:5]
        v=p2.getValues()
        self.assertEqual(4,len(v));
        for i in range(4):
            v1=v[i]
            self.assertEqual(15,len(v1))
            l=0
            for j in range(5):
                for k in range(3):
                    self.assertAlmostEqual((i+4)*100.+(j+1)*10.+k+3,v1[l],12);
                    l+=1
                    pass
                pass
            pass
        #
        p3=Power[0:4,:,2:4].dot(Power[6:,:,3:5])
        v=p3.getValues()
        self.assertEqual(4,len(v));
        expected=[[162192.0, 178952.0, 196112.0, 213672.0, 231632.0], [347792.0, 368552.0, 389712.0, 411272.0, 433232.0], [573392.0, 598152.0, 623312.0, 648872.0, 674832.0], [838992.0, 867752.0, 896912.0, 926472.0, 956432.0]]
        for i in range(4):
            v1=v[i]
            self.assertEqual(5,len(v1))
            l=0
            for j in range(5):
                self.assertAlmostEqual(expected[i][j],v1[l],8);
                l+=1
                pass
            pass
        #
        Power[:,:,2:4]=7.
        v=Power.getValues()
        self.assertEqual(10,len(v));
        for i in range(10):
            v1=v[i]
            self.assertEqual(35,len(v1))
            l=0
            for j in range(5):
                for k in range(2):
                    self.assertAlmostEqual((i+1)*100.+(j+1)*10.+k+1,v1[l],12);
                    l+=1
                    pass
                self.assertAlmostEqual(7.,v1[l],12);
                l+=1
                self.assertAlmostEqual(7.,v1[l],12);
                l+=1
                for k in range(3):
                    self.assertAlmostEqual((i+1)*100.+(j+1)*10.+k+5,v1[l],12);
                    l+=1
                    pass
                pass
            pass
        Power[1:5,:,3]=p3
        v=Power[1:5,:,:].getValues()
        self.assertEqual(4,len(v));
        for i in range(4):
            v1=v[i]
            self.assertEqual(35,len(v1))
            l=0
            for j in range(5):
                for k in range(2):
                    self.assertAlmostEqual((i+2)*100.+(j+1)*10.+k+1,v1[l],12);
                    l+=1
                    pass
                self.assertAlmostEqual(7.,v1[l],12);
                l+=1
                self.assertAlmostEqual(expected[i][j],v1[l],8);
                l+=1
                for k in range(3):
                    self.assertAlmostEqual((i+2)*100.+(j+1)*10.+k+5,v1[l],12);
                    l+=1
                    pass
                pass
            pass
        Power[:,:,1:].eigenValues()
        Power[:]=0.07
        pass
    def setUp(self):
        pass
    pass

unittest.main()
