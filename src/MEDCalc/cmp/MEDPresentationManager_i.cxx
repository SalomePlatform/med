// Copyright (C) 2011-2023  CEA, EDF
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

#include "MEDPresentationManager_i.hxx"
#include "MEDFactoryClient.hxx"
#include "Basics_Utils.hxx"

// presentations
#include "MEDPresentationMeshView.hxx"
#include "MEDPresentationScalarMap.hxx"
#include "MEDPresentationContour.hxx"
#include "MEDPresentationSlices.hxx"
#include "MEDPresentationVectorField.hxx"
#include "MEDPresentationDeflectionShape.hxx"
#include "MEDPresentationPointSprite.hxx"
#include "MEDPresentationPlot3D.hxx"
#include "MEDPresentationStreamLines.hxx"
#include "MEDPresentationCutSegment.hxx"

#include <SALOME_KernelServices.hxx>

#include <iostream>
#include <sstream>

MEDPresentationManager_i* MEDPresentationManager_i::_instance = NULL;

MEDPresentationManager_i*
MEDPresentationManager_i::getInstance() {
  if ( _instance == NULL )
    _instance = new MEDPresentationManager_i();
  return _instance;
}

MEDPresentationManager_i::MEDPresentationManager_i() :
    _presentations(),
    _activeViewPythonId(-1)
{
}

MEDPresentationManager_i::~MEDPresentationManager_i()
{
  /*
  std::vector<MEDPresentation*>::iterator itr = this->_presentations.begin();
  for ( ; itr != this->_presentations.end(); ++itr) {
    delete *itr;
    *itr = NULL;
  }
  this->_presentations.clear();
  */
}

MEDPresentation::TypeID
MEDPresentationManager_i::GenerateID()
{
  static MEDPresentation::TypeID START_ID = -1;
  START_ID++;
  return START_ID;
}

MEDPresentation*
MEDPresentationManager_i::_getPresentation(MEDPresentation::TypeID presentationID) const
{
  STDLOG("Get presentation " << presentationID);
  std::map<MEDPresentation::TypeID, MEDPresentation*>::const_iterator citr = _presentations.find(presentationID);
  if (citr == _presentations.end())
    return nullptr;
  return (*citr).second;
}

void
MEDPresentationManager_i::setPresentationStringProperty(MEDPresentation::TypeID presentationID, const char* propName, const char* propValue)
{
  MEDPresentation* pres = _getPresentation(presentationID);
  if (pres)
    pres->setStringProperty(propName, propValue);
  else
    throw KERNEL::createSalomeException("setPresentationStringProperty(): presentation not found!!");
}

char*
MEDPresentationManager_i::getPresentationStringProperty(MEDPresentation::TypeID presentationID, const char* propName)
{
  MEDPresentation* pres = _getPresentation(presentationID);
  if (pres) {
    return CORBA::string_dup(pres->getStringProperty(propName).c_str());
  }
  else
    throw KERNEL::createSalomeException("getPresentationStringProperty(): presentation not found!!");
}

void
MEDPresentationManager_i::setPresentationIntProperty(MEDPresentation::TypeID presentationID, const char* propName,
                                                     const CORBA::Long propValue)
{
  MEDPresentation* pres = _getPresentation(presentationID);
  if (pres)
    pres->setIntProperty(propName, propValue);
  else
    throw KERNEL::createSalomeException("setPresentationIntProperty(): presentation not found!!");
}

CORBA::Long
MEDPresentationManager_i::getPresentationIntProperty(MEDPresentation::TypeID presentationID, const char* propName)
{
  MEDPresentation* pres = _getPresentation(presentationID);
  if (pres) {
    return (CORBA::Long) pres->getIntProperty(propName);
  }
  else
    throw KERNEL::createSalomeException("getPresentationIntProperty(): presentation not found!!");

}

void
MEDPresentationManager_i::setPresentationDoubleProperty(MEDPresentation::TypeID presentationID, const char* propName,
  const CORBA::Double propValue)
{
  MEDPresentation* pres = _getPresentation(presentationID);
  if (pres)
    pres->setDoubleProperty(propName, propValue);
  else
    throw KERNEL::createSalomeException("setPresentationDoubleProperty(): presentation not found!!");
}

