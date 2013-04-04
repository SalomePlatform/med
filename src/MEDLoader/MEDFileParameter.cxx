// Copyright (C) 2007-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "MEDFileParameter.hxx"
#include "MEDFileUtilities.hxx"
#include "MEDLoaderBase.hxx"

#include "InterpKernelAutoPtr.hxx"

#include <set>

using namespace ParaMEDMEM;

MEDFileParameter1TS::MEDFileParameter1TS(int iteration, int order, double time):_iteration(iteration),_order(order),_time(time)
{
}

MEDFileParameter1TS::MEDFileParameter1TS():_iteration(-1),_order(-1),_time(0.)
{
}

bool MEDFileParameter1TS::isEqual(const MEDFileParameter1TS *other, double eps, std::string& what) const
{
  std::ostringstream oss;
  if(!other)
    { what="Other is null"; return false; }
  if(_iteration!=other->_iteration)
    { oss << "iteration number mismatches " << _iteration << " != " << other->_iteration; what=oss.str(); return false; }
  if(_order!=other->_order)
    { oss << "order number mismatches " << _iteration << " != " << other->_iteration; what=oss.str(); return false; }
  if(fabs(_time-other->_time)>eps)
    { oss << "time mismatches " << _time << " != " << other->_time << " eps=" << eps; what=oss.str(); return false; }
  return true;
}

MEDFileParameter1TS *MEDFileParameterDouble1TSWTI::deepCpy() const throw(INTERP_KERNEL::Exception)
{
  return new MEDFileParameterDouble1TSWTI(*this);
}

bool MEDFileParameterDouble1TSWTI::isEqual(const MEDFileParameter1TS *other, double eps, std::string& what) const
{
  if(!MEDFileParameter1TS::isEqual(other,eps,what))
    return false;
  const MEDFileParameterDouble1TSWTI *otherC=dynamic_cast<const MEDFileParameterDouble1TSWTI *>(other);
  if(!otherC)
    { what="IsEqual fails because this is double parameter other no !"; return false; }
  if(fabs(_arr-otherC->_arr)>eps)
    { std::ostringstream oss; oss << "value differ " << _arr << " != " << otherC->_arr << " (eps=" << eps << ")"; return false; }
  return true;
}

std::size_t MEDFileParameterDouble1TSWTI::getHeapMemorySize() const
{
  return sizeof(MEDFileParameterDouble1TSWTI);
}

std::string MEDFileParameterDouble1TSWTI::simpleRepr() const
{
  std::ostringstream oss;
  simpleRepr2(0,oss);
  return oss.str();
}

void MEDFileParameterDouble1TSWTI::simpleRepr2(int bkOffset, std::ostream& oss) const
{
  std::string startOfLine(bkOffset,' ');
  oss << startOfLine << "ParameterDoubleItem with (iteration,order) = (" << _iteration << "," << _order << ")" << std::endl;
  oss << startOfLine << "Time associacited = " << _time << std::endl;
  oss << startOfLine << "The value is ***** " << _arr <<  " *****" << std::endl;
}

MEDFileParameterDouble1TSWTI *MEDFileParameterDouble1TSWTI::New(int iteration, int order, double time)
{
  return new MEDFileParameterDouble1TSWTI(iteration,order,time);
}

MEDFileParameterDouble1TSWTI::MEDFileParameterDouble1TSWTI():_arr(std::numeric_limits<double>::max())
{
}

MEDFileParameterDouble1TSWTI::MEDFileParameterDouble1TSWTI(int iteration, int order, double time):MEDFileParameter1TS(iteration,order,time)
{
}

void MEDFileParameterDouble1TSWTI::finishLoading(med_idt fid, const std::string& name, int dt, int it, int nbOfSteps) throw(INTERP_KERNEL::Exception)
{
  std::ostringstream oss; oss << "MEDFileParameterDouble1TS::finishLoading : no specified time step (" << dt << "," << it << ") ! Time steps available : ";
  for(int i=0;i<nbOfSteps;i++)
    {
      int locDt,locIt;
      double tim;
      MEDparameterComputationStepInfo(fid,name.c_str(),i+1,&locDt,&locIt,&tim);
      if(dt==locDt && it==locIt)
        {
          _iteration=locDt; _order=locIt; _time=tim;
          MEDparameterValueRd(fid,name.c_str(),_iteration,_order,reinterpret_cast<unsigned char *const>(&_arr));
          return ;
        }
      else
        {
          oss << "(" << locDt << "," << locIt << ")";
          if(i!=nbOfSteps-1)
            oss << ", ";
        }
    }
  throw INTERP_KERNEL::Exception(oss.str().c_str());
}

void MEDFileParameterDouble1TSWTI::readValue(med_idt fid, const std::string& name) throw(INTERP_KERNEL::Exception)
{
  MEDparameterValueRd(fid,name.c_str(),_iteration,_order,reinterpret_cast<unsigned char *const>(&_arr));
}

