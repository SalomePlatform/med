// Copyright (C) 2016-2023  CEA, EDF
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

#include "MEDWidgetHelperPlot3D.hxx"
#include "MEDPresentationPlot3D.hxx"  // from component side.
#include "PresentationController.hxx"

#include <Basics_Utils.hxx>

#include <sstream>

MEDWidgetHelperPlot3D::MEDWidgetHelperPlot3D(const PresentationController* presController,
                           MEDCALC::MEDPresentationManager_ptr presManager, int presId, const std::string & presName,
                           WidgetPresentationParameters * paramW):
  MEDWidgetHelper(presController, presManager, presId, presName, paramW)
  {}

MEDWidgetHelperPlot3D::~MEDWidgetHelperPlot3D() 
{}

void MEDWidgetHelperPlot3D::loadParametersFromEngine()
{
  MEDWidgetHelper::loadParametersFromEngine();
  _planeNormal[0] = _presManager->getPresentationDoubleProperty(_presId, MEDPresentationPlot3D::PROP_PLANE_NORMAL_X.c_str());
  _planeNormal[1] = _presManager->getPresentationDoubleProperty(_presId, MEDPresentationPlot3D::PROP_PLANE_NORMAL_Y.c_str());
  _planeNormal[2] = _presManager->getPresentationDoubleProperty(_presId, MEDPresentationPlot3D::PROP_PLANE_NORMAL_Z.c_str());
  _planePos = _presManager->getPresentationDoubleProperty(_presId, MEDPresentationPlot3D::PROP_PLANE_POS.c_str());
  _isPlanar = _presManager->getPresentationIntProperty(_presId, MEDPresentationPlot3D::PROP_IS_PLANAR.c_str());
}

void MEDWidgetHelperPlot3D::updateWidget(bool connect)
{
  MEDWidgetHelper::updateWidget(connect);

  _paramWidget->setNormal(_planeNormal[0], _planeNormal[1], _planeNormal[2]);
  _paramWidget->setPlanePosition(_planePos);

  // Connect spin boxes changes
  if (connect)
  {
    QObject::connect( _paramWidget, SIGNAL(spinNormalValuesChanged(double, double, double)), this, SLOT(onPlaneNormalChanged(double, double, double)) );
    QObject::connect( _paramWidget, SIGNAL(spinPlanePosValueChanged(double)), this, SLOT(onPlanePositionChanged(double)) );
  }
  if(_isPlanar)
    _paramWidget->hidePlot3D();
}

void MEDWidgetHelperPlot3D::releaseWidget()
{
  MEDWidgetHelper::releaseWidget();

  QObject::disconnect( _paramWidget, SIGNAL(spinNormalValuesChanged(double, double, double)), this, SLOT(onPlaneNormalChanged(double, double, double)) );
  QObject::disconnect( _paramWidget, SIGNAL(spinPlanePosValueChanged(double)), this, SLOT(onPlanePositionChanged(double)) );
}

void MEDWidgetHelperPlot3D::onPlaneNormalChanged(double normX, double normY, double normZ)
{
    STDLOG("MEDWidgetHelperSlices::onNbSlicesChanged");
    PresentationEvent* event = new PresentationEvent();
    event->eventtype = PresentationEvent::EVENT_CHANGE_NORMAL;
    event->presentationId = _presId;
    event->aDoubleN[0] = normX;
    event->aDoubleN[1] = normY;
    event->aDoubleN[2] = normZ;

    emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}
  
void MEDWidgetHelperPlot3D::onPlanePositionChanged(double planePos)
{
    STDLOG("MEDWidgetHelperSlices::onNbSlicesChanged");
    PresentationEvent* event = new PresentationEvent();
    event->eventtype = PresentationEvent::EVENT_CHANGE_PLANE_POS;
    event->presentationId = _presId;
    event->aDouble3 = planePos;

    emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}