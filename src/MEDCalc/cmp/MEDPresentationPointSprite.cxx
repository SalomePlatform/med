// Copyright (C) 2016-2020  CEA/DEN, EDF R&D
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

#include "MEDPresentationPointSprite.hxx"
#include "MEDPresentationException.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationPointSprite::TYPE_NAME = "MEDPresentationPointSprite";

MEDPresentationPointSprite::MEDPresentationPointSprite(const MEDCALC::PointSpriteParameters& params,
                                                   const MEDCALC::ViewModeType viewMode) :
    MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange), _params(params)
{
}

void
MEDPresentationPointSprite::initFieldMeshInfos()
{
  MEDPresentation::initFieldMeshInfos();
  _colorByType = "POINTS";
}

void
MEDPresentationPointSprite::scaleBallRadius()
{
  std::ostringstream oss;
  int nbCompo = getIntProperty(MEDPresentation::PROP_NB_COMPONENTS);

  // set component or euclidean norm as the array to scale the ball radius
  if (nbCompo > 1)
  {
    if (_selectedComponentIndex == -1)
      oss << _dispVar << ".ScaleArrayComponent = 'Magnitude';";
    else
      oss << _dispVar << ".ScaleArrayComponent = " << _selectedComponentIndex << ";";
    pushAndExecPyLine(oss.str()); oss.str("");
  }

  // rescale transfer function to update scalar bar and get min-max range
  rescaleTransferFunction();

  // _rangeVar (to get min and max of the fields) is updated in rescaleTransferFunction
  oss << _dispVar << ".ScaleTransferFunction.RescaleTransferFunction(" << _rangeVar << "[0], " << _rangeVar << "[1]);";
  pushAndExecPyLine(oss.str()); oss.str("");
}


void
MEDPresentationPointSprite::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  setOrCreateRenderView(); // instantiate __viewXXX
  createSource();
  setTimestamp();

  // Populate internal array of available components:
  fillAvailableFieldComponents();
  int nbCompo = getIntProperty(MEDPresentation::PROP_NB_COMPONENTS);

  // Point sprite needs point data:
  applyCellToPointIfNeeded();

  pushAndExecPyLine(_objVar + " = " + _srcObjVar);

  showObject(); // needs to be done before cell size computation to be sure ParaView has computed bounds, etc ...

  colorBy();    // see initFieldInfo() - necessarily POINTS like in Contour

  std::ostringstream oss;

  // Set point sprite:
  oss << _dispVar << ".SetRepresentationType('Point Gaussian');";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _dispVar << ".ScaleByArray = 1;";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _dispVar << ".SetScaleArray = ['POINTS', '" << _fieldName << "'];";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Compute the best radius again (like reset using current data value in GUI)
  oss << "pvs.ResetProperty('GaussianRadius');";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Make the radius twice the default radius
  oss << _dispVar << ".GaussianRadius = 2*" << _dispVar << ".GaussianRadius" << ";";
  pushAndExecPyLine(oss.str()); oss.str("");

  showScalarBar();
  selectColorMap();

  // Scale the radius array with the component and the scalar map
  scaleBallRadius();

  resetCameraAndRender();
}

void
MEDPresentationPointSprite::updatePipeline(const MEDCALC::PointSpriteParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (std::string(params.displayedComponent) != std::string(_params.displayedComponent))
    {
      updateComponent<MEDPresentationPointSprite, MEDCALC::PointSpriteParameters>(std::string(params.displayedComponent));
      scaleBallRadius();
      pushAndExecPyLine("pvs.Render();");
    }
  if (params.scalarBarRange != _params.scalarBarRange)
    updateScalarBarRange<MEDPresentationPointSprite, MEDCALC::PointSpriteParameters>(params.scalarBarRange);
  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationPointSprite, MEDCALC::PointSpriteParameters>(params.colorMap);
}