void MEDFileParameterDouble1TSWTI::finishLoading(med_idt fid, const std::string& name, int timeStepId) throw(INTERP_KERNEL::Exception)
{
  int locDt,locIt;
  double dt;
  MEDparameterComputationStepInfo(fid,name.c_str(),timeStepId+1,&locDt,&locIt,&dt);
  _iteration=locDt; _order=locIt; _time=dt;
  MEDparameterValueRd(fid,name.c_str(),_iteration,_order,reinterpret_cast<unsigned char *const>(&_arr));
}

void MEDFileParameterDouble1TSWTI::writeLL(med_idt fid, const std::string& name, const MEDFileWritable& mw) const throw(INTERP_KERNEL::Exception)
{
  char nameW[MED_NAME_SIZE+1];
  MEDLoaderBase::safeStrCpy(name.c_str(),MED_NAME_SIZE,nameW,mw.getTooLongStrPolicy());
  MEDparameterValueWr(fid,nameW,_iteration,_order,_time,reinterpret_cast<const unsigned char *>(&_arr));
}

std::size_t MEDFileParameterTinyInfo::getHeapMemSizeOfStrings() const
{
  return _dt_unit.capacity()+_name.capacity()+_desc_name.capacity();
}

bool MEDFileParameterTinyInfo::isEqualStrings(const MEDFileParameterTinyInfo& other, std::string& what) const
{
  std::ostringstream oss;
  if(_name!=other._name)
    { oss << "name differ ! this=" << _name << " and other=" << other._name; what=oss.str(); return false; }
  if(_desc_name!=other._desc_name)
    { oss << "name differ ! this=" << _desc_name << " and other=" << other._desc_name; what=oss.str(); return false; }
  if(_dt_unit!=other._dt_unit)
    { oss << "unit of time differ ! this=" << _dt_unit << " and other=" << other._dt_unit; what=oss.str(); return false; }
  return true;
}

void MEDFileParameterTinyInfo::writeLLHeader(med_idt fid, med_parameter_type typ) const throw(INTERP_KERNEL::Exception)
{
  char nameW[MED_NAME_SIZE+1],descW[MED_COMMENT_SIZE+1],dtunitW[MED_SNAME_SIZE+1];
  MEDLoaderBase::safeStrCpy(_name.c_str(),MED_NAME_SIZE,nameW,getTooLongStrPolicy());
  MEDLoaderBase::safeStrCpy(_desc_name.c_str(),MED_COMMENT_SIZE,descW,getTooLongStrPolicy());
  MEDLoaderBase::safeStrCpy(_dt_unit.c_str(),MED_SNAME_SIZE,dtunitW,getTooLongStrPolicy());
  MEDparameterCr(fid,nameW,typ,descW,dtunitW);
}

void MEDFileParameterTinyInfo::mainRepr(int bkOffset, std::ostream& oss) const
{
  std::string startOfLine(bkOffset,' ');
  oss << startOfLine << "Parameter with name \"" << _name << "\"" << std::endl;
  oss << startOfLine << "Parameter with description \"" << _desc_name << "\"" << std::endl;
  oss << startOfLine << "Parameter with unit name \"" << _dt_unit << "\"" << std::endl;
}

MEDFileParameterDouble1TS *MEDFileParameterDouble1TS::New()
{
  return new MEDFileParameterDouble1TS;
}

MEDFileParameterDouble1TS *MEDFileParameterDouble1TS::New(const char *fileName) throw(INTERP_KERNEL::Exception)
{
  return new MEDFileParameterDouble1TS(fileName);
}

MEDFileParameterDouble1TS *MEDFileParameterDouble1TS::New(const char *fileName, const char *paramName) throw(INTERP_KERNEL::Exception)
{
  return new MEDFileParameterDouble1TS(fileName,paramName);
}

MEDFileParameterDouble1TS *MEDFileParameterDouble1TS::New(const char *fileName, const char *paramName, int dt, int it) throw(INTERP_KERNEL::Exception)
{
  return new MEDFileParameterDouble1TS(fileName,paramName,dt,it);
}

MEDFileParameterDouble1TS::MEDFileParameterDouble1TS()
{
}

MEDFileParameterDouble1TS::MEDFileParameterDouble1TS(const char *fileName, const char *paramName, int dt, int it) throw(INTERP_KERNEL::Exception)
{
  MEDFileUtilities::CheckFileForRead(fileName);
  MEDFileUtilities::AutoFid fid=MEDfileOpen(fileName,MED_ACC_RDONLY);
  int nbPar=MEDnParameter(fid);
  std::ostringstream oss; oss << "MEDFileParameterDouble1TS : no double param name \"" << paramName << "\" ! Double Parameters available are : ";
  INTERP_KERNEL::AutoPtr<char> pName=MEDLoaderBase::buildEmptyString(MED_NAME_SIZE);
  INTERP_KERNEL::AutoPtr<char> descName=MEDLoaderBase::buildEmptyString(MED_COMMENT_SIZE);
  INTERP_KERNEL::AutoPtr<char> unitName=MEDLoaderBase::buildEmptyString(MED_SNAME_SIZE);
  med_parameter_type paramType;
  for(int i=0;i<nbPar;i++)
    {
      int nbOfSteps;
      MEDparameterInfo(fid,i+1,pName,&paramType,descName,unitName,&nbOfSteps);
      std::string paramNameCpp=MEDLoaderBase::buildStringFromFortran(pName,MED_NAME_SIZE);
      if(paramNameCpp==paramName && paramType==MED_FLOAT64)
        {
          _dt_unit=MEDLoaderBase::buildStringFromFortran(unitName,MED_SNAME_SIZE);
          _name=paramNameCpp;
          _desc_name=MEDLoaderBase::buildStringFromFortran(descName,MED_COMMENT_SIZE);
          finishLoading(fid,_name,dt,it,nbOfSteps);
          return ;
        }
      else
        {
          oss << paramNameCpp;
          if(i!=nbPar-1) oss << ", ";
        }
    }
  throw INTERP_KERNEL::Exception(oss.str().c_str());
}

