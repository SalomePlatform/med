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

#include "SPythonParser.hxx"

#include "InterpKernelException.hxx"

#include <algorithm>
#include <sstream>

using namespace MEDCoupling;

const char SPythonPredParser::FIELD_TYPE_STR[]="MEDCalculatorDBField";

const char SPythonParser::NUMBERS[]="0123456789";

#if PY_VERSION_HEX < 0x03050000
static char*
Py_EncodeLocale(const wchar_t *text, size_t *error_pos)
{
	return _Py_wchar2char(text, error_pos);
}
#endif

SPythonParser::SPythonParser():_type(EMPTY_TYPE)
{
}

/*!
 * This method checks that the input 's' follows the following pattern :
 * "{number || var[.attribute]?{@number@}? || @number@ }*{+ - * / }*"
 * \b All of {var} should be of type int,float funct or MEDCalculatorDBField.
 */
bool SPythonParser::parseWithoutEqual(const std::string& s, int parLev, PyObject *glob, PyObject *loc, std::vector<SPythonParser>& alreadyParsed)
{
  keepSelectedLevOfPar(s,parLev,glob,loc);
  TypeOfEntity ty=_pred.getType();
  switch(ty)
    {
    case FLOAT_TYPE:
    case INT_TYPE:
    case UNKNOWN_TYPE:
      {
        _type=ty;
        return false;
      }
    case FIELDDB_TYPE:
      {
        std::string tmp;
        if(isParenthesisMatching(_content,tmp))
          {
            _type=FIELDDB_TYPE;
            _content_py=tmp;
            return true;
          }
        if(isAlreadyControledParenthesis(_content))
          {
            _type=FIELDDB_TYPE;
            return true;
          }
        return false;
      }
    case IDENTITY_TYPE:
      {
        _type=analyzeType(glob,loc,alreadyParsed);
        break;
      }
    case FUNC_TYPE:
    case EMPTY_TYPE:
      return false;
    }
  return _type==FIELDDB_TYPE || _type==INT_TYPE || _type==FLOAT_TYPE;
}

void SPythonParser::keepSelectedLevOfPar(const std::string& s, int parLev, PyObject *glob, PyObject *loc)
{
  int curLev=0;
  std::size_t i=0;
  std::size_t len=s.length();
  //bool found=false; // todo: unused
  if(parLev==0)
    {
      _content=s;
      _content_py=s;
      std::string pred;
      _pred.assign(pred,glob,loc);
      return ;
    }
  for(i=0;i<len;i++)
    {
      switch(s[i])
        {
        case '(':
          ++curLev;
          break;
        case ')':
          curLev--;
          break;
        }
      if(curLev==parLev)
        {
          std::size_t end=s.find_first_of(')',i);
          end=end!=std::string::npos?end-i:std::string::npos;
          _content=s.substr(i,end);
          //found=true; // todo: unused
          break;
        }
    }
  _content=_content.substr(1,_content.length()-1);
  _content_py=_content;
  std::string pred;
  if(i==0)
    {
      _pred.assign(pred,glob,loc);
      return ;
    }
  std::size_t begin=s.find_last_of("(+-*/^",i-1,6);
  begin=begin!=std::string::npos?begin+1:0;
  pred=s.substr(begin,i-begin);
  _pred.assign(pred,glob,loc);
}


/*!
 * Size of returned vector is depth of parenthesis.
 * Foreach level number of parenthesis groups.
 */
std::vector<int> SPythonParser::levOfParenthesis(const std::string& s)
{
  std::vector<int> ret(1);
  ret[0]=1;
  std::size_t curLev=0;
  std::size_t len=s.length();
  for(std::size_t i=0;i<len;i++)
    {
      switch(s[i])
        {
        case '(':
          if(ret.size()<=++curLev)
            ret.push_back(1);
          else
            ret[curLev]++;
          break;
        case ')':
          curLev--;
          break;
        }
    }
  return ret;
}

/*!
 * Analyzes regarding _content attribute the type of value.
 */
