// Copyright (C) 2016  CEA/DEN, EDF R&D
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

#include "MEDPresentationMeshView.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationMeshView::TYPE_NAME = "MEDPresentationMeshView";
const std::string MEDPresentationMeshView::PROP_MESH_MODE = "meshMode";

MEDPresentationMeshView::MEDPresentationMeshView(const MEDCALC::MeshViewParameters& params,
                                               const MEDCALC::ViewModeType viewMode) :
        // Cheating a bit here - a mesh view doesn't need a color map or a range:
        MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, MEDCALC::COLOR_MAP_DEFAULT,
                        MEDCALC::SCALAR_BAR_RANGE_DEFAULT),
        _params(params)
{
  setIntProperty(MEDPresentationMeshView::PROP_MESH_MODE, params.mode);
}

std::string
MEDPresentationMeshView::getMeshViewCommand() const
{
  std::ostringstream oss1;

  switch (_params.mode)
  {
    case MEDCALC::MESH_MODE_WIREFRAME:
      oss1 << _dispVar << ".SetRepresentationType('Wireframe');";
      break;
    case MEDCALC::MESH_MODE_SURFACE_EDGES:
      oss1 << _dispVar << ".SetRepresentationType('Surface With Edges');";
      break;
    case MEDCALC::MESH_MODE_SURFACE:
      oss1 << _dispVar << ".SetRepresentationType('Surface');";
      break;
    default:
      const char * mes = "Unexpected getMeshViewCommand error!";
      STDLOG(mes);
      throw KERNEL::createSalomeException(mes);
  }

  return oss1.str();
}

void
MEDPresentationMeshView::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  std::ostringstream oss;
  std::string view(getRenderViewVar());

  oss << _srcObjVar << " = pvs.MEDReader(FileName='" << _fileName << "');";
  pushAndExecPyLine(oss.str()); oss.str("");
  pushAndExecPyLine( getRenderViewCommand() ); // instanciate __viewXXX
  oss << _objVar << " = " << _srcObjVar << ";";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _dispVar << " = pvs.Show(" << _objVar << ", " << view << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  pushAndExecPyLine(getMeshViewCommand());
  pushAndExecPyLine(getResetCameraCommand());
  pushAndExecPyLine("pvs.Render();");
}

void
MEDPresentationMeshView::updatePipeline(const MEDCALC::MeshViewParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (params.mode != _params.mode)
    updateMeshMode(params.mode);
}

void
MEDPresentationMeshView::updateMeshMode(const MEDCALC::MeshModeType mode)
{
  _params.mode = mode;

  // GUI helper:
  setIntProperty(MEDPresentationMeshView::PROP_MESH_MODE, mode);

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;
    std::string cmd = getMeshViewCommand();
    pushAndExecPyLine(cmd);
    pushAndExecPyLine("pvs.Render();");
  }
}