MEDFileParameterDouble1TS::MEDFileParameterDouble1TS(const char *fileName, const char *paramName) throw(INTERP_KERNEL::Exception)
{
  MEDFileUtilities::CheckFileForRead(fileName);
  MEDFileUtilities::AutoFid fid=MEDfileOpen(fileName,MED_ACC_RDONLY);
  int nbPar=MEDnParameter(fid);
  std::ostringstream oss; oss << "MEDFileParameterDouble1TS : no double param name \"" << paramName << "\" ! Double Parameters available are : ";
  INTERP_KERNEL::AutoPtr<char> pName=MEDLoaderBase::buildEmptyString(MED_NAME_SIZE);
  INTERP_KERNEL::AutoPtr<char> descName=MEDLoaderBase::buildEmptyString(MED_COMMENT_SIZE);
  INTERP_KERNEL::AutoPtr<char> unitName=MEDLoaderBase::buildEmptyString(MED_SNAME_SIZE);
  med_parameter_type paramType;
  for(int i=0;i<nbPar;i++)
    {
      int nbOfSteps;
      MEDparameterInfo(fid,i+1,pName,&paramType,descName,unitName,&nbOfSteps);
      std::string paramNameCpp=MEDLoaderBase::buildStringFromFortran(pName,MED_NAME_SIZE);
      if(paramNameCpp==paramName && paramType==MED_FLOAT64)
        {
          if(nbOfSteps>0)
            {
              _dt_unit=MEDLoaderBase::buildStringFromFortran(unitName,MED_SNAME_SIZE);
              _name=paramNameCpp;
              _desc_name=MEDLoaderBase::buildStringFromFortran(descName,MED_COMMENT_SIZE);
              finishLoading(fid,_name,0);
              return ;
            }
          else
            {
              std::ostringstream oss2; oss2 << "Param name \"" << paramName << "\" exists but no time steps on it !";
              throw INTERP_KERNEL::Exception(oss2.str().c_str());
            }
        }
      else
        {
          oss << paramNameCpp;
          if(i!=nbPar-1) oss << ", ";
        }
    }
  throw INTERP_KERNEL::Exception(oss.str().c_str());
}

MEDFileParameterDouble1TS::MEDFileParameterDouble1TS(const char *fileName) throw(INTERP_KERNEL::Exception)
{
  MEDFileUtilities::CheckFileForRead(fileName);
  MEDFileUtilities::AutoFid fid=MEDfileOpen(fileName,MED_ACC_RDONLY);
  int nbPar=MEDnParameter(fid);
  if(nbPar<1)
    {
      std::ostringstream oss2; oss2 << "No parameter in file \"" << fileName << "\" !";  
      throw INTERP_KERNEL::Exception(oss2.str().c_str());
    }
  INTERP_KERNEL::AutoPtr<char> pName=MEDLoaderBase::buildEmptyString(MED_NAME_SIZE);
  INTERP_KERNEL::AutoPtr<char> descName=MEDLoaderBase::buildEmptyString(MED_COMMENT_SIZE);
  INTERP_KERNEL::AutoPtr<char> unitName=MEDLoaderBase::buildEmptyString(MED_SNAME_SIZE);
  med_parameter_type paramType;
  int nbOfSteps;
  MEDparameterInfo(fid,1,pName,&paramType,descName,unitName,&nbOfSteps);
  std::string paramNameCpp=MEDLoaderBase::buildStringFromFortran(pName,MED_NAME_SIZE);
  if(paramType==MED_FLOAT64)
    {
      if(nbOfSteps>0)
        {
          _dt_unit=MEDLoaderBase::buildStringFromFortran(unitName,MED_SNAME_SIZE);
          _name=paramNameCpp;
          _desc_name=MEDLoaderBase::buildStringFromFortran(descName,MED_COMMENT_SIZE);
          finishLoading(fid,_name,0);
          return ;
        }
      else
        {
          std::ostringstream oss2; oss2 << "Double param name \"" << paramNameCpp << "\" exists in file \""<< fileName << "\"but no time steps on it !";
          throw INTERP_KERNEL::Exception(oss2.str().c_str());
        }
    }
  else
    {
      std::ostringstream oss2; oss2 << "First parameter in file \"" << fileName << "\" is not double !";  
      throw INTERP_KERNEL::Exception(oss2.str().c_str());
    }
}

