// Copyright (C) 2007-2023  CEA, EDF
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

#ifndef _MED_PRESENTATION_MANAGER_I_HXX_
#define _MED_PRESENTATION_MANAGER_I_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDDataManager)
#include CORBA_SERVER_HEADER(MEDPresentationManager)
#include "SALOME_GenericObj_i.hh"

#include "MEDDataManager_i.hxx"
#include "MEDPresentation.hxx"
#include "MEDCALC.hxx"

#include <vector>
#include <string>

class MEDPresentationManager_i: public POA_MEDCALC::MEDPresentationManager,
                                               public SALOME::GenericObj_i
{
 public:

  MEDCALC_EXPORT static MEDPresentationManager_i* getInstance();

  MEDCALC_EXPORT MEDPresentation::TypeID makeMeshView(const MEDCALC::MeshViewParameters&, const MEDCALC::ViewModeType);
  MEDCALC_EXPORT MEDPresentation::TypeID makeScalarMap(const MEDCALC::ScalarMapParameters&, const MEDCALC::ViewModeType);
  MEDCALC_EXPORT MEDPresentation::TypeID makeContour(const MEDCALC::ContourParameters&, const MEDCALC::ViewModeType);
  MEDCALC_EXPORT MEDPresentation::TypeID makeVectorField(const MEDCALC::VectorFieldParameters&, const MEDCALC::ViewModeType);
  MEDCALC_EXPORT MEDPresentation::TypeID makeSlices(const MEDCALC::SlicesParameters&, const MEDCALC::ViewModeType);
  MEDCALC_EXPORT MEDPresentation::TypeID makeDeflectionShape(const MEDCALC::DeflectionShapeParameters&, const MEDCALC::ViewModeType);
  MEDCALC_EXPORT MEDPresentation::TypeID makePointSprite(const MEDCALC::PointSpriteParameters&, const MEDCALC::ViewModeType);
  MEDCALC_EXPORT MEDPresentation::TypeID makePlot3D(const MEDCALC::Plot3DParameters&, const MEDCALC::ViewModeType);
  MEDCALC_EXPORT MEDPresentation::TypeID makeStreamLines(const MEDCALC::StreamLinesParameters&, const MEDCALC::ViewModeType);
  MEDCALC_EXPORT MEDPresentation::TypeID makeCutSegment(const MEDCALC::CutSegmentParameters&, const MEDCALC::ViewModeType);

  MEDCALC_EXPORT void setPresentationStringProperty(MEDPresentation::TypeID presentationID, const char* propName, const char* propValue);
  MEDCALC_EXPORT char* getPresentationStringProperty(MEDPresentation::TypeID presentationID, const char* propName);

  MEDCALC_EXPORT void setPresentationIntProperty(MEDPresentation::TypeID presentationID, const char* propName, const CORBA::Long propValue);
  MEDCALC_EXPORT CORBA::Long getPresentationIntProperty(MEDPresentation::TypeID presentationID, const char* propName);

  MEDCALC_EXPORT void setPresentationDoubleProperty(MEDPresentation::TypeID presentationID, const char* propName, const CORBA::Double propValue);
  MEDCALC_EXPORT CORBA::Double getPresentationDoubleProperty(MEDPresentation::TypeID presentationID, const char* propName);

  MEDCALC_EXPORT MEDCALC::MeshViewParameters   getMeshViewParameters(MEDPresentation::TypeID presentationID);
  MEDCALC_EXPORT MEDCALC::ScalarMapParameters* getScalarMapParameters(MEDPresentation::TypeID presentationID);
  MEDCALC_EXPORT MEDCALC::ContourParameters*    getContourParameters(MEDPresentation::TypeID presentationID);
  MEDCALC_EXPORT MEDCALC::SlicesParameters*    getSlicesParameters(MEDPresentation::TypeID presentationID);
  MEDCALC_EXPORT MEDCALC::VectorFieldParameters*     getVectorFieldParameters(MEDPresentation::TypeID presentationID);
  MEDCALC_EXPORT MEDCALC::PointSpriteParameters*    getPointSpriteParameters(MEDPresentation::TypeID presentationID);
  MEDCALC_EXPORT MEDCALC::Plot3DParameters*         getPlot3DParameters(MEDPresentation::TypeID presentationID);
  MEDCALC_EXPORT MEDCALC::DeflectionShapeParameters*     getDeflectionShapeParameters(MEDPresentation::TypeID presentationID);
  MEDCALC_EXPORT MEDCALC::StreamLinesParameters*     getStreamLinesParameters(MEDPresentation::TypeID presentationID);
  MEDCALC_EXPORT MEDCALC::CutSegmentParameters*     getCutSegmentParameters(MEDPresentation::TypeID presentationID);

  MEDCALC_EXPORT void updateMeshView(MEDPresentation::TypeID, const MEDCALC::MeshViewParameters&);
  MEDCALC_EXPORT void updateScalarMap(MEDPresentation::TypeID, const MEDCALC::ScalarMapParameters&);
  MEDCALC_EXPORT void updateContour(MEDPresentation::TypeID, const MEDCALC::ContourParameters&);
  MEDCALC_EXPORT void updateVectorField(MEDPresentation::TypeID, const MEDCALC::VectorFieldParameters&);
  MEDCALC_EXPORT void updateSlices(MEDPresentation::TypeID, const MEDCALC::SlicesParameters&);
  MEDCALC_EXPORT void updateDeflectionShape(MEDPresentation::TypeID, const MEDCALC::DeflectionShapeParameters&);
  MEDCALC_EXPORT void updatePointSprite(MEDPresentation::TypeID, const MEDCALC::PointSpriteParameters&);
  MEDCALC_EXPORT void updatePlot3D(MEDPresentation::TypeID, const MEDCALC::Plot3DParameters&);
  MEDCALC_EXPORT void updateStreamLines(MEDPresentation::TypeID, const MEDCALC::StreamLinesParameters&);
  MEDCALC_EXPORT void updateCutSegment(MEDPresentation::TypeID, const MEDCALC::CutSegmentParameters&);

  MEDCALC_EXPORT CORBA::Boolean removePresentation(MEDPresentation::TypeID);
  MEDCALC_EXPORT CORBA::Boolean activateView(MEDPresentation::TypeID);
  MEDCALC_EXPORT CORBA::Long getActiveViewPythonId();
//  MEDCALC_EXPORT MEDCALC::ViewModeType getPresentationViewMode(MEDPresentation::TypeID);
  MEDCALC_EXPORT char * getParavisDump(MEDPresentation::TypeID presentationID);
  MEDCALC_EXPORT MEDCALC::PresentationsList* getAllPresentations();
  MEDCALC_EXPORT void cleanUp();
  MEDCALC_EXPORT MEDCALC::PresentationVisibility stateInActiveView(MEDPresentation::TypeID presentationID);

 private:
  MEDPresentationManager_i();
  virtual ~MEDPresentationManager_i();

  static MEDPresentation::TypeID GenerateID();

  // Create a new presentation instance and return its unique ID
  template<typename PresentationType, typename PresentationParameters>
  MEDPresentation::TypeID _makePresentation(const PresentationParameters params, MEDCALC::ViewModeType viewMode);

  // Update presentation
  template<typename PresentationType, typename PresentationParameters>
  void _updatePresentation(MEDPresentation::TypeID presentationID, const PresentationParameters params);

  // Get presentation parameters
  template<typename PresentationType, typename PresentationParameters>
  void _getParameters(MEDPresentation::TypeID presentationID, PresentationParameters & params) const;

  MEDPresentation* _getPresentation(MEDPresentation::TypeID) const;

 private :

  // The MEDPresentationManager is a singleton, whose instance can be obtained
  // using the getInstance static method.
  static MEDPresentationManager_i* _instance;

  // Owns a list of MEDPresentation objects
  std::map<MEDPresentation::TypeID, MEDPresentation*> _presentations;

  long _activeViewPythonId;
};

#include "MEDPresentationManager_i.txx"

#endif // _MED_PRESENTATION_MANAGER_I_HXX_
