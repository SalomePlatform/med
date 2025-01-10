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

#include "SPythonInterpreter.hxx"
#include "SPythonParser.hxx"

#include <vector>
#include <sstream>
#include <algorithm>

using namespace MEDCoupling;

const char *SPythonInterpreter::INDENT_TOKEN[]={"def","class","for","if","while","try","except"};

const char SPythonInterpreter::NUMBERS[]="0123456789";

SPythonInterpreter::SPythonInterpreter(PyObject *globals, PyObject *locals):_indent_must_change(false),_glob(globals),_loc(locals)
{
  _indent_pos.insert(0);
}

void SPythonInterpreter::initialize()
{
  _indent_pos.clear();
  _indent_pos.insert(0);
  _indent_must_change=false;
  _cmd.clear();
}

bool SPythonInterpreter::run(const char *str, bool& isSPython)
{
  isSPython=false;
  std::string s(str);
  if(s.empty())
    {
      finishSession();
      _indent_must_change=false;
      return true;
    }
  std::size_t pos=s.find_first_not_of(' ');
  if(pos==std::string::npos)
    return false;
  if(s[pos]=='#')
    return false;
  if(!checkIndentCoherency(s,pos))
    return true;
  if(!isIndenter(s,pos))
    {
      _indent_must_change=false;
      if(pos==0)
        {
          if(isSPythonExpression(s))
            {
              isSPython=true;
              return true;
            }
          else
            {
              _cmd+=s;
              _cmd+="\n";
              return finishSession();
            }
        }
      _cmd+=s;
      _cmd+="\n";
      return false;
    }
  else
    {
      _indent_must_change=true;
      _cmd+=s;
      _cmd+="\n";
      return false;
    }
}

bool SPythonInterpreter::finishSession()
{
  PyObject *res=0;
  if(_cmd.empty())
    return false;
  res=PyRun_String(_cmd.c_str(),Py_file_input,_glob,_loc);
  _cmd.clear();
  checkPythonInterp(res);
  //_indent_pos.clear();
  //_indent_pos.insert(0);
  return true;
}

void SPythonInterpreter::checkPythonInterp(PyObject *r)
{
  if(!r)
    PyErr_Print();
}

bool SPythonInterpreter::checkIndentCoherency(const std::string& s, std::size_t p)
{
  if(!_indent_must_change)
    {
      if(_indent_pos.find(p)!=_indent_pos.end())
        {
          std::set<int>::iterator it=_indent_pos.begin();
          bool found=false;
          for(;it!=_indent_pos.end() && !found;it++)
            if(*it==(int)p)
              found=true;
          if(found)
            _indent_pos.erase(it,_indent_pos.end());
          return true;
        }
      else
        {//let python doing the job of error msg !
          _cmd+=s;
          finishSession();
          _indent_pos.clear();
          return true;
        }
    }
  else
    {
      if((int)p>*_indent_pos.rbegin())
        {
          _indent_pos.insert(p);
          return true;
        }
      else
        {//let python doing the job of error msg !
          _cmd+=s;
          finishSession();
          _indent_pos.clear();
          return true;
        }
    }
}

/*!
 * looks that s contains at the begin of non empty char a python keyword that forces indentation of next line.
 */
bool SPythonInterpreter::isIndenter(const std::string& s, std::size_t p)
{
  std::string s1=s.substr(p);
  for(int i=0;i<NB_OF_INDENT;i++)
    {
      std::string elt(INDENT_TOKEN[i]);
      std::size_t sz=elt.length();
      if(s1.length()>=sz)
        if(s1.substr(0,sz)==elt)
          return true;
    }
  return false;
}

std::string SPythonInterpreter::strip(const std::string& s)
{
  std::size_t sz=s.length();
  std::size_t n1=std::count(s.c_str(),s.c_str()+sz,' ');
  std::size_t n2=std::count(s.c_str(),s.c_str()+sz,'\n');
  std::size_t n3=std::count(s.c_str(),s.c_str()+sz,'\t');
  std::string ret(sz-n1-n2-n3,'$');
  std::size_t i=0,j=0;
  for(;i!=std::string::npos;)
    {
      i=s.find_first_not_of(" \n\t",i);
      if(i!=std::string::npos)
        ret[j++]=s[i++];
    }
  return ret;
}

bool SPythonInterpreter::isSPythonExpression(const std::string& s)
{
  std::string w(s);
  if(w.find("import ")!=std::string::npos)
    return false;
  if(w.find_first_of('@')!=std::string::npos)
    return false;
  if(w.find("del ")!=std::string::npos)
    return false;
  const char PRINT[]="print(";
  const char ENDPRINT[]=")";
  bool isPrint=w.find(PRINT)!=std::string::npos;
  isPrint &= w.find(ENDPRINT)!=std::string::npos;
  if(isPrint)
    {
      std::size_t p=w.find(PRINT);
      w=w.substr(p+sizeof(PRINT)-sizeof(ENDPRINT)-1);
    }
  std::string result;
  if(!isSPythonExpressionLev1(w,result))
    return false;
  if(isPrint)
    result=std::string(PRINT)+result;
  _cmd+=result+"\n";
  finishSession();
  return true;
}

bool SPythonInterpreter::isSPythonExpressionLev1(const std::string& s, std::string& result)
{
  std::string sst=strip(s);
  SPythonParserHL p(_glob,_loc);
  if(!p.parse(sst,result))
    return false;
  return true;
}

bool SPythonInterpreter::isCandidateParenthesis(const std::string& s, std::size_t p1, std::size_t& n)
{
  std::size_t p2=s.find_first_of(')',p1);
  std::size_t p3=s.find_first_of('(',p1+1);
  if(p2!=std::string::npos && (p3==std::string::npos || (p3!=std::string::npos && p2<p3) ))
    {
      n=p2-p1;
      return true;
    }
  return false;
}
