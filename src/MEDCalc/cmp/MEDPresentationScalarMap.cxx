// Copyright (C) 2016-2022  CEA/DEN, EDF R&D
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

#include "MEDPresentationScalarMap.hxx"
#include "MEDPresentationException.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationScalarMap::TYPE_NAME = "MEDPresentationScalarMap";

MEDPresentationScalarMap::MEDPresentationScalarMap(const MEDCALC::ScalarMapParameters& params,
                                                   const MEDCALC::ViewModeType viewMode) :
    MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange), _params(params)
{
}

void
MEDPresentationScalarMap::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  setOrCreateRenderView(); // instantiate __viewXXX
  createSource();
  setTimestamp();

  // Populate internal array of available components:
  fillAvailableFieldComponents();

  // Nothing to do in a scalar map, obj = source:
  pushAndExecPyLine(_objVar + " = " + _srcObjVar);

  showObject();

  colorBy();
  showScalarBar();
  selectColorMap();
  rescaleTransferFunction();
  resetCameraAndRender();
  pushAndExecPyLine(_dispVar+".SetRepresentationType('Surface')");
}

void
MEDPresentationScalarMap::updatePipeline(const MEDCALC::ScalarMapParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationScalarMap, MEDCALC::ScalarMapParameters>(params.colorMap);

  if (std::string(params.displayedComponent) != std::string(_params.displayedComponent))
    updateComponent<MEDPresentationScalarMap, MEDCALC::ScalarMapParameters>(std::string(params.displayedComponent));
  if (params.scalarBarRange != _params.scalarBarRange ||
      params.hideDataOutsideCustomRange != _params.hideDataOutsideCustomRange ||
      params.scalarBarRangeArray[0] != _params.scalarBarRangeArray[0] ||
      params.scalarBarRangeArray[1] != _params.scalarBarRangeArray[1] )
    updateScalarBarRange<MEDPresentationScalarMap, MEDCALC::ScalarMapParameters>(params.scalarBarRange,
                                                                                 params.hideDataOutsideCustomRange,
                                                                                 params.scalarBarRangeArray[0],
                                                                                 params.scalarBarRangeArray[1]);
  if (params.visibility != _params.visibility)
    updateVisibility<MEDPresentationScalarMap, MEDCALC::ScalarMapParameters>(params.visibility);
  if (params.scalarBarVisibility != _params.scalarBarVisibility)
    updateScalarBarVisibility<MEDPresentationScalarMap, MEDCALC::ScalarMapParameters>(params.scalarBarVisibility);
}

