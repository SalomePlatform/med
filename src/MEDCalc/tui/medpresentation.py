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
from medcalc.medevents import notifyGui_addPresentation, notifyGui_removePresentation, notifyGui_error, notifyGui_modifyPresentation, notifyGui_visibilityChanged
from functools import reduce
import pvsimple as pvs

__manager = medcalc.medcorba.factory.getPresentationManager()
DEFAULT_VISIBILITY = True
DEFAULT_SCALAR_BAR_VISIBILITY = True
DEFAULT_USE_CUSTOM_RANGE = False
DEFAULT_SCALAR_BAR_RANGE = [0.0, 100.0]
DEFAULT_PLANE_NORMAL = [0.0, 0.0, 1.0]
DEFAULT_PLANE_POS = 1.0
DEFAULT_SCALE_FACTOR = 0.2
DEFAULT_CUT_POINT1 = [0.0, 0.0, 0.0]
DEFAULT_CUT_POINT2 = [1.0, 1.0, 1.0]
DEFAULT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE = False
DEFAULT_CUSTOM_SCALE_FACTOR = False
DEFAULT_CONTOUR_COMPONENT = ""

def MakeMeshView(meshID,
                 viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                 meshMode=MEDCALC.MESH_MODE_DEFAULT):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  visibility=True
  params = MEDCALC.MeshViewParameters(meshID, meshMode, visibility)
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
  visibility = DEFAULT_VISIBILITY
  scalarBarVisibility = DEFAULT_SCALAR_BAR_VISIBILITY
  scalarBarRangeArray = DEFAULT_SCALAR_BAR_RANGE
  hideDataOutsideCustomRange = DEFAULT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE
  params = MEDCALC.ScalarMapParameters(proxy.id, displayedComponent, scalarBarRange, colorMap, visibility,
                                       scalarBarVisibility, scalarBarRangeArray, hideDataOutsideCustomRange)
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
  visibility = DEFAULT_VISIBILITY
  scalarBarVisibility = DEFAULT_SCALAR_BAR_VISIBILITY
  hideDataOutsideCustomRange = DEFAULT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE
  scalarBarRangeArray = DEFAULT_SCALAR_BAR_RANGE
  contourComponent = DEFAULT_CONTOUR_COMPONENT
  params = MEDCALC.ContourParameters(proxy.id, scalarBarRange, colorMap, visibility, scalarBarVisibility,
                                     scalarBarRangeArray, hideDataOutsideCustomRange, nbContours, contourComponent)
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
  visibility = DEFAULT_VISIBILITY
  scalarBarVisibility = DEFAULT_SCALAR_BAR_VISIBILITY
  scalarBarRangeArray = DEFAULT_SCALAR_BAR_RANGE
  hideDataOutsideCustomRange = DEFAULT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE
  scaleFactor = DEFAULT_SCALE_FACTOR
  customScaleFactor = DEFAULT_CUSTOM_SCALE_FACTOR
  params = MEDCALC.VectorFieldParameters(proxy.id, scalarBarRange, colorMap, visibility, scalarBarVisibility,
                                         scalarBarRangeArray, hideDataOutsideCustomRange, scaleFactor, customScaleFactor)
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
  visibility = DEFAULT_VISIBILITY
  scalarBarVisibility = DEFAULT_SCALAR_BAR_VISIBILITY
  scalarBarRangeArray = DEFAULT_SCALAR_BAR_RANGE
  hideDataOutsideCustomRange = DEFAULT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE
  params = MEDCALC.SlicesParameters(proxy.id, displayedComponent,scalarBarRange, colorMap, visibility,
                                    scalarBarVisibility, scalarBarRangeArray, hideDataOutsideCustomRange,
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
  visibility = DEFAULT_VISIBILITY
  scalarBarVisibility = DEFAULT_SCALAR_BAR_VISIBILITY
  scalarBarRangeArray = DEFAULT_SCALAR_BAR_RANGE
  hideDataOutsideCustomRange = DEFAULT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE
  params = MEDCALC.DeflectionShapeParameters(proxy.id, scalarBarRange, colorMap, visibility,
                                    scalarBarVisibility, scalarBarRangeArray, hideDataOutsideCustomRange)
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
  visibility = DEFAULT_VISIBILITY
  scalarBarVisibility = DEFAULT_SCALAR_BAR_VISIBILITY
  scalarBarRangeArray = DEFAULT_SCALAR_BAR_RANGE
  hideDataOutsideCustomRange = DEFAULT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE
  params = MEDCALC.PointSpriteParameters(proxy.id, displayedComponent, scalarBarRange, colorMap, visibility,
                                         scalarBarVisibility, scalarBarRangeArray, hideDataOutsideCustomRange)
  try:
    presentation_id = __manager.makePointSprite(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while creating the point sprite:\n" + e.details.text)
    raise Exception(e.details.text)

# sphinx doc: begin of MakePlot3D
def MakePlot3D(proxy,
              viewMode=MEDCALC.VIEW_MODE_DEFAULT,
              scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
              colorMap=MEDCALC.COLOR_MAP_DEFAULT
              ):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  visibility = DEFAULT_VISIBILITY
  scalarBarVisibility = DEFAULT_SCALAR_BAR_VISIBILITY
  scalarBarRangeArray = DEFAULT_SCALAR_BAR_RANGE
  hideDataOutsideCustomRange = DEFAULT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE
  scalarBarRangeArray = DEFAULT_SCALAR_BAR_RANGE
  planeNormal = DEFAULT_PLANE_NORMAL
  planePos = DEFAULT_PLANE_POS
  params = MEDCALC.Plot3DParameters(proxy.id, scalarBarRange, colorMap, visibility,
                                    scalarBarVisibility, scalarBarRangeArray, hideDataOutsideCustomRange, planeNormal, planePos)
  try:
    presentation_id = __manager.makePlot3D(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while creating the Plot3D:\n" + e.details.text)
    raise Exception(e.details.text)
# sphinx doc: end of MakePlot3D

def MakeStreamLines(proxy,
              viewMode=MEDCALC.VIEW_MODE_DEFAULT,
              scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
              colorMap=MEDCALC.COLOR_MAP_DEFAULT
              ):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  visibility = DEFAULT_VISIBILITY
  scalarBarVisibility = DEFAULT_SCALAR_BAR_VISIBILITY
  scalarBarRangeArray = DEFAULT_SCALAR_BAR_RANGE
  hideDataOutsideCustomRange = DEFAULT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE
  integrDir = MEDCALC.INTEGRATION_DIR_DEFAULT
  params = MEDCALC.StreamLinesParameters(proxy.id, scalarBarRange, colorMap, visibility,
                                    scalarBarVisibility, scalarBarRangeArray, hideDataOutsideCustomRange, integrDir)
  try:
    presentation_id = __manager.makeStreamLines(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while creating the StreamLines:\n" + e.details.text)
    raise Exception(e.details.text)


def MakeCutSegment(proxy,
              viewMode=MEDCALC.VIEW_MODE_DEFAULT,
              scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
              colorMap=MEDCALC.COLOR_MAP_DEFAULT
              ):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  visibility = DEFAULT_VISIBILITY
  scalarBarVisibility = DEFAULT_SCALAR_BAR_VISIBILITY
  hideDataOutsideCustomRange = DEFAULT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE
  scalarBarRangeArray = DEFAULT_SCALAR_BAR_RANGE
  point1 = DEFAULT_CUT_POINT1
  point2 = DEFAULT_CUT_POINT2
  params = MEDCALC.CutSegmentParameters(proxy.id, scalarBarRange, colorMap, visibility,
                                        scalarBarVisibility, scalarBarRangeArray, hideDataOutsideCustomRange, point1, point2)
  try:
    presentation_id = __manager.makeCutSegment(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while creating the CutSegment:\n" + e.details.text)
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
# sphinx doc: begin of GetPlot3DParameters
GetPlot3DParameters = lambda pres_id: __GetGENERICParameters("Plot3D", pres_id)
# sphinx doc: end of GetPlot3DParameters
GetStreamLinesParameters = lambda pres_id: __GetGENERICParameters("StreamLines", pres_id)
GetCutSegmentParameters = lambda pres_id: __GetGENERICParameters("CutSegment", pres_id)


def __UpdateGENERIC(tag, presentation_id, params):
  old_params = __GetGENERICParameters(tag, presentation_id)
  exec("__manager.update%s(presentation_id, params)" % tag)
  notifyGui_modifyPresentation(presentation_id)
  if old_params.visibility != params.visibility:
    # visibility is changed
    notifyGui_visibilityChanged(presentation_id)

UpdateMeshView = lambda pres_id, params: __UpdateGENERIC("MeshView", pres_id, params)
UpdateScalarMap = lambda pres_id, params: __UpdateGENERIC("ScalarMap", pres_id, params)
UpdateContour = lambda pres_id, params: __UpdateGENERIC("Contour", pres_id, params)
UpdateSlices = lambda pres_id, params: __UpdateGENERIC("Slices", pres_id, params)
UpdateVectorField = lambda pres_id, params: __UpdateGENERIC("VectorField", pres_id, params)
UpdatePointSprite = lambda pres_id, params: __UpdateGENERIC("PointSprite", pres_id, params)
UpdateDeflectionShape = lambda pres_id, params: __UpdateGENERIC("DeflectionShape", pres_id, params)
# sphinx doc: begin of UpdatePlot3D
UpdatePlot3D = lambda pres_id, params: __UpdateGENERIC("Plot3D", pres_id, params)
# sphinx doc: end of UpdatePlot3D
UpdateStreamLines = lambda pres_id, params: __UpdateGENERIC("StreamLines", pres_id, params)
UpdateCutSegment = lambda pres_id, params: __UpdateGENERIC("CutSegment", pres_id, params)


def get_bound_project(bound_box, planeNormal):
  """Get bounds projection"""
  EPS = 1E-3

  def dot_product(a, b):
    """Dot product of two 3-vectors."""
    dot = a[0] * b[0] + a[1] * b[1] + a[2] * b[2]
    return dot

  bound_points = [[bound_box[1], bound_box[2], bound_box[4]],
                  [bound_box[0], bound_box[3], bound_box[4]],
                  [bound_box[0], bound_box[2], bound_box[5]]]
  print(bound_points)

  bound_prj = [0.0, 0.0, 0.0]

  for i in range(0, 3):
    tmp = dot_product(planeNormal, bound_points[i])
    bound_prj[i] = tmp

  return bound_prj


def GetPositions(obj, planeNormal, displacement):
  """Compute plane positions."""
  positions = []
  bounds = obj.GetDataInformation().GetBounds()
  bound_prj = get_bound_project(bounds, planeNormal)
  positions = [i * displacement for i in bound_prj]
  return positions

def IsPlanarObj(obj):
  """
  Check if the given input is planar
  """
  bounds_info = obj.GetDataInformation().GetBounds()
  FLT_MIN = 1E-37

  if (abs(bounds_info[0] - bounds_info[1]) <= FLT_MIN or
    abs(bounds_info[2] - bounds_info[3]) <= FLT_MIN or
    abs(bounds_info[4] - bounds_info[5]) <= FLT_MIN):
    return True

  return False

def GetPlaneNormalVector(obj):
  """Get Normal Vector"""
  bounds = obj.GetDataInformation().GetBounds()
  FLT_MIN = 1E-37

  if abs(bounds[4] - bounds[5]) <= FLT_MIN:
    p0 = [bounds[0], bounds[2], 0]
    p1 = [bounds[1], bounds[2], 0]
    p2 = [bounds[0], bounds[3], 0]
  elif abs(bounds[2] - bounds[3]) <= FLT_MIN:
    p0 = [bounds[0], 0, bounds[4]]
    p1 = [bounds[1], 0, bounds[4]]
    p2 = [bounds[0], 0, bounds[5]]
  else:
    p0 = [0, bounds[2], bounds[4]]
    p1 = [0, bounds[3], bounds[4]]
    p2 = [0, bounds[2], bounds[5]]
  
  x0, y0, z0 = p0
  x1, y1, z1 = p1
  x2, y2, z2 = p2
  ux, uy, uz = u = [x1-x0, y1-y0, z1-z0]
  vx, vy, vz = v = [x2-x0, y2-y0, z2-z0]
  u_cross_v = [uy*vz-uz*vy, uz*vx-ux*vz, ux*vy-uy*vx]

  return u_cross_v

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


def FindOrCreateView(vtype):
  """
  Find and active or create a view with type vtype
  """
  result = None
  view = pvs.GetActiveView()
  if not view:
    result = pvs.GetActiveViewOrCreate(vtype)
  else:  
    if view.SMProxy.GetXMLName() == vtype:
      result = view
    else:
      layout1 = pvs.GetLayout(view)
      views = pvs.GetViewsInLayout(layout1)
      view = next((v for v in views if v.SMProxy.GetXMLName() == vtype), None)
      if view is None:
        result = pvs.CreateView(vtype)
        pvs.AssignViewToLayout(view=view, layout=layout1, hint=0)
      else:
        pvs.SetActiveView(view)
        result = view
  return result