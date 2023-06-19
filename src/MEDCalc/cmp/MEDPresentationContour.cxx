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

#include "MEDPresentationContour.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationContour::TYPE_NAME = "MEDPresentationContour";
const std::string MEDPresentationContour::PROP_NB_CONTOUR = "nbContour";
const std::string MEDPresentationContour::PROB_CONTOUR_COMPONENT_ID = "contourComponent";

MEDPresentationContour::MEDPresentationContour(const MEDCALC::ContourParameters& params,
                                               const MEDCALC::ViewModeType viewMode) :
        MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange),
        _params(params)
{
  setIntProperty(MEDPresentationContour::PROP_NB_CONTOUR, params.nbContours);
  setIntProperty(MEDPresentationContour::PROB_CONTOUR_COMPONENT_ID, 0);
  std::stringstream oss;
  oss << "__contourProgrammable" << GeneratePythonId();
  _countourProgrammableVar = oss.str();
}

void
MEDPresentationContour::initProgFilter() {
  std::ostringstream oss;
  std::string typ = "PointData"; // Contour Filter is applicable only to Point Data
  oss << _countourProgrammableVar << " = pvs.ProgrammableFilter(Input = " << _srcObjVar << ");";
  oss << _countourProgrammableVar << ".Script = \"\"\"import numpy as np" << std::endl;
  oss << "import paraview.vtk.numpy_interface.dataset_adapter as dsa" << std::endl;
  oss << "input0 = inputs[0]" << std::endl;
  oss << "inputDataArray=input0." << typ << "['" << _fieldName << "']" << std::endl;
  oss << "npa = inputDataArray.GetArrays()" << std::endl;
  oss << "if type(npa) == list:" << std::endl;
  oss << "\tarrs = []" << std::endl;
  oss << "\tfor a in npa:" << std::endl;
  oss << "\t\tmgm = np.linalg.norm(a, axis = -1)" << std::endl;
  oss << "\t\tmga = mgm.reshape(mgm.size, 1)" << std::endl;
  oss << "\t\tarrs.append(mga)" << std::endl;
  oss << "\tca = dsa.VTKCompositeDataArray(arrs)" << std::endl;
  oss << "\toutput." << typ << ".append(ca, '" << _fieldName << "_magnitude')" << std::endl;
  oss << "else:" << std::endl;
  oss << "\tmgm = np.linalg.norm(npa, axis = -1)" << std::endl;
  oss << "\tmga = mgm.reshape(mgm.size, 1)" << std::endl;
  oss << "\toutput." << typ << ".append(mga, '" << _fieldName << "_magnitude')" << std::endl;
  for (std::vector<std::string>::size_type ii = 1; ii < _nbComponents + 1 ; ii++) {
    oss << "dataArray" << ii << " = inputDataArray[:, [" << ii - 1 << "]]" << std::endl;
    oss << "output." << typ << ".append(dataArray" << ii << ", '" << _fieldName << "_" << ii <<  "')" << std::endl;
  }
  oss << "\"\"\"" << std::endl;
  pushAndExecPyLine(oss.str());
}

void
MEDPresentationContour::initFieldMeshInfos()
{
  MEDPresentation::initFieldMeshInfos();
  _colorByType = "POINTS";
}

void
MEDPresentationContour::setNumberContours()
{
  std::ostringstream oss;
  std::string aVar = _srcObjVar;
  if (_nbComponents > 1) {
    aVar = _countourProgrammableVar;
  }

  oss << "min_max = " << aVar << ".PointData.GetArray('" << getContourComponentName() << "').GetRange();";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << "delta = (min_max[1]-min_max[0])/float(" << _params.nbContours << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << ".Isosurfaces = [min_max[0]+0.5*delta+i*delta for i in range(" << _params.nbContours << ")];";
  pushAndExecPyLine(oss.str()); oss.str("");
}

void
MEDPresentationContour::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  createSource();
  setTimestamp();

  // Populate internal array of available components:
  fillAvailableFieldComponents();
  if (_params.nbContours < 1)
  {
    const char * mes = "Invalid number of contours!";
    STDLOG(mes);
    throw KERNEL::createSalomeException(mes);
  }
  setOrCreateRenderView(); // instantiate __viewXXX, needs to be after the exception above otherwise previous elements in the view will be hidden.

  // Contour needs point data:
  applyCellToPointIfNeeded();
  std::ostringstream oss;

  // Calculate component
  if (_nbComponents > 1)
  {
    pushAndExecPyLine(oss.str()); oss.str("");
    initProgFilter();
    _nbComponentsInThresholdInput = 1; // Because we extract all components as a separate array usign Programmable Filter
    _selectedComponentIndex = 0;
    oss << _objVar << " = pvs.Contour(Input=" << _countourProgrammableVar << ");";
    pushAndExecPyLine(oss.str()); oss.str("");

    oss << _objVar << ".ContourBy = ['POINTS', '" << _fieldName << "_magnitude" << "'];";
    pushAndExecPyLine(oss.str());
  }
  else
  {
    oss << _objVar << " = pvs.Contour(Input=" << _srcObjVar << ");";
    pushAndExecPyLine(oss.str()); oss.str("");

    oss << _objVar << ".ContourBy = ['POINTS', '" << _fieldName << "'];";
    pushAndExecPyLine(oss.str()); oss.str("");
  }

  // Colorize contour
  oss << _objVar << ".ComputeScalars = 1;";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Set number of contours
  setNumberContours();

  showObject();
  colorBy();
  showScalarBar();
  selectColorMap();
  rescaleTransferFunction();
  resetCameraAndRender();
}