bool MEDFileParameterDouble1TS::isEqual(const MEDFileParameter1TS *other, double eps, std::string& what) const
{
  if(!MEDFileParameterDouble1TSWTI::isEqual(other,eps,what))
    return false;
  const MEDFileParameterDouble1TS *otherC=dynamic_cast<const MEDFileParameterDouble1TS *>(other);
  if(!otherC)
    { what="Other is not of type MEDFileParameterDouble1TS as this"; return false; }
  if(!isEqualStrings(*otherC,what))
    return false;
  return true;
}

MEDFileParameter1TS *MEDFileParameterDouble1TS::deepCpy() const throw(INTERP_KERNEL::Exception)
{
  return new MEDFileParameterDouble1TS(*this);
}

std::string MEDFileParameterDouble1TS::simpleRepr() const
{
  std::ostringstream oss;
  MEDFileParameterTinyInfo::mainRepr(0,oss);
  MEDFileParameterDouble1TSWTI::simpleRepr2(0,oss);
  return oss.str();
}

std::size_t MEDFileParameterDouble1TS::getHeapMemorySize() const
{
  return getHeapMemSizeOfStrings()+sizeof(MEDFileParameterDouble1TS);
}

void MEDFileParameterDouble1TS::write(const char *fileName, int mode) const throw(INTERP_KERNEL::Exception)
{
  med_access_mode medmod=MEDFileUtilities::TraduceWriteMode(mode);
  MEDFileUtilities::AutoFid fid=MEDfileOpen(fileName,medmod);
  MEDFileParameterTinyInfo::writeLLHeader(fid,MED_FLOAT64);
  MEDFileParameterDouble1TSWTI::writeLL(fid,_name,*this);
}

MEDFileParameterMultiTS *MEDFileParameterMultiTS::New()
{
  return new MEDFileParameterMultiTS;
}

MEDFileParameterMultiTS *MEDFileParameterMultiTS::New(const char *fileName) throw(INTERP_KERNEL::Exception)
{
  return new MEDFileParameterMultiTS(fileName);
}

MEDFileParameterMultiTS *MEDFileParameterMultiTS::New(const char *fileName, const char *paramName) throw(INTERP_KERNEL::Exception)
{
  return new MEDFileParameterMultiTS(fileName,paramName);
}

MEDFileParameterMultiTS::MEDFileParameterMultiTS()
{
}

MEDFileParameterMultiTS::MEDFileParameterMultiTS(const MEDFileParameterMultiTS& other, bool deepCopy):MEDFileParameterTinyInfo(other),_param_per_ts(other._param_per_ts)
{
  if(deepCopy)
    for(std::size_t i=0;i<_param_per_ts.size();i++)
      {
        const MEDFileParameter1TS *elt=_param_per_ts[i];
        if(elt)
          _param_per_ts[i]=elt->deepCpy();
      }
}

