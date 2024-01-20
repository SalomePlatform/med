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

#include "MEDPresentationStreamLines.hxx"
#include "MEDPresentationException.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationStreamLines::TYPE_NAME = "MEDPresentationStreamLines";
const std::string MEDPresentationStreamLines::PROP_INTEGR_DIR_TYPE = "intDirType";

MEDPresentationStreamLines::MEDPresentationStreamLines(const MEDCALC::StreamLinesParameters& params,
                                                   const MEDCALC::ViewModeType viewMode) :
    MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange),
    _params(params)
{
  setIntProperty(MEDPresentationStreamLines::PROP_INTEGR_DIR_TYPE, params.integrDir);
}

std::string
MEDPresentationStreamLines::getIntegrDirType() const
{
  switch(_params.integrDir)
  {
    case MEDCALC::INTEGRATION_DIR_BOTH:
      return "BOTH";
    case MEDCALC::INTEGRATION_DIR_FORWARD:
      return "FORWARD";
    case MEDCALC::INTEGRATION_DIR_BACKWARD:
      return "BACKWARD";
    default:
      const char * mes = "Unexpected getIntegrDirType() error!";
      STDLOG(mes);
      throw KERNEL::createSalomeException(mes);
  }
  return ""; // never happens
}

void
MEDPresentationStreamLines::addThirdZeroComponent()
{
  std::ostringstream oss;
  
  oss << _objStreamCalc << " = pvs.Calculator(Input=" << _objStreamCalc << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  std::string typ = "Point Data"; // Because CellDatatoPointData filter has been applied erlier
  oss << _objStreamCalc << ".AttributeType = '" <<  typ << "';";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objStreamCalc << ".ResultArrayName = '" <<  _fieldName << "_CALC';";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objStreamCalc << ".Function = '" <<  _fieldName << "_X*iHat + " << _fieldName << "_Y*jHat + 0.0*kHat';";
  pushAndExecPyLine(oss.str()); oss.str("");
  /*
  oss << _dispVar << " = pvs.Show(" << _objStreamCalc << ", " << getRenderViewVar() << ");";
  oss << _lutVar << " = pvs.GetColorTransferFunction('" << _fieldName << "', " << _dispVar << ", separate=True);";
  execPyLine(oss.str()); oss.str("");
  oss << "pvs.ColorBy(" << getDispVar() << ", ('" << "POINTS" << "', '" << _fieldName << "_CALC'), separate=True);";
  execPyLine(oss.str()); oss.str("");
  oss << "pvs.Hide(" << _objStreamCalc << ", " << getRenderViewVar() << ");";
  execPyLine(oss.str()); oss.str("");
  */
}

void
MEDPresentationStreamLines::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;
  
  createSource();
  setTimestamp();

  fillAvailableFieldComponents();
  int nbCompo = getIntProperty(MEDPresentation::PROP_NB_COMPONENTS);
  if (nbCompo <= 1)
  {
    const char * msg = "Stream Lines field presentation does not work for scalar field!";
    STDLOG(msg);
    throw KERNEL::createSalomeException(msg);
  }

  setOrCreateRenderView();

  std::ostringstream oss;
  if (_pvFieldType == "CELLS")
  {
    oss << "__objStreamCalc";
    _objStreamCalc = oss.str(); oss.str("");
    oss << _objStreamCalc << " = pvs.CellDatatoPointData(" << _srcObjVar << ");";
    oss << _objStreamCalc << ".PassCellData = 1;";
    pushAndExecPyLine(oss.str()); oss.str("");
  }
  else
  {
    oss << "__objStreamCalc";
    _objStreamCalc = oss.str(); oss.str("");
    oss << _objStreamCalc << "=" << _srcObjVar;
    pushAndExecPyLine(oss.str()); oss.str("");
  }

  if (_nbComponents == 2)
    addThirdZeroComponent();

  std::string dirType = getIntegrDirType();

  oss.str("");
  oss << _objVar << " = " << _objStreamCalc << ";";
  oss << _objVar << ".UpdatePipeline();";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << " = pvs.StreamTracer(Input=" << _objVar <<
      ", SeedType='Point Cloud' if not medcalc.IsPlanarObj(" << _objVar <<
      ") else 'Line');";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << ".IntegrationDirection = '" << dirType << "';";
  pushAndExecPyLine(oss.str()); oss.str("");

  showObject();

  oss << "pvs.ColorBy(" << getDispVar() << ", ('" << "POINTS" << "', '" << getFieldName() << "'), separate=True);";
  pushAndExecPyLine(oss.str()); oss.str("");
  
  showScalarBar();
  selectColorMap();
  rescaleTransferFunction();
  scalarBarTitle();
  resetCameraAndRender();
}