TypeOfEntity SPythonParser::analyzeType(PyObject *glob, PyObject *loc, const std::vector<SPythonParser>& alreadyParsed)
{
  static const int OPS_SEP_LGTH=5;
  static const char OPS_SEP[OPS_SEP_LGTH+1]="+-*/^";
  _type=EMPTY_TYPE;
  TypeOfEntity type;
  std::string content=_content;//.substr(1,_content.length()-2);
  std::size_t p=0;
  int parId=0;
  while(p!=std::string::npos)
    {
      std::size_t p2=content.find_first_of(OPS_SEP,p,OPS_SEP_LGTH);
      std::size_t p3=p2!=std::string::npos?p2-p:p2;
      std::string elt=content.substr(p,p3);
      type=getTypeOfElt(elt,glob,loc,alreadyParsed);
      _type=combineType(_type,type);
      p=p2!=std::string::npos?p2+1:p2;
      parId++;
    }
  return _type;
}

TypeOfEntity SPythonParser::getTypeOfElt(const std::string& elt, PyObject *glob, PyObject *loc, const std::vector<SPythonParser>& alreadyParsed) const
{
  std::size_t sz=elt.length();
  if(sz>=3)
    {
      if(elt[0]=='@' && elt[sz-1]=='@')
        {
          std::string tmp=elt.substr(1,sz-2);
          int id;
          std::istringstream iss(tmp); iss >> id;
          return alreadyParsed[id].getType();
        }
    }
  return SPythonPredParser::getTypeOfVar(elt,glob,loc);
}

TypeOfEntity SPythonParser::combineType(TypeOfEntity t1, TypeOfEntity t2) const
{
  if(t1==EMPTY_TYPE)
    return t2;
  if(t1==t2)
    return t1;
  if(t1==UNKNOWN_TYPE || t2==UNKNOWN_TYPE)
    return UNKNOWN_TYPE;
  if((t1==INT_TYPE && t2==FLOAT_TYPE) || (t2==INT_TYPE && t1==FLOAT_TYPE))
    return FLOAT_TYPE;
  if((t1==INT_TYPE && t2==FIELDDB_TYPE) || (t2==INT_TYPE && t1==FIELDDB_TYPE))
    return FIELDDB_TYPE;
  if((t1==FLOAT_TYPE && t2==FIELDDB_TYPE) || (t2==FLOAT_TYPE && t1==FIELDDB_TYPE))
    return FIELDDB_TYPE;
  return UNKNOWN_TYPE;
}

/*!
 * This method makes the assumption that s as a minimum length of size 2 with first char == '(' and the last ')'.
 */
bool SPythonParser::isParenthesisMatching(const std::string& w, std::string& res)
{
  std::ostringstream result;
  //result << '(';
  std::size_t sLgth=w.length();
  //std::string w(s,1,sLgth-2);
  int nbOfParams=std::count(w.c_str(),w.c_str()+sLgth,',')+1;
  std::size_t pos=0;
  for(int i=0;i<nbOfParams;i++)
    {
      std::size_t pos2=w.find(',',pos);
      std::size_t pos3=pos2!=std::string::npos?pos2-pos:std::string::npos;
      std::string w1=w.substr(pos,pos3);
      std::string w1out;
      bool isNum;
      if(!isElementInParenthesisMatching(w1,w1out,isNum))
        return false;
      if(!isNum)
        result << '\"';
      result << w1out;
      if(!isNum)
        result << '\"';
      if(i!=nbOfParams-1)
        result << ',';
      pos=pos2!=std::string::npos?pos2+1:std::string::npos;
    }
  //result << ')';
  res=result.str();
  return true;
}

/*!
 * This method checks that s match the following regexp : "@[0123456789]?@"
 */
bool SPythonParser::isAlreadyControledParenthesis(const std::string& s)
{
  std::size_t len=s.length();
  if(len<3)
    return false;
  if(s[0]!='@' || s[len-1]!='@')
    return false;
  std::string tmp=s.substr(1,len-2);
  return tmp.find_first_not_of(NUMBERS,0,10)==std::string::npos;
}

/*!
 * No assumption here check that the following regexp is followed [' ']*[0:9]*[' ']*:?[' ']*[0:9]*[' ']*
 */