MEDFileParameterMultiTS::MEDFileParameterMultiTS(const char *fileName) throw(INTERP_KERNEL::Exception)
{
  MEDFileUtilities::CheckFileForRead(fileName);
  MEDFileUtilities::AutoFid fid=MEDfileOpen(fileName,MED_ACC_RDONLY);
  int nbPar=MEDnParameter(fid);
  if(nbPar<1)
    {
      std::ostringstream oss; oss << "MEDFileParameterMultiTS : no parameter in file \"" << fileName << "\" !" ;
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  INTERP_KERNEL::AutoPtr<char> pName=MEDLoaderBase::buildEmptyString(MED_NAME_SIZE);
  INTERP_KERNEL::AutoPtr<char> descName=MEDLoaderBase::buildEmptyString(MED_COMMENT_SIZE);
  INTERP_KERNEL::AutoPtr<char> unitName=MEDLoaderBase::buildEmptyString(MED_SNAME_SIZE);
  med_parameter_type paramType;
  int nbOfSteps;
  MEDparameterInfo(fid,1,pName,&paramType,descName,unitName,&nbOfSteps);
  std::string paramNameCpp=MEDLoaderBase::buildStringFromFortran(pName,MED_NAME_SIZE);
  _dt_unit=MEDLoaderBase::buildStringFromFortran(unitName,MED_SNAME_SIZE);
  _name=paramNameCpp;
  _desc_name=MEDLoaderBase::buildStringFromFortran(descName,MED_COMMENT_SIZE);
  finishLoading(fid,paramType,nbOfSteps);
}

MEDFileParameterMultiTS::MEDFileParameterMultiTS(const char *fileName, const char *paramName) throw(INTERP_KERNEL::Exception)
{
  MEDFileUtilities::CheckFileForRead(fileName);
  MEDFileUtilities::AutoFid fid=MEDfileOpen(fileName,MED_ACC_RDONLY);
  int nbPar=MEDnParameter(fid);
  std::ostringstream oss; oss << "MEDFileParameterDouble1TS : no double param name \"" << paramName << "\" ! Double Parameters available are : ";
  INTERP_KERNEL::AutoPtr<char> pName=MEDLoaderBase::buildEmptyString(MED_NAME_SIZE);
  INTERP_KERNEL::AutoPtr<char> descName=MEDLoaderBase::buildEmptyString(MED_COMMENT_SIZE);
  INTERP_KERNEL::AutoPtr<char> unitName=MEDLoaderBase::buildEmptyString(MED_SNAME_SIZE);
  med_parameter_type paramType;
  for(int i=0;i<nbPar;i++)
    {
      int nbOfSteps;
      MEDparameterInfo(fid,i+1,pName,&paramType,descName,unitName,&nbOfSteps);
      std::string paramNameCpp=MEDLoaderBase::buildStringFromFortran(pName,MED_NAME_SIZE);
      if(paramNameCpp==paramName)
        {
          if(nbOfSteps>0)
            {
              _dt_unit=MEDLoaderBase::buildStringFromFortran(unitName,MED_SNAME_SIZE);
              _name=paramNameCpp;
              _desc_name=MEDLoaderBase::buildStringFromFortran(descName,MED_COMMENT_SIZE);
              finishLoading(fid,paramType,nbOfSteps);
              return ;
            }
          else
            {
              std::ostringstream oss2; oss2 << "Param name \"" << paramName << "\" exists but no time steps on it !";
              throw INTERP_KERNEL::Exception(oss2.str().c_str());
            }
        }
      else
        {
          oss << paramNameCpp;
          if(i!=nbPar-1) oss << ", ";
        }
    }
  throw INTERP_KERNEL::Exception(oss.str().c_str());
}

void MEDFileParameterMultiTS::finishLoading(med_idt fid, med_parameter_type typ, int nbOfSteps) throw(INTERP_KERNEL::Exception)
{
  _param_per_ts.resize(nbOfSteps);
  for(int i=0;i<nbOfSteps;i++)
    {
      int dt,it;
      double tim;
      MEDparameterComputationStepInfo(fid,_name.c_str(),i+1,&dt,&it,&tim);
      switch(typ)
        {
        case MED_FLOAT64:
          _param_per_ts[i]=MEDFileParameterDouble1TSWTI::New(dt,it,tim);
          _param_per_ts[i]->readValue(fid,_name.c_str());
          break;
          /*case MED_INT32;
         _param_per_ts[i]=;
         break;*/
        default:
          throw INTERP_KERNEL::Exception("MEDFileParameterMultiTS::finishLoading : supporting only FLOAT64 !");
        }
    }
}

std::size_t MEDFileParameterMultiTS::getHeapMemorySize() const
{
  std::size_t ret=sizeof(MEDFileParameterMultiTS);
  std::size_t ret2=sizeof(MEDCouplingAutoRefCountObjectPtr<MEDFileParameter1TS>)*_param_per_ts.capacity();
  for(std::size_t i=0;i<_param_per_ts.size();i++)
    {
      const MEDFileParameter1TS *pt(_param_per_ts[i]);
      if(pt)
        ret2+=pt->getHeapMemorySize();
    }
  return ret2+ret;
}

MEDFileParameterMultiTS *MEDFileParameterMultiTS::deepCpy() const throw(INTERP_KERNEL::Exception)
{
  return new MEDFileParameterMultiTS(*this,true);
}

bool MEDFileParameterMultiTS::isEqual(const MEDFileParameterMultiTS *other, double eps, std::string& what) const
{
  if(!other)
    { what="other is null !"; return false; }
  if(_param_per_ts.size()!=other->_param_per_ts.size())
    { what="number of time steps differs !"; return false; }
  std::ostringstream oss;
  for(std::size_t i=0;i<_param_per_ts.size();i++)
    {
      const MEDFileParameter1TS *a(_param_per_ts[i]),*b(other->_param_per_ts[i]);
      if((a && !b) || (!a && b))
        { oss << "At time step id #" << i << " pointer is defined on one side not in the other !"; what=oss.str(); return false; }
      if(a)
        if(!a->isEqual(b,eps,what))
          { oss << " At time step id #" << i << " non equality !"; what+=oss.str(); return false; }
    }
  return true;
}

void MEDFileParameterMultiTS::write(const char *fileName, int mode) const throw(INTERP_KERNEL::Exception)
{
  med_access_mode medmod=MEDFileUtilities::TraduceWriteMode(mode);
  MEDFileUtilities::AutoFid fid=MEDfileOpen(fileName,medmod);
  writeLL(fid,*this);
}

void MEDFileParameterMultiTS::writeLL(med_idt fid, const MEDFileWritable& mw) const throw(INTERP_KERNEL::Exception)
{
  std::set<med_parameter_type> diffType;
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameter1TS> >::const_iterator it=_param_per_ts.begin();it!=_param_per_ts.end();it++)
    {
      const MEDFileParameter1TS *elt(*it);
      if(dynamic_cast<const MEDFileParameterDouble1TSWTI *>(elt))
        diffType.insert(MED_FLOAT64);
    }
  if(diffType.size()>1)
    throw INTERP_KERNEL::Exception("MEDFileParameterMultiTS::writeLL : impossible to mix type of data in parameters in MED file ! Only float64 or only int32 ...");
  if(diffType.empty())
    return;
  med_parameter_type typ=*diffType.begin();
  MEDFileParameterTinyInfo::writeLLHeader(fid,typ);
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameter1TS> >::const_iterator it=_param_per_ts.begin();it!=_param_per_ts.end();it++)
    {
      const MEDFileParameter1TS *elt(*it);
      if(elt)
        elt->writeLL(fid,_name,mw);
    }
}

