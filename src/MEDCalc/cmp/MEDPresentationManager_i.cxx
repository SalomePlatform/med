// Copyright (C) 2011-2016  CEA/DEN, EDF R&D
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

#include "MEDPresentationManager_i.hxx"
#include "MEDFactoryClient.hxx"

// presentations
#include "MEDPresentationScalarMap.hxx"
#include "MEDPresentationContour.hxx"
#include "MEDPresentationVectorField.hxx"
#include "MEDPresentationSlices.hxx"
#include "MEDPresentationDeflectionShape.hxx"
#include "MEDPresentationPointSprite.hxx"

#include <iostream>

MEDPresentationManager_i* MEDPresentationManager_i::_instance = NULL;

MEDPresentationManager_i*
MEDPresentationManager_i::getInstance() {
  if ( _instance == NULL )
    _instance = new MEDPresentationManager_i();
  return _instance;
}

MEDPresentationManager_i::MEDPresentationManager_i()
{
}

MEDPresentationManager_i::~MEDPresentationManager_i()
{
  /*
  std::vector<MEDPresentation*>::iterator itr = this->_presentations.begin();
  for ( ; itr != this->_presentations.end(); ++itr) {
    delete *itr;
    *itr = NULL;
  }
  this->_presentations.clear();
  */
}

MEDPresentation::TypeID
MEDPresentationManager_i::GenerateID()
{
  static MEDPresentation::TypeID START_ID = -1;
  START_ID++;
  return START_ID;
}

void
MEDPresentationManager_i::setPresentationProperty(MEDPresentation::TypeID presentationID, const char * propName, const char * propValue)
{
  if (_presentations.find(presentationID) != _presentations.end())
    {
      MEDPresentation * pres(_presentations[presentationID]);
      pres->setProperty(propName, propValue);
    }
  else
    {
      std::cerr << "setPresentationProperty(): presentation not found!!" << std::endl;
    }
}

char*
MEDPresentationManager_i::getPresentationProperty(MEDPresentation::TypeID presentationID, const char* propName)
{
  if (_presentations.find(presentationID) != _presentations.end()) {
    MEDPresentation* pres = _presentations[presentationID];
    return (char*) pres->getProperty(propName).c_str();
  }
  else {
    std::cerr << "getPresentationProperty(): presentation not found!!" << std::endl;
    return (char*) "";
  }
}

MEDPresentation::TypeID
MEDPresentationManager_i::makeScalarMap(const MEDCALC::ScalarMapParameters& params)
{
  return _makePresentation<MEDPresentationScalarMap>(params);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makeContour(const MEDCALC::ContourParameters& params)
{
  return _makePresentation<MEDPresentationContour>(params);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makeVectorField(const MEDCALC::VectorFieldParameters& params)
{
  return _makePresentation<MEDPresentationVectorField>(params);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makeSlices(const MEDCALC::SlicesParameters& params)
{
  return _makePresentation<MEDPresentationSlices>(params);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makeDeflectionShape(const MEDCALC::DeflectionShapeParameters& params)
{
  return _makePresentation<MEDPresentationDeflectionShape>(params);
}

MEDPresentation::TypeID
MEDPresentationManager_i::makePointSprite(const MEDCALC::PointSpriteParameters& params)
{
  return _makePresentation<MEDPresentationPointSprite>(params);
}