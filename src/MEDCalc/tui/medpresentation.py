# Copyright (C) 2011-2016  CEA/DEN, EDF R&D
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

import medcalc
import MEDCALC
from medcalc.medevents import notifyGui_addPresentation

__manager = medcalc.medcorba.factory.getPresentationManager()

def MakeScalarMap(proxy, viewMode=MEDCALC.VIEW_MODE_REPLACE):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements

  print "In MakeScalarMap (Python)"

  print "viewMode:", viewMode, " [", type(viewMode), "]"

  params = MEDCALC.ScalarMapParameters(proxy.id, viewMode)
  presentation_id = __manager.makeScalarMap(params)
  notifyGui_addPresentation(proxy.id, presentation_id)
#

def MakeContour(proxy, viewMode=MEDCALC.VIEW_MODE_REPLACE):
  print "Not implemented yet"
  #params = MEDCALC.ContourParameters(proxy.id, viewMode)
  #presentation_id = __manager.makeContour(params)
  #notifyGui_addPresentation(proxy.id, presentation_id)
#

def MakeVectorField(proxy, viewMode=MEDCALC.VIEW_MODE_REPLACE):
  params = MEDCALC.VectorFieldParameters(proxy.id, viewMode)
  presentation_id = __manager.makeVectorField(params)
  notifyGui_addPresentation(proxy.id, presentation_id)
#

def MakeSlices(proxy, viewMode=MEDCALC.VIEW_MODE_REPLACE):
  print "Not implemented yet"
  #params = MEDCALC.SlicesParameters(proxy.id, viewMode)
  #presentation_id = __manager.makeSlices(params)
  #notifyGui_addPresentation(proxy.id, presentation_id)
#

def MakeDeflectionShape(proxy, viewMode=MEDCALC.VIEW_MODE_REPLACE):
  params = MEDCALC.DeflectionShapeParameters(proxy.id, viewMode)
  presentation_id = __manager.makeDeflectionShape(params)
  notifyGui_addPresentation(proxy.id, presentation_id)
#

def MakePointSprite(proxy, viewMode=MEDCALC.VIEW_MODE_REPLACE):
  params = MEDCALC.PointSpriteParameters(proxy.id, viewMode)
  presentation_id = __manager.makePointSprite(params)
  notifyGui_addPresentation(proxy.id, presentation_id)
#