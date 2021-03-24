# Copyright (C) 2012-2021  CEA/DEN, EDF R&D
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

from MEDLoader import MEDLoader, ON_NODES

medfilename = "timeseries.med" # med source filename
meshname = "Grid_80x80"        # name of the support mesh
dimrestriction = 0             # 0=no restriction
fieldname = "Pulse"            # name of the field series

# Load the support mesh
mesh = MEDLoader.ReadUMeshFromFile(medfilename,meshname,dimrestriction)

# Load the field at timestamps 3
iteration, order = (3,-1)      # timestamps to consider
p3=MEDLoader.ReadField(ON_NODES,medfilename,meshname,dimrestriction,
                       fieldname,iteration,order)
p3.setMesh(mesh)

# Load the field at timestamps 4 
iteration, order = (4,-1)      # timestamps to consider
p4=MEDLoader.ReadField(ON_NODES,medfilename,meshname,dimrestriction,
                       fieldname,iteration,order)
p4.setMesh(mesh)

result = p3+p4
result.setName("p3+p4")

outfilename = "addition.med"
MEDLoader.WriteField(outfilename,result,True)
MEDLoader.WriteField(outfilename,p3,False)
MEDLoader.WriteField(outfilename,p4,False)

