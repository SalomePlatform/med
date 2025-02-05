// Copyright (C) 2007-2025  CEA, EDF
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
#ifndef __MEDCALCULATORBROWSERMESH_HXX__
#define __MEDCALCULATORBROWSERMESH_HXX__

#include "MedCalculatorDefines.hxx"

#include <string>

//  This class corresponds to a MED mesh of a file
//  It contains the mesh name and a flag to know if it's selected or not
//  It's used only for selecting mesh, out of all fields.
namespace MEDCoupling
{
  class MEDCALCULATOR_EXPORT MEDCalculatorBrowserMesh
  {
  public :
    MEDCalculatorBrowserMesh();
    ~MEDCalculatorBrowserMesh();
    MEDCalculatorBrowserMesh(const char *mname);
    bool operator==(const std::string&);// Equal to string operator, to test if meshname of this mesh is the same as input argument
    std::string str();//  Return a std::string corresponding to x/o (selected or not) Mesh meshname
    void select();//  Set selection flag to True (select this mesh)
    void unselect();//  Set selection flag to False (unselect this mesh)
    bool isSelected();//  Return selection (is this mesh selected or not?)
    const std::string& getName() const;//  Return the name of this mesh
  private :
    std::string _name;//  name of mesh
    bool _selection;//  select flag
  };
}

#endif
