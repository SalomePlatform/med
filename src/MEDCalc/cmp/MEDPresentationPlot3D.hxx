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

#ifndef SRC_MEDCALC_CMP_MEDPRESENTATION_PLOT3D_HXX_
#define SRC_MEDCALC_CMP_MEDPRESENTATION_PLOT3D_HXX_

#include "MEDCALC.hxx"
#include "MEDPresentation.hxx"
#include <string>

class MEDCALC_EXPORT MEDPresentationPlot3D : public MEDPresentation
{
public:
  static const std::string TYPE_NAME;
  static const std::string PROP_PLANE_NORMAL_X;
  static const std::string PROP_PLANE_NORMAL_Y;
  static const std::string PROP_PLANE_NORMAL_Z;
  static const std::string PROP_PLANE_POS;
  static const std::string PROP_IS_PLANAR;

  MEDPresentationPlot3D(const MEDCALC::Plot3DParameters& params, const MEDCALC::ViewModeType viewMode);
  virtual ~MEDPresentationPlot3D() {}

  void updatePipeline(const MEDCALC::Plot3DParameters& params);

  void getParameters(MEDCALC::Plot3DParameters & params) const { params = _params; } ;
  void setParameters(const MEDCALC::Plot3DParameters & params) { _params = params; } ;

  void initFieldMeshInfos() override;


protected:
  virtual void internalGeneratePipeline();
  void getSliceObj();
  void getMagnitude();
  void updatePlaneNormal(const MEDCALC::DoubleArray);
  void updatePlanePos(const double);
  std::string _objSlice;
  std::string _objCalc;

private:
  MEDCALC::Plot3DParameters _params;
  int _isPlanar;
};

#endif