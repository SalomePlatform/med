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

#ifndef SRC_MEDCALC_GUI_MEDWIDGETHELPERVECTORFIELD_HXX_
#define SRC_MEDCALC_GUI_MEDWIDGETHELPERVECTORFIELD_HXX_

#include "MEDWidgetHelperComponent.hxx"

class PresentationController;

class MEDWidgetHelperVectorField: public MEDWidgetHelper
{
  Q_OBJECT

public:
  MEDWidgetHelperVectorField(const PresentationController* presController,
                           MEDCALC::MEDPresentationManager_ptr presManager, int presId, const std::string & presName,
                           WidgetPresentationParameters * paramW);

  virtual ~MEDWidgetHelperVectorField();

  virtual void updateWidget(bool connect);
  virtual void releaseWidget();

  virtual std::string getPythonTag() const { return "VectorField"; }

protected slots:
  void onScaleFactorChanged(double);
  void onCustomScaleFactorChanged(int);

protected:
  virtual void loadParametersFromEngine();
  double _scaleFactor;
  bool _customScaleFactor;
};

#endif /* SRC_MEDCALC_GUI_MEDWIDGETHELPERVECTORFIELD_HXX_ */
