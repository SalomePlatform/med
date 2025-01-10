// Copyright (C) 2016-2025  CEA, EDF
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

#include "MEDWidgetHelperMeshView.hxx"
#include "MEDPresentationMeshView.hxx"  // from component side.
#include "PresentationController.hxx"

#include <Basics_Utils.hxx>

#include <sstream>

MEDWidgetHelperMeshView::MEDWidgetHelperMeshView(const PresentationController * presController,
                                               MEDCALC::MEDPresentationManager_ptr presManager, int presId,
                                               const std::string & presName, WidgetPresentationParameters * paramW):
  MEDWidgetHelper(presController, presManager, presId, presName, paramW)
{
}

MEDWidgetHelperMeshView::~MEDWidgetHelperMeshView()
{}

void MEDWidgetHelperMeshView::loadParametersFromEngine()
{
  MEDWidgetHelper::loadParametersFromEngine();
  _meshMode = static_cast<MEDCALC::MeshModeType>(
          _presManager->getPresentationIntProperty(_presId, MEDPresentationMeshView::PROP_MESH_MODE.c_str()));
}

void MEDWidgetHelperMeshView::updateWidget(bool connect)
{
  MEDWidgetHelper::updateWidget(connect);

  // MeshView presentation needs the mesh mode that's all.
  _paramWidget->setMeshMode(_meshMode);
  // Hide color map and scalar bar range
  _paramWidget->toggleCommonFieldWidget(false);

  if (connect)
    {
      // Connect combo box changes
      QObject::connect( this, SIGNAL(presentationUpdateSignal(const PresentationEvent *)),
                        _presController, SIGNAL(presentationSignal(const PresentationEvent *)) );
      QObject::connect( _paramWidget, SIGNAL(comboMeshIndexChanged(int)), this, SLOT(onMeshModeChanged(int)) );
    }
}

void MEDWidgetHelperMeshView::releaseWidget()
{
  MEDWidgetHelper::releaseWidget();

  // Show color map and scalar bar range
  _paramWidget->toggleCommonFieldWidget(true);

  QObject::disconnect( this, SIGNAL(presentationUpdateSignal(const PresentationEvent *)),
                       _presController, SIGNAL(presentationSignal(const PresentationEvent *)) );
  QObject::disconnect( _paramWidget, SIGNAL(comboMeshIndexChanged(int)), this, SLOT(onMeshModeChanged(int)) );
}

void MEDWidgetHelperMeshView::onMeshModeChanged(int /*meshMode*/)
{
  STDLOG("MEDWidgetHelperMeshView::onMeshModeChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_CHANGE_MESH_MODE;
  event->presentationId = _presId;
  event->anInteger = static_cast<int>(_paramWidget->getMeshMode());

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}

