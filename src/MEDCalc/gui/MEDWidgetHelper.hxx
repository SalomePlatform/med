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

#ifndef SRC_MEDCALC_GUI_MEDWIDGETHELPER_HXX_
#define SRC_MEDCALC_GUI_MEDWIDGETHELPER_HXX_

#include <Python.h>
#include "WidgetPresentationParameters.hxx"
#include "PresentationEvent.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDPresentationManager)

class WidgetPresentationParameters;
class PresentationController;

/*!
 * The widget helper hierarchy is in charge of setting up the WidgetPresentationParameters
 * to display the Qt widgets relevant to the underlying presentation on the component side.
 */
class MEDWidgetHelper : public QObject
{
  Q_OBJECT

public:
  virtual ~MEDWidgetHelper();

  virtual void updateWidget(bool connect);
  virtual void releaseWidget();

  ///! Returns the string X in the Python command "Update<X>" or "Get<X>Parameters"
  virtual std::string getPythonTag() const = 0;

signals:
  void presentationUpdateSignal(const PresentationEvent *);

protected:
  // Only child classes to be instantiated:
  MEDWidgetHelper(const PresentationController* presController,
                  MEDCALC::MEDPresentationManager_ptr presManager, int presId, const std::string & presName,
                  WidgetPresentationParameters * paramWidget);
  virtual void loadParametersFromEngine();

protected slots:
  void onComponentChanged(int idx);
  void onColorMapChanged(int idx);
  void onScalarBarRangeChanged(int idx);
  void onScalarBarVisibilityChanged(int idx);
  void onUseCustomRangeChanged(int idx);
  void onCustomRangeValueChanged(double min, double max);
  void onHideDataOutsideCustomRangeChanged(int idx);


protected:
  ///! GUI needs to talk directly to the pres manager to activate a view, get some params, etc ...:
  MEDCALC::MEDPresentationManager_ptr _presManager;
  const PresentationController * _presController  ;
  const int _presId;
  const std::string _presName;

  WidgetPresentationParameters* _paramWidget;

  // Component related members
  int _selectedCompo;                  // Selected field component
  int _nbCompos;                       // Total number of available components
  std::vector<std::string> _allCompos; // All available components
  bool _scalarBarVisibility;
  double _scalarBarRangeArray[2];
  bool _hideDataOutsideCustomRange;

  // Color map
  MEDCALC::ColorMapType _colorMap;

  // Scalar bar range
  MEDCALC::ScalarBarRangeType _scalarBarRange;
};

#endif
