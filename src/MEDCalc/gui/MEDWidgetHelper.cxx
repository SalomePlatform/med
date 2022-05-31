// Copyright (C) 2016-2022  CEA/DEN, EDF R&D
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

#include "MEDWidgetHelper.hxx"
#include "PresentationController.hxx"
#include "MEDPresentation.hxx"
#include <Basics_Utils.hxx>

MEDWidgetHelper::MEDWidgetHelper(const PresentationController * presController,
                                 MEDCALC::MEDPresentationManager_ptr presManager, int presId,
                                 const std::string & presName,  WidgetPresentationParameters * paramWidget):
  _presManager(presManager),
  _presController(presController),
  _presId(presId),
  _presName(presName),
  _paramWidget(paramWidget)
{}

MEDWidgetHelper::~MEDWidgetHelper()
{}

void MEDWidgetHelper::loadParametersFromEngine()
{
  _selectedCompo = _presManager->getPresentationIntProperty(_presId, MEDPresentation::PROP_SELECTED_COMPONENT.c_str());
  _nbCompos = _presManager->getPresentationIntProperty(_presId, MEDPresentation::PROP_NB_COMPONENTS.c_str());
  _allCompos.clear();
  _allCompos.reserve(_nbCompos);
  for (int i = 0; i < _nbCompos; i++)
    {
      std::ostringstream oss;
      oss << MEDPresentation::PROP_COMPONENT << i;
      _allCompos.push_back(_presManager->getPresentationStringProperty(_presId, oss.str().c_str()));
    }
  _colorMap = static_cast<MEDCALC::ColorMapType>(
      _presManager->getPresentationIntProperty(_presId, MEDPresentation::PROP_COLOR_MAP.c_str()));
  _scalarBarRange = static_cast<MEDCALC::ScalarBarRangeType>(
      _presManager->getPresentationIntProperty(_presId, MEDPresentation::PROP_SCALAR_BAR_RANGE.c_str()));

  _scalarBarVisibility = static_cast<bool>(
    _presManager->getPresentationIntProperty(_presId, MEDPresentation::PROP_SCALAR_BAR_VISIBILITY.c_str()));

  _scalarBarRangeArray[0] = _presManager->getPresentationDoubleProperty(_presId, MEDPresentation::PROP_SCALAR_BAR_MIN_VALUE.c_str());
  _scalarBarRangeArray[1] = _presManager->getPresentationDoubleProperty(_presId, MEDPresentation::PROP_SCALAR_BAR_MAX_VALUE.c_str());

  _hideDataOutsideCustomRange = _presManager->getPresentationIntProperty(_presId, MEDPresentation::PROP_HIDE_DATA_OUTSIDE_CUSTOM_RANGE.c_str());
}

void MEDWidgetHelper::updateWidget(bool connect)
{
  // Set presentation name
  _paramWidget->setPresName(_presName);

  // Show dynamic part of the widget (i.e. everything else than color map and scalar bar range)
  _paramWidget->toggleWidget(true);

  loadParametersFromEngine();

  // Set properly color map and scalar bar range
  STDLOG("MEDWidgetHelper::udpateWidget() colorMap is " << _colorMap);
  _paramWidget->setColorMap(_colorMap);
  STDLOG("MEDWidgetHelper::udpateWidget() scalarBarRange is " << _scalarBarRange);
  _paramWidget->setScalarBarRange(_scalarBarRange);

  _paramWidget->setScalarBarVisibility(_scalarBarVisibility);

  _paramWidget->setScalarBarRangeValue(_scalarBarRangeArray[0], _scalarBarRangeArray[1]);
 
  _paramWidget->setHideDataOutsideCustomRange(_hideDataOutsideCustomRange);

  if (connect)
    {
      QObject::connect( this, SIGNAL(presentationUpdateSignal(const PresentationEvent *)),
                              _presController, SIGNAL(presentationSignal(const PresentationEvent *)) );
      QObject::connect( _paramWidget, SIGNAL(comboScalarBarRangeIndexChanged(int)), this, SLOT(onScalarBarRangeChanged(int)) );
      QObject::connect( _paramWidget, SIGNAL(comboColorMapIndexChanged(int)), this, SLOT(onColorMapChanged(int)) );
      QObject::connect( _paramWidget, SIGNAL(checkboxScalarBarVisibilityChanged(int)), this, SLOT(onScalarBarVisibilityChanged(int)) );
      QObject::connect( _paramWidget, SIGNAL(checkboxCustomRangeChanged(int)), this, SLOT(onUseCustomRangeChanged(int)));
      QObject::connect( _paramWidget, SIGNAL(spinboxCustomRangeChanged(double, double)), this, SLOT(onCustomRangeValueChanged(double, double)));
      QObject::connect( _paramWidget, SIGNAL(checkboxHideDataOutsideCustomRangeChanged(int)), this, SLOT(onHideDataOutsideCustomRangeChanged(int)));
  }
}