void
MEDPresentationStreamLines::updatePipeline(const MEDCALC::StreamLinesParameters& params)
{
   if (params.fieldHandlerId != _params.fieldHandlerId)
     throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

   if (params.colorMap != _params.colorMap)
     updateColorMap<MEDPresentationStreamLines, MEDCALC::StreamLinesParameters>(params.colorMap);

   if (params.scalarBarRange != _params.scalarBarRange ||
     params.hideDataOutsideCustomRange != _params.hideDataOutsideCustomRange ||
     params.scalarBarRangeArray[0] != _params.scalarBarRangeArray[0] ||
     params.scalarBarRangeArray[1] != _params.scalarBarRangeArray[1])
     updateScalarBarRange<MEDPresentationStreamLines, MEDCALC::StreamLinesParameters>(params.scalarBarRange,
       params.hideDataOutsideCustomRange,
       params.scalarBarRangeArray[0],
       params.scalarBarRangeArray[1]);

   if (params.integrDir != _params.integrDir)
     updateIntegrDir(params.integrDir);

   if (params.visibility != _params.visibility)
     updateVisibility<MEDPresentationStreamLines, MEDCALC::StreamLinesParameters>(params.visibility);
   if (params.scalarBarVisibility != _params.scalarBarVisibility)
     updateScalarBarVisibility<MEDPresentationStreamLines, MEDCALC::StreamLinesParameters>(params.scalarBarVisibility);
}

void
MEDPresentationStreamLines::updateIntegrDir(const MEDCALC::IntegrationDirType integrDir)
{
  _params.integrDir = integrDir;

  // GUI helper:
  setIntProperty(MEDPresentationStreamLines::PROP_INTEGR_DIR_TYPE, integrDir);

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;
    std::ostringstream oss;
    std::string dirType = getIntegrDirType();
    oss << _objVar << ".IntegrationDirection = '" << dirType << "'";
    pushAndExecPyLine(oss.str()); oss.str("");
    pushAndExecPyLine("pvs.Render();");
  }
}

void
MEDPresentationStreamLines::initFieldMeshInfos()
{
  MEDPresentation::initFieldMeshInfos();
  _colorByType = "POINTS";
}

std::string MEDPresentationStreamLines::additionalThresholdInitializationActions() {
  if (_hideDataOutsideCustomRange) {
    std::ostringstream oss;
    ComponentThresold& currentThreshold = _presentationThresolds[getThresholdIndex()];
    oss << currentThreshold._thresholdVar << ".AllScalars = 0;";
    return oss.str();
  }
  return "";
}

std::string MEDPresentationStreamLines::getFieldName() const {
  std::ostringstream oss;
  oss << _fieldName;
  if (_nbComponents == 2) {
    oss << "_CALC";
  }
  return oss.str();
}

void
MEDPresentationStreamLines::scalarBarTitle()
{
  if (_nbComponents == 2 && !_hideDataOutsideCustomRange) {
    std::ostringstream oss;
    oss << "pvs.GetScalarBar(" << getLutVar() << ").Title = '" << _fieldName << "';";
    pushAndExecPyLine(oss.str());
  }
  MEDPresentation::scalarBarTitle();
}
