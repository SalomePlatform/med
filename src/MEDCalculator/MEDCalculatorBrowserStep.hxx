// Copyright (C) 2007-2023  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (CEA/DEN)

#ifndef __MEDCALCULATORBROWSERSTEP_HXX__
#define __MEDCALCULATORBROWSERSTEP_HXX__

#include "MedCalculatorDefines.hxx"

#include <string>

//  This class corresponds to a time step (dt,it) of a field
//  It contains the time step (dt) and the corresponding time value
//  It also contains methods for selecting or unselecting this time step and it hold the name of the supporting mesh.
namespace MEDCoupling
{
  class MEDCALCULATOR_EXPORT MEDCalculatorBrowserStep
  {
  public :
    MEDCalculatorBrowserStep(int ts=0, int order=0, double tv=0.0, std::string m="") : _time_step(ts), _order(order), _time_value(tv), _selection(false), _mesh(m) { }
    ~MEDCalculatorBrowserStep();
    bool operator==(int);//  Equal to operator, to use with std::find on vector<CalculatorBrowserStep> to find a specific time step
    bool operator==(int) const;//  Equal to operator, to use with std::find on vector< const CalculatorBrowserStep> to find a specific time step
    bool operator==(bool);//  Equal to operator, to use with std::find on std::vector<CalculatorBrowseStep> to find selected time step
    std::string str();//  Return a std::string corresponding to x/o (selected or not) timeStep ( timeValue )
    void select();//  Set selection flag to True (select this time step)
    void unselect();//  Set selection flag to False (unselect this time step)
    bool isSelected();//  Return selection (is this time step selected or not?)
    int getTimeStep() const;//  Return the time step id
    int getOrder() const { return _order; }
    const double& getTimeValue() const;//  Return the time Value
    const std::string& getCorrespondingMeshFromStep() const;//  Return the name of the supporting mesh
  private :
    int _time_step;//  time step id
    int _order;
    double _time_value;//  time value for this time step
    bool _selection;//  select flag
    std::string _mesh;//  name of supporting mesh
  };
}

#endif
