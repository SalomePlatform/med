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

#include "MEDWidgetHelperCutSegment.hxx"
#include "MEDPresentationCutSegment.hxx"  // from component side.
#include "PresentationController.hxx"

#include <Basics_Utils.hxx>

#include <sstream>

MEDWidgetHelperCutSegment::MEDWidgetHelperCutSegment(const PresentationController* presController,
                           MEDCALC::MEDPresentationManager_ptr presManager, int presId, const std::string & presName,
                           WidgetPresentationParameters * paramW):
     MEDWidgetHelper(presController, presManager, presId, presName, paramW)
  {}

MEDWidgetHelperCutSegment::~MEDWidgetHelperCutSegment() 
{}

void MEDWidgetHelperCutSegment::loadParametersFromEngine()
{
  MEDWidgetHelper::loadParametersFromEngine();
  _point1[0] = _presManager->getPresentationDoubleProperty(_presId, MEDPresentationCutSegment::PROP_POINT1_X.c_str());
  _point1[1] = _presManager->getPresentationDoubleProperty(_presId, MEDPresentationCutSegment::PROP_POINT1_Y.c_str());
  _point1[2] = _presManager->getPresentationDoubleProperty(_presId, MEDPresentationCutSegment::PROP_POINT1_Z.c_str());
  _point2[0] = _presManager->getPresentationDoubleProperty(_presId, MEDPresentationCutSegment::PROP_POINT2_X.c_str());
  _point2[1] = _presManager->getPresentationDoubleProperty(_presId, MEDPresentationCutSegment::PROP_POINT2_Y.c_str());
  _point2[2] = _presManager->getPresentationDoubleProperty(_presId, MEDPresentationCutSegment::PROP_POINT2_Z.c_str());
}

void MEDWidgetHelperCutSegment::updateWidget(bool connect)
{
  MEDWidgetHelper::updateWidget(connect);

  _paramWidget->setCutPoint1(_point1[0], _point1[1], _point1[2]);
  _paramWidget->setCutPoint2(_point2[0], _point2[1], _point2[2]);

  // Connect spin boxes changes
  if (connect)
  {
    QObject::connect( _paramWidget, SIGNAL(spinCutPoint1ValuesChanged(double, double, double)), this, SLOT(onCutPoint1Changed(double, double, double)) );
    QObject::connect( _paramWidget, SIGNAL(spinCutPoint2ValuesChanged(double, double, double)), this, SLOT(onCutPoint2Changed(double, double, double)) );
  }
}

void MEDWidgetHelperCutSegment::releaseWidget()
{
  MEDWidgetHelper::releaseWidget();

  QObject::disconnect( _paramWidget, SIGNAL(spinCutPoint1ValuesChanged(double, double, double)), this, SLOT(onCutPoint1Changed(double, double, double)) );
  QObject::disconnect( _paramWidget, SIGNAL(spinCutPoint2ValuesChanged(double, double, double)), this, SLOT(onCutPoint2Changed(double, double, double)) );
}

void MEDWidgetHelperCutSegment::onCutPoint1Changed(double x, double y, double z)
{
    STDLOG("MEDWidgetHelperCutSegment::onCutPoint1Changed");
    PresentationEvent* event = new PresentationEvent();
    event->eventtype = PresentationEvent::EVENT_CHANGE_CUT_POINT1;
    event->presentationId = _presId;
    event->aDoubleP1[0] = x;
    event->aDoubleP1[1] = y;
    event->aDoubleP1[2] = z;

    emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}

void MEDWidgetHelperCutSegment::onCutPoint2Changed(double x, double y, double z)
{
    STDLOG("MEDWidgetHelperCutSegment::onCutPoint2Changed");
    PresentationEvent* event = new PresentationEvent();
    event->eventtype = PresentationEvent::EVENT_CHANGE_CUT_POINT2;
    event->presentationId = _presId;
    event->aDoubleP2[0] = x;
    event->aDoubleP2[1] = y;
    event->aDoubleP2[2] = z;

    emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}