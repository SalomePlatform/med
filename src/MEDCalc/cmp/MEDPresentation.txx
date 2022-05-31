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

#ifndef _MED_PRESENTATION_TXX_
#define _MED_PRESENTATION_TXX_

#include "MEDPyLockWrapper.hxx"
#include <sstream>
#include <SALOME_KernelServices.hxx>

template<typename PresentationType, typename PresentationParameters>
void
MEDPresentation::updatePipeline(const PresentationParameters& params)
{
  static_cast<PresentationType*>(this)->updatePipeline(params);
}

template<typename PresentationType, typename PresentationParameters>
void
MEDPresentation::getParameters(PresentationParameters& params) const
{
  const PresentationType * p = static_cast<const PresentationType*>(this);
  p->getParameters(params);
}

template<typename PresentationType, typename PresentationParameters>
void
MEDPresentation::setParameters(const PresentationParameters& params)
{
  PresentationType * p = static_cast<PresentationType*>(this);
  p->setParameters(params);
}

/**
 * Update the ParaVis pipeline so that the given component appears on screen.
 * Blank means "Euclidean norm"
 * The property PROP_SELECTED_COMPONENT holding the corresponding index selected is also updated (help for the GUI).
 */
template<typename PresentationType, typename PresentationParameters>
void
MEDPresentation::updateComponent(const std::string& newCompo)
{
  PresentationType * p = static_cast<PresentationType*>(this);

  PresentationParameters params;
  p->getParameters(params);
  params.displayedComponent = newCompo.c_str();
  p->setParameters(params);

  int nbCompo = getIntProperty(MEDPresentation::PROP_NB_COMPONENTS);
  int idx = -1;
  for (int i=0; i < nbCompo; i++)
    {
      std::ostringstream oss_p;
      oss_p << MEDPresentation::PROP_COMPONENT << i;
      std::string compo = getStringProperty(oss_p.str());
      if (compo == newCompo)
        {
          idx = i;
          break;
        }
    }
  if (idx == -1 && newCompo != "")
    {
      std::string msg("updateComponent(): internal error - field component not found!");
      throw KERNEL::createSalomeException(msg.c_str());
    }
  setIntProperty(MEDPresentation::PROP_SELECTED_COMPONENT, idx+1); // +1 because idx=0 means Euclidean norm
  p->_selectedComponentIndex = idx;

  // Update ParaView pipeline:
  {
    MEDPyLockWrapper lock;

    std::ostringstream oss;
    selectFieldComponent();
    // The component has changed, we need to rescale the scalar bar to adapt:
    rescaleTransferFunction();
    pushAndExecPyLine("pvs.Render();");
  }
}

template<typename PresentationType, typename PresentationParameters>
void
MEDPresentation::updateColorMap(MEDCALC::ColorMapType colorMap)
{
  PresentationType * p = static_cast<PresentationType*>(this);

  PresentationParameters params;
  p->getParameters(params);
  params.colorMap = colorMap;
  p->setParameters(params);

  p->_colorMap = colorMap;

  // GUI helper:
  setIntProperty(MEDPresentation::PROP_COLOR_MAP, colorMap);

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;
    selectColorMap();
    pushAndExecPyLine("pvs.Render();");
  }
}

template<typename PresentationType, typename PresentationParameters>
void
MEDPresentation::updateScalarBarRange(MEDCALC::ScalarBarRangeType sbRange, 
                                      bool hideDataOutsideCustomRange,
                                      double minValue, double maxValue)
{
  PresentationType * p = static_cast<PresentationType*>(this);
  PresentationParameters params;
  p->getParameters(params);
  bool prevHideDataoutsideCustomRange = params.hideDataOutsideCustomRange;

  params.scalarBarRange = sbRange;
  params.scalarBarRangeArray[0] = minValue;
  params.scalarBarRangeArray[1] = maxValue;
  params.hideDataOutsideCustomRange = hideDataOutsideCustomRange;
  p->setParameters(params);

  p->_sbRange = sbRange;
  p->_scalarBarRangeArray[0] = minValue;
  p->_scalarBarRangeArray[1] = maxValue;
  p->_hideDataOutsideCustomRange = hideDataOutsideCustomRange;

  // GUI helper:
  setIntProperty(MEDPresentation::PROP_SCALAR_BAR_RANGE, sbRange);
  setDoubleProperty(MEDPresentation::PROP_SCALAR_BAR_MIN_VALUE, minValue);
  setDoubleProperty(MEDPresentation::PROP_SCALAR_BAR_MAX_VALUE, maxValue);
  setIntProperty(MEDPresentation::PROP_HIDE_DATA_OUTSIDE_CUSTOM_RANGE, static_cast<int>(hideDataOutsideCustomRange));
 
  // Update the pipeline:
    if (hideDataOutsideCustomRange || hideDataOutsideCustomRange != prevHideDataoutsideCustomRange) {
      MEDPyLockWrapper lock;
      threshold(); //Swith on/off threshould or update threshould range
      pushAndExecPyLine("pvs.Render();");      
    }
    else
    {
      MEDPyLockWrapper lock;
      rescaleTransferFunction();
      pushAndExecPyLine("pvs.Render();");
    }
}

template<typename PresentationType, typename PresentationParameters>
void 
MEDPresentation::updateVisibility(const bool theVisibility)
{
  PresentationType * p = static_cast<PresentationType*>(this);

  PresentationParameters params;
  p->getParameters(params);
  params.visibility = theVisibility;
  p->setParameters(params);

  p->_presentationVisibility = theVisibility;

  // GUI helper:
  setIntProperty(MEDPresentation::PROP_VISIBILITY, theVisibility);

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;
    visibility();
    pushAndExecPyLine("pvs.Render();");
  }
}

template<typename PresentationType, typename PresentationParameters>
void 
MEDPresentation::updateScalarBarVisibility(const bool theVisibility)
{
  PresentationType * p = static_cast<PresentationType*>(this);

  PresentationParameters params;
  p->getParameters(params);
  params.scalarBarVisibility = theVisibility;
  p->setParameters(params);

  p->_scalarBarVisibility = theVisibility;

  // GUI helper:
  setIntProperty(MEDPresentation::PROP_SCALAR_BAR_VISIBILITY, theVisibility);

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;
    scalarBarVisibility();
    pushAndExecPyLine("pvs.Render();");
  }
}


#endif // _MED_PRESENTATION_TXX_
