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
#include "MEDCalculatorBrowserLiteStruct.hxx"
#include "MEDCalculatorBrowserStep.hxx"

#include "MEDLoader.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingFieldDouble.hxx"

#include <sstream>
#include <deque>
#include <string>

using namespace MEDCoupling;

//  Default constructor
MEDCalculatorBrowserLiteStruct::MEDCalculatorBrowserLiteStruct() : _any_selection(false)
{
}

//  Destructor
MEDCalculatorBrowserLiteStruct::~MEDCalculatorBrowserLiteStruct()
{

}

//  Constructor with parameters :
//  - f, full file name with path
//  - n, file name (file.med)
//  Read the med file to get meshes and fields information
//  Fill meshes vector with meshes names
//  Fill fields vector creating using field constructor with MED and fieldname parameters
MEDCalculatorBrowserLiteStruct::MEDCalculatorBrowserLiteStruct(const char *f) : _file(f), _any_selection(false)
{
  computeBaseName();
  std::vector<std::string> meshNames=GetMeshNames(_file.c_str());
  for(std::vector<std::string>::const_iterator iter=meshNames.begin();iter!=meshNames.end();iter++)
    _meshes.push_back(MEDCalculatorBrowserMesh((*iter).c_str()));
  std::vector<std::string> fieldNames=GetAllFieldNames(_file.c_str());
  for(std::vector<std::string>::const_iterator iter=fieldNames.begin();iter!=fieldNames.end();iter++)
    _fields.push_back(MEDCalculatorBrowserField(_file.c_str(),(*iter).c_str()));
}

//  str method
//  Construct a std::string to print LiteStruct, using std::cout for example
//  Put x or o for selected or not
//  Add Field::str() for each field
//  Add Mesh::str() for each mesh
//  Return a std::string
std::string MEDCalculatorBrowserLiteStruct::str()
{
  std::ostringstream res;
  _any_selection?res<<"x ":res<<"o ";
  res<<"LiteStruct "<<_name;
  for (unsigned int i = 0; i < _meshes.size(); i += 1)
    {
      res<<"\n\t"<<_meshes[i].str();
    }
  for (unsigned int i = 0; i < _fields.size(); i += 1)
    {
      res<<"\n\t"<<_fields[i].str();
    }

  return res.str();
}

//  Select all fields and meshes
void MEDCalculatorBrowserLiteStruct::selectAll()
{
  selectAllMeshes();
  selectAllFields();
  _any_selection = true;
}

//  Select all meshes
void MEDCalculatorBrowserLiteStruct::selectAllMeshes()
{
  for (unsigned int i = 0; i < _meshes.size(); i += 1)
    _meshes[i].select();
  _any_selection = true;
}

//  Select all fields
void MEDCalculatorBrowserLiteStruct::selectAllFields()
{
  for (unsigned int i = 0; i < _fields.size(); i += 1)
    {
      _fields[i].selectAllSteps();
      _fields[i].selectAllComponents();
    }
  _any_selection = true;
}

//  Unselect all fields and meshes
void MEDCalculatorBrowserLiteStruct::unselectAll()
{
  unselectAllMeshes();
  unselectAllFields();
  _any_selection = false;
}

//  Unselect all meshes
void MEDCalculatorBrowserLiteStruct::unselectAllMeshes()
{
  for (unsigned int i = 0; i < _meshes.size(); i += 1)
    _meshes[i].unselect();
  _any_selection=isSelection();
}

//  Unselect all fields
void MEDCalculatorBrowserLiteStruct::unselectAllFields()
{
  for (unsigned int i = 0; i < _fields.size(); i += 1)
    {
      _fields[i].unselectAllSteps();
      _fields[i].unselectAllComponents();
    }
  _any_selection=isSelection();
}

//  Return if there is any selection in this file or not
bool MEDCalculatorBrowserLiteStruct::isSelection()
{
  for (unsigned int i = 0; i < _meshes.size(); i += 1)
    {
      if(_meshes[i].isSelected()) return true;
    }
  for (unsigned int i = 0; i < _fields.size(); i += 1)
    {
      if(_fields[i].isSelected()) return true;
    }
  return false;
}

//  Return the name of the file
const std::string& MEDCalculatorBrowserLiteStruct::getName() const
{
  return _name;
}

//  Return the full path name of the file
const std::string& MEDCalculatorBrowserLiteStruct::getFile() const
{
  return _file;
}

