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

#ifndef __MEDCALCULATORBROWSERFIELD_HXX__
#define __MEDCALCULATORBROWSERFIELD_HXX__

#include "MedCalculatorDefines.hxx"
#include "MEDCalculatorBrowserStep.hxx"

#include "InterpKernelException.hxx"
#include "MEDCouplingRefCountObject.hxx"

#include <string>
#include <vector>

//  This class correspond to a MED field of a file
//  It contains the field name, the components, the time steps and selection methods
//  It's used to select a field, it's components and time steps
namespace MEDCoupling
{
  class MEDCalculatorBrowserStep;//  top prevent cross include
  class MEDCALCULATOR_EXPORT MEDCalculatorBrowserField
  {
  public :
    MEDCalculatorBrowserField(const char* nm);
    ~MEDCalculatorBrowserField();
    MEDCalculatorBrowserField(const char *fname, const char *fieldName);
    bool operator==(const std::string&);//  Equal to string operator, to test if fieldname of this field is the same as input argument
    bool operator==(bool);//  Equal to bool operator, to use with std::find on std::vector<CalculatorBrowserField> to find selected fields
    std::string str();//  Return a std::string corresponding to x/o (selected or not) Field fieldname \n steps
    void selectStep(int);//  Select a specific time step according to its id
    void selectAllSteps();//  Select all time steps
    void unselectStep(int);//  Unselect a specific time step according to its id
    void unselectAllSteps();// Unselect all time steps
    void selectComponent(int);//  Select a specific component according to its id
    void selectAllComponents();//  Select all components
    void unselectComponent(int);//  Unselect a specific component according to its id
    void unselectAllComponents();// Unselect all components
    bool isSelected();//  Return if this field is selected or not, i.e. selection flag to true or false
    const std::string& getFileName() const { return _file_name; }
    TypeOfField getType() const { return _type; }
    const std::string& getName() const;//  Return fieldname
    const std::vector<MEDCalculatorBrowserStep>& getSteps() const;//  Return a copy of the steps vector
    void setSteps(const std::vector<MEDCalculatorBrowserStep>& steps);
    const std::vector<std::string>& getComponents() const;//  Return the components vector
    const std::vector<bool>& getSelectedComponents() const;//  Return the selected components vector
    const double& getTimeValue(int) const;//  Return the time value of the time step with id equal to input
    unsigned int getStepsSize() const;//  Return the number of time steps
    unsigned int getComponentsSize() const;//  Return the number of components
    void addStep(int,double);//  Add a time step to steps vector with (id, time value)
    const std::vector<std::string>& getCorrespondingMeshesFromField() const;//  Return a std::vector of std::string which contains all the meshes name used by all the time steps as supporting meshes
    const std::string& getCorrespondingMeshFromStep(int) const;//  Return a std::string which contains the mesh name used by the time steps (id) as supporting meshes
    void setComponentName(int,const std::string&);//  Change the name of a component
    bool isAnySelection() const;
    void setMeshName(const std::string& m);
    MEDCalculatorBrowserField getSelectedTimeSteps() const;
  private:
    std::string _name;// field name
    std::string _file_name;// file name
    TypeOfField _type;
    std::vector<MEDCalculatorBrowserStep> _steps;// liste of time steps
    std::vector<std::string> _components;//  list of components
    std::vector<bool> _selected_components;//  list of selected components, by default, all
    bool _selection;//  select flag
    std::vector<std::string> _corresponding_meshes;//  vector containing all the meshes name used by time steps as support
  };
}

#endif
