// Copyright (C) 2016-2025  CEA, EDF
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

#include "MEDPresentationCutSegment.hxx"
#include "MEDPresentationException.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationCutSegment::TYPE_NAME = "MEDPresentationCutSegment";
const std::string MEDPresentationCutSegment::PROP_POINT1_X = "cutPoint1_X";
const std::string MEDPresentationCutSegment::PROP_POINT1_Y = "cutPoint1_Y";
const std::string MEDPresentationCutSegment::PROP_POINT1_Z = "cutPoint1_Z";
const std::string MEDPresentationCutSegment::PROP_POINT2_X = "cutPoint2_X";
const std::string MEDPresentationCutSegment::PROP_POINT2_Y = "cutPoint2_Y";
const std::string MEDPresentationCutSegment::PROP_POINT2_Z = "cutPoint2_Z";


MEDPresentationCutSegment::MEDPresentationCutSegment(const MEDCALC::CutSegmentParameters& params,
                                                   const MEDCALC::ViewModeType viewMode) :
    MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange),
    _params(params)
{
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT1_X, params.point1[0]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT1_Y, params.point1[1]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT1_Z, params.point1[2]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT2_X, params.point2[0]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT2_Y, params.point2[1]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT2_Z, params.point2[2]);
  
  int id = GeneratePythonId();
  std::ostringstream oss_ch;
  oss_ch << "__chartViewDisp" << id;
  _chartViewDispVar = oss_ch.str();
}

MEDPresentationCutSegment::~MEDPresentationCutSegment() {
  //3D Presentation is erased by base class descructor,
  // hide only 2D Presentation here:
  std::ostringstream oss;
  oss << "pvs.Hide(" << _objVar << ", " << getChartViewVar() << ");";
  oss << getChartViewVar() << ".Update();";
  pushAndExecPyLine(oss.str());
}

void
MEDPresentationCutSegment::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  createSource();
  setTimestamp();

  fillAvailableFieldComponents();
  setOrCreateRenderView();
  setOrCreateChartView();

  std::ostringstream oss;
  oss << _objVar << "= pvs.PlotOverLine(Input=" << _srcObjVar << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  // Get bouding box "min" and "max" as initial values of 
  oss << "P1 = " << _objVar << ".Source.Point1.GetData()"; // Source.Point1 is paraview.servermanager.VectorProperty, so call GetData()
  execPyLine(oss.str()); oss.str("");
  PyObject * obj1 = getPythonObjectFromMain("P1");
  if (obj1 && PyList_Check(obj1)) {
    PyObject* objP0 = PyList_GetItem(obj1, 0);
    PyObject* objP1 = PyList_GetItem(obj1, 1);
    PyObject* objP2 = PyList_GetItem(obj1, 2);
    if (PyFloat_Check(objP0) && PyFloat_Check(objP1) && PyFloat_Check(objP2)) {
      _params.point1[0] = PyFloat_AsDouble(objP0);
      _params.point1[1] = PyFloat_AsDouble(objP1);
      _params.point1[2] = PyFloat_AsDouble(objP2);
    }
  }

  oss << "P2 = " << _objVar << ".Source.Point2.GetData()"; // Source.Point2 is paraview.servermanager.VectorProperty, so call GetData()
  execPyLine(oss.str()); oss.str("");
  PyObject * obj2 = getPythonObjectFromMain("P2");
  if (obj2 && PyList_Check(obj2)) {
    PyObject* objP0 = PyList_GetItem(obj2, 0);
    PyObject* objP1 = PyList_GetItem(obj2, 1);
    PyObject* objP2 = PyList_GetItem(obj2, 2);
    if (PyFloat_Check(objP0) && PyFloat_Check(objP1) && PyFloat_Check(objP2)) {
      _params.point2[0] = PyFloat_AsDouble(objP0);
      _params.point2[1] = PyFloat_AsDouble(objP1);
      _params.point2[2] = PyFloat_AsDouble(objP2);
    }
  }

  // To update GUI
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT1_X, _params.point1[0]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT1_Y, _params.point1[1]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT1_Z, _params.point1[2]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT2_X, _params.point2[0]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT2_Y, _params.point2[1]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT2_Z, _params.point2[2]);

  showObject();
  colorBy();    // see initFieldInfo() - necessarily POINTS because of the conversion above
  showScalarBar();
  selectColorMap();
  rescaleTransferFunction();
  resetCameraAndRender();
}

void
MEDPresentationCutSegment::updatePipeline(const MEDCALC::CutSegmentParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationCutSegment, MEDCALC::CutSegmentParameters>(params.colorMap);

  if (params.scalarBarRange != _params.scalarBarRange ||
    params.hideDataOutsideCustomRange != _params.hideDataOutsideCustomRange ||
    params.scalarBarRangeArray[0] != _params.scalarBarRangeArray[0] ||
    params.scalarBarRangeArray[1] != _params.scalarBarRangeArray[1])
    updateScalarBarRange<MEDPresentationCutSegment, MEDCALC::CutSegmentParameters>(params.scalarBarRange,
      params.hideDataOutsideCustomRange,
      params.scalarBarRangeArray[0],
      params.scalarBarRangeArray[1]);

  if (params.point1[0] != _params.point1[0] ||
      params.point1[2] != _params.point1[1] ||
      params.point1[1] != _params.point1[2])
    updatePoint1(params.point1);

  if (params.point2[0] != _params.point2[0] ||
      params.point2[2] != _params.point2[1] ||
      params.point2[1] != _params.point2[2])
    updatePoint2(params.point2);

  if (params.visibility != _params.visibility)
    updateVisibility<MEDPresentationCutSegment, MEDCALC::CutSegmentParameters>(params.visibility);
  if (params.scalarBarVisibility != _params.scalarBarVisibility)
    updateScalarBarVisibility<MEDPresentationCutSegment, MEDCALC::CutSegmentParameters>(params.scalarBarVisibility);
}

