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

#ifndef __MEDCALCULATORBROWSERLITESTRUCT_HXX__
#define __MEDCALCULATORBROWSERLITESTRUCT_HXX__

#include "MedCalculatorDefines.hxx"
#include "MEDCalculatorBrowserMesh.hxx"
#include "MEDCalculatorBrowserField.hxx"

#include <string>
#include <vector>

//  This class correspond to a MED File
//  It contains fields and meshes from a file
//  It also has methods for selecting/unselecting meshes, field and components from fields
namespace MEDCoupling
{
  class MEDCALCULATOR_EXPORT MEDCalculatorBrowserLiteStruct
  {
  public :
    MEDCalculatorBrowserLiteStruct();
    ~MEDCalculatorBrowserLiteStruct();
    MEDCalculatorBrowserLiteStruct(const char *);//  Constructor with a complete file name (with path) and simplified file name as parameters
    bool operator==(const std::string&);//  Equal to string operator, compare simplified name to input
    std::string str();//  Return a std::string corresponding to x/o (selected or not) File filename \n meshes \n fields
    void setSelected(bool);//  Set selection to input bool
    void selectAll();//  Select all meshes and fields
    void selectAllMeshes();//  Select all meshes
    void selectAllFields();//  Select all fields
    void unselectAll();//  Unselect all meshes and fields
    void unselectAllMeshes();//  Unselect all meshes
    void unselectAllFields();//  Unselect all fields
    bool isSelection();//  Return if there is selection or not (selection flag)
    const std::string& getName() const;//  Return file simplified name
    const std::string& getFile() const;//  Return full file name
    unsigned int getNumberOfMeshes();//  Return number of meshes
    unsigned int getNumberOfFields();//  Return number of fields
    const std::string& getMeshName(int) const;//  Return a mesh name according to its id
    const std::string& getFieldName(int) const;//  Return a field name according to its id
    MEDCalculatorBrowserField& getField(int);//  Return a reference on a field according to its id
    const MEDCalculatorBrowserField& getField(int) const;
    MEDCalculatorBrowserField& getField(const std::string&);//  Return a reference on a field according to its name
    MEDCalculatorBrowserMesh& getMesh(int);//  Return a reference on a mesh according to its id
    MEDCalculatorBrowserMesh& getMesh(const std::string&);//  Return a reference on a mesh according to its name
    void selectMesh(const std::string&);//  Select a specific mesh according to its name
    void selectField(const std::string&);//  Select a specific field according to its name
    void unselectMesh(const std::string&);//  Unselect a specific mesh according to its name
    void unselectField(const std::string&);//  Unselect a specific field according to its name
    std::vector<std::string> getCorrespondingMeshesFromField(int);//  Return a list of meshes names supporting time steps of a field
    std::vector<std::string> getCorrespondingMeshesFromLS();//  Return a list of meshes supporting all fields of this file
  private:
    void computeBaseName();
  private :
    std::string _name;//  simplified file name
    std::string _file;//  full path file name
    std::vector<MEDCalculatorBrowserMesh> _meshes;//  list of all meshes from this file
    std::vector<MEDCalculatorBrowserField> _fields;//  list of all fields from this file
    bool _any_selection;//  selection flag
  };
}

#endif