//  Return the number of meshes
unsigned int MEDCalculatorBrowserLiteStruct::getNumberOfMeshes()
{
  return _meshes.size();
}

//  Return the number of fields
unsigned int MEDCalculatorBrowserLiteStruct::getNumberOfFields()
{
  return _fields.size();
}

//  Return the mesh name for the mesh at id = i
const std::string& MEDCalculatorBrowserLiteStruct::getMeshName(int i) const
{
  return _meshes[i].getName();
}

//  Return the field name for the field at id = i
const std::string& MEDCalculatorBrowserLiteStruct::getFieldName(int i) const
{
  return _fields[i].getName();
}

//  Return a non-const reference on the field at id = i
MEDCalculatorBrowserField& MEDCalculatorBrowserLiteStruct::getField(int i)
{
  return _fields[i];
}

const MEDCalculatorBrowserField& MEDCalculatorBrowserLiteStruct::getField(int i) const
{
  return _fields[i];
}

//  Return a non-const reference on the field which name is equal to name
MEDCalculatorBrowserField& MEDCalculatorBrowserLiteStruct::getField(const std::string& name)
{
  return *std::find(_fields.begin(),_fields.end(),name);
}

//  Return a non-const reference on the mesh at id = i
MEDCalculatorBrowserMesh& MEDCalculatorBrowserLiteStruct::getMesh(int i)
{
  return _meshes[i];
}

//  Return a non-const reference on the mesh which name is equal to name
MEDCalculatorBrowserMesh& MEDCalculatorBrowserLiteStruct::getMesh(const std::string& name)
{ 
  return *std::find(_meshes.begin(),_meshes.end(),name);
}

//  Select a mesh according to its name
void MEDCalculatorBrowserLiteStruct::selectMesh(const std::string& name)
{
  std::vector<MEDCalculatorBrowserMesh>::iterator it = std::find(_meshes.begin(),_meshes.end(),name);
  if(it != _meshes.end())
    it->select();
  _any_selection = true;
}

//  Select a field according to its name
void MEDCalculatorBrowserLiteStruct::selectField(const std::string& name)
{
  std::vector<MEDCalculatorBrowserField>::iterator it = std::find(_fields.begin(),_fields.end(),name);
  if(it != _fields.end()){
    it->selectAllSteps();
    it->selectAllComponents();
  }
  _any_selection = true;
}

//  Unselect a specific mesh according to its name
//  Check if there is always selection
void MEDCalculatorBrowserLiteStruct::unselectMesh(const std::string& name)
{
  std::vector<MEDCalculatorBrowserMesh>::iterator it = std::find(_meshes.begin(),_meshes.end(),name);
  if(it != _meshes.end())
    it->unselect();
  _any_selection=isSelection();
}

//  Unselect a specific field according to its name
//  check if there is always selection
void MEDCalculatorBrowserLiteStruct::unselectField(const std::string& name)
{
  std::vector<MEDCalculatorBrowserField>::iterator it = std::find(_fields.begin(),_fields.end(),name);
  if(it != _fields.end())
    {
      it->unselectAllSteps();
      it->unselectAllComponents();
    }
  _any_selection=isSelection();
}

//  Return a list of meshes names supporting time steps of a field
std::vector<std::string> MEDCalculatorBrowserLiteStruct::getCorrespondingMeshesFromField(int fieldind)
{
  return _fields[fieldind].getCorrespondingMeshesFromField();
}

//  Return a list of meshes supporting all fields of this file
std::vector<std::string> MEDCalculatorBrowserLiteStruct::getCorrespondingMeshesFromLS()
{
  std::vector<std::string> res;
  for (unsigned int i = 0; i < _meshes.size(); i += 1)
    res.push_back(_meshes[i].getName());
  return res;
}

//  Equal to string operator, compare simplified name to input
bool MEDCalculatorBrowserLiteStruct::operator==(const std::string& nm)
{
  return _name==nm;
}

//  Set selection to true
void MEDCalculatorBrowserLiteStruct::setSelected(bool sel)
{
  _any_selection=sel;
}

void MEDCalculatorBrowserLiteStruct::computeBaseName()
{
  std::size_t p=_file.find_last_of("/");
  if(p!=std::string::npos)
    _name=_file.substr(p+1);
  else
    {
      _name=_file;
    }
}
