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

import MEDCouplingCorbaSwigTest
from salome.MEDCouplingClient import *

from salome.kernel import MEDCouplingCorbaServant_idl
import CORBA

orb=CORBA.ORB_init([''])
f=file(MEDCouplingCorbaSwigTest.FileIOR1,"r")
ior=f.read()
f=MEDCouplingCorbaSwigTest.testField()
fCorbaIOR=orb.string_to_object(ior)
print(fCorbaIOR)

for i in range(50):
    fCpy=MEDCouplingFieldDoubleClient.New(fCorbaIOR)
    print(fCpy.isEqual(f,1e-12,1e-12))
    pass
fCorbaIOR.UnRegister()

f=file(MEDCouplingCorbaSwigTest.FileIOR2,"r")
ior=f.read()
m=MEDCouplingCorbaSwigTest.testMesh()
mCorbaIOR=orb.string_to_object(ior)
for i in range(50):
    mCpy=MEDCouplingUMeshClient.New(mCorbaIOR)
    print(mCpy.isEqual(m,1e-12))
    pass
mCorbaIOR.UnRegister()

