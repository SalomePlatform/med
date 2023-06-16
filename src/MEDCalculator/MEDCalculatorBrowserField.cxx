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

#include "MEDCalculatorBrowserStep.hxx"
#include "MEDCalculatorBrowserField.hxx"

#include "MEDLoader.hxx"
#include "MEDCouplingFieldDouble.hxx"

#include <set>
#include <sstream>
#include <algorithm>
#include <functional>

using namespace MEDCoupling;

//  Default constructor
//  Set field name to nm and selection to flase
//  Init empty vector for all other parameters
MEDCalculatorBrowserField::MEDCalculatorBrowserField(const char* nm) : _name(nm),_selection(false)
{
}

//  Destructor
MEDCalculatorBrowserField::~MEDCalculatorBrowserField()
{
}

//  Constructor with two parameters :
//   - fname, the fileName
//   - fieldname, the fieldname
//  First set name to fieldname
//  Then, read the Med structur to fill time steps, components and meshes
MEDCalculatorBrowserField::MEDCalculatorBrowserField(const char *fname, const char *fieldName) : _name(fieldName), _file_name(fname), _selection(false)
{
  std::vector< std::string > meshNames=GetMeshNamesOnField(fname,fieldName);
  std::vector< std::pair< std::pair<int,int>, double > > dtits=GetAllFieldIterations(fname, fieldName);
  for(std::vector<std::pair< std::pair<int,int>, double > >::const_iterator iter=dtits.begin();iter!=dtits.end();iter++)
    {
      _steps.push_back(MEDCalculatorBrowserStep((*iter).first.first,(*iter).first.second,(*iter).second,meshNames[0]));
    }
  std::vector<TypeOfField> types=GetTypesOfField(fname,meshNames[0].c_str(),fieldName);
  if(types.empty())
    throw INTERP_KERNEL::Exception("MEDCalculatorBrowserField::MEDCalculatorBrowserField : the file is not loadable using MED File 3 API ! Probably presence of field on edges faces...");
  _type=types[0];//To improve
  MCAuto<MEDCouplingFieldDouble> tmpf;
  try
    {
      MCAuto<MEDCouplingField> tmpf2(ReadField(_type,fname,meshNames[0].c_str(),0,fieldName,dtits[0].first.first,dtits[0].first.second));
      tmpf=DynamicCast<MEDCouplingField,MEDCouplingFieldDouble>(tmpf2);
    }
  catch(INTERP_KERNEL::Exception& e)
    {
      if(_type==ON_CELLS)
        {
          MCAuto<MEDCouplingField> tmpf2(ReadField(_type,fname,meshNames[0].c_str(),-1,fieldName,dtits[0].first.first,dtits[0].first.second));
          tmpf=DynamicCast<MEDCouplingField,MEDCouplingFieldDouble>(tmpf2);
        }
      else
        throw e;
    }
  int NumberOfComponents=tmpf->getNumberOfComponents();
  for(int i=0;i<NumberOfComponents;i++)
    {
      std::string c=tmpf->getArray()->getInfoOnComponent(i);
      if(c=="")
        c="-noname-";
      _components.push_back(c);
    }
  _corresponding_meshes=GetMeshNamesOnField(fname,fieldName);
}

//  Equal to string operator,
//  Test if fieldname of this field is the same as nm
bool MEDCalculatorBrowserField::operator==(const std::string& nm)
{
  return nm==_name;
}

//  Equal to bool operator,
//  Test selection flag is set just as sel
bool MEDCalculatorBrowserField::operator==(bool sel)
{
  return _selection==sel;
}

//  str method
//  Construct a std::string to print Field, using std::cout for example
//  Put x or o for selected or not
//  Add Field fieldname
//  Add x/o components
//  Add MEDCalculatorBrowserStep::str() for each step
//  Return a std::string
std::string MEDCalculatorBrowserField::str()
{
  std::ostringstream res;
  _selection?res<<"x ":res<<"o ";
  res<<"Field "<<_name;

  res<<"\n\t\tComponents :"<<std::endl;
  for (unsigned int i = 0; i < _components.size(); i += 1)
    {
      res<<"\n\t\t\t"<<i<<" : "<<_components[i]<<""<<std::endl;
    }

  res<<"\n\t\tTimessteps :"<<std::endl;
  for (unsigned int i = 0; i < _steps.size(); i += 1)
    {
      res<<"\n\t\t\t"<<_steps[i].str();
    }

  return res.str();
}

//  Select a specific step according to its id
//  Use std::find to get the corresponding step
//  Also select all components if none is selected (avoid having time steps selected, so field selected, without components)
void MEDCalculatorBrowserField::selectStep(int ts)
{
  std::vector<MEDCalculatorBrowserStep>::iterator it = std::find(_steps.begin(),_steps.end(),ts);
  if(it != _steps.end())
    {
      it->select();
      _selection=true;
      std::vector<bool>::iterator itb = std::find(_selected_components.begin(),_selected_components.end(),true);
      if(itb == _selected_components.end())
        {
          for (unsigned int i = 0; i < _selected_components.size(); i += 1)
            {
            _selected_components[i] = true;
            }
        }
    }
}


//  Select all time steps
//  Also select all components if none is selected (avoid having time steps selected, so field selected, without components)
void MEDCalculatorBrowserField::selectAllSteps()
{
  for (std::vector<MEDCalculatorBrowserStep>::iterator it = _steps.begin(); it != _steps.end(); ++it)
    {
      it->select();
    }
  std::vector<bool>::iterator itb = std::find(_selected_components.begin(),_selected_components.end(),true);
  if(itb == _selected_components.end())
    {
      for (unsigned int i = 0; i < _selected_components.size(); i += 1)
        {
          _selected_components[i] = true;
        }
    }
  _selection=true;
}

