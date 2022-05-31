#!/usr/bin/env python3
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

# This script illustrates the basic usage of MEDCoupling and MEDLoader
# to read fields in a med file and make basics operation on the values
# (gboulant - 11/07/2011)


# _T1A
medfilename = "timeseries.med" # med source filename
meshName = "Grid_80x80"        # name of the support mesh
dimrestriction = 0             # 0=no restriction
fieldName = "Pulse"            # name of the field series
# _T1B

# ==============================================================
# Make a scaling. This does not require the loading of the mesh
# _T2A
from MEDLoader import MEDLoader, ON_NODES

iteration, order = (3,-1)      # timestamps to consider
field=MEDLoader.ReadField(ON_NODES,
                          medfilename, meshName, dimrestriction,
                          fieldName, iteration, order)

field.applyFunc("f*3")
outfilename = "scaling.med"
MEDLoader.WriteField(outfilename,field,True)
# _T2B


# ==============================================================
# Make an addition. This requires to load first the mesh

# _T3A
# Load the support mesh
mesh = MEDLoader.ReadUMeshFromFile(medfilename, meshName, dimrestriction)

# Load the field at timestamps 3
iteration, order = (3,-1)
p3=MEDLoader.ReadField(ON_NODES,
                       medfilename,meshName,dimrestriction,
                       fieldName,iteration,order)
# Associate the mesh
p3.setMesh(mesh)

# Load the field at timestamps 4 
iteration, order = (4,-1)
p4=MEDLoader.ReadField(ON_NODES,
                       medfilename, meshName, dimrestriction,
                       fieldName, iteration, order)
# Associate the mesh
p4.setMesh(mesh)

# Make the addition
result = p3+p4
result.setName("p3+p4")

# We can finally save the result together with the operands fields
outfilename = "addition.med"
MEDLoader.WriteField(outfilename,result,True)
MEDLoader.WriteField(outfilename,p3,False)
MEDLoader.WriteField(outfilename,p4,False)
# _T3B

# ==============================================================
# Make an addition. A different way that loads fields all at once
mesh = MEDLoader.ReadUMeshFromFile(medfilename, meshName, dimrestriction)
timestamps = [(3,-1),(4,-1)]
p3,p4 = MEDLoader.ReadFieldsOnSameMesh(ON_NODES,medfilename,
                                       meshName,dimrestriction,
                                       fieldName,timestamps)
p3.setMesh(mesh)
p4.setMesh(mesh)
result = p3+p4
result.setName("p3+p4")

outfilename = "addition_01.med"
MEDLoader.WriteField(outfilename,result,True)