bool SPythonParser::isElementInParenthesisMatching(const std::string& s, std::string& result, bool& isNumber)
{
  isNumber=false;
  std::ostringstream ret;
  std::size_t pos1=s.find_first_not_of(' ');
  if(pos1==std::string::npos)
    return false;
  std::size_t pos2=s.find_first_not_of(NUMBERS,pos1,10);
  std::string elt1=s.substr(pos1,pos2-pos1);
  ret << elt1;
  std::size_t pos3=s.find_first_not_of(' ',pos2);
  if(pos3==std::string::npos)
    {//'56'
      isNumber=true;
      result=ret.str();
      return true;
    }
  if(s[pos3]!=':')
    return false;
  ret << ':';
  pos3++;
  std::size_t pos4=s.find_first_not_of(' ',pos3);
  if(pos4==std::string::npos)
    {// '56:'
      result=ret.str();
      return true;
    }
  std::size_t pos5=s.find_first_not_of(NUMBERS,pos4,10);
  if(pos5==pos4)//an another character found after : !
    return false;
  std::string elt2;
  if(pos5==std::string::npos)
    {
      elt2=s.substr(pos4,std::string::npos);
      ret << elt2;
      result=ret.str();
      return true;
    }
  elt2=s.substr(pos4,pos5-pos4);
  ret << elt2;
  result=ret.str();
  std::size_t pos6=s.find_first_not_of(' ',pos5);
  if(pos6==pos5)//an another character found after 2nd elt !
    return false;
  return pos6==std::string::npos;
}

std::string SPythonParser::substitute(const std::vector<SPythonParser>& v) const
{
  std::string ret=_content_py;
  replaceFromCompacted(ret,v);
  return ret;
}

std::string SPythonParser::replaceByCompacted(const std::string& s, int parLev, int id) const
{
  std::string scpy(s);
  int curLev=0;
  std::size_t i=0;
  std::size_t len=s.length();
  std::size_t begin=0,end=0;
  for(i=0;i<len;i++)
    {
      switch(s[i])
        {
        case '(':
          ++curLev;
          break;
        case ')':
          curLev--;
          break;
        }
      if(curLev==parLev)
        {
          if(i!=0)
            {
              begin=s.find_last_of("(+-*/^",i-1,6);
              begin=begin!=std::string::npos?begin+1:0;
            }
          else
            begin=0;
          end=s.find_first_of(')',i+1);
          end=end!=std::string::npos?end-begin+1:std::string::npos;
          break;
        }
    }
  std::ostringstream oss,oss1;
  oss << '@' << id << '@';
  return scpy.replace(begin,end,oss.str());
}

std::string SPythonParser::getRepr(const std::vector<SPythonParser>& v) const
{
  std::string ret(_pred.getRepr());
  ret+='(';
  ret+=_content_py;
  ret+=')';
  replaceFromCompacted(ret,v);
  return ret;
}

void SPythonParser::replaceFromCompacted(std::string& ret,const std::vector<SPythonParser>& v)
{
  std::size_t pos=ret.find_first_of('@',0);
  std::size_t pos2;
  while(pos!=std::string::npos)
    {
      pos2=ret.find_first_of('@',pos+1);
      if(pos2==std::string::npos)
        throw INTERP_KERNEL::Exception("Internal Error occurs !");
      std::string tmp=ret.substr(pos+1,pos2-pos-1);
      std::istringstream iss(tmp);
      int id;
      iss >> id;
      std::string tmp2=v[id].getRepr(v);
      ret.replace(pos,pos2-pos+1,tmp2);
      pos=ret.find_first_of('@',pos2+1+tmp2.size()-tmp.size()-2);
    }
}


SPythonPredParser::SPythonPredParser():_type(EMPTY_TYPE)
{
}

/*!
 * This method analyses _pred attribute to deduce type of returned param.
 */
void SPythonPredParser::assign(const std::string& s, PyObject *glob, PyObject *loc)
{
  _var.clear();
  _method.clear();
  if(s.empty()) { _type=IDENTITY_TYPE; return ; }
  std::size_t p=s.find_last_of('.');
  if(p==std::string::npos)
    _var=s;
  else
    {
      _var=s.substr(0,p);
      _method=s.substr(p+1);
    }
  if(_method.empty())
    {
      int type=getTypeOfVar(_var,glob,loc);
      if(type!=FUNC_TYPE)
        {
          if(type==FIELDDB_TYPE)
            _type=FIELDDB_TYPE;
          else
            _type=UNKNOWN_TYPE;
        }
      else
        _type=IDENTITY_TYPE;
    }
  else
    {
      int type=getTypeOfVar(_var,glob,loc);
      if(type==FIELDDB_TYPE)
        {//To improve in case that some FieldDB swigged method return a different type than FieldDB
          _type=FIELDDB_TYPE;
        }
      else
        _type=UNKNOWN_TYPE;
    }
}

