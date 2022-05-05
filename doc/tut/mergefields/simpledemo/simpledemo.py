# Copyright (C) 2012-2022  CEA/DEN, EDF R&D
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

from MEDLoader import *

it,dt = (-1,-1)
mesh = MEDLoader.ReadUMeshFromFile("field1.med","DomainMesh_1",0)
f1 = MEDLoader.ReadField(ON_CELLS,"field1.med","DomainMesh_1",0,"field1",it,dt)
f1.setMesh(mesh)

mesh = MEDLoader.ReadUMeshFromFile("field2.med","DomainMesh_2",0)
f2 = MEDLoader.ReadField(ON_CELLS,"field2.med","DomainMesh_2",0,"field2",it,dt)
f2.setMesh(mesh)

mesh = MEDLoader.ReadUMeshFromFile("field3.med","DomainMesh_3",0)
f3 = MEDLoader.ReadField(ON_CELLS,"field3.med","DomainMesh_3",0,"field3",it,dt)
f3.setMesh(mesh)

#mesh = MEDLoader.ReadUMeshFromFile("field4.med","DomainMesh_4",0)
#f4 = MEDLoader.ReadField(ON_CELLS,"field4.med","DomainMesh_4",0,"field4",it,dt)
#f4.setMesh(mesh)

fmerge=MEDCouplingFieldDouble.MergeFields([f1,f2,f3])
fmerge.setName("merge")

MEDLoader.WriteField("merge.med",fmerge,True)

