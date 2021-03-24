# Copyright (C) 2011-2021  CEA/DEN, EDF R&D
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
import MEDCALC, SALOME
from medcalc.medevents import notifyGui_addPresentation, notifyGui_removePresentation, notifyGui_error, notifyGui_modifyPresentation
from functools import reduce

__manager = medcalc.medcorba.factory.getPresentationManager()

def MakeMeshView(meshID,
                 viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                 meshMode=MEDCALC.MESH_MODE_DEFAULT):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.MeshViewParameters(meshID, meshMode)
  try:
    presentation_id = __manager.makeMeshView(params, viewMode)
    notifyGui_addPresentation(meshID, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while creating the mesh view:\n" + e.details.text)
    raise Exception(e.details.text)


def MakeScalarMap(proxy,
                  viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                  displayedComponent=MEDCALC.DISPLAY_DEFAULT,
                  scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
                  colorMap=MEDCALC.COLOR_MAP_DEFAULT
                  ):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.ScalarMapParameters(proxy.id, displayedComponent, scalarBarRange, colorMap)
  try:
    presentation_id = __manager.makeScalarMap(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while creating the scalar map:\n" + e.details.text)
    raise Exception(e.details.text)

def MakeContour(proxy,
                viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
                colorMap=MEDCALC.COLOR_MAP_DEFAULT,
                nbContours=MEDCALC.NB_CONTOURS_DEFAULT
                ):
  params = MEDCALC.ContourParameters(proxy.id, scalarBarRange, colorMap, nbContours)
  try:
    presentation_id = __manager.makeContour(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while creating the contour:\n" + e.details.text)
    raise Exception(e.details.text)

#

def MakeVectorField(proxy,
                  viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                  scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
                  colorMap=MEDCALC.COLOR_MAP_DEFAULT
                  ):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.VectorFieldParameters(proxy.id, scalarBarRange, colorMap)
  try:
    presentation_id = __manager.makeVectorField(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while creating the vector field:\n" + e.details.text)
    raise Exception(e.details.text)

def MakeSlices(proxy,
                viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                displayedComponent=MEDCALC.DISPLAY_DEFAULT,
                scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
                colorMap=MEDCALC.COLOR_MAP_DEFAULT,
                sliceOrientation=MEDCALC.SLICE_ORIENTATION_DEFAULT,
                nbSlices=MEDCALC.NB_SLICES_DEFAULT):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.SlicesParameters(proxy.id, displayedComponent,scalarBarRange, colorMap, 
                                    sliceOrientation, nbSlices)
  try:
    presentation_id = __manager.makeSlices(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while creating the slices:\n" + e.details.text)
    raise Exception(e.details.text)


def MakeDeflectionShape(proxy,
                  viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                  scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
                  colorMap=MEDCALC.COLOR_MAP_DEFAULT
                  ):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.DeflectionShapeParameters(proxy.id, scalarBarRange, colorMap)
  try:
    presentation_id = __manager.makeDeflectionShape(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while creating the deflection shape:\n" + e.details.text)
    raise Exception(e.details.text)


def MakePointSprite(proxy,
                  viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                  displayedComponent=MEDCALC.DISPLAY_DEFAULT,
                  scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
                  colorMap=MEDCALC.COLOR_MAP_DEFAULT
                  ):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.PointSpriteParameters(proxy.id, displayedComponent, scalarBarRange, colorMap)
  try:
    presentation_id = __manager.makePointSprite(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while creating the point sprite:\n" + e.details.text)
    raise Exception(e.details.text)

def RemovePresentation(presentation_id):
  ok = __manager.removePresentation(presentation_id)
  if ok:
    notifyGui_removePresentation(presentation_id)
#

def __GetGENERICParameters(tag, presentation_id):
  exec("params = __manager.get%sParameters(presentation_id)" % tag)
  return locals()['params']

GetMeshViewParameters = lambda pres_id: __GetGENERICParameters("MeshView", pres_id)
GetScalarMapParameters = lambda pres_id: __GetGENERICParameters("ScalarMap", pres_id)
GetContourParameters = lambda pres_id: __GetGENERICParameters("Contour", pres_id)
GetSlicesParameters = lambda pres_id: __GetGENERICParameters("Slices", pres_id)
GetPointSpriteParameters = lambda pres_id: __GetGENERICParameters("PointSprite", pres_id)
GetVectorFieldParameters = lambda pres_id: __GetGENERICParameters("VectorField", pres_id)
GetDeflectionShapeParameters = lambda pres_id: __GetGENERICParameters("DeflectionShape", pres_id)


def __UpdateGENERIC(tag, presentation_id, params):
  exec("__manager.update%s(presentation_id, params)" % tag)
  notifyGui_modifyPresentation(presentation_id)

UpdateMeshView = lambda pres_id, params: __UpdateGENERIC("MeshView", pres_id, params)
UpdateScalarMap = lambda pres_id, params: __UpdateGENERIC("ScalarMap", pres_id, params)
UpdateContour = lambda pres_id, params: __UpdateGENERIC("Contour", pres_id, params)
UpdateSlices = lambda pres_id, params: __UpdateGENERIC("Slices", pres_id, params)
UpdateVectorField = lambda pres_id, params: __UpdateGENERIC("VectorField", pres_id, params)
UpdatePointSprite = lambda pres_id, params: __UpdateGENERIC("PointSprite", pres_id, params)
UpdateDeflectionShape = lambda pres_id, params: __UpdateGENERIC("DeflectionShape", pres_id, params)

def ComputeCellAverageSize(obj):
  """
  @return the average cell size
  """
  bb, nCells = obj.GetDataInformation().GetBounds(), obj.GetDataInformation().GetNumberOfCells()
  bb = list(zip(bb[::2], bb[1::2]))
  deltas = [x[1]-x[0] for x in bb]
  ## Filter out null dimensions:
  avgDelta = sum(deltas) / 3.0
  deltas = [d for d in deltas if abs(d) > 1.0e-12*avgDelta]
  ##
  vol = reduce(lambda x,y:x*y, deltas, 1.0) 
  cellSize = (vol/nCells)**(1.0/float(len(deltas)))
  return cellSize

def GetDomainCenter(obj):
  """
  @return the center of the domain as the central point of the bounding box
  """
  bb = obj.GetDataInformation().GetBounds()
  bb = list(zip(bb[::2], bb[1::2]))
  mids = [x[0] + 0.5*(x[1]-x[0]) for x in bb]
  return mids

def GetSliceOrigins(obj, nbSlices, normal):
  """
  Compute all origin points for the position of the slices.
  @param normal is a list of 3 floats either 0 or 1 indicating the normal vector of the slices
  """
  from math import sqrt
  bb = obj.GetDataInformation().GetBounds()
  bb = list(zip(bb[::2], bb[1::2]))
  origin = [x[0] + 0.5*(x[1]-x[0]) for x in bb]
  deltas = [x[1]-x[0] for x in bb]
  # Compute extent of slices:
  l = [normal[i]*deltas[i]**2 for i in range(3)]   # either the X extend, or the XY diagonal, or the XYZ diagonal
  plus = lambda x,y: x+y
  extent = sqrt(reduce(plus, l, 0.0))
  norm = sqrt(reduce(plus, normal, 0.0))
  normal = [normal[i]/norm for i in range(3)]
  origins = []
  step = extent/nbSlices
  for j in range(nbSlices):
    orig_j = [origin[i]+normal[i]*(-0.5*extent + step*(0.5+j)) for i in range(3)]
    origins.append(orig_j)
  return origins

def SelectSourceField(obj, meshName, fieldName, discretisation):
  """
  Properly set the AllArrays property of a MEDReader source to point to the correct field.
  Setting the fieldName to void string is allowed (meaning we work on the mesh only).
  """
  if fieldName == "":
    return
  tree = obj.GetProperty("FieldsTreeInfo")[::2]
  it = None
  for t in tree:
    arr = t.split("/")
    arr = arr[:-1] + arr[-1].split("@@][@@")
    if arr[1] == meshName and arr[3] == fieldName and arr[4] == discretisation:
      obj.AllArrays = [t]
      return
  raise Exception("Field not found")