bool SPythonPredParser::empty() const
{
  return _var.empty() && _method.empty();
}

TypeOfEntity SPythonPredParser::getTypeOfVar(const std::string& var, PyObject *glob, PyObject *loc)
{
  static const char TMPVAR[]="tmpvvr37911022";
  std::ostringstream oss; oss << TMPVAR << "=isinstance(" << var << "," << FIELD_TYPE_STR << ")";
  PyObject *res=PyRun_String(oss.str().c_str(),Py_single_input,glob,loc);
  if(res==0)
    return UNKNOWN_TYPE;
  if(PyDict_GetItemString(glob,TMPVAR)==Py_True)
    return FIELDDB_TYPE;
  oss.str(std::string(TMPVAR));
  oss << TMPVAR << "=type(" << var << ").__name__";
  PyRun_String(oss.str().c_str(),Py_single_input,glob,loc);
  PyObject *p=PyDict_GetItemString(glob,TMPVAR);
#if PY_VERSION_HEX < 0x030c0000 // See PEP-623
  const char *type=Py_EncodeLocale(PyUnicode_AS_UNICODE(p), NULL);
#else
  const char *type=Py_EncodeLocale(PyUnicode_AsWideCharString(p,NULL), NULL);
#endif
  std::string typecpp=std::string(type);
  if(typecpp=="function")
    return FUNC_TYPE;
  if(typecpp=="int")
    return INT_TYPE;
  if(typecpp=="float")
    return FLOAT_TYPE;
  return UNKNOWN_TYPE;
}

std::string SPythonPredParser::getRepr() const
{
  if(_method.empty())
      return _var;
  else
    {
      std::string tmp(_var);
      tmp+='.';
      return tmp+_method;
    }
}

SPythonParserHL::SPythonParserHL(PyObject *glob, PyObject *loc):_glob(glob),_loc(loc)
{
}

bool SPythonParserHL::parse(const std::string& s, std::string& result)
{
  std::vector<std::string> ps=splitBetweenEqualChar(s);
  TypeOfEntity type;
  if(ps.empty())
    return false;
  if(ps.size()==1)
    return parseWithoutEqual(ps[0],type,result);
  result.clear();
  if(!parseWithoutEqual(ps.back(),type,result))
    return false;
  for(int n=ps.size()-1;n!=0;n--)
    {
      std::string leftRes;
      TypeOfEntity typeLeft;
      if(parseWithoutEqual(ps[n-1],typeLeft,leftRes))
          {
            if(typeLeft==FIELDDB_TYPE)
              result=leftRes+".assign("+result+")";
            else
              result=leftRes+"="+result;
          }
      else
        result=ps[n-1]+"="+result;
    }
  return true;
}

bool SPythonParserHL::parseWithoutEqual(const std::string& s, TypeOfEntity& type, std::string& result)
{
  if(s.empty())
    return false;
  std::string sst(s);
  std::vector<int> v=SPythonParser::levOfParenthesis(sst);
  std::size_t levOfPar=v.size();
  std::vector<SPythonParser> allSubs;
  int k=0;
  for(std::size_t i=levOfPar;i!=0;i--)
    for(int j=0;j<v[i-1];j++,k++)
      {
        SPythonParser subs;
        if(!subs.parseWithoutEqual(sst,i-1,_glob,_loc,allSubs))
          return false;
        if(i!=1)
          sst=subs.replaceByCompacted(sst,i-1,k);
        allSubs.push_back(subs);
      }
  result=allSubs.back().substitute(allSubs);
  type=allSubs.back().getType();
  return true;
}

std::vector<std::string> SPythonParserHL::splitBetweenEqualChar(const std::string &s)
{
  std::size_t p=0,p2,p3;
  std::vector<std::string> ret;
  while(p!=std::string::npos)
    {
      p2=s.find_first_of('=',p);
      p3=p2!=std::string::npos?p2-p:p2;
      std::string tmp=s.substr(p,p3);
      ret.push_back(tmp);
      p=p2!=std::string::npos?p2+1:p2;
    }
  return ret;
}
