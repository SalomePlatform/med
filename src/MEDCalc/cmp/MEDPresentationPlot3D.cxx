// Copyright (C) 2016-2023  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "MEDPyLockWrapper.hxx"

#include "MEDPresentationPlot3D.hxx"
#include "MEDPresentationException.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationPlot3D::TYPE_NAME = "MEDPresentationPlot3D";
const std::string MEDPresentationPlot3D::PROP_PLANE_NORMAL_X = "planeNormalX";
const std::string MEDPresentationPlot3D::PROP_PLANE_NORMAL_Y = "planeNormalY";
const std::string MEDPresentationPlot3D::PROP_PLANE_NORMAL_Z = "planeNormalZ";
const std::string MEDPresentationPlot3D::PROP_PLANE_POS = "planePos";
const std::string MEDPresentationPlot3D::PROP_IS_PLANAR = "isPlanar";


MEDPresentationPlot3D::MEDPresentationPlot3D(const MEDCALC::Plot3DParameters& params,
                                                   const MEDCALC::ViewModeType viewMode) :
    MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange),
    _params(params)
{
  setDoubleProperty(MEDPresentationPlot3D::PROP_PLANE_NORMAL_X, params.planeNormal[0]);
  setDoubleProperty(MEDPresentationPlot3D::PROP_PLANE_NORMAL_Y, params.planeNormal[1]);
  setDoubleProperty(MEDPresentationPlot3D::PROP_PLANE_NORMAL_Z, params.planeNormal[2]);
  setDoubleProperty(MEDPresentationPlot3D::PROP_PLANE_POS, params.planePos);
  
  _isPlanar = 0;
  setIntProperty(MEDPresentationPlot3D::PROP_IS_PLANAR, _isPlanar);
}

void
MEDPresentationPlot3D::initFieldMeshInfos()
{
  MEDPresentation::initFieldMeshInfos();
  _colorByType = "POINTS";
}


void
MEDPresentationPlot3D::getSliceObj()
{
  std::ostringstream oss;
  oss << "__objSlice";
  _objSlice = oss.str(); oss.str("");

  oss << _objSlice << " = pvs.Slice(" << _srcObjVar << ");";
	pushAndExecPyLine(oss.str()); oss.str("");
  
  oss << _objSlice << ".SliceType.Normal = [" << 
    _params.planeNormal[0] << ", " <<
	  _params.planeNormal[1] << ", " <<
	  _params.planeNormal[2] << "];";
  pushAndExecPyLine(oss.str()); oss.str("");

  oss << "slicePos = medcalc.GetPositions(" << _srcObjVar << ", [" <<
    _params.planeNormal[0] << ", " <<
	  _params.planeNormal[1] << ", " <<
	  _params.planeNormal[2] << "]," <<
    _params.planePos << ");";
  pushAndExecPyLine(oss.str()); oss.str("");

  oss << _objSlice << ".SliceType.Origin = slicePos";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << " = " << _objSlice;
  execPyLine(oss.str()); oss.str(""); 
}

void
MEDPresentationPlot3D::getMagnitude()
{
  std::ostringstream oss;
  oss << "__objCalc";
  _objCalc = oss.str(); oss.str("");

	oss << _objCalc <<"= pvs.Calculator(Input=" << _objSlice << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  if (_pvFieldType == "CELLS") {
    oss << _objCalc << ".AttributeType = 'Cell Data'";
    pushAndExecPyLine(oss.str()); oss.str("");
  }
  oss << _objCalc << ".ResultArrayName = 'resCalcMag'";
  pushAndExecPyLine(oss.str()); oss.str("");
  std::string fieldName = _fieldName;
  if(_nbComponents == 2)
    fieldName += "_Vector";
  oss << _objCalc << ".Function = 'mag(" << fieldName << ")'";
  pushAndExecPyLine(oss.str()); oss.str("");
  execPyLine(oss.str()); oss.str("");
}

