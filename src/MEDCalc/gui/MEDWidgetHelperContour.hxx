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

#ifndef SRC_MEDCALC_GUI_MEDWIDGETHELPERCONTOUR_HXX_
#define SRC_MEDCALC_GUI_MEDWIDGETHELPERCONTOUR_HXX_

#include "MEDWidgetHelper.hxx"

class PresentationController;

class MEDWidgetHelperContour: public MEDWidgetHelper
{
  Q_OBJECT

public:
  MEDWidgetHelperContour(const PresentationController* presController,
                           MEDCALC::MEDPresentationManager_ptr presManager, int presId, const std::string & presName,
                           WidgetPresentationParameters * paramW);
  virtual ~MEDWidgetHelperContour();

  virtual void updateWidget(bool connect);
  virtual void releaseWidget();

  virtual std::string getPythonTag() const { return "Contour"; }

protected slots:
  void onNbContourChanged(int nbContour);
  void onContourComponentTypeChanged(int index);

protected:
  virtual void loadParametersFromEngine();

private:
  int _nbContours;
  int _contourComponent;
};

#endif /* SRC_MEDCALC_GUI_MEDWIDGETHELPERCONTOUR_HXX_ */
