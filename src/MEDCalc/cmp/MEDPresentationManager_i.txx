// Copyright (C) 2007-2021  CEA/DEN, EDF R&D
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

#ifndef _MED_PRESENTATION_MANAGER_I_TXX_
#define _MED_PRESENTATION_MANAGER_I_TXX_

#include <Basics_Utils.hxx>

template<typename PresentationType, typename PresentationParameters>
MEDPresentation::TypeID
MEDPresentationManager_i::_makePresentation(const PresentationParameters params, const MEDCALC::ViewModeType viewMode)
{
  int activeViewId = getActiveViewPythonId();

  // Replace = Remove then add
  std::vector<int> to_del;
  if (viewMode == MEDCALC::VIEW_MODE_REPLACE)
    {
      // Prepare all presentations to be removed from this view:
      std::map<MEDPresentation::TypeID, MEDPresentation*>::const_iterator it;
      for (it = _presentations.begin(); it != _presentations.end(); ++it)
        {
          int viewId2 = (*it).second->getPyViewID();
          if (viewId2 == activeViewId)
            to_del.push_back((*it).first);
        }
  }

  // Create a new presentation instance
  PresentationType* presentation = NULL;
  MEDPresentation::TypeID newID = MEDPresentationManager_i::GenerateID();
  STDLOG("Generated presentation ID: " << newID);
  try {
    presentation = new PresentationType(params, viewMode);  // on stack or on heap?? heap for now
    presentation->initFieldMeshInfos();
    // In replace or overlap mode we force the display in the active view:
    if(activeViewId != -1 && (viewMode == MEDCALC::VIEW_MODE_REPLACE || viewMode == MEDCALC::VIEW_MODE_OVERLAP))
      presentation->setPyViewID(activeViewId);
    else
      presentation->setPyViewID(newID);  // ensures a new ID for the view
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    STDLOG("Error: " << e.what());
    return -1;
  }

  STDLOG("Add new presentation " << newID);
  _presentations.insert( std::pair<MEDPresentation::TypeID, MEDPresentation *>(newID, presentation) );
  presentation->generatePipeline();

  // If generatePipeline didn't throw, we can actually remove presentations to be deleted:
  STDLOG("About to remove all presentations from view " << activeViewId);
  for (std::vector<int>::const_iterator it2 = to_del.begin(); it2 != to_del.end(); ++it2)
    removePresentation(*it2);

  // Make the view holding the newly created presentation the active one:
  activateView(newID);
  return newID;
}

template<typename PresentationType, typename PresentationParameters>
void
MEDPresentationManager_i::_updatePresentation(MEDPresentation::TypeID presentationID, const PresentationParameters params)
{
  STDLOG("Update presentation " << presentationID);
  MEDPresentation* presentation = _getPresentation(presentationID);
  if (!presentation) {
    std::cerr << "_updatePresentation(): presentation not found!!" << std::endl;
    return;
  }

  presentation->updatePipeline<PresentationType>(params);
}

template<typename PresentationType, typename PresentationParameters>
void
MEDPresentationManager_i::_getParameters(MEDPresentation::TypeID presentationID, PresentationParameters & params) const
{
  MEDPresentation* presentation = _getPresentation(presentationID);
  if (!presentation) {
    std::cerr << "_getParameters(): presentation not found!!" << std::endl;
    return;
  }

  presentation->getParameters<PresentationType>(params);
}


#endif // _MED_PRESENTATION_MANAGER_I_TXX_
