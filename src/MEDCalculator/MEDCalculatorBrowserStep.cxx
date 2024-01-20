// Copyright (C) 2007-2024  CEA, EDF
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

#include "MEDCalculatorBrowserStep.hxx"

#include <sstream>

using namespace MEDCoupling;

MEDCalculatorBrowserStep::~MEDCalculatorBrowserStep()
{
}

//  Equal to int operator
//  Used to compare the time step id with input argument.
//  Return if equal or not.
bool MEDCalculatorBrowserStep::operator==(int ts)
{
  return _time_step==ts;
}

//  Equal to int operator
//  Used to compare the time step id with input argument.
//  Return if equal or not.
bool MEDCalculatorBrowserStep::operator==(int ts) const
{
  return _time_step==ts;
}

//  Equal to bool operator
//  To test if this time step is selected or not comparing to a boolean
//  Return if it has the same statement or not.
bool MEDCalculatorBrowserStep::operator==(bool sel)
{
  return sel==_selection;
}

//  str method
//  Construct a std::string to print this time step, using std::cout for example
//  Put x or o for selected or not
//  Add time step id value ( time value )
//  Return a std::string
std::string MEDCalculatorBrowserStep::str()
{
  std::ostringstream res;
  _selection?res<<"x ":res<<"o ";
  res << _time_step << "," << _order;
  res << " (";
  res << _time_value;
  res << ")";
  return res.str();
}

//  Select this time step setting selection flag to true
void MEDCalculatorBrowserStep::select()
{
  _selection = true;
}

//  Unselect this time step setting selection flag to false
void MEDCalculatorBrowserStep::unselect()
{
  _selection = false;
}

//  Return if this time step is selected or not
bool MEDCalculatorBrowserStep::isSelected()
{
  return _selection;
}

//  Return the timestep id
int MEDCalculatorBrowserStep::getTimeStep() const
{
  return _time_step;
}

//  Return the time value
const double& MEDCalculatorBrowserStep::getTimeValue() const
{
  return _time_value;
}

//  Return the name of the support meshing
const std::string& MEDCalculatorBrowserStep::getCorrespondingMeshFromStep() const
{
  return _mesh;
}
