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

#ifndef SRC_MEDCALC_CMP_MEDPRESENTATION_SLICES_HXX_
#define SRC_MEDCALC_CMP_MEDPRESENTATION_SLICES_HXX_

#include "MEDCALC.hxx"
#include "MEDPresentation.hxx"

class MEDCALC_EXPORT MEDPresentationSlices : public MEDPresentation
{
public:
  static const std::string TYPE_NAME;
  static const std::string PROP_NB_SLICES;
  static const std::string PROP_SLICE_ORIENTATION;

  MEDPresentationSlices(const MEDCALC::SlicesParameters& params, const MEDCALC::ViewModeType viewMode);
  virtual ~MEDPresentationSlices() {}

  void updatePipeline(const MEDCALC::SlicesParameters& params);

  void getParameters(MEDCALC::SlicesParameters & params) const { params = _params; } ;
  void setParameters(const MEDCALC::SlicesParameters & params) { _params = params; } ;

protected:
  void updateNbSlices(const int nbSlices);
  void updateOrientation(const MEDCALC::SliceOrientationType orientation);
  virtual void internalGeneratePipeline();

  void deleteGroup();
  void setSliceParametersAndGroup();
  void adaptNumberOfSlices();
  void generateAndDisplay();
  void selectSliceOrientation(const std::string & obj);

  std::string getNormalVector() const;

private:
  MEDCALC::SlicesParameters _params;
  std::string _sliceListVar;
};

#endif
