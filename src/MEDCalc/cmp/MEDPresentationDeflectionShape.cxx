// Copyright (C) 2016-2021  CEA/DEN, EDF R&D
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

#include "MEDPresentationDeflectionShape.hxx"
#include "MEDPresentationException.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationDeflectionShape::TYPE_NAME = "MEDPresentationDeflectionShape";

MEDPresentationDeflectionShape::MEDPresentationDeflectionShape(const MEDCALC::DeflectionShapeParameters& params,
                                                   const MEDCALC::ViewModeType viewMode) :
    MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange), _params(params)
{
}

void
MEDPresentationDeflectionShape::initFieldMeshInfos()
{
  MEDPresentation::initFieldMeshInfos();
  _colorByType = "POINTS";
}

void
MEDPresentationDeflectionShape::autoScale()
{
  std::ostringstream oss;
  oss << "import medcalc;";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << ".ScaleFactor = 3.0*medcalc.ComputeCellAverageSize(" << _srcObjVar << ")/(" << _rangeVar
      << "[1]-" << _rangeVar << "[0]);";
  pushAndExecPyLine(oss.str()); oss.str("");
}

void
MEDPresentationDeflectionShape::internalGeneratePipeline()
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
      const char * msg = "Deflection shape presentation does not work for scalar field!"; // this message will appear in GUI too
      STDLOG(msg);
      throw KERNEL::createSalomeException(msg);
    }

  setOrCreateRenderView(); // needs to be after the exception above otherwise previous elements in the view will be hidden.

  // Warp needs point data:
  applyCellToPointIfNeeded();

  std::ostringstream oss;
  oss << _objVar << " = pvs.WarpByVector(Input=" << _srcObjVar << ");";
  pushAndExecPyLine(oss.str()); oss.str("");

  showObject(); // to be done first so that the scale factor computation properly works

  std::string fieldName = nbCompo <= 2 ? _fieldName + "_Vector" : _fieldName;
  oss << _objVar << ".Vectors = ['POINTS', '" << fieldName << "'];";
  pushAndExecPyLine(oss.str()); oss.str("");

  colorBy();    // see initFieldInfo() - necessarily POINTS
  showScalarBar();
  selectColorMap();
  rescaleTransferFunction();

  autoScale();   // to be called after transfer function so we have the range

  resetCameraAndRender();
}

void
MEDPresentationDeflectionShape::updatePipeline(const MEDCALC::DeflectionShapeParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (params.scalarBarRange != _params.scalarBarRange)
    {
      updateScalarBarRange<MEDPresentationDeflectionShape, MEDCALC::DeflectionShapeParameters>(params.scalarBarRange);
      autoScale();
      pushAndExecPyLine("pvs.Render();");
    }
  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationDeflectionShape, MEDCALC::DeflectionShapeParameters>(params.colorMap);
}
