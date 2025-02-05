// Copyright (C) 2021-2025  CEA, EDF
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

// File   : MEDCALCGUI_Displayer.cxx
// Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)

// Local includes
#include "PresentationController.hxx"
#include "MEDCALCGUI_Displayer.hxx"
#include <MEDCalcConstants.hxx>
#include <MEDFactoryClient.hxx>

// KERNEL includes
#include <Basics_Utils.hxx>
#include <SALOME_KernelServices.hxx>
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)
#include <SALOMEDSImpl_AttributeParameter.hxx>

// GUI includes
#include <PVViewer_ViewModel.h>


MEDCALCGUI_Displayer::MEDCALCGUI_Displayer(PresentationController* presentationController): 
  LightApp_Displayer(),
  _presentationController(presentationController)
{
}
MEDCALCGUI_Displayer::~MEDCALCGUI_Displayer()  
{
  _presentationController = nullptr;
}

void MEDCALCGUI_Displayer::Display(const QStringList& list, const bool /*val*/, SALOME_View* /*theView*/) 
{
  STDLOG("MEDCALCGUI_Displayer::Display");
  changeVisibility(list, true);
}

void MEDCALCGUI_Displayer::Erase(const QStringList& list, const bool forced, const bool updateViewer, SALOME_View* theView) {
  STDLOG( "MEDCALCGUI_Displayer::Erase" );
  changeVisibility(list, false);
}

bool MEDCALCGUI_Displayer::canBeDisplayed(const QString& entry, const QString& viewer_type) const {
  bool result = false;
  if (viewer_type != PVViewer_Viewer::Type())
    return result;
  MEDCALC::PresentationVisibility aState = visibilityState(entry);
  result = (aState != MEDCALC::PRESENTATION_NOT_IN_VIEW);
  STDLOG("MEDCALCGUI_Displayer::canBeDisplayed result is " << entry.toUtf8().constData() << " = " << result);
  return result;
}

bool MEDCALCGUI_Displayer::IsDisplayed(const QString& entry, SALOME_View* /*view*/) const 
{
  return (visibilityState(entry) == MEDCALC::PRESENTATION_VISIBLE);
}

void MEDCALCGUI_Displayer::changeVisibility(const QStringList& list, const bool visible)
{
  SALOMEDS::Study_var aStudy = KERNEL::getStudyServant();
  if (aStudy->_is_nil())
    return;
  QStringList::const_iterator it = list.constBegin();
  for (; it != list.constEnd(); ++it)
  {
    QString entry = *it;
    SALOMEDS::SObject_var sobject = aStudy->FindObjectID(entry.toUtf8().constData());
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeParameter_var aParam;
    if (sobject->FindAttribute(anAttr, "AttributeParameter")) {
      aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
      if (!aParam->_is_nil() && aParam->IsSet(PRESENTATION_ID, PT_INTEGER)) {
        int presId = aParam->GetInt(PRESENTATION_ID);
        if (aParam->IsSet(PRESENTATION_TYPE, PT_STRING)) {
          std::string type = aParam->GetString(PRESENTATION_TYPE);
          PresentationEvent* event = new PresentationEvent();
          event->eventtype = visible ?
            PresentationEvent::EVENT_DISPLAY_PRESENTATION : PresentationEvent::EVENT_ERASE_PRESENTATION;
          event->presentationId = presId;
          event->presentationType = PresentationController::presentationName2Type(type);
          _presentationController->emitPresentationSignal(event);
        }
      }
    }
  }
}
    
MEDCALC::PresentationVisibility
MEDCALCGUI_Displayer::visibilityState(const QString& entry) const
{
  MEDCALC::PresentationVisibility result = MEDCALC::PRESENTATION_NOT_IN_VIEW;
  SALOMEDS::Study_var aStudy = KERNEL::getStudyServant();
  if (aStudy->_is_nil())
    return result;

  SALOMEDS::SObject_var sobject = aStudy->FindObjectID(entry.toUtf8().constData());
  if (!sobject->_is_nil()) {
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeParameter_var aParam;
    if (sobject->FindAttribute(anAttr, "AttributeParameter")) {
      aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
      if (!aParam->_is_nil() && aParam->IsSet(IS_PRESENTATION, PT_BOOLEAN)) {
        if (aParam->IsSet(PRESENTATION_ID, PT_INTEGER)) {
          long prsId = aParam->GetInt(PRESENTATION_ID);
          MEDCALC::MEDPresentationManager_var presentationManager =
            MEDFactoryClient::getFactory()->getPresentationManager();
          result = presentationManager->stateInActiveView(prsId);
        }
      }
    }
  }
  STDLOG("MEDCALCGUI_Displayer::state result is " << entry.toUtf8().constData() << " = " << result);
  return result;
}