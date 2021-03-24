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

#include "MEDPresentationContour.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationContour::TYPE_NAME = "MEDPresentationContour";
const std::string MEDPresentationContour::PROP_NB_CONTOUR = "nbContour";

MEDPresentationContour::MEDPresentationContour(const MEDCALC::ContourParameters& params,
                                               const MEDCALC::ViewModeType viewMode) :
        MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange),
        _params(params)
{
  setIntProperty(MEDPresentationContour::PROP_NB_CONTOUR, params.nbContours);
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

  oss << "min_max = " << _srcObjVar << ".PointData.GetArray('" << _fieldName << "').GetRange();";
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
  if (getIntProperty(MEDPresentation::PROP_NB_COMPONENTS) > 1)
    {
      const char * msg = "Contour presentation only works for scalar field!"; // this message will appear in GUI too
      STDLOG(msg);
      throw KERNEL::createSalomeException(msg);
    }
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
  oss << _objVar << " = pvs.Contour(Input=" << _srcObjVar << ");";
  pushAndExecPyLine(oss.str()); oss.str("");

  showObject();

  oss << _objVar << ".ContourBy = ['POINTS', '" << _fieldName << "'];";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Colorize contour
  oss << _objVar << ".ComputeScalars = 1;";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Set number of contours
  setNumberContours();

  colorBy();    // see initFieldInfo() - necessarily POINTS because of the conversion above
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

  if (params.scalarBarRange != _params.scalarBarRange)
    updateScalarBarRange<MEDPresentationContour, MEDCALC::ContourParameters>(params.scalarBarRange);
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