void
MEDPresentationContour::updatePipeline(const MEDCALC::ContourParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (params.scalarBarRange != _params.scalarBarRange ||
      params.hideDataOutsideCustomRange != _params.hideDataOutsideCustomRange ||
    params.scalarBarRangeArray[0] != _params.scalarBarRangeArray[0] ||
    params.scalarBarRangeArray[1] != _params.scalarBarRangeArray[1])
    updateScalarBarRange<MEDPresentationContour, MEDCALC::ContourParameters>(params.scalarBarRange,
                                                                             params.hideDataOutsideCustomRange,
                                                                             params.scalarBarRangeArray[0],
                                                                             params.scalarBarRangeArray[1]);
  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationContour, MEDCALC::ContourParameters>(params.colorMap);

  if (params.nbContours != _params.nbContours)
    {
      if (params.nbContours < 1)
        {
          const char * mes = "Invalid number of contours!";
          STDLOG(mes);
          throw KERNEL::createSalomeException(mes);
        }
      updateNbContours(params.nbContours);
    }

  if (std::string(params.contourComponent) != std::string(_params.contourComponent))
    updateContourComponent(std::string(params.contourComponent));

  if (params.visibility != _params.visibility)
    updateVisibility<MEDPresentationContour, MEDCALC::ContourParameters>(params.visibility);

  if (params.scalarBarVisibility != _params.scalarBarVisibility)
    updateScalarBarVisibility<MEDPresentationContour, MEDCALC::ContourParameters>(params.scalarBarVisibility);

}

void
MEDPresentationContour::updateNbContours(const int nbContours)
{
  _params.nbContours = nbContours;

  // GUI helper:
  setIntProperty(MEDPresentationContour::PROP_NB_CONTOUR, nbContours);

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;
    setNumberContours();
    pushAndExecPyLine("pvs.Render();");
  }
}

void
MEDPresentationContour::updateContourComponent(const std::string& newCompo)
{
  _params.contourComponent = newCompo.c_str();

  int id = getContourComponentId();

  // GUI helper:
  setIntProperty(MEDPresentationContour::PROB_CONTOUR_COMPONENT_ID, id);

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;
    std::ostringstream oss;
    oss << _objVar << ".ContourBy = ['POINTS', '" << getContourComponentName() << "'];";
    pushAndExecPyLine(oss.str()); oss.str("");
    scalarBarTitle();
    pushAndExecPyLine("pvs.Render();");
  }
}


int MEDPresentationContour::getContourComponentId() const {
  int result = -1;
  if (std::string(_params.contourComponent) == "") { // Magnitude
    result = 0;
  }
  for (std::vector<std::string>::size_type i = 0; i < _nbComponents; i++) {
    std::ostringstream oss_p;
    oss_p << MEDPresentation::PROP_COMPONENT << i;
    std::string compo = getStringProperty(oss_p.str());
    if (std::string(_params.contourComponent) == compo) {
      result = i + 1;
      break;
    }
  }
  if (result == -1) {
    std::ostringstream oss;
    oss << "MEDPresentationContour::getContourComponentId(): unknown component '" <<_params.contourComponent 
      <<  "' !\n";
    STDLOG(oss.str());
    throw KERNEL::createSalomeException(oss.str().c_str());
  }
  return result;
}

std::string MEDPresentationContour::getContourComponentName() const {
  std::ostringstream result;
  result << _fieldName;
  if (_nbComponents > 1) {
    int id = getContourComponentId();
    switch (id) {
    case 0: result << "_magnitude"; break;
    default: result << "_" << id; break;
    }
  }
  return result.str();
}

std::string MEDPresentationContour::getFieldName() const {
  return getContourComponentName();
}

void
MEDPresentationContour::scalarBarTitle()
{
  if (_nbComponents >  1) {
    // get selected component name:
    int id = getContourComponentId();
    std::string compoName;
    if (id != 0)
    {
      std::ostringstream oss1;
      oss1 << MEDPresentation::PROP_COMPONENT << id - 1;
      compoName = getStringProperty(oss1.str());
    }
    else
    {
        compoName = "Magnitude";
    }
    std::ostringstream oss;
      oss << "pvs.GetScalarBar(" << getLutVar() << ").Title = '" << _fieldName << "';";
    oss << "pvs.GetScalarBar(" << getLutVar() << ").ComponentTitle = '" << compoName << "';";
    pushAndExecPyLine(oss.str()); oss.str("");
  }
  else {
    MEDPresentation::scalarBarTitle();
  }
}