void
MEDPresentationPlot3D::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  createSource();
  setTimestamp();

  fillAvailableFieldComponents();
  setOrCreateRenderView();

  std::ostringstream oss;
  oss << "is_planar = medcalc.IsPlanarObj("<< _srcObjVar<<");";
  pushAndExecPyLine(oss.str()); oss.str("");
  getPythonObjectFromMain("is_planar");
  PyObject * obj = getPythonObjectFromMain("is_planar");

  // Planar mesh?
  if (obj && PyBool_Check(obj) && (obj == Py_False))
  {
	  getSliceObj();
  }
  else
  {
    oss << "__objSlice";
    _objSlice = oss.str(); oss.str("");
    oss << _objSlice << " = " << _srcObjVar;
	  execPyLine(oss.str()); oss.str("");
    _isPlanar = 1;
    setIntProperty(MEDPresentationPlot3D::PROP_IS_PLANAR, _isPlanar);
  }
  // Vector field?
  if(_nbComponents > 1)
  {
    getMagnitude();
    oss << "scalarArray = "<< _objCalc << ".ResultArrayName;";
    execPyLine(oss.str()); oss.str("");
  }
  else
  {
    oss << "__objCalc";
    _objCalc = oss.str(); oss.str("");
    oss << _objCalc << " = " << _objSlice;
	  execPyLine(oss.str()); oss.str("");
  }
  // cell data?
  if (_pvFieldType == "CELLS")
  {
    oss << _objCalc << " = pvs.CellDatatoPointData(" << _objCalc << ");";
    oss << _objCalc << ".PassCellData = 1;";
    pushAndExecPyLine(oss.str()); oss.str("");
  }

  oss << _objVar << " = pvs.WarpByScalar(Input=" << _objCalc << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  if(_nbComponents > 1)
    oss << _objVar << ".Scalars = ['" << "POINTS" << "', " << "scalarArray]";
  else
    oss << _objVar << ".Scalars = ['" << "POINTS" << "', '" << _fieldName << "']";
  pushAndExecPyLine(oss.str()); oss.str("");
  if (obj && PyBool_Check(obj) && (obj == Py_False))
  {
    oss << _objVar << ".Normal = [" << 
    _params.planeNormal[0] << ", " <<
	  _params.planeNormal[1] << ", " <<
	  _params.planeNormal[2] << "];";
  }
  else
  {
    oss << "P2 = " << "medcalc.GetPlaneNormalVector(" << _objSlice << ")";
    execPyLine(oss.str()); oss.str("");
    PyObject * obj2 = getPythonObjectFromMain("P2");
    if (obj2 && PyList_Check(obj2)) {
      PyObject* objP0 = PyList_GetItem(obj2, 0);
      PyObject* objP1 = PyList_GetItem(obj2, 1);
      PyObject* objP2 = PyList_GetItem(obj2, 2);
      if (PyFloat_Check(objP0) && PyFloat_Check(objP1) && PyFloat_Check(objP2)) {
        _params.planeNormal[0] = PyFloat_AsDouble(objP0);
        _params.planeNormal[1] = PyFloat_AsDouble(objP1);
        _params.planeNormal[2] = PyFloat_AsDouble(objP2);
      }
    }

  setDoubleProperty(MEDPresentationPlot3D::PROP_PLANE_NORMAL_X, _params.planeNormal[0]);
  setDoubleProperty(MEDPresentationPlot3D::PROP_PLANE_NORMAL_Y, _params.planeNormal[1]);
  setDoubleProperty(MEDPresentationPlot3D::PROP_PLANE_NORMAL_Z, _params.planeNormal[2]);
    oss << _objVar << ".Normal = " << "medcalc.GetPlaneNormalVector(" << _objSlice << ")";
  }
  pushAndExecPyLine(oss.str()); oss.str("");

  showObject();
  colorBy();    // see initFieldInfo() - necessarily POINTS because of the conversion above
  showScalarBar();
  selectColorMap();
  rescaleTransferFunction();
  resetCameraAndRender();
}

