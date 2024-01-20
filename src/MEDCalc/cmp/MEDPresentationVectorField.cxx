// Copyright (C) 2016-2024  CEA, EDF
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

#include "MEDPresentationVectorField.hxx"
#include "MEDPresentationException.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationVectorField::TYPE_NAME = "MEDPresentationVectorField";
const std::string MEDPresentationVectorField::PROP_SCALE_FACTOR = "scaleFactor";
const std::string MEDPresentationVectorField::PROP_CUSTOM_SCALE_FACTOR = "customScaleFactor";

MEDPresentationVectorField::MEDPresentationVectorField(const MEDCALC::VectorFieldParameters& params,
                                                   const MEDCALC::ViewModeType viewMode) :
    MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange), _params(params)
{
  setDoubleProperty(MEDPresentationVectorField::PROP_SCALE_FACTOR, params.scaleFactor);
  setIntProperty(MEDPresentationVectorField::PROP_CUSTOM_SCALE_FACTOR, params.customScaleFactor);
}

void
MEDPresentationVectorField::initFieldMeshInfos()
{
  MEDPresentation::initFieldMeshInfos();
  _colorByType = "POINTS";
}

void
MEDPresentationVectorField::autoScale()
{
  std::ostringstream oss;
  oss << "__autoScaleF = 2.0*medcalc.ComputeCellAverageSize(" << _srcObjVar << ")/(" << _rangeVar
      << "[1]-" << _rangeVar << "[0]);";
  MEDPyLockWrapper lock;
  execPyLine(oss.str());
  PyObject * obj = getPythonObjectFromMain("__autoScaleF");
  if (obj && PyFloat_Check(obj)) {
    _params.scaleFactor = PyFloat_AsDouble(obj);
    setDoubleProperty(MEDPresentationVectorField::PROP_SCALE_FACTOR, _params.scaleFactor);
  }

}

void
MEDPresentationVectorField::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  createSource();
  setTimestamp();

  // Populate internal array of available components:
  fillAvailableFieldComponents();
  int nbCompo = getIntProperty(MEDPresentation::PROP_NB_COMPONENTS);
  if (nbCompo <= 1)
    {
      const char * msg = "Vector field presentation does not work for scalar field!"; // this message will appear in GUI too
      STDLOG(msg);
      throw KERNEL::createSalomeException(msg);
    }

  setOrCreateRenderView();  // instantiate __viewXXX, needs to be after the exception above otherwise previous elements in the view will be hidden.

  std::ostringstream oss;
  oss << _objVar << " = pvs.Glyph(Input=" << _srcObjVar << ", GlyphType='Arrow');";
  pushAndExecPyLine(oss.str()); oss.str("");

  showObject(); // to be done first so that the scale factor computation properly works

  std::string fieldName = nbCompo <= 2 ? _fieldName + "_Vector" : _fieldName;

  oss << _objVar << ".ScaleArray = ['"<< _pvFieldType << "', '" << fieldName << "'];";
  pushAndExecPyLine(oss.str()); oss.str("");

  oss << _objVar << ".OrientationArray = ['"<< _pvFieldType << "', '" << fieldName << "'];";

  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << ".VectorScaleMode = 'Scale by Magnitude';";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << ".GlyphMode = 'All Points';";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << "GlyphTransform = 'Transform2';";  // not sure this is really needed?
  pushAndExecPyLine(oss.str()); oss.str("");

  colorBy();    // see initFieldInfo() - necessarily POINTS
  showScalarBar();
  selectColorMap();
  rescaleTransferFunction();

  scale();   // to be called after transfer function so we have the range

  resetCameraAndRender();
}

void
MEDPresentationVectorField::updatePipeline(const MEDCALC::VectorFieldParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (params.scalarBarRange != _params.scalarBarRange ||
      params.hideDataOutsideCustomRange != _params.hideDataOutsideCustomRange ||
      params.scalarBarRangeArray[0] != _params.scalarBarRangeArray[0] ||
      params.scalarBarRangeArray[1] != _params.scalarBarRangeArray[1])
    {
      updateScalarBarRange<MEDPresentationVectorField, MEDCALC::VectorFieldParameters>(params.scalarBarRange,
                                                                                       params.hideDataOutsideCustomRange,
                                                                                       params.scalarBarRangeArray[0],
                                                                                       params.scalarBarRangeArray[1]);
      scale();
      pushAndExecPyLine("pvs.Render();");
    }
  if (params.scaleFactor != _params.scaleFactor ||
      params.customScaleFactor != _params.customScaleFactor) {
    updateScaleFactor(params.scaleFactor, params.customScaleFactor);
  }

  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationVectorField, MEDCALC::VectorFieldParameters>(params.colorMap);

  if (params.visibility != _params.visibility)
    updateVisibility<MEDPresentationVectorField, MEDCALC::VectorFieldParameters>(params.visibility);

  if (params.scalarBarVisibility != _params.scalarBarVisibility)
    updateScalarBarVisibility<MEDPresentationVectorField, MEDCALC::VectorFieldParameters>(params.scalarBarVisibility);

}

void
MEDPresentationVectorField::updateScaleFactor(const double scaleFactor, const bool customScaleFactor)
{
  if (customScaleFactor) {
    _params.scaleFactor = scaleFactor;
    setDoubleProperty(MEDPresentationVectorField::PROP_SCALE_FACTOR, _params.scaleFactor);
  }

  _params.customScaleFactor = customScaleFactor;

  setIntProperty(MEDPresentationVectorField::PROP_CUSTOM_SCALE_FACTOR, _params.customScaleFactor);
  scale();
}

void 
MEDPresentationVectorField::scale()
{
  if (!_params.customScaleFactor) {
    autoScale();
  }
  // Update Scale Factor
  MEDPyLockWrapper lock;
  std::ostringstream oss;
  oss << _objVar << ".ScaleFactor = " << _params.scaleFactor << ";";
  pushAndExecPyLine(oss.str()); oss.str("");
  pushAndExecPyLine("pvs.Render();");
}