void
MEDPresentationCutSegment::updatePoint1(MEDCALC::DoubleArray point1)
{
  _params.point1[0] = point1[0];
  _params.point1[1] = point1[1];
  _params.point1[2] = point1[2];

  // GUI helper:
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT1_X, point1[0]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT1_Y, point1[1]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT1_Z, point1[2]);
  
  // Update Point1
  std::ostringstream oss;
  oss << _objVar << ".Source.Point1 = [" <<  
      _params.point1[0] << ", " <<
      _params.point1[1] << ", " <<
      _params.point1[2] << "];";
    pushAndExecPyLine(oss.str()); oss.str("");
    oss << getChartViewVar() << ".Update();";
    oss << "pvs.Render();";
    pushAndExecPyLine(oss.str());
}

void
MEDPresentationCutSegment::updatePoint2(MEDCALC::DoubleArray point2)
{
  _params.point2[0] = point2[0];
  _params.point2[1] = point2[1];
  _params.point2[2] = point2[2];

  // GUI helper:
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT2_X, point2[0]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT2_Y, point2[1]);
  setDoubleProperty(MEDPresentationCutSegment::PROP_POINT2_Z, point2[2]);
  
  // Update Point2
  std::ostringstream oss;
  oss << _objVar << ".Source.Point2 = [" <<  
    _params.point2[0] << ", " <<
    _params.point2[1] << ", " <<
    _params.point2[2] << "];";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << getChartViewVar() << ".Update();";
  oss << "pvs.Render();";
  pushAndExecPyLine(oss.str());
}

std::string MEDPresentationCutSegment::getChartViewVar() const {
  std::ostringstream oss;    
  oss << "__chartView" << getPyViewID();
  return oss.str();
}

void MEDPresentationCutSegment::setOrCreateChartView() {
  std::string lineChartViewName = getChartViewVar();
  std::ostringstream oss;
  oss << lineChartViewName << " = medcalc.FindOrCreateView('XYChartView');";
  pushAndExecPyLine(oss.str());
}

void MEDPresentationCutSegment::visibility() {
  // Base class implementation: hide 3D presentation in the RenderView
  MEDPresentation::visibility();
  // Show/Hide 2D Presenation
  std::ostringstream oss;
  oss << getChartViewDispVar() << ".Visibility = " << (_presentationVisibility ? "True" : "False") << ";";
  oss << getChartViewVar() << ".Update();";
  pushAndExecPyLine(oss.str());
}

void MEDPresentationCutSegment::showObject() {
  // 3D Presentation:
  MEDPresentation::showObject();
  
  // 2D Presentation:
  std::ostringstream oss;
  std::ostringstream oss_series;
  oss_series << _fieldName;
  if (_nbComponents > 1) {
    oss_series << "_Magnitude";
  }
  oss << _chartViewDispVar << " = pvs.Show(" << _objVar << ", " << getChartViewVar() << ", 'XYChartRepresentation');";
  oss << _chartViewDispVar << ".SeriesVisibility = ['" << oss_series.str() << "'];";
  oss << getChartViewVar() << ".Update();";
  pushAndExecPyLine(oss.str());
}

void MEDPresentationCutSegment::hideObject() {
  // 3D Presentation:
  MEDPresentation::hideObject();

  // 2D Presentation:
  std::ostringstream oss;
  oss << "pvs.Hide(" << _objVar << ", " << getChartViewVar() << ");";
  oss << getChartViewVar() << ".Update();";
  pushAndExecPyLine(oss.str());
}

std::string MEDPresentationCutSegment::getChartViewDispVar() {
  return _chartViewDispVar;
}

bool MEDPresentationCutSegment::activateView() {
  // if XYChartView of current MEDPresentationCutSegment is active, keep it active
  // Owerwise call impplementation of base class
  MEDPyLockWrapper lock;
  execPyLine("__XYChartViewAlive = " + getChartViewDispVar() + " in pvs.GetActiveView()");
  PyObject * obj = getPythonObjectFromMain("__XYChartViewAlive");
  bool XYChartViewAlive = true;
  if (obj && PyBool_Check(obj))
    XYChartViewAlive = (obj == Py_True);
  if (!XYChartViewAlive) {
    return MEDPresentation::activateView();
  }
  return XYChartViewAlive;
}

MEDCALC::PresentationVisibility 
MEDPresentationCutSegment::presentationStateInActiveView() {
  MEDPyLockWrapper lock;
  MEDCALC::PresentationVisibility result = MEDCALC::PRESENTATION_NOT_IN_VIEW;

  execPyLine("__isInView = ( " + getRenderViewVar() + " == pvs.GetActiveView() or "\
    + getChartViewVar() +" == pvs.GetActiveView())");
  PyObject * obj = getPythonObjectFromMain("__isInView");

  if (obj && PyBool_Check(obj) && (obj == Py_True)) {
    result = _presentationVisibility ? MEDCALC::PRESENTATION_VISIBLE : MEDCALC::PRESENTATION_INVISIBLE;
  }
  return result;
}

std::string MEDPresentationCutSegment::additionalThresholdInitializationActions() {
  if (_hideDataOutsideCustomRange) {
    std::ostringstream oss;
    ComponentThresold& currentThreshold = _presentationThresolds[getThresholdIndex()];
    oss << currentThreshold._thresholdVar << ".AllScalars = 0;";
    return oss.str();
  }
  return "";
}
