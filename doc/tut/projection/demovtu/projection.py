# Copyright (C) 2012-2024  CEA, EDF
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
from MEDCouplingRemapper import *

ms=MEDFileMesh.New("MeshSource.med") ; ms=ms.getMeshAtLevel(0)
mt=MEDFileMesh.New("MeshTarget.med") ; mt=mt.getMeshAtLevel(0)
fs=ms.fillFromAnalytic(ON_CELLS,1,"319.*cos(((x)*(x)*3+(y-0.52)*(y-0.52)+(z-0.1)*(z-0.1))*7)")
fs.setNature(IntensiveMaximum)
fs.setName("Temperature")
MEDCouplingFieldDouble.WriteVTK("MeshSource.vtu",[fs])

mt2=mt.deepCopy()
mt2.translate([0.4,0.,0.])
mt2.writeVTK("MeshTarget.vtu")
#
remap=MEDCouplingRemapper()
remap.prepare(ms,mt,"P0P0")
ft=remap.transferField(fs,1e100)
ft.setName("Temperature")
ft.getMesh().translate([0.4,0.,0.])
MEDCouplingFieldDouble.WriteVTK("FieldTarget.vtu",[ft])


# Image illustrating the starting point:
# - load MeshSource.vtu (contains mesh+field)
# - load MeshTarget.vtu (contains only the mesh with translation)
#
# Image illustrating the result point:
# - load MeshSource.vtu
# - load FieldTarget.vtu
 