void MEDWidgetHelper::releaseWidget()
{
  QObject::disconnect( this, SIGNAL(presentationUpdateSignal(const PresentationEvent *)),
                          _presController, SIGNAL(presentationSignal(const PresentationEvent *)) );
  QObject::disconnect( _paramWidget, SIGNAL(comboScalarBarRangeIndexChanged(int)), this, SLOT(onScalarBarRangeChanged(int)) );
  QObject::disconnect( _paramWidget, SIGNAL(comboColorMapIndexChanged(int)), this, SLOT(onColorMapChanged(int)) );
  QObject::disconnect( _paramWidget, SIGNAL(checkboxScalarBarVisibilityChanged(int)), this, SLOT(onScalarBarVisibilityChanged(int)));
  QObject::disconnect( _paramWidget, SIGNAL(checkboxCustomRangeChanged(int)), this, SLOT(onUseCustomRangeChanged(int)));
  QObject::disconnect( _paramWidget, SIGNAL(spinboxCustomRangeChanged(double, double)), this, SLOT(onCustomRangeChanged(double, double)));
  QObject::disconnect( _paramWidget, SIGNAL(checkboxHideDataOutsideCustomRangeChanged(int)), this, SLOT(onHideDataOutsideCustomRangeChanged(int)));



  // Reset default for color map and scalar bar range
  _paramWidget->setColorMap(MEDCALC::COLOR_MAP_DEFAULT);
  _paramWidget->setScalarBarRange(MEDCALC::SCALAR_BAR_RANGE_DEFAULT);
}

void MEDWidgetHelper::onScalarBarVisibilityChanged(int flag) {
  STDLOG("MEDWidgetHelper::onScalarBarVisibilityChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_SCALAR_BAR_VISIBILITY_CHANGED;
  event->presentationId = _presId;
  event->anInteger = flag;

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}

void MEDWidgetHelper::onComponentChanged(int idx)
{
  STDLOG("MEDWidgetHelper::onComponentChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_CHANGE_COMPONENT;
  event->presentationId = _presId;
  event->anInteger = idx;
  event->aString = _paramWidget->getComponent();

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}

void MEDWidgetHelper::onColorMapChanged(int /*idx*/)
{
  STDLOG("MEDWidgetHelper::onColorMapChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_CHANGE_COLORMAP;
  event->presentationId = _presId;
  // The actual selected item in the combo list is retrieved by the workspace controller _getColorMap()
//  event->anInteger = idx;

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}

void MEDWidgetHelper::onScalarBarRangeChanged(int idx)
{
  STDLOG("MEDWidgetHelper::onScalarBarRangeChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_CHANGE_TIME_RANGE;
  event->presentationId = _presId;
  event->anInteger = idx;
  // The actual selected item in the combo list is retrieved by the workspace controller _getScalarBarRange()
  //event->aString = _paramWidget->getScalarBarRange();

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}

void MEDWidgetHelper::onUseCustomRangeChanged(int flag) {
  STDLOG("MEDWidgetHelper::onUseCustomRangeChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_CHANGE_TIME_RANGE;
  event->presentationId = _presId;
  event->anInteger = flag ? _paramWidget->getRangeComboBox()->count() : _paramWidget->getRangeComboBox()->currentIndex();
  event->aDouble1 = _paramWidget->getMixCustomRange();
  event->aDouble2 = _paramWidget->getMaxCustomRange();

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}

void MEDWidgetHelper::onCustomRangeValueChanged(double min, double max) {
  STDLOG("MEDWidgetHelper::onCustomRangeChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_CUSTOM_RANGE_CHANGED;
  event->presentationId = _presId;
  event->aDouble1 = min;
  event->aDouble2 = max;

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}

void MEDWidgetHelper::onHideDataOutsideCustomRangeChanged(int flag) {
  STDLOG("MEDWidgetHelper::onCustomRangeFlagChanged");
  PresentationEvent* event = new PresentationEvent();
  event->eventtype = PresentationEvent::EVENT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE_CHANGED;
  event->presentationId = _presId;
  event->anInteger = flag;
  event->aDouble1 = _paramWidget->getMixCustomRange();
  event->aDouble2 = _paramWidget->getMaxCustomRange();

  emit presentationUpdateSignal(event); // --> PresentationController::processPresentationEvent
}