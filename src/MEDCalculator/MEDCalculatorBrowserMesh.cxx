// Copyright (C) 2007-2022  CEA/DEN, EDF R&D
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
#include "MEDCalculatorBrowserMesh.hxx"

#include <sstream>

using namespace MEDCoupling;

//  Default constructor
MEDCalculatorBrowserMesh::MEDCalculatorBrowserMesh() : _selection(false)
{

}

//  Destructor
MEDCalculatorBrowserMesh::~MEDCalculatorBrowserMesh()
{
}

MEDCalculatorBrowserMesh::MEDCalculatorBrowserMesh(const char *mname) : _name(mname), _selection(false)
{
}

//  str method
//  Construct a std::string to print mesh, using std::cout for example
//  Put x or o for selected or not
//  Add Mesh meshname
//  Return a std::string
std::string MEDCalculatorBrowserMesh::str()
{
  std::ostringstream res;
  _selection?res<<"x ":res<<"o ";
  res<<"Mesh "<<_name;
  return res.str();
}

//  Select this mesh setting selection flag to true
void MEDCalculatorBrowserMesh::select()
{
  _selection = true;
}

//  Unselect this mesh setting selection flag to false
void MEDCalculatorBrowserMesh::unselect()
{
  _selection = false;
}

//  Return if this mesh is selected or not
bool MEDCalculatorBrowserMesh::isSelected()
{
  return _selection;
}

//  Return the mesh name
const std::string& MEDCalculatorBrowserMesh::getName() const
{
  return _name;
}

//  Return if the mesh name is equal to input or not, useful for std::find for example
bool MEDCalculatorBrowserMesh::operator==(const std::string& nm)
{
  return _name==nm;
}
