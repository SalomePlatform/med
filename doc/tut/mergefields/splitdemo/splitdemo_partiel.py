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

fns=['DomainMesh_1.med', 'DomainMesh_2.med','DomainMesh_3.med']
outs=["out1.vtu","out2.vtu","out3.vtu","out4.vtu"]
trs=[(-0.714,-0.714,0.),(0.,0.,0.),(-0.577,-0.577,-0.577)]
scal=0.5
fs=3*[None]
dds=3*[None]
mns=3*[None]
i=0

splitview=False

for fn in fns:
    m=MEDFileMesh.New(fn)
    m=m.getMeshAtLevel(0)
    m.writeVTK(outs[i])
    mns[i]=m.deepCopy()
    if splitview:
        m.translate([scal*elt for elt in trs[i]])
    f=MEDCouplingFieldDouble.New(ON_CELLS,NO_TIME)
    f.setMesh(m)
    da=DataArrayDouble.New()
    da.alloc(m.getNumberOfCells(),1)
    da.fillWithValue(i)
    dds[i]=da.deepCopy()
    f.setArray(da)
    fs[i]=f
    i+=1
    pass


f=MEDCouplingFieldDouble.MergeFields(fs)
f.setName("PID")

MEDCouplingFieldDouble.WriteVTK("Domain.vtu",[f])
dd=DataArrayDouble.Aggregate(dds)
m=MEDCouplingUMesh.MergeUMeshes(mns)
if splitview:
    m.translate([3.*elt for elt in [0.577,0.577,0.577]])
f2=MEDCouplingFieldDouble.New(ON_CELLS,NO_TIME)
f2.setMesh(m)
f2.setArray(dd)
f2.setName("PID")
MEDCouplingFieldDouble.WriteVTK('DomainMesh.vtu',[f2])
#m=MEDCouplingUMesh.MergeUMeshes(mns)
#m.writeVTK("DomainMesh.vtu")
