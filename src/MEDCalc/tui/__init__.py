# Copyright (C) 2015-2016  CEA/DEN, EDF R&D
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

# This functions are to be used to notify the USER of some events
# arising on the field operation. It is NOT to be used for logging
# purpose
def inf(msg): print "INF: "+str(msg)
def wrn(msg): print "WRN: "+str(msg)
def err(msg): print "ERR: "+str(msg)
def dbg(msg): print "DBG: "+str(msg)

# Initialize CORBA stuff
import medcorba

# Connect event listener
import medevents

# Fields utilities
from fieldproxy import newFieldProxy, FieldProxy

# Input/Output
from medio import LoadDataSource
from medio import LoadImageAsDataSource
from medio import GetFirstMeshFromDataSource
from medio import GetFirstFieldFromMesh

# Presentations
from medpresentation import MakeMeshView
from medpresentation import MakeScalarMap
from medpresentation import MakeContour
from medpresentation import MakeVectorField
from medpresentation import MakeSlices
from medpresentation import MakePointSprite
from medpresentation import RemovePresentation
from medpresentation import MakeDeflectionShape

from medpresentation import GetMeshViewParameters
from medpresentation import GetScalarMapParameters
from medpresentation import GetContourParameters
from medpresentation import GetSlicesParameters
from medpresentation import GetPointSpriteParameters
from medpresentation import GetVectorFieldParameters
from medpresentation import GetDeflectionShapeParameters

from medpresentation import UpdateMeshView
from medpresentation import UpdateScalarMap
from medpresentation import UpdateContour
from medpresentation import UpdateSlices
from medpresentation import UpdateVectorField
from medpresentation import UpdatePointSprite
from medpresentation import UpdateDeflectionShape

from medpresentation import ComputeCellAverageSize, GetDomainCenter, GetSliceOrigins, SelectSourceField

# Processing
from medprocessing import ChangeUnderlyingMesh
from medprocessing import InterpolateField

# Console commands
import medconsole

# Playing test scenarii
from medtest import PlayQtTestingScenario
from medtest import RequestSALOMETermination
