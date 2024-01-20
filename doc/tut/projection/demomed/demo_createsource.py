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

#
# Another possibility (to illustrate the creation of values) is to
# create the source field using the function fillFromAnalytic
#
from MEDLoader import *
from MEDCouplingRemapper import *

# Read the source mesh and create a field on it
msource  = MEDLoader.ReadUMeshFromFile("meshsource.med","meshsource",0)

equation = "319.*cos(((x)*(x)*3+(y-0.52)*(y-0.52)+(z-0.1)*(z-0.1))*7)"
fsource=msource.fillFromAnalytic(ON_CELLS,1,equation)
fsource.setName("Temperature")
fsource.setNature(IntensiveMaximum)

# Read the target mesh
mtarget = MEDLoader.ReadUMeshFromFile("meshtarget.med","meshtarget",0)

# Remapper of type P0P0 (interpolation from cells to cells)
remap = MEDCouplingRemapper()
remap.prepare(msource,mtarget,"P0P0")

defaultValue = 1e100
ftarget = remap.transferField(fsource,defaultValue)
ftarget.setName("Temperature")

outfilename = "createsource_fieldtarget.med"
MEDLoader.WriteField(outfilename,ftarget,True)

