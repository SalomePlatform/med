// Copyright (C) 2016  CEA/DEN, EDF R&D
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

#ifndef SRC_MEDCALC_CMP_MEDPRESENTATION_CONTOUR_HXX_
#define SRC_MEDCALC_CMP_MEDPRESENTATION_CONTOUR_HXX_

#include "MEDCALC.hxx"
#include "MEDPresentation.hxx"

class MEDCALC_EXPORT MEDPresentationContour : public MEDPresentation
{
public:
  static const std::string TYPE_NAME;
  static const std::string PROP_NB_CONTOUR;

  MEDPresentationContour(const MEDCALC::ContourParameters& params, const MEDCALC::ViewModeType viewMode);
  virtual ~MEDPresentationContour() {}

  virtual void initFieldMeshInfos();

  void updatePipeline(const MEDCALC::ContourParameters& params);

  void getParameters(MEDCALC::ContourParameters & params) const { params = _params; } ;
  void setParameters(const MEDCALC::ContourParameters & params) { _params = params; } ;

protected:
  void updateNbContours(const int nbContours);
  virtual void internalGeneratePipeline();

  void setNumberContours();

private:
  MEDCALC::ContourParameters _params;
};

#endif
