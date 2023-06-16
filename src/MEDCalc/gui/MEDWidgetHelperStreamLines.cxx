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

#include "MEDWidgetHelperStreamLines.hxx"
#include "MEDPresentationStreamLines.hxx"  // from component side.
#include "PresentationController.hxx"

#include <Basics_Utils.hxx>

#include <sstream>

MEDWidgetHelperStreamLines::MEDWidgetHelperStreamLines(const PresentationController* presController,
                           MEDCALC::MEDPresentationManager_ptr presManager, int presId, const std::string & presName,
                           WidgetPresentationParameters * paramW):
  MEDWidgetHelper(presController, presManager, presId, presName, paramW)
  {}

MEDWidgetHelperStreamLines::~MEDWidgetHelperStreamLines()
{}

void MEDWidgetHelperStreamLines::loadParametersFromEngine()
{
  MEDWidgetHelper::loadParametersFromEngine();
  _intDirType = static_cast<MEDCALC::IntegrationDirType>(
        _presManager->getPresentationIntProperty(_presId, MEDPresentationStreamLines::PROP_INTEGR_DIR_TYPE.c_str()));
}

void MEDWidgetHelperStreamLines::updateWidget(bool connect)
{
  MEDWidgetHelper::updateWidget(connect);

  _paramWidget->setIntegrationDir(_intDirType);

  // Connect spin boxes changes
  if (connect)
  {
    QObject::connect( _paramWidget, SIGNAL(comboIntegrDirIndexChanged(int)), this, SLOT(onIntegrDirTypeChanged(int)) );
  }
}

void MEDWidgetHelperStreamLines::releaseWidget()
{
  MEDWidgetHelper::releaseWidget();

  QObject::disconnect( _paramWidget, SIGNAL(comboIntegrDirIndexChanged(int)), this, SLOT(onIntegrDirTypeChanged(int)) );
}

void MEDWidgetHelperStreamLines::onIntegrDirTypeChanged(int index)
{
  STDLOG("MEDWidgetHelperStreamLines::onIntegrDirTypeChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_CHANGE_INTEGR_DIR;
  event->presentationId = _presId;
  event->anInteger = static_cast<int>(_paramWidget->getIntegrationDir());

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}

