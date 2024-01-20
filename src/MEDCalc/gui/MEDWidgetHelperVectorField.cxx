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

#include "MEDWidgetHelperVectorField.hxx"
#include "MEDPresentationVectorField.hxx"  // from component side.
#include "PresentationController.hxx"

#include <Basics_Utils.hxx>

#include <sstream>


MEDWidgetHelperVectorField::MEDWidgetHelperVectorField(const PresentationController* presController,
                           MEDCALC::MEDPresentationManager_ptr presManager, int presId, const std::string & presName,
                           WidgetPresentationParameters * paramW):
     MEDWidgetHelper(presController, presManager, presId, presName, paramW)
  {}
MEDWidgetHelperVectorField::~MEDWidgetHelperVectorField() {}

void MEDWidgetHelperVectorField::loadParametersFromEngine()
{
    MEDWidgetHelper::loadParametersFromEngine();
    _scaleFactor = _presManager->getPresentationDoubleProperty(_presId, MEDPresentationVectorField::PROP_SCALE_FACTOR.c_str());
    _customScaleFactor = _presManager->getPresentationIntProperty(_presId, MEDPresentationVectorField::PROP_CUSTOM_SCALE_FACTOR.c_str());
}

void MEDWidgetHelperVectorField::updateWidget(bool connect)
{
  MEDWidgetHelper::updateWidget(connect);
  _paramWidget->setScaleFactor(_scaleFactor);
  _paramWidget->setScaleFactorFlag(_customScaleFactor);

  // Connect spin boxes changes
  if (connect)
  {
    QObject::connect( _paramWidget, SIGNAL(spinboxScaleFactorChaged(double)), this, SLOT(onScaleFactorChanged(double)) );
    QObject::connect( _paramWidget, SIGNAL(checkboxCustomScaleFactorChanged(int)), this, SLOT(onCustomScaleFactorChanged(int)));

  }
}

void MEDWidgetHelperVectorField::releaseWidget()
{
  MEDWidgetHelper::releaseWidget();

  QObject::disconnect( _paramWidget, SIGNAL(spinboxScaleFactorChaged(double)), this, SLOT(onScaleFactorChanged(double)) );
}

void MEDWidgetHelperVectorField::onScaleFactorChanged(double scale)
{
    STDLOG("MEDWidgetHelperVectorField::onScaleFactorChanged");
    PresentationEvent* event = new PresentationEvent();
    event->eventtype = PresentationEvent::EVENT_CHANGE_SCALE_FACTOR;
    event->presentationId = _presId;
    event->aDouble3 = scale;

    emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}

void MEDWidgetHelperVectorField::onCustomScaleFactorChanged(int flag)
{
  STDLOG("MEDWidgetHelperVectorField::onScaleFactorChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_CHANGE_CUSTOM_SCALE_FACTOR;
  event->presentationId = _presId;
  event->anInteger = flag;
  event->aDouble3 = _paramWidget->getScaleFactor();

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}