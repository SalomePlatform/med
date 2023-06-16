// Copyright (C) 2016-2023  CEA/DEN, EDF R&D
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

#include "MEDWidgetHelperSlices.hxx"
#include "MEDPresentationSlices.hxx"  // from component side.
#include "PresentationController.hxx"

#include <Basics_Utils.hxx>

#include <sstream>

MEDWidgetHelperSlices::MEDWidgetHelperSlices(const PresentationController * presController,
                                               MEDCALC::MEDPresentationManager_ptr presManager, int presId,
                                               const std::string & presName, WidgetPresentationParameters * paramW):
  MEDWidgetHelperComponent(presController, presManager, presId, presName, paramW),
  _nbSlices(-1),
  _sliceOrientation(MEDCALC::SLICE_ORIENTATION_DEFAULT)
{}

MEDWidgetHelperSlices::~MEDWidgetHelperSlices()
{}

void MEDWidgetHelperSlices::loadParametersFromEngine()
{
  MEDWidgetHelper::loadParametersFromEngine();
  _nbSlices = _presManager->getPresentationIntProperty(_presId, MEDPresentationSlices::PROP_NB_SLICES.c_str());
  _sliceOrientation = static_cast<MEDCALC::SliceOrientationType>(
      _presManager->getPresentationIntProperty(_presId, MEDPresentationSlices::PROP_SLICE_ORIENTATION.c_str()));
}

void MEDWidgetHelperSlices::updateWidget(bool connect)
{
  MEDWidgetHelperComponent::updateWidget(connect);

  _paramWidget->setNbSlices(_nbSlices);
  _paramWidget->setSliceOrientation(_sliceOrientation);

  // Connect combo box changes
  if (connect)
    {
      QObject::connect( _paramWidget, SIGNAL(spinBoxValueChanged(int)), this, SLOT(onNbSlicesChanged(int)) );
      QObject::connect( _paramWidget, SIGNAL(comboOrientIndexChanged(int)), this, SLOT(onSliceOrientationChanged(int)) );
    }
}

void MEDWidgetHelperSlices::releaseWidget()
{
  MEDWidgetHelper::releaseWidget();

  QObject::disconnect( _paramWidget, SIGNAL(spinBoxValueChanged(int)), this, SLOT(onNbSlicesChanged(int)) );
  QObject::disconnect( _paramWidget, SIGNAL(comboOrientIndexChanged(int)), this, SLOT(onSliceOrientationChanged(int)) );
}

void MEDWidgetHelperSlices::onNbSlicesChanged(int nbSlices)
{
  STDLOG("MEDWidgetHelperSlices::onNbSlicesChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_CHANGE_NB_SLICES;
  event->presentationId = _presId;
  event->anInteger = nbSlices;

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}


void MEDWidgetHelperSlices::onSliceOrientationChanged(int /*sliceOrient*/)
{
  STDLOG("MEDWidgetHelperSlices::onSliceOrientationChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_CHANGE_SLICE_ORIENTATION;
  event->presentationId = _presId;
  event->anInteger = static_cast<int>(_paramWidget->getSliceOrientation());

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}
