// Copyright (C) 2016-2023  CEA/DEN, EDF R&D
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

#ifndef SRC_MEDCALC_CMP_MEDPRESENTATION_CUT_SEGMENT_HXX_
#define SRC_MEDCALC_CMP_MEDPRESENTATION_CUT_SEGMENT_HXX_

#include "MEDCALC.hxx"
#include "MEDPresentation.hxx"
#include <string>

class MEDCALC_EXPORT MEDPresentationCutSegment : public MEDPresentation
{
public:
  static const std::string TYPE_NAME;
  static const std::string PROP_POINT1_X;
  static const std::string PROP_POINT1_Y;
  static const std::string PROP_POINT1_Z;
  static const std::string PROP_POINT2_X;
  static const std::string PROP_POINT2_Y;
  static const std::string PROP_POINT2_Z;

  MEDPresentationCutSegment(const MEDCALC::CutSegmentParameters& params, const MEDCALC::ViewModeType viewMode);
  virtual ~MEDPresentationCutSegment();

  void updatePipeline(const MEDCALC::CutSegmentParameters& params);

  void getParameters(MEDCALC::CutSegmentParameters & params) const { params = _params; } ;
  void setParameters(const MEDCALC::CutSegmentParameters & params) { _params = params; } ;

  bool activateView() override;
  MEDCALC::PresentationVisibility presentationStateInActiveView() override;

protected:
  virtual void internalGeneratePipeline();
  void updatePoint1(const MEDCALC::DoubleArray);
  void updatePoint2(const MEDCALC::DoubleArray);

  std::string getChartViewVar() const;
  void setOrCreateChartView();
  std::string getChartViewDispVar();

  std::string additionalThresholdInitializationActions() override;
  void visibility() override;
  void showObject() override;
  void hideObject() override;


private:
  MEDCALC::CutSegmentParameters _params;
  std::string _chartViewDispVar;
};

#endif