CORBA::Double
MEDPresentationManager_i::getPresentationDoubleProperty(MEDPresentation::TypeID presentationID, const char* propName)
{
  MEDPresentation* pres = _getPresentation(presentationID);
  if (pres) {
    return (CORBA::Double) pres->getDoubleProperty(propName);
  }
  else
    throw KERNEL::createSalomeException("getPresentationIntProperty(): presentation not found!!");

}


MEDPresentation::TypeID
MEDPresentationManager_i::makeMeshView(const MEDCALC::MeshViewParameters& params, const MEDCALC::ViewModeType viewMode)
{
  return _makePresentation<MEDPresentationMeshView>(params, viewMode);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makeScalarMap(const MEDCALC::ScalarMapParameters& params, const MEDCALC::ViewModeType viewMode)
{
  return _makePresentation<MEDPresentationScalarMap>(params, viewMode);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makeContour(const MEDCALC::ContourParameters& params, const MEDCALC::ViewModeType viewMode)
{
  return _makePresentation<MEDPresentationContour>(params, viewMode);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makeVectorField(const MEDCALC::VectorFieldParameters& params, const MEDCALC::ViewModeType viewMode)
{
  return _makePresentation<MEDPresentationVectorField>(params, viewMode);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makeSlices(const MEDCALC::SlicesParameters& params, const MEDCALC::ViewModeType viewMode)
{
  return _makePresentation<MEDPresentationSlices>(params, viewMode);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makeDeflectionShape(const MEDCALC::DeflectionShapeParameters& params, const MEDCALC::ViewModeType viewMode)
{
  return _makePresentation<MEDPresentationDeflectionShape>(params, viewMode);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makePointSprite(const MEDCALC::PointSpriteParameters& params, const MEDCALC::ViewModeType viewMode)
{
  return _makePresentation<MEDPresentationPointSprite>(params, viewMode);
}

// sphinx doc: begin of makePlot3D
MEDPresentation::TypeID
MEDPresentationManager_i::makePlot3D(const MEDCALC::Plot3DParameters& params, const MEDCALC::ViewModeType viewMode)
{
  return _makePresentation<MEDPresentationPlot3D>(params, viewMode);
}
// sphinx doc: end of makePlot3D

MEDPresentation::TypeID
MEDPresentationManager_i::makeStreamLines(const MEDCALC::StreamLinesParameters& params, const MEDCALC::ViewModeType viewMode)
{
  return _makePresentation<MEDPresentationStreamLines>(params, viewMode);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makeCutSegment(const MEDCALC::CutSegmentParameters& params, const MEDCALC::ViewModeType viewMode)
{
  return _makePresentation<MEDPresentationCutSegment>(params, viewMode);
}

MEDCALC::MeshViewParameters
MEDPresentationManager_i::getMeshViewParameters(MEDPresentation::TypeID presentationID)
{
  MEDCALC::MeshViewParameters* p = new MEDCALC::MeshViewParameters();
  _getParameters<MEDPresentationMeshView>(presentationID, *p);
  MEDCALC::MeshViewParameters_var tmp(p);
  return tmp._retn();
}


MEDCALC::ScalarMapParameters*
MEDPresentationManager_i::getScalarMapParameters(MEDPresentation::TypeID presentationID)
{
  MEDCALC::ScalarMapParameters* p = new MEDCALC::ScalarMapParameters();
  _getParameters<MEDPresentationScalarMap>(presentationID, *p);
  MEDCALC::ScalarMapParameters_var tmp(p);
  return tmp._retn();
}

MEDCALC::ContourParameters*
MEDPresentationManager_i::getContourParameters(MEDPresentation::TypeID presentationID)
{
  MEDCALC::ContourParameters* p = new MEDCALC::ContourParameters();
  _getParameters<MEDPresentationContour>(presentationID, *p);
  MEDCALC::ContourParameters_var tmp(p);
  return tmp._retn();
}

MEDCALC::SlicesParameters*
MEDPresentationManager_i::getSlicesParameters(MEDPresentation::TypeID presentationID)
{
  MEDCALC::SlicesParameters* p = new MEDCALC::SlicesParameters();
  _getParameters<MEDPresentationSlices>(presentationID, *p);
  MEDCALC::SlicesParameters_var tmp(p);
  return tmp._retn();
}

MEDCALC::VectorFieldParameters*
MEDPresentationManager_i::getVectorFieldParameters(MEDPresentation::TypeID presentationID)
{
  MEDCALC::VectorFieldParameters* p = new MEDCALC::VectorFieldParameters;
  _getParameters<MEDPresentationVectorField>(presentationID, *p);
  MEDCALC::VectorFieldParameters_var tmp(p);
  return tmp._retn();
}

MEDCALC::PointSpriteParameters*
MEDPresentationManager_i::getPointSpriteParameters(MEDPresentation::TypeID presentationID)
{
  MEDCALC::PointSpriteParameters* p = new MEDCALC::PointSpriteParameters();
  _getParameters<MEDPresentationPointSprite>(presentationID, *p);
  MEDCALC::PointSpriteParameters_var tmp(p);
  return tmp._retn();
}

// sphinx doc: begin of getPlot3DParameters
MEDCALC::Plot3DParameters*
MEDPresentationManager_i::getPlot3DParameters(MEDPresentation::TypeID presentationID)
{
  MEDCALC::Plot3DParameters* p = new MEDCALC::Plot3DParameters();
  _getParameters<MEDPresentationPlot3D>(presentationID, *p);
  MEDCALC::Plot3DParameters_var tmp(p);
  return tmp._retn();
}
// sphinx doc: end of getPlot3DParameters

MEDCALC::StreamLinesParameters*
MEDPresentationManager_i::getStreamLinesParameters(MEDPresentation::TypeID presentationID)
{
  MEDCALC::StreamLinesParameters* p = new MEDCALC::StreamLinesParameters();
  _getParameters<MEDPresentationStreamLines>(presentationID, *p);
  MEDCALC::StreamLinesParameters_var tmp(p);
  return tmp._retn();
}

MEDCALC::CutSegmentParameters*
MEDPresentationManager_i::getCutSegmentParameters(MEDPresentation::TypeID presentationID)
{
  MEDCALC::CutSegmentParameters* p = new MEDCALC::CutSegmentParameters();
  _getParameters<MEDPresentationCutSegment>(presentationID, *p);
  MEDCALC::CutSegmentParameters_var tmp(p);
  return tmp._retn();
}

MEDCALC::DeflectionShapeParameters*
MEDPresentationManager_i::getDeflectionShapeParameters(MEDPresentation::TypeID presentationID)
{
  MEDCALC::DeflectionShapeParameters* p = new MEDCALC::DeflectionShapeParameters();
  _getParameters<MEDPresentationDeflectionShape>(presentationID, *p);
  MEDCALC::DeflectionShapeParameters_var tmp(p);
  return tmp._retn();
}


void
MEDPresentationManager_i::updateMeshView(MEDPresentation::TypeID presentationID, const MEDCALC::MeshViewParameters& params)
{
  return _updatePresentation<MEDPresentationMeshView>(presentationID, params);
}

void
MEDPresentationManager_i::updateScalarMap(MEDPresentation::TypeID presentationID, const MEDCALC::ScalarMapParameters& params)
{
  return _updatePresentation<MEDPresentationScalarMap>(presentationID, params);
}

void
MEDPresentationManager_i::updateContour(MEDPresentation::TypeID presentationID, const MEDCALC::ContourParameters& params)
{
  return _updatePresentation<MEDPresentationContour>(presentationID, params);
}

void
MEDPresentationManager_i::updateVectorField(MEDPresentation::TypeID presentationID, const MEDCALC::VectorFieldParameters& params)
{
  return _updatePresentation<MEDPresentationVectorField>(presentationID, params);
}

void
MEDPresentationManager_i::updateSlices(MEDPresentation::TypeID presentationID, const MEDCALC::SlicesParameters& params)
{
  return _updatePresentation<MEDPresentationSlices>(presentationID, params);
}

void
MEDPresentationManager_i::updateDeflectionShape(MEDPresentation::TypeID presentationID, const MEDCALC::DeflectionShapeParameters& params)
{
  return _updatePresentation<MEDPresentationDeflectionShape>(presentationID, params);
}

void
MEDPresentationManager_i::updatePointSprite(MEDPresentation::TypeID presentationID, const MEDCALC::PointSpriteParameters& params)
{
  return _updatePresentation<MEDPresentationPointSprite>(presentationID, params);
}

// sphinx doc: begin of updatePlot3D
void
MEDPresentationManager_i::updatePlot3D(MEDPresentation::TypeID presentationID, const MEDCALC::Plot3DParameters& params)
{
  return _updatePresentation<MEDPresentationPlot3D>(presentationID, params);
}
// sphinx doc: end of updatePlot3D

void
MEDPresentationManager_i::updateStreamLines(MEDPresentation::TypeID presentationID, const MEDCALC::StreamLinesParameters& params)
{
  return _updatePresentation<MEDPresentationStreamLines>(presentationID, params);
}

void
MEDPresentationManager_i::updateCutSegment(MEDPresentation::TypeID presentationID, const MEDCALC::CutSegmentParameters& params)
{
  return _updatePresentation<MEDPresentationCutSegment>(presentationID, params);
}

CORBA::Boolean
MEDPresentationManager_i::removePresentation(MEDPresentation::TypeID presentationID)
{
  STDLOG("Remove presentation " << presentationID);
  MEDPresentation* presentation = _getPresentation(presentationID);
  
  if (!presentation)
    return false;
  
  delete presentation;
  _presentations.erase(presentationID);

  STDLOG("Presentation " << presentationID << " has been removed.");
  return true;
}

CORBA::Boolean
MEDPresentationManager_i::activateView(MEDPresentation::TypeID presentationID)
{
  std::map<MEDPresentation::TypeID, MEDPresentation*>::const_iterator citr = _presentations.find(presentationID);
  if (citr == _presentations.end()) {
    std::cerr << "activateView(): presentation not found!!" << std::endl;
    return false;
  }
  MEDPresentation* presentation = (*citr).second;

  if (!presentation->activateView())
    {
      // The view has been recreated - transfer all presentations that were in the deleted view to this new one
      int viewId = presentation->getPyViewID();
      std::map<MEDPresentation::TypeID, MEDPresentation*>::iterator citr = _presentations.begin();
      for (; citr != _presentations.end(); ++citr)
        {
          if(citr->second->getPyViewID() == viewId)
            citr->second->recreateViewSetup();
        }
    }
  _activeViewPythonId = presentation->getPyViewID();
  STDLOG("Activated view " << _activeViewPythonId);
  return true;
}

CORBA::Long
MEDPresentationManager_i::getActiveViewPythonId()
{
  //TODO: should be more elaborate to re-identify the active ParaView view when another
  //mechanism than MED module has activated another view.
  // Idea: 1/ call Python to current active view
  //       2/ compare with all id(__viewX) from currently existing presentations
  return _activeViewPythonId;
}


//MEDCALC::ViewModeType
//MEDPresentationManager_i::getPresentationViewMode(MEDPresentation::TypeID presentationID)
//{
//  MEDPresentation* pres = _getPresentation(presentationID);
//  if (pres) {
//    return pres->getViewMode();
//  } else {
//    std::cerr << "setPresentationProperty(): presentation not found!!" << std::endl;
//    return MEDCALC::VIEW_MODE_DEFAULT;
//  }
//}

char *
MEDPresentationManager_i::getParavisDump(MEDPresentation::TypeID presentationID)
{
  MEDPresentation* pres = _getPresentation(presentationID);
  if (pres) {
    return CORBA::string_dup(pres->paravisDump().c_str());
  }
  else
    throw KERNEL::createSalomeException("getParavisDump(): presentation not found!!");
}

MEDCALC::PresentationsList*
MEDPresentationManager_i::getAllPresentations()
{
  MEDCALC::PresentationsList* presList = new MEDCALC::PresentationsList;
  presList->length(_presentations.size());
  std::map<MEDPresentation::TypeID, MEDPresentation*>::const_iterator it;
  int i;
  for (i = 0, it = _presentations.begin(); it != _presentations.end(); ++it, ++i)
    (*presList)[i] = it->first;
  return presList;
}

MEDCALC::PresentationVisibility 
MEDPresentationManager_i::stateInActiveView(MEDPresentation::TypeID presentationID)
{
  MEDPresentation* pres = _getPresentation(presentationID);
  if (pres)
    return pres->presentationStateInActiveView();
  else
    throw KERNEL::createSalomeException("stateInActiveView(): presentation not found!!");
}
 

void
MEDPresentationManager_i::cleanUp()
{
  STDLOG("Cleanup");
  _activeViewPythonId = -1;
  std::map<MEDPresentation::TypeID, MEDPresentation*>::iterator it;
  for (it = _presentations.begin(); it != _presentations.end(); ++it)
    delete(it->second);
  _presentations.clear();
}
