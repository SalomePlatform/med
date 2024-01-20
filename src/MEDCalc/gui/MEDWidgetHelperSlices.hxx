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

#ifndef SRC_MEDCALC_GUI_MEDWIDGETHELPERSLICES_HXX_
#define SRC_MEDCALC_GUI_MEDWIDGETHELPERSLICES_HXX_

#include "MEDWidgetHelperComponent.hxx"

class PresentationController;

class MEDWidgetHelperSlices: public MEDWidgetHelperComponent
{
  Q_OBJECT

public:
  MEDWidgetHelperSlices(const PresentationController* presController,
                           MEDCALC::MEDPresentationManager_ptr presManager, int presId, const std::string & presName,
                           WidgetPresentationParameters * paramW);
  virtual ~MEDWidgetHelperSlices();

  virtual void updateWidget(bool connect);
  virtual void releaseWidget();

  virtual std::string getPythonTag() const { return "Slices"; }

protected slots:
  void onSliceOrientationChanged(int idx);
  void onNbSlicesChanged(int nbSlices);

protected:
  virtual void loadParametersFromEngine();

private:
  int _nbSlices;
  MEDCALC::SliceOrientationType _sliceOrientation;
};

#endif /* SRC_MEDCALC_GUI_MEDWIDGETHELPERSLICES_HXX_ */