std::string MEDFileParameterMultiTS::simpleRepr() const
{
  std::ostringstream oss;
  simpleRepr2(0,oss);
  return oss.str();
}

void MEDFileParameterMultiTS::simpleRepr2(int bkOffset, std::ostream& oss) const
{
  MEDFileParameterTinyInfo::mainRepr(bkOffset,oss);
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameter1TS> >::const_iterator it=_param_per_ts.begin();it!=_param_per_ts.end();it++)
    {
      const MEDFileParameter1TS *elt(*it);
      if(elt)
        elt->simpleRepr2(bkOffset+2,oss);
    }
}

void MEDFileParameterMultiTS::appendValue(int dt, int it, double time, double val) throw(INTERP_KERNEL::Exception)
{
  MEDCouplingAutoRefCountObjectPtr<MEDFileParameterDouble1TSWTI> elt=MEDFileParameterDouble1TSWTI::New(dt,it,time);
  elt->setValue(val);
  MEDCouplingAutoRefCountObjectPtr<MEDFileParameter1TS> elt2((MEDFileParameterDouble1TSWTI*)elt); elt2->incrRef();
  _param_per_ts.push_back(elt2);
}

double MEDFileParameterMultiTS::getDoubleValue(int iteration, int order) const throw(INTERP_KERNEL::Exception)
{
  int pos=getPosOfTimeStep(iteration,order);
  const MEDFileParameter1TS *elt=_param_per_ts[pos];
  if(!elt)
    {
      std::ostringstream oss; oss << "MEDFileParameterMultiTS::getDoubleValue : time iteration it=" << iteration << " order=" << order;
      oss << " exists but elt is empty !"; 
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  const MEDFileParameterDouble1TSWTI *eltC=dynamic_cast<const MEDFileParameterDouble1TSWTI *>(elt);
  if(!eltC)
    {
      std::ostringstream oss; oss << "MEDFileParameterMultiTS::getDoubleValue : time iteration it=" << iteration << " order=" << order;
      oss << " exists but not double !"; 
    }
  return eltC->getValue();
}

int MEDFileParameterMultiTS::getPosOfTimeStep(int iteration, int order) const throw(INTERP_KERNEL::Exception)
{
  int ret=0;
  std::ostringstream oss; oss << "MEDFileParameterMultiTS::getPosOfTimeStep : no such iteration=" << iteration << " order=" << order << " ! Possibilities are :";
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameter1TS> >::const_iterator it=_param_per_ts.begin();it!=_param_per_ts.end();it++,ret++)
    {
      const MEDFileParameter1TS *elt(*it);
      if(elt)
        {
          if(elt->getIteration()==iteration && elt->getOrder())
            return ret;
          else
            oss << "(" << elt->getIteration() << "," << elt->getOrder() << "), ";
        }
    }
  throw INTERP_KERNEL::Exception(oss.str().c_str());
}

int MEDFileParameterMultiTS::getPosGivenTime(double time, double eps) const throw(INTERP_KERNEL::Exception)
{
  int ret=0;
  std::ostringstream oss; oss << "MEDFileParameterMultiTS::getPosGivenTime : no such time=" << time << " ! Possibilities are :";
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameter1TS> >::const_iterator it=_param_per_ts.begin();it!=_param_per_ts.end();it++,ret++)
    {
      const MEDFileParameter1TS *elt(*it);
      if(elt)
        {
          if(fabs(elt->getTimeValue()-time)<=eps)
            return ret;
          else
            oss << elt->getTimeValue() << ", ";
        }
    }
  throw INTERP_KERNEL::Exception(oss.str().c_str());
}

/*!
 * \return an internal pointer that can be null. Warning the caller is \b not responsible of the returned pointer.
 */
