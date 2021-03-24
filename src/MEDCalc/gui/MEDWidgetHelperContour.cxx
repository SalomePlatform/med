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

#include "MEDWidgetHelperContour.hxx"
#include "MEDPresentationContour.hxx"  // from component side.
#include "PresentationController.hxx"

#include <Basics_Utils.hxx>

#include <sstream>

MEDWidgetHelperContour::MEDWidgetHelperContour(const PresentationController * presController,
                                               MEDCALC::MEDPresentationManager_ptr presManager, int presId,
                                               const std::string & presName, WidgetPresentationParameters * paramW):
  MEDWidgetHelper(presController, presManager, presId, presName, paramW),
  _nbContours(-1)
{}

MEDWidgetHelperContour::~MEDWidgetHelperContour()
{}

void MEDWidgetHelperContour::loadParametersFromEngine()
{
  MEDWidgetHelper::loadParametersFromEngine();
  _nbContours = _presManager->getPresentationIntProperty(_presId, MEDPresentationContour::PROP_NB_CONTOUR.c_str());
}

void MEDWidgetHelperContour::updateWidget(bool connect)
{
  MEDWidgetHelper::updateWidget(connect);
  STDLOG("MEDWidgetHelperContour::udpateWidget() nbContour is " << _nbContours);

  // Contour presentation needs the number of contours
  _paramWidget->setNbContour(_nbContours);

  // Connect combo box changes
  if (connect)
    {
      QObject::connect( this, SIGNAL(presentationUpdateSignal(const PresentationEvent *)),
                        _presController, SIGNAL(presentationSignal(const PresentationEvent *)) );
      QObject::connect( _paramWidget, SIGNAL(spinBoxValueChanged(int)), this, SLOT(onNbContourChanged(int)) );
    }
}

void MEDWidgetHelperContour::releaseWidget()
{
  MEDWidgetHelper::releaseWidget();

  QObject::disconnect( this, SIGNAL(presentationUpdateSignal(const PresentationEvent *)),
                       _presController, SIGNAL(presentationSignal(const PresentationEvent *)) );
//  QObject::disconnect( _paramWidget, SIGNAL(comboCompoIndexChanged(int)), this, SLOT(onComponentChanged(int)) );
  QObject::disconnect( _paramWidget, SIGNAL(spinBoxValueChanged(int)), this, SLOT(onNbContourChanged(int)) );
}

void MEDWidgetHelperContour::onNbContourChanged(int nbContour)
{
  STDLOG("MEDWidgetHelperContour::onNbContourChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_CHANGE_NB_CONTOUR;
  event->presentationId = _presId;
  event->anInteger = nbContour;

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}

