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

# _T1A
from MEDLoader import MEDLoader, ON_CELLS
from MEDCouplingRemapper import MEDCouplingRemapper, IntensiveMaximum

# Read the source mesh and the source field
it,dt = (-1,-1)
msource = MEDLoader.ReadUMeshFromFile("fieldsource.med","meshsource",0)
fsource = MEDLoader.ReadField(ON_CELLS,"fieldsource.med","meshsource",0,
                              "Temperature",it,dt)
fsource.setMesh(msource)
fsource.setNature(IntensiveMaximum)

# Read the target mesh
mtarget = MEDLoader.ReadUMeshFromFile("meshtarget.med","meshtarget",0)

# Remapper of type P0P0 (interpolation from cells to cells)
remap = MEDCouplingRemapper()
remap.prepare(msource,mtarget,"P0P0")

defaultValue = 1e100
ftarget = remap.transferField(fsource,defaultValue)
ftarget.setName("Temperature")

outfilename = "loadsource_fieldtarget.med"
MEDLoader.WriteField(outfilename,ftarget,True)
# _T1B