MEDFileParameter1TS *MEDFileParameterMultiTS::getTimeStepAtPos(int posId) const throw(INTERP_KERNEL::Exception)
{
  if(posId<0 || posId>=(int)_param_per_ts.size())
    {
      std::ostringstream oss; oss << "MEDFileParameterMultiTS::getTimeStepAtPos : invalid pos ! Should be in [0," << _param_per_ts.size() << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  return const_cast<MEDFileParameter1TS *>(static_cast<const MEDFileParameter1TS *>(_param_per_ts[posId]));
}

void MEDFileParameterMultiTS::eraseTimeStepIds(const int *startIds, const int *endIds) throw(INTERP_KERNEL::Exception)
{
  std::vector<bool> b(_param_per_ts.size(),true);
  int len=(int)_param_per_ts.size();
  for(const int *w=startIds;w!=endIds;w++)
    if(*w>=0 && *w<len)
      b[*w]=false;
    else
      {
        std::ostringstream oss; oss << "MEDFileParameterMultiTS::eraseTimeStepIds : At pos #" << std::distance(startIds,w) << " value is " << *w << " should be in [0," << len << ") !"; throw INTERP_KERNEL::Exception(oss.str().c_str());
      }
  std::size_t newNb=std::count(b.begin(),b.end(),true);
  std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameter1TS> > paramPerTs(newNb);
  std::size_t j=0;
  for(std::size_t i=0;i<_param_per_ts.size();i++)
    if(b[i])
      paramPerTs[j++]=_param_per_ts[i];
  _param_per_ts=paramPerTs;
}

std::vector< std::pair<int,int> > MEDFileParameterMultiTS::getIterations() const throw(INTERP_KERNEL::Exception)
{
  std::vector< std::pair<int,int> > ret;
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameter1TS> >::const_iterator it=_param_per_ts.begin();it!=_param_per_ts.end();it++)
    {
      const MEDFileParameter1TS *elt(*it);
      if(elt)
        ret.push_back(std::pair<int,int>(elt->getIteration(),elt->getOrder()));
    }
  return ret;
}

/*!
 * \param [out] ret1
 */
std::vector< std::pair<int,int> > MEDFileParameterMultiTS::getTimeSteps(std::vector<double>& ret1) const throw(INTERP_KERNEL::Exception)
{
  std::vector< std::pair<int,int> > ret0;
  ret1.clear();
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameter1TS> >::const_iterator it=_param_per_ts.begin();it!=_param_per_ts.end();it++)
    {
      const MEDFileParameter1TS *elt(*it);
      if(elt)
        {
          ret0.push_back(std::pair<int,int>(elt->getIteration(),elt->getOrder()));
          ret1.push_back(elt->getTimeValue());
        }
    }
  return ret0;
}

MEDFileParameters *MEDFileParameters::New()
{
  return new MEDFileParameters;
}

MEDFileParameters *MEDFileParameters::New(const char *fileName) throw(INTERP_KERNEL::Exception)
{
  return new MEDFileParameters(fileName);
}

MEDFileParameters::MEDFileParameters(const char *fileName) throw(INTERP_KERNEL::Exception)
{
  MEDFileUtilities::CheckFileForRead(fileName);
  MEDFileUtilities::AutoFid fid=MEDfileOpen(fileName,MED_ACC_RDONLY);
  int nbPar=MEDnParameter(fid);
  _params.resize(nbPar);
  INTERP_KERNEL::AutoPtr<char> pName=MEDLoaderBase::buildEmptyString(MED_NAME_SIZE);
  INTERP_KERNEL::AutoPtr<char> descName=MEDLoaderBase::buildEmptyString(MED_COMMENT_SIZE);
  INTERP_KERNEL::AutoPtr<char> unitName=MEDLoaderBase::buildEmptyString(MED_SNAME_SIZE);
  med_parameter_type paramType;
  for(int i=0;i<nbPar;i++)
    {
      int nbOfSteps;
      MEDparameterInfo(fid,i+1,pName,&paramType,descName,unitName,&nbOfSteps);
      std::string paramNameCpp=MEDLoaderBase::buildStringFromFortran(pName,MED_NAME_SIZE);
      _params[i]=MEDFileParameterMultiTS::New(fileName,paramNameCpp.c_str());
    }
}

MEDFileParameters::MEDFileParameters()
{
}

std::size_t MEDFileParameters::getHeapMemorySize() const
{
  std::size_t ret=sizeof(MEDFileParameters);
  std::size_t ret2=sizeof(MEDCouplingAutoRefCountObjectPtr<MEDFileParameterMultiTS>)*_params.capacity();
  for(std::size_t i=0;i<_params.size();i++)
    {
      const MEDFileParameterMultiTS *pt(_params[i]);
      if(pt)
        ret2+=pt->getHeapMemorySize();
    }
  return ret2+ret;
}

MEDFileParameters *MEDFileParameters::deepCpy() const throw(INTERP_KERNEL::Exception)
{
  return new MEDFileParameters(*this,true);
}

bool MEDFileParameters::isEqual(const MEDFileParameters *other, double eps, std::string& what) const
{
  if(!other)
    { what="other is null !"; return false; }
  if(_params.size()!=other->_params.size())
    { what="number of parameters differs !"; return false; }
  std::ostringstream oss;
  for(std::size_t i=0;i<_params.size();i++)
    {
      const MEDFileParameterMultiTS *a(_params[i]),*b(other->_params[i]);
      if((a && !b) || (!a && b))
        { oss << "At param with id #" << i << " pointer is defined on one side not in the other !"; what=oss.str(); return false; }
      if(a)
        if(!a->isEqual(b,eps,what))
          { oss << " At param with id #" << i << " non equality !"; what+=oss.str(); return false; }
    }
  return true;
}

MEDFileParameters::MEDFileParameters(const MEDFileParameters& other, bool deepCopy):MEDFileWritable(other),_params(other._params)
{
  if(deepCopy)
    for(std::size_t i=0;i<_params.size();i++)
      {
        const MEDFileParameterMultiTS *elt=_params[i];
        if(elt)
          _params[i]=elt->deepCpy();
      }
}

