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

#ifndef WIDGET_PRESENTATION_PARAMETERS_HXX
#define WIDGET_PRESENTATION_PARAMETERS_HXX

#include "MEDCALCGUIdialogs.hxx"

#include "ui_WidgetPresentationParameters.h"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDPresentationManager)

#include <QObject>
#include <vector>
#include <string>

class MEDCALCGUI_DIALOGS_EXPORT WidgetPresentationParameters : public QWidget
{
  Q_OBJECT

public:
  WidgetPresentationParameters(QWidget* parent = 0);
  virtual ~WidgetPresentationParameters() {}

  std::string getComponent() const;
  void setComponents(std::vector<std::string> compos, int selecIndex);

  MEDCALC::MeshModeType getMeshMode() const;
  void setMeshMode(MEDCALC::MeshModeType mode);

  MEDCALC::ScalarBarRangeType getScalarBarRange() const;
  void setScalarBarRange(MEDCALC::ScalarBarRangeType);

//  double getScalarBarTimestep() const;
//  double getScalarBarMinVal() const;
//  double getScalarBarMaxVal() const;
  MEDCALC::ColorMapType getColorMap() const;
  void setColorMap(MEDCALC::ColorMapType);

  void setPresName(const std::string& name);

  int getNbContour() const;
  void setNbContour(int nbContour);

  int getNbSlices() const;
  void setNbSlices(int nbSlices);

  MEDCALC::SliceOrientationType getSliceOrientation() const;
  void setSliceOrientation(MEDCALC::SliceOrientationType orient);

  void toggleWidget(bool show);
  bool isShown() const;

  void toggleCommonFieldWidget(bool show);

signals:
  void comboScalarBarRangeIndexChanged(int);
  void comboColorMapIndexChanged(int);
  void comboCompoIndexChanged(int);
  void comboOrientIndexChanged(int);
  void comboMeshIndexChanged(int);
  void spinBoxValueChanged(int);

private slots:
  void onComboScalarBarRangeIndexChanged(int);
  void onComboColorMapIndexChanged(int);
  void onComboCompoIndexChanged(int);
  void onComboOrientIndexChanged(int);
  void onComboMeshIndexChanged(int);
  void onSpinBoxEditingFinished();

private:
  Ui_WidgetPresentationParameters _ui; // instance of the class defined in ui_WidgetPresentationParameters.h
  bool _blockSig;
};


#endif
