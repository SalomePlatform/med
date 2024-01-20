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

#ifndef SRC_MEDCALC_CMP_MEDPRESENTATION_MESHVIEW_HXX_
#define SRC_MEDCALC_CMP_MEDPRESENTATION_MESHVIEW_HXX_

#include "MEDCALC.hxx"
#include "MEDPresentation.hxx"

class MEDCALC_EXPORT MEDPresentationMeshView : public MEDPresentation
{
public:
  static const std::string TYPE_NAME;
  static const std::string PROP_MESH_MODE;

  MEDPresentationMeshView(const MEDCALC::MeshViewParameters& params, const MEDCALC::ViewModeType viewMode);
  virtual void initFieldMeshInfos();
  virtual ~MEDPresentationMeshView() {}

  void updatePipeline(const MEDCALC::MeshViewParameters& params);

  void getParameters(MEDCALC::MeshViewParameters & params) const { params = _params; } ;
  void setParameters(const MEDCALC::MeshViewParameters & params) { _params = params; } ;

  virtual void recreateViewSetup();

  virtual void showScalarBar() override {};
  virtual void hideScalarBar() override {};

protected:
  void updateMeshMode(const MEDCALC::MeshModeType mode);
  virtual void internalGeneratePipeline();

  void representationType();

private:
  MEDCALC::MeshViewParameters _params;
};

#endif