//  Unselect a specific time step according to its id
//  Check if there is still time step selected :
//  - if yes, do nothing;
//  - if not, set selection flag to false
void MEDCalculatorBrowserField::unselectStep(int ts)
{
  std::vector<MEDCalculatorBrowserStep>::iterator it = std::find(_steps.begin(),_steps.end(),ts);
  if(it != _steps.end())
    {
      it->unselect();
    }
  it = std::find(_steps.begin(),_steps.end(),true);
  if(it == _steps.end())
    {
      _selection=false;
    }
}

//  Unselect all time steps
//  Set selection flag to false
void MEDCalculatorBrowserField::unselectAllSteps()
{
  for (std::vector<MEDCalculatorBrowserStep>::iterator it = _steps.begin(); it != _steps.end(); ++it)
    {
      it->unselect();
    }
  _selection=false;
}

//  Return if this field is selected or not, i.e. if some time steps are selected or not
bool MEDCalculatorBrowserField::isSelected()
{
  return _selection;
}

//  Return field name
const std::string& MEDCalculatorBrowserField::getName() const
{
  return _name;
}

//  Return steps vector
const std::vector<MEDCalculatorBrowserStep>& MEDCalculatorBrowserField::getSteps() const
{
  return _steps;
}

//  Return the time value corresponding to time step with id ts
const double& MEDCalculatorBrowserField::getTimeValue(int ts) const
{
  std::vector<MEDCalculatorBrowserStep>::const_iterator it = std::find(_steps.begin(),_steps.end(),ts);
  return it->getTimeValue();
}


//  Return the number of time steps
unsigned int MEDCalculatorBrowserField::getStepsSize() const
{
  return _steps.size();
}

//  Add a time step to steps vector with (id, time value)
void MEDCalculatorBrowserField::addStep(int ts, double tv)
{
  _steps.push_back(MEDCalculatorBrowserStep(ts,tv));
}

//  Select a specific component according to its id
void MEDCalculatorBrowserField::selectComponent(int id)
{
  _selected_components[id]=true;
}

//  Select all components
void MEDCalculatorBrowserField::selectAllComponents()
{
  for (unsigned int i = 0; i < _selected_components.size(); i += 1)
    {
      _selected_components[i] = true;
    }
}

//  Unselect a specific component according to its id
void MEDCalculatorBrowserField::unselectComponent(int id)
{
  _selected_components[id]=false;
}

// Unselect all components
void MEDCalculatorBrowserField::unselectAllComponents()
{
  for (unsigned int i = 0; i < _selected_components.size(); i += 1)
    {
      _selected_components[i] = false;
    }
}

//  Return a copy of the components vector
const std::vector<std::string>& MEDCalculatorBrowserField::getComponents() const
{
  return _components;
}

//  Return a copy of the selected components vector
const std::vector<bool>& MEDCalculatorBrowserField::getSelectedComponents() const
{
  return _selected_components;
}

//  Return the number of components
unsigned int MEDCalculatorBrowserField::getComponentsSize() const
{
  return _components.size();
}

//  Return a std::vector of std::string which contains all the meshes name used by all the time steps as supporting meshes
const std::vector<std::string>& MEDCalculatorBrowserField::getCorrespondingMeshesFromField() const
{
  return _corresponding_meshes;
}

//  Return a std::string which contains all the meshes name used by the time steps (id) as supporting meshes
const std::string& MEDCalculatorBrowserField::getCorrespondingMeshFromStep(int i) const
{
  return _steps[i].getCorrespondingMeshFromStep();
}

//  Change the name of a component, if input is empty, the component name is -noname- (for print)
void MEDCalculatorBrowserField::setComponentName(int i,const std::string& name)
{
  std::string name2(name);
  if(name2 == std::string(""))
    name2 = "-noname-";
  _components[i] = name2;
}

/*!
 * Contrary to isSelected see all time steps.
 */
bool MEDCalculatorBrowserField::isAnySelection() const
{
  return std::find(_steps.begin(),_steps.end(),true)!=_steps.end();
}

void MEDCalculatorBrowserField::setMeshName(const std::string& m)
{
  _corresponding_meshes.resize(1);
  _corresponding_meshes[0]=m;
}

/*!
 * This method returns a restriction of this containing only selected TimeSteps.
 * This methods throws an exception if multiple meshes exists in selected time steps.
 */
MEDCalculatorBrowserField MEDCalculatorBrowserField::getSelectedTimeSteps() const
{
  int nbOfTs=std::count(_steps.begin(),_steps.end(),true);
  std::vector<MEDCalculatorBrowserStep> ts(nbOfTs);
  std::vector<MEDCalculatorBrowserStep>::const_iterator it=_steps.begin();
  std::set<std::string> meshes;
  for(int i=0;i<nbOfTs;i++)
    {
      it=std::find(it,_steps.end(),true);
      ts[i]=*it;
      meshes.insert((*it).getCorrespondingMeshFromStep());
      if(meshes.size()>1)
        throw INTERP_KERNEL::Exception("Not managed fields with variable meshes during time !");
    }
  MEDCalculatorBrowserField ret(*this);
  ret.setSteps(ts);
  ret.setMeshName(*(meshes.begin()));
  return ret;
}

void MEDCalculatorBrowserField::setSteps(const std::vector<MEDCalculatorBrowserStep>& steps)
{
  _steps=steps;
}
