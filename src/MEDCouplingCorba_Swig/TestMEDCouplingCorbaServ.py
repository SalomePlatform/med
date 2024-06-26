# Copyright (C) 2007-2024  CEA, EDF
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

import CORBA
import MEDCouplingCorbaSwigTest
import MEDCouplingCorbaSwigTestServ

orb=CORBA.ORB_init(['']);
poa=orb.resolve_initial_references("RootPOA");
mgr=poa._get_the_POAManager();
mgr.activate();
compo=MEDCouplingCorbaSwigTestServ.MEDCouplingMeshFieldFactoryComponentPy(orb)
compoPtr=compo._this()
ior=orb.object_to_string(compoPtr)
test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
f=file(test.buildFileNameForIOR(),"w")
f.write(ior)
f.close()
orb.run()

