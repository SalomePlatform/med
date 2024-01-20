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

#include "MEDPresentationSlices.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationSlices::TYPE_NAME = "MEDPresentationSlices";
const std::string MEDPresentationSlices::PROP_NB_SLICES = "nbSlices";
const std::string MEDPresentationSlices::PROP_SLICE_ORIENTATION = "slicesOrientation";

MEDPresentationSlices::MEDPresentationSlices(const MEDCALC::SlicesParameters& params,
                                               const MEDCALC::ViewModeType viewMode) :
        MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange),
        _params(params)
{
  setIntProperty(MEDPresentationSlices::PROP_NB_SLICES, params.nbSlices);
  setIntProperty(MEDPresentationSlices::PROP_SLICE_ORIENTATION, params.orientation);

  int id = GeneratePythonId();
  std::ostringstream oss;
  oss << "__objLst" << id;
  _sliceListVar = oss.str();
}

void
MEDPresentationSlices::setSliceParametersAndGroup()
{
  std::ostringstream oss;
  int nbSlices = getIntProperty(MEDPresentationSlices::PROP_NB_SLICES);
  std::string normal = getNormalVector();

  oss << "__origins = medcalc.GetSliceOrigins(" << _srcObjVar << ", " << nbSlices << ", " << normal << ");";
  pushAndExecPyLine(oss.str()); oss.str("");

  oss << "for sliceNum in range(" << nbSlices << "):\n";
  oss << "  " << _sliceListVar << "[sliceNum].SliceType.Normal = " << normal << ";\n";
  oss << "  " << _sliceListVar << "[sliceNum].SliceType.Origin = __origins[sliceNum];\n";
  pushAndExecPyLine(oss.str()); oss.str("");

  oss << _objVar << " = pvs.GroupDatasets(Input=" << _sliceListVar << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
}

void
MEDPresentationSlices::deleteGroup()
{
  deleteThresholds();
  std::ostringstream oss;
  oss << "pvs.Hide(" << _objVar << ");";
  oss << "pvs.Delete(" << _objVar << ");del " <<_objVar<<";";
  pushAndExecPyLine(oss.str()); oss.str("");
}

void
MEDPresentationSlices::adaptNumberOfSlices()
{
  std::ostringstream oss;
  int nbSlices = getIntProperty(MEDPresentationSlices::PROP_NB_SLICES);

  oss << "for _ in range(int(max(len(" << _sliceListVar << ")-" << nbSlices << ", 0))):\n";
  oss << "  pvs.Delete(" << _sliceListVar << ".pop());\n";
  pushAndExecPyLine(oss.str()); oss.str("");

  oss << "for _ in range(" << nbSlices << "-int(max(len(" << _sliceListVar << "), 0))):\n";
  oss << "  obj = pvs.Slice(Input=" << _srcObjVar << ");\n";
  oss << "  obj.SliceType = 'Plane';\n";
  oss << "  " << _sliceListVar << ".append(obj);\n";
  pushAndExecPyLine(oss.str()); oss.str("");
}

void
MEDPresentationSlices::generateAndDisplay()
{
  adaptNumberOfSlices();
  setSliceParametersAndGroup();
  recreateViewSetup();
}

std::string
MEDPresentationSlices::getNormalVector() const
{
  switch(_params.orientation)
  {
    case MEDCALC::SLICE_NORMAL_TO_X:
      return "[1.0, 0.0, 0.0]";
    case MEDCALC::SLICE_NORMAL_TO_Y:
      return "[0.0, 1.0, 0.0]";
    case MEDCALC::SLICE_NORMAL_TO_Z:
      return "[0.0, 0.0, 1.0]";
    case MEDCALC::SLICE_NORMAL_TO_XY:
      return "[1.0, 1.0, 0.0]";
    case MEDCALC::SLICE_NORMAL_TO_XZ:
      return "[1.0, 0.0, 1.0]";
    case MEDCALC::SLICE_NORMAL_TO_YZ:
      return "[0.0, 1.0, 1.0]";
    case MEDCALC::SLICE_NORMAL_TO_XYZ:
      return "[1.0, 1.0, 1.0]";
    default:
      const char * mes = "Unexpected getSliceOrientationCommand() error!";
      STDLOG(mes);
      throw KERNEL::createSalomeException(mes);
  }
  return ""; // never happens
}

void
MEDPresentationSlices::selectSliceOrientation(const std::string & obj)
{
  std::ostringstream oss1;
  oss1 << obj << ".SliceType.Normal = " << getNormalVector() << ";";
  pushAndExecPyLine(oss1.str()); oss1.str("");
}

void
MEDPresentationSlices::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  std::ostringstream oss;

  createSource();
  setTimestamp();

  // Populate internal array of available components:
  fillAvailableFieldComponents();
  if (_params.nbSlices < 1)
  {
      const char * mes = "Invalid number of slices!";
      STDLOG(mes);
      throw KERNEL::createSalomeException(mes);
  }

  setOrCreateRenderView(); // instantiate __viewXXX

  // Now create the initial slices list
  oss << _sliceListVar << " = [];";
  pushAndExecPyLine(oss.str()); oss.str("");

  generateAndDisplay();
}

void
MEDPresentationSlices::updatePipeline(const MEDCALC::SlicesParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (std::string(params.displayedComponent) != std::string(_params.displayedComponent))
    updateComponent<MEDPresentationSlices, MEDCALC::SlicesParameters>(std::string(params.displayedComponent));
  if (params.scalarBarRange != _params.scalarBarRange ||
      params.hideDataOutsideCustomRange != _params.hideDataOutsideCustomRange ||
      params.scalarBarRangeArray[0] != _params.scalarBarRangeArray[0] ||
      params.scalarBarRangeArray[1] != _params.scalarBarRangeArray[1])
    updateScalarBarRange<MEDPresentationSlices, MEDCALC::SlicesParameters>(params.scalarBarRange,
                                                                           params.hideDataOutsideCustomRange,
                                                                           params.scalarBarRangeArray[0],
                                                                           params.scalarBarRangeArray[1]);
  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationSlices, MEDCALC::SlicesParameters>(params.colorMap);

  if (params.orientation != _params.orientation)
    updateOrientation(params.orientation);
  if (params.nbSlices != _params.nbSlices)
    {
      if (params.nbSlices < 1)
      {
          const char * mes = "Invalid number of slices!";
          STDLOG(mes);
          throw KERNEL::createSalomeException(mes);
      }
      updateNbSlices(params.nbSlices);
    }
  if (params.visibility != _params.visibility)
    updateVisibility<MEDPresentationSlices, MEDCALC::SlicesParameters>(params.visibility);

  if (params.scalarBarVisibility != _params.scalarBarVisibility)
    updateScalarBarVisibility<MEDPresentationSlices, MEDCALC::SlicesParameters>(params.scalarBarVisibility);
}

void
MEDPresentationSlices::updateNbSlices(const int nbSlices)
{
  _params.nbSlices = nbSlices;

  // GUI helper:
  setIntProperty(MEDPresentationSlices::PROP_NB_SLICES, nbSlices);

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;
    deleteGroup();
    generateAndDisplay();
  }
}

void
MEDPresentationSlices::updateOrientation(const MEDCALC::SliceOrientationType orientation)
{
  _params.orientation = orientation;

  // GUI helper:
  setIntProperty(MEDPresentationSlices::PROP_SLICE_ORIENTATION, static_cast<int>(orientation));

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;

    deleteGroup();
    generateAndDisplay();
  }
}