void MEDFileParameters::write(const char *fileName, int mode) const throw(INTERP_KERNEL::Exception)
{
  med_access_mode medmod=MEDFileUtilities::TraduceWriteMode(mode);
  MEDFileUtilities::AutoFid fid=MEDfileOpen(fileName,medmod);
  writeLL(fid);
}

void MEDFileParameters::writeLL(med_idt fid) const throw(INTERP_KERNEL::Exception)
{
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameterMultiTS> >::const_iterator it=_params.begin();it!=_params.end();it++)
    {
      const MEDFileParameterMultiTS *elt(*it);
      if(elt)
        elt->writeLL(fid,*this);
    }
}

std::vector<std::string> MEDFileParameters::getParamsNames() const throw(INTERP_KERNEL::Exception)
{
  std::vector<std::string> ret(_params.size());
  int i=0;
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameterMultiTS> >::const_iterator it=_params.begin();it!=_params.end();it++,i++)
    {
      const MEDFileParameterMultiTS *p=(*it);
      if(p)
        {
          ret[i]=p->getName();
        }
      else
        {
          std::ostringstream oss; oss << "MEDFileParameters::getParamsNames : At rank #" << i << " param is not defined !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  return ret;
}

std::string MEDFileParameters::simpleRepr() const
{
  std::ostringstream oss;
  simpleReprWithoutHeader(oss);
  return oss.str();
}

void MEDFileParameters::simpleReprWithoutHeader(std::ostream& oss) const
{
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameterMultiTS> >::const_iterator it=_params.begin();it!=_params.end();it++)
    {
      const MEDFileParameterMultiTS *elt(*it);
      if(elt)
        elt->simpleRepr2(2,oss);
    }
}

void MEDFileParameters::resize(int newSize) throw(INTERP_KERNEL::Exception)
{
  if(newSize<0)
    throw INTERP_KERNEL::Exception("MEDFileParameters::resize : should be positive !");
  _params.resize(newSize);
}

void MEDFileParameters::pushParam(MEDFileParameterMultiTS *param) throw(INTERP_KERNEL::Exception)
{
  if(param)
    param->incrRef();
  MEDCouplingAutoRefCountObjectPtr<MEDFileParameterMultiTS> elt(param);
  _params.push_back(elt);
}

void MEDFileParameters::setParamAtPos(int i, MEDFileParameterMultiTS *param) throw(INTERP_KERNEL::Exception)
{
  if(i<0)
    throw INTERP_KERNEL::Exception("MEDFileParameters::setParamAtPos : should be positive !");
  if(i>=(int)_params.size())
    _params.resize(i+1);
  if(param)
    param->incrRef();
  MEDCouplingAutoRefCountObjectPtr<MEDFileParameterMultiTS> elt(param);
  _params[i]=elt;
}

/*!
 * \return an internal pointer that can be null. Warning the caller is \b not responsible of the returned pointer.
 */
MEDFileParameterMultiTS *MEDFileParameters::getParamAtPos(int i) const throw(INTERP_KERNEL::Exception)
{
  if(i<0 || i>=(int)_params.size())
    {
      std::ostringstream oss; oss << "MEDFileParameters::getParamAtPos : should be in [0," << _params.size() << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  const MEDFileParameterMultiTS *elt=_params[i];
  return const_cast<MEDFileParameterMultiTS *>(elt);
}

/*!
 * \return an internal pointer that can be null. Warning the caller is \b not responsible of the returned pointer.
 */
MEDFileParameterMultiTS *MEDFileParameters::getParamWithName(const char *paramName) const throw(INTERP_KERNEL::Exception)
{
  int pos=getPosFromParamName(paramName);
  return getParamAtPos(pos);
}

void MEDFileParameters::destroyParamAtPos(int i) throw(INTERP_KERNEL::Exception)
{
  if(i<0 || i>=(int)_params.size())
    {
      std::ostringstream oss; oss << "MEDFileParameters::destroyParamAtPos : should be in [0," << _params.size() << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  _params[i]=MEDCouplingAutoRefCountObjectPtr<MEDFileParameterMultiTS>(0);
}

int MEDFileParameters::getPosFromParamName(const char *paramName) const throw(INTERP_KERNEL::Exception)
{
  std::ostringstream oss; oss << "MEDFileParameters::getPosFromParamName : no such name=" << paramName << " ! Possibilities are :";
  int ret=0;
  for(std::vector< MEDCouplingAutoRefCountObjectPtr<MEDFileParameterMultiTS> >::const_iterator it=_params.begin();it!=_params.end();it++,ret++)
    {
      const MEDFileParameterMultiTS *elt(*it);
      if(elt)
        {
          if(std::string(elt->getName())==paramName)
            return ret;
          else
            oss << elt->getName() << ", ";
        }
    }
  throw INTERP_KERNEL::Exception(oss.str().c_str());
}

int MEDFileParameters::getNumberOfParams() const throw(INTERP_KERNEL::Exception)
{
  return (int)_params.size();
}