void
MEDPresentationPlot3D::updatePipeline(const MEDCALC::Plot3DParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationPlot3D, MEDCALC::Plot3DParameters>(params.colorMap);

  if (params.scalarBarRange != _params.scalarBarRange ||
    params.hideDataOutsideCustomRange != _params.hideDataOutsideCustomRange ||
    params.scalarBarRangeArray[0] != _params.scalarBarRangeArray[0] ||
    params.scalarBarRangeArray[1] != _params.scalarBarRangeArray[1])
    updateScalarBarRange<MEDPresentationPlot3D, MEDCALC::Plot3DParameters>(params.scalarBarRange,
      params.hideDataOutsideCustomRange,
      params.scalarBarRangeArray[0],
      params.scalarBarRangeArray[1]);

  if (params.planeNormal[0] != _params.planeNormal[0] ||
      params.planeNormal[1] != _params.planeNormal[1] ||
      params.planeNormal[2] != _params.planeNormal[2])
    updatePlaneNormal(params.planeNormal);

  if (_isPlanar == 0 && params.planePos != _params.planePos)
    updatePlanePos(params.planePos);

  if (params.visibility != _params.visibility)
    updateVisibility<MEDPresentationPlot3D, MEDCALC::Plot3DParameters>(params.visibility);
  if (params.scalarBarVisibility != _params.scalarBarVisibility)
    updateScalarBarVisibility<MEDPresentationPlot3D, MEDCALC::Plot3DParameters>(params.scalarBarVisibility);
}

void
MEDPresentationPlot3D::updatePlaneNormal(MEDCALC::DoubleArray planeNormal)
{
  _params.planeNormal[0] = planeNormal[0];
  _params.planeNormal[1] = planeNormal[1];
  _params.planeNormal[2] = planeNormal[2];

  // GUI helper:
  setDoubleProperty(MEDPresentationPlot3D::PROP_PLANE_NORMAL_X, planeNormal[0]);
  setDoubleProperty(MEDPresentationPlot3D::PROP_PLANE_NORMAL_Y, planeNormal[1]);
  setDoubleProperty(MEDPresentationPlot3D::PROP_PLANE_NORMAL_Z, planeNormal[2]);
  
  // Update Plane Normal
  {
    MEDPyLockWrapper lock;
    std::ostringstream oss;
    if(_isPlanar == 0)
    {
      oss << _objSlice << ".SliceType.Normal = [" << 
      _params.planeNormal[0] << ", " <<
	    _params.planeNormal[1] << ", " <<
	    _params.planeNormal[2] << "];";
      pushAndExecPyLine(oss.str()); oss.str("");
      updatePlanePos(_params.planePos);
    }

    oss << _objVar << ".Normal = [" << 
    _params.planeNormal[0] << ", " <<
	  _params.planeNormal[1] << ", " <<
	  _params.planeNormal[2] << "];";
    pushAndExecPyLine(oss.str()); oss.str("");
    pushAndExecPyLine("pvs.Render();");
  }
}

void
MEDPresentationPlot3D::updatePlanePos(const double planePos)
{
  _params.planePos = planePos;

  // GUI helper:
  setDoubleProperty(MEDPresentationPlot3D::PROP_PLANE_POS, planePos);

  // Update Plane Position
  {
    MEDPyLockWrapper lock;
    std::ostringstream oss;
    oss << "slicePos = medcalc.GetPositions(" << _srcObjVar << ", [" <<
      _params.planeNormal[0] << ", " <<
	    _params.planeNormal[1] << ", " <<
	    _params.planeNormal[2] << "]," <<
      _params.planePos << ");";
    pushAndExecPyLine(oss.str()); oss.str("");

    oss << _objSlice << ".SliceType.Origin = slicePos";
    pushAndExecPyLine(oss.str()); oss.str("");
    pushAndExecPyLine("pvs.Render();");
  }
}