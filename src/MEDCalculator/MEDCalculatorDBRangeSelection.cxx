// Copyright (C) 2007-2023  CEA, EDF
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

#include "MEDCalculatorDBRangeSelection.hxx"

#include <string>
#include <limits>
#include <sstream>

using namespace MEDCoupling;

const char MEDCalculatorDBRangeSelection::ALL_ELTS[]=":";

MEDCalculatorDBRangeSelection::MEDCalculatorDBRangeSelection(const char *v)
try
  {
    setValue(v);
  }
 catch(INTERP_KERNEL::Exception& e)
   {
     throw e;
   }

MEDCalculatorDBRangeSelection::MEDCalculatorDBRangeSelection(int v)
{
  setValue(v);
}

MEDCalculatorDBRangeSelection::MEDCalculatorDBRangeSelection()
{
  setValue(ALL_ELTS);
}

void MEDCalculatorDBRangeSelection::setPyStart(int val)
{
  _start=TraducePyVal(val);
}

void MEDCalculatorDBRangeSelection::setPyEnd(int val)
{
  _end=TraducePyVal(val);
}

std::vector<std::size_t> MEDCalculatorDBRangeSelection::getIds(int lgth) const
{
  if(_start>=lgth || _start<0)
    {
      std::ostringstream oss;
      oss << "RangeSelection::getIds : Specified range is outside possible value : " << lgth << " ! ";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  int trueEnd=_end;
  if(_end<0)
    trueEnd=lgth+_end;
  if(_end==std::numeric_limits<int>::max())
    trueEnd=lgth;
  if(trueEnd>lgth)
    throw INTERP_KERNEL::Exception("RangeSelection::getIds : end specficied is higher than length !");
  if(_start>trueEnd)
    throw INTERP_KERNEL::Exception("RangeSelection::getIds : begin of range after end !");
  std::vector<std::size_t> ret(trueEnd-_start);
  int j=0;
  for(int i=_start;i<trueEnd;i++,j++)
    ret[j]=i;
  return ret;
}

int MEDCalculatorDBRangeSelection::getSize(int lgth) const
{
  return getIds(lgth).size();
}

bool MEDCalculatorDBRangeSelection::isAll() const
{
  return _start==0 && _end==std::numeric_limits<int>::max();
}

void MEDCalculatorDBRangeSelection::setAll()
{
  _start=0;
  _end=std::numeric_limits<int>::max();
}

MEDCalculatorDBRangeSelection& MEDCalculatorDBRangeSelection::operator=(const char *v)
{
  setValue(v);
  return *this;
}

MEDCalculatorDBRangeSelection& MEDCalculatorDBRangeSelection::operator=(int v)
{
  setValue(v);
  return *this;
}

void MEDCalculatorDBRangeSelection::setValue(const char *v)
{
  try
  {
    std::string s(v);
    std::size_t pos=s.find_first_of(SEPARATOR);
    if(pos!=std::string::npos)
      {
        std::string s1=s.substr(0,pos);
        std::string s2=s.substr(pos+1);
        std::size_t pos2=s2.find_first_of(SEPARATOR);
        if(pos2!=std::string::npos)
          throw INTERP_KERNEL::Exception("RangeSelection constructor : Only one ':' supported !");
        if(s1.empty())
          _start=0;
        else
          {
            std::istringstream iss(s1);
            iss.exceptions(std::istream::failbit | std::istream::badbit);
            iss >> _start;
            if(!iss.eof())
              throw INTERP_KERNEL::Exception("Invalid 1st part of ':' !");
          }
        //
        if(s2.empty())
          _end=std::numeric_limits<int>::max();
        else
          {
            std::istringstream iss(s2);
            iss.exceptions(std::istream::failbit | std::istream::badbit);
            iss >> _end;
            if(!iss.eof())
              throw INTERP_KERNEL::Exception("Invalid 2nd part of ':' !");
          }
        if(_end>0)
          if(_start>_end)
            throw INTERP_KERNEL::Exception("RangeSelection constructor : begin of range after end !");
      }
  }
 catch(INTERP_KERNEL::Exception& e)
   {
     throw e;
   }
 catch(std::istream::failure& e)
   {
     throw INTERP_KERNEL::Exception("RangeSelection constructor : impossible to analyze one side of expr ':' !");
   }
}

void MEDCalculatorDBRangeSelection::setValue(int v)
{
  _start=v;
  _end=v+1;
}

int MEDCalculatorDBRangeSelection::TraducePyVal(int val)
{
  return val;
}
