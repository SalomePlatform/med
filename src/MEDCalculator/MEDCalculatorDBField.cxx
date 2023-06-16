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

#include "MEDCalculatorDBField.hxx"
#include "MEDCalculatorBrowserField.hxx"

#include "MEDLoaderBase.hxx"
#include "MEDLoader.hxx"

#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingFieldOverTime.hxx"

#include "MEDCouplingFieldOverTimeServant.hxx"
#include "SALOME_NamingService.hxx"

#include <cmath>

using namespace MEDCoupling;

MEDCalculatorDBFieldReal *MEDCalculatorDBField::New(const MEDCalculatorBrowserField& ls)
{
  return new MEDCalculatorDBFieldReal(ls);
}

MEDCalculatorDBFieldCst *MEDCalculatorDBField::New(double val)
{
  return new MEDCalculatorDBFieldCst(val);
}

std::size_t MEDCalculatorDBField::getHeapMemorySizeWithoutChildren() const
{
  return 0;
}

std::vector<const BigMemoryObject *> MEDCalculatorDBField::getDirectChildrenWithNull() const
{
  return std::vector<const BigMemoryObject *>();
}

MEDCalculatorDBField *MEDCalculatorDBField::operator+(double val) const
{
  MCAuto<MEDCalculatorDBFieldCst> par2=new MEDCalculatorDBFieldCst(val);
  return (*this)+(*par2);
}

MEDCalculatorDBField *MEDCalculatorDBField::operator-(double val) const
{
  MCAuto<MEDCalculatorDBFieldCst> par2=new MEDCalculatorDBFieldCst(val);
  return (*this)-(*par2);
}

MEDCalculatorDBField *MEDCalculatorDBField::operator*(double val) const
{
  MCAuto<MEDCalculatorDBFieldCst> par2=new MEDCalculatorDBFieldCst(val);
  return (*this)*(*par2);
}

MEDCalculatorDBField *MEDCalculatorDBField::operator/(double val) const
{
  MCAuto<MEDCalculatorDBFieldCst> par2=new MEDCalculatorDBFieldCst(val);
  return (*this)/(*par2);
}

MEDCalculatorDBFieldReal *MEDCalculatorDBFieldReal::operator()(const MEDCalculatorDBRangeSelection& t, const MEDCalculatorDBRangeSelection& p, const MEDCalculatorDBRangeSelection& c)
{
  MEDCalculatorDBFieldReal *ret=new MEDCalculatorDBFieldReal(*this);
  ret->_t=t;
  ret->_p=p;
  ret->_c=c;
  return ret;
}

MEDCalculatorDBFieldReal::~MEDCalculatorDBFieldReal()
{
}

MEDCalculatorDBFieldReal::MEDCalculatorDBFieldReal(TypeOfField type):_type(type)
{
}

void MEDCalculatorDBFieldReal::setName(const char *name)
{
  _name=name;
  /*fetchData();
  std::vector<int> ids=_t.getIds(_time_steps.size());
  for(std::vector<int>::const_iterator iter=ids.begin();iter!=ids.end();iter++)
  _time_steps[*iter]->setName(name);*/
}

void MEDCalculatorDBFieldReal::setDescription(const char *descr)
{
  _description=descr;
  /*fetchData();
  std::vector<int> ids=_t.getIds(_time_steps.size());
  for(std::vector<int>::const_iterator iter=ids.begin();iter!=ids.end();iter++)
  _time_steps[*iter]->setDescription(descr);*/
}

void MEDCalculatorDBFieldReal::write(const char *fName, bool writeFromScratch) const
{
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  int step=ids[0];
  const MEDCouplingFieldDouble *field=_time_steps[step]->getField(_type,_file_name,_mesh_name,_field_name);
  const MEDCouplingUMesh *mesh=static_cast<const MEDCouplingUMesh *>(field->getMesh());
  int status=MEDLoaderBase::getStatusOfFile(fName);
  if(!writeFromScratch && status==MEDLoaderBase::EXIST_RW)
    {
      std::vector<std::string> ms=GetMeshNames(fName);
      if(std::find(ms.begin(),ms.end(),mesh->getName())!=ms.end())
        {
          std::ostringstream oss; oss << "In file \"" << fName << "\" the mesh with name \"" << mesh->getName() << "\" already exists !"; 
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
      std::vector<std::string> fs=GetAllFieldNames(fName);
      if(std::find(fs.begin(),fs.end(),field->getName())!=fs.end())
        {
          std::ostringstream oss; oss << "In file \"" << fName << "\" the field with name \"" << field->getName() << "\" already exists !"; 
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  WriteUMesh(fName,mesh,writeFromScratch);
  for(std::vector<std::size_t>::const_iterator iter=ids.begin();iter!=ids.end();iter++)
    _time_steps[*iter]->write(fName,_name,_description);
}

void MEDCalculatorDBFieldReal::display() const
{
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  std::vector< MCAuto<MEDCouplingFieldDouble> > fs2(ids.size());
  int ii=0;
  for(std::vector<std::size_t>::const_iterator iter=ids.begin();iter!=ids.end();iter++)
    fs2[ii++]=_time_steps[*iter]->getFieldWithoutQuestion(_c_labels.size(),_c);
  std::vector<MEDCouplingFieldDouble *> fs(fs2.size());
  std::copy(fs2.begin(),fs2.end(),fs.begin());
  MCAuto<MEDCouplingFieldOverTime> fot=MEDCouplingFieldOverTime::New(fs);
  //
  int argc=0;
  CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
  CORBA::Object_var obj=orb->resolve_initial_references("RootPOA");
  PortableServer::POA_var poa=PortableServer::POA::_narrow(obj);
  PortableServer::POAManager_var mgr=poa->the_POAManager();
  mgr->activate();
  MEDCouplingFieldOverTimeServant *fots=new MEDCouplingFieldOverTimeServant(fot);
  SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_var fotPtr=fots->_this();
  //
  SALOME_NamingService ns(orb);
  ns.Change_Directory("/Containers");
  std::vector<std::string> subdirs=ns.list_subdirs();
  std::ostringstream path;
  path << "/Containers/" << subdirs[0] << "/FactoryServer/PARAVIS_inst_1";
  //
  CORBA::Object_var paravis=ns.Resolve(path.str().c_str());
  CORBA::Request_var req=paravis->_request("ExecuteScript");
  CORBA::NVList_ptr args=req->arguments();
  CORBA::Any ob;
  std::ostringstream script;
  char *ior=orb->object_to_string(fotPtr);
  script << "src1 = ParaMEDCorbaPluginSource()\nsrc1.IORCorba = '" << ior << "'\nasc=GetAnimationScene()\nrw=GetRenderView()\ndr=Show()\ndr.Visibility = 1\n";
  CORBA::string_free(ior);
  ob <<= script.str().c_str();
  args->add_value("script",ob,CORBA::ARG_IN);
  req->set_return_type(CORBA::_tc_void);
  req->invoke();
  // clean-up
}

std::string MEDCalculatorDBFieldReal::simpleRepr() const
{
  std::ostringstream oss;
  oss << "Multitime field with name : \""<< _name << "\".\n";
  oss << "Description of the field is : \"" << _description << "\".\n";
  oss << "Number of time steps of multitime field : " << getNumberOfSteps() << ".\n";
  oss << "Number of components of multitime field : " << getNumberOfComponents() << ".\n";
  oss << "Components names attached are : ";
  std::vector<std::size_t> ids=_c.getIds(_c_labels.size());
  for(std::vector<std::size_t>::const_iterator iter=ids.begin();iter!=ids.end();iter++)
    oss << "\"" << _c_labels[*iter] << "\" ";
  oss << ".\nNumber of fetched field in multime field : " << getNumberOfFetchedSteps() << "/" << getNumberOfSteps() << ".\n";
  return oss.str();
}

MEDCalculatorDBFieldReal::MEDCalculatorDBFieldReal(const MEDCalculatorBrowserField& ls):_file_name(ls.getFileName()),_mesh_name(ls.getCorrespondingMeshesFromField().front()),_field_name(ls.getName()),_type(ls.getType()),
                                                                     _c_labels(ls.getComponents())
{
  const std::vector<MEDCalculatorBrowserStep>& steps=ls.getSteps();
  int sz=steps.size();
  for(int i=0;i<sz;i++)
    {
      MCAuto<MEDCalculatorDBSliceField> elt(new MEDCalculatorDBSliceField(steps[i].getTimeStep(),steps[i].getOrder()));
      _time_steps.push_back(elt);
    }
}

const MEDCalculatorDBFieldReal& MEDCalculatorDBFieldReal::operator=(const MEDCalculatorDBFieldReal& other)
{
  checkConsistencyLight(other);
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  std::vector<std::size_t> ids2=other._t.getIds(other._time_steps.size());
  unsigned int sz=ids.size();
  if(sz!=ids2.size())
    throw INTERP_KERNEL::Exception("FieldReal::operator= : Timesteps lengths mismatch !");
  fetchData();
  other.fetchData();
  for(unsigned int i=0;i<sz;i++)
    _time_steps[ids[i]]->assign(other._time_steps[ids2[i]],_c_labels.size(),_c,other._c_labels.size(),other._c);
  return *this;
}

const MEDCalculatorDBFieldReal& MEDCalculatorDBFieldReal::operator=(double val)
{
  MCAuto<MEDCalculatorDBFieldReal> other=buildCstFieldFromThis(val);
  return (*this)=*other;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::operator+(const MEDCalculatorDBField& other) const
{
  const MEDCalculatorDBField *other2=&other;
  const MEDCalculatorDBFieldReal *otherr=dynamic_cast<const MEDCalculatorDBFieldReal *>(other2);
  if(otherr)
    return add(*otherr);
  else
    {
      const MEDCalculatorDBFieldCst *otherc=dynamic_cast<const MEDCalculatorDBFieldCst *>(other2);
      if(otherc)
        {
          MCAuto<MEDCalculatorDBFieldReal> othercr=buildCstFieldFromThis(otherc->getValue());
          MEDCalculatorDBField *ret=add(*othercr);
          return ret;
        }
      else
        throw INTERP_KERNEL::Exception("FieldReal::operator+ : unrecognized type of parameter received !");
    }
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::add(const MEDCalculatorDBFieldReal& other) const
{
  checkConsistencyLight(other);
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  other.fetchData();
  DataArrayIdType *cellCor,*nodeCor;
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  std::vector<std::size_t> ids2=other._t.getIds(other._time_steps.size());
  if(ids.size()!=ids2.size())
    throw INTERP_KERNEL::Exception("FieldReal::add : Timesteps lengths mismatch !");
  int step=ids[0];
  int step2=ids2[0];
  const MEDCouplingMesh *mesh=_time_steps[step]->getMesh(_type,_file_name,_mesh_name,_field_name);
  const MEDCouplingMesh *otherm=other._time_steps[step2]->getMesh(_type,other._file_name,other._mesh_name,other._field_name);
  mesh->checkGeoEquivalWith(otherm,1,1e-12,cellCor,nodeCor);//1 for fast check
  int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->add(other._time_steps[ids2[i]],cellCor,nodeCor,_c_labels.size(),_c,other._c_labels.size(),other._c);
  int newNbOfCompo=_c.getSize(_c_labels.size());
  ret->_c_labels.resize(newNbOfCompo);
  if(cellCor)
    cellCor->decrRef();
  if(nodeCor)
    nodeCor->decrRef();
  ret->incrRef();
  return ret;
}

bool MEDCalculatorDBFieldReal::isEqual(const MEDCalculatorDBField& other, double precM, double precF) const
{
  const MEDCalculatorDBField *other2=&other;
  const MEDCalculatorDBFieldReal *otherr=dynamic_cast<const MEDCalculatorDBFieldReal *>(other2);
  if(otherr)
    return isEqualSameType(*otherr,precM,precF);
  else
    {
      const MEDCalculatorDBFieldCst *otherc=dynamic_cast<const MEDCalculatorDBFieldCst *>(other2);
      if(otherc)
        {
          MCAuto<MEDCalculatorDBFieldReal> othercr=buildCstFieldFromThis(otherc->getValue());
          bool ret=isEqualSameType(*othercr,precM,precF);
          return ret;
        }
      else
        throw INTERP_KERNEL::Exception("FieldReal::isEqual : unrecognized type of parameter received !");
    }
}

bool MEDCalculatorDBFieldReal::isEqualSameType(const MEDCalculatorDBFieldReal& other, double precM, double precF) const
{
  if(_description!=other._description)
    return false;
  fetchData();
  other.fetchData();
  DataArrayIdType *cellCor,*nodeCor;
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  std::vector<std::size_t> ids2=other._t.getIds(other._time_steps.size());
  if(ids.size()!=ids2.size())
    return false;
  int step=ids[0];
  int step2=ids2[0];
  const MEDCouplingMesh *mesh=_time_steps[step]->getMesh(_type,_file_name,_mesh_name,_field_name);
  const MEDCouplingMesh *otherm=other._time_steps[step2]->getMesh(_type,other._file_name,other._mesh_name,other._field_name);
  mesh->checkGeoEquivalWith(otherm,0,precM,cellCor,nodeCor);
  int sz=ids.size();
  for(int i=0;i<sz;i++)
    if(!_time_steps[ids[i]]->isEqual(other._time_steps[ids2[i]],cellCor,nodeCor,_c_labels.size(),_c,other._c_labels.size(),other._c,precF))
      return false;
  if(cellCor)
    cellCor->decrRef();
  if(nodeCor)
    nodeCor->decrRef();
  return true;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::operator-(const MEDCalculatorDBField& other) const
{
  const MEDCalculatorDBField *other2=&other;
  const MEDCalculatorDBFieldReal *otherr=dynamic_cast<const MEDCalculatorDBFieldReal *>(other2);
  if(otherr)
    return substract(*otherr);
  else
    {
      const MEDCalculatorDBFieldCst *otherc=dynamic_cast<const MEDCalculatorDBFieldCst *>(other2);
      if(otherc)
        {
          MCAuto<MEDCalculatorDBFieldReal> othercr=buildCstFieldFromThis(otherc->getValue());
          MEDCalculatorDBField *ret=substract(*othercr);
          return ret;
        }
      else
        throw INTERP_KERNEL::Exception("FieldReal::operator- : unrecognized type of parameter received !");
    }
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::substract(const MEDCalculatorDBFieldReal& other) const
{
  checkConsistencyLight(other);
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  other.fetchData();
  DataArrayIdType *cellCor,*nodeCor;
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  std::vector<std::size_t> ids2=other._t.getIds(other._time_steps.size());
  if(ids.size()!=ids2.size())
    throw INTERP_KERNEL::Exception("FieldReal::substract : Timesteps lengths mismatch !");
  int step=ids[0];
  int step2=ids2[0];
  const MEDCouplingMesh *mesh=_time_steps[step]->getMesh(_type,_file_name,_mesh_name,_field_name);
  const MEDCouplingMesh *otherm=other._time_steps[step2]->getMesh(_type,other._file_name,other._mesh_name,other._field_name);
  mesh->checkGeoEquivalWith(otherm,1,1e-12,cellCor,nodeCor);//1 for fast check
  int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->substract(other._time_steps[ids2[i]],cellCor,nodeCor,_c_labels.size(),_c,other._c_labels.size(),other._c);
  int newNbOfCompo=_c.getSize(_c_labels.size());
  ret->_c_labels.resize(newNbOfCompo);
  if(cellCor)
    cellCor->decrRef();
  if(nodeCor)
    nodeCor->decrRef();
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::operator*(const MEDCalculatorDBField& other) const
{
  const MEDCalculatorDBField *other2=&other;
  const MEDCalculatorDBFieldReal *otherr=dynamic_cast<const MEDCalculatorDBFieldReal *>(other2);
  if(otherr)
    return multiply(*otherr);
  else
    {
      const MEDCalculatorDBFieldCst *otherc=dynamic_cast<const MEDCalculatorDBFieldCst *>(other2);
      if(otherc)
        {
          MCAuto<MEDCalculatorDBFieldReal> othercr=buildCstFieldFromThis(otherc->getValue());
          MEDCalculatorDBField *ret=multiply(*othercr);
          return ret;
        }
      else
        throw INTERP_KERNEL::Exception("FieldReal::operator* : unrecognized type of parameter received !");
    }
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::multiply(const MEDCalculatorDBFieldReal& other) const
{
  checkConsistencyLight(other);
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  other.fetchData();
  DataArrayIdType *cellCor,*nodeCor;
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  std::vector<std::size_t> ids2=other._t.getIds(other._time_steps.size());
  if(ids.size()!=ids2.size())
    throw INTERP_KERNEL::Exception("FieldReal::multiply : Timesteps lengths mismatch !");
  int step=ids[0];
  int step2=ids2[0];
  const MEDCouplingMesh *mesh=_time_steps[step]->getMesh(_type,_file_name,_mesh_name,_field_name);
  const MEDCouplingMesh *otherm=other._time_steps[step2]->getMesh(_type,other._file_name,other._mesh_name,other._field_name);
  mesh->checkGeoEquivalWith(otherm,1,1e-12,cellCor,nodeCor);//1 for fast check
  int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->multiply(other._time_steps[ids2[i]],cellCor,nodeCor,_c_labels.size(),_c,other._c_labels.size(),other._c);
  int newNbOfCompo=_c.getSize(_c_labels.size());
  ret->_c_labels.resize(newNbOfCompo);
  if(cellCor)
    cellCor->decrRef();
  if(nodeCor)
    nodeCor->decrRef();
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::operator/(const MEDCalculatorDBField& other) const
{
  const MEDCalculatorDBField *other2=&other;
  const MEDCalculatorDBFieldReal *otherr=dynamic_cast<const MEDCalculatorDBFieldReal *>(other2);
  if(otherr)
    return divide(*otherr);
  else
    {
      const MEDCalculatorDBFieldCst *otherc=dynamic_cast<const MEDCalculatorDBFieldCst *>(other2);
      if(otherc)
        {
          MCAuto<MEDCalculatorDBFieldReal> othercr=buildCstFieldFromThis(otherc->getValue());
          MEDCalculatorDBField *ret=divide(*othercr);
          return ret;
        }
      else
        throw INTERP_KERNEL::Exception("FieldReal::operator/ : unrecognized type of parameter received !");
    }
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::divide(const MEDCalculatorDBFieldReal& other) const
{
  checkConsistencyLight(other);
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  other.fetchData();
  DataArrayIdType *cellCor,*nodeCor;
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  std::vector<std::size_t> ids2=other._t.getIds(other._time_steps.size());
  if(ids.size()!=ids2.size())
    throw INTERP_KERNEL::Exception("FieldReal::divide : Timesteps lengths mismatch !");
  int step=ids[0];
  int step2=ids2[0];
  const MEDCouplingMesh *mesh=_time_steps[step]->getMesh(_type,_file_name,_mesh_name,_field_name);
  const MEDCouplingMesh *otherm=other._time_steps[step2]->getMesh(_type,other._file_name,other._mesh_name,other._field_name);
  mesh->checkGeoEquivalWith(otherm,1,1e-12,cellCor,nodeCor);//1 for fast check
  int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->divide(other._time_steps[ids2[i]],cellCor,nodeCor,_c_labels.size(),_c,other._c_labels.size(),other._c);
  int newNbOfCompo=_c.getSize(_c_labels.size());
  ret->_c_labels.resize(newNbOfCompo);
  if(cellCor)
    cellCor->decrRef();
  if(nodeCor)
    nodeCor->decrRef();
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::operator^(const MEDCalculatorDBFieldReal& other) const
{
  return crossProduct(other);
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::dot(const MEDCalculatorDBFieldReal& other) const
{
  checkConsistencyLight(other);
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  other.fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  std::vector<std::size_t> ids2=other._t.getIds(other._time_steps.size());
  unsigned int sz=ids.size();
  if(sz!=ids2.size())
    throw INTERP_KERNEL::Exception("FieldReal::dot : Timesteps lengths mismatch !");
  ret->_time_steps.resize(sz);
  for(unsigned int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->dot(other._time_steps[ids2[i]],_c_labels.size(),_c,other._c_labels.size(),other._c);
  ret->_time_steps.resize(sz);
  ret->_c_labels.resize(1);
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::crossProduct(const MEDCalculatorDBFieldReal& other) const
{
  checkConsistencyLight(other);
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  other.fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  std::vector<std::size_t> ids2=other._t.getIds(other._time_steps.size());
  unsigned int sz=ids.size();
  if(sz!=ids2.size())
    throw INTERP_KERNEL::Exception("FieldReal::crossProduct : Timesteps lengths mismatch !");
  ret->_time_steps.resize(sz);
  for(unsigned int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->crossProduct(other._time_steps[ids2[i]],_c_labels.size(),_c,other._c_labels.size(),other._c);
  ret->_time_steps.resize(sz);
  ret->_c_labels.resize(3);
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::doublyContractedProduct() const
{
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  unsigned int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(unsigned int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->doublyContractedProduct(_c_labels.size(),_c);
  ret->_time_steps.resize(sz);
  ret->_c_labels.resize(1);
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::determinant() const
{
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  unsigned int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(unsigned int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->determinant(_c_labels.size(),_c);
  ret->_time_steps.resize(sz);
  ret->_c_labels.resize(1);
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::eigenValues() const
{
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  unsigned int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(unsigned int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->eigenValues(_c_labels.size(),_c);
  ret->_time_steps.resize(sz);
  if(sz!=0)
    {
      int ncomp=ret->_time_steps[0]->getFieldAttribute()->getNumberOfComponents();
      ret->_c_labels.resize(ncomp);
    }
  else
    ret->_c_labels.resize(0);
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::eigenVectors() const
{
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  unsigned int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(unsigned int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->eigenVectors(_c_labels.size(),_c);
  ret->_time_steps.resize(sz);
  if(sz!=0)
    {
      int ncomp=ret->_time_steps[0]->getFieldAttribute()->getNumberOfComponents();
      ret->_c_labels.resize(ncomp);
    }
  else
    ret->_c_labels.resize(0);
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::inverse() const
{
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  unsigned int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(unsigned int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->inverse(_c_labels.size(),_c);
  ret->_time_steps.resize(sz);
  if(sz!=0)
    {
      int ncomp=ret->_time_steps[0]->getFieldAttribute()->getNumberOfComponents();
      ret->_c_labels.resize(ncomp);
    }
  else
    ret->_c_labels.resize(0);
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::trace() const
{
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  unsigned int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(unsigned int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->trace(_c_labels.size(),_c);
  ret->_time_steps.resize(sz);
  ret->_c_labels.resize(1);
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::deviator() const
{
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  unsigned int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(unsigned int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->deviator(_c_labels.size(),_c);
  ret->_time_steps.resize(sz);
  if(sz!=0)
    {
      int ncomp=ret->_time_steps[0]->getFieldAttribute()->getNumberOfComponents();
      ret->_c_labels.resize(ncomp);
    }
  else
    ret->_c_labels.resize(0);
  ret->incrRef();
  return ret;
}

MEDCalculatorDBField *MEDCalculatorDBFieldReal::magnitude() const
{
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  unsigned int sz=ids.size();
  ret->_time_steps.resize(sz);
  for(unsigned int i=0;i<sz;i++)
    ret->_time_steps[i]=_time_steps[ids[i]]->magnitude(_c_labels.size(),_c);
  ret->_time_steps.resize(sz);
  ret->_c_labels.resize(1);
  ret->incrRef();
  return ret;
}

void MEDCalculatorDBFieldReal::applyFunc(const char *func)
{
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  for(std::vector<std::size_t>::const_iterator it=ids.begin();it!=ids.end();it++)
    _time_steps[*it]->applyFunc(func,_c_labels.size(),_c);
}

MEDCalculatorDBFieldReal *MEDCalculatorDBFieldReal::buildCstFieldFromThis(double val) const
{
  MCAuto<MEDCalculatorDBFieldReal> ret=new MEDCalculatorDBFieldReal(_type);
  ret->_p=_p;
  ret->_c_labels.resize(_c.getSize(_c_labels.size()));
  std::vector<std::size_t> stps=_t.getIds(_time_steps.size());
  int stepSize=stps.size();
  ret->_time_steps.resize(stepSize);
  if(stepSize==0)
    throw INTERP_KERNEL::Exception("MEDCalculatorDBFieldReal::buildCstFieldFromThis : no time steps defined !");
  for(int i=0;i<stepSize;i++)
    {
      const MEDCouplingFieldDouble *f=_time_steps[stps[i]]->getField(_type,_file_name,_mesh_name,_field_name);
      ret->_time_steps[i]=new MEDCalculatorDBSliceField(_time_steps[stps[i]]->buildCstFromThis(val,ret->_c_labels.size(),f));
    }
  ret->incrRef();
  return ret;
}

void MEDCalculatorDBFieldReal::checkConsistencyLight(const MEDCalculatorDBFieldReal& other) const
{
  if(_type!=other._type)
    throw INTERP_KERNEL::Exception("Types of field mismatch !");
  if(getNumberOfSteps()!=other.getNumberOfSteps())
    throw INTERP_KERNEL::Exception("Time steps mismatch !");
  if(getNumberOfComponents()!=other.getNumberOfComponents())
    throw INTERP_KERNEL::Exception("Components mismatch !");
}

void MEDCalculatorDBFieldReal::fetchData() const
{
  std::vector<std::pair<int,int> > idstoFetch;
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  int sz=ids.size();
  std::vector<std::size_t> idsInGlobalToFetch;
  for(int i=0;i<sz;i++)
    {
      MCAuto<MEDCalculatorDBSliceField> elt=_time_steps[ids[i]];
      if(!elt->isFetched())
        {
          int dt,it;
          elt->getDtIt(dt,it);
          std::pair<int,int> p(dt,it);
          idstoFetch.push_back(p);
          idsInGlobalToFetch.push_back(ids[i]);
        }
    }
  std::vector<MEDCouplingFieldDouble *> fs=ReadFieldsOnSameMesh(_type,_file_name.c_str(),_mesh_name.c_str(),0,_field_name.c_str(),idstoFetch);
  sz=fs.size();
  for(int i=0;i<sz;i++)
    {
      MCAuto<MEDCalculatorDBSliceField> elt=_time_steps[idsInGlobalToFetch[i]];
      elt->setField(fs[i]);
    }
}

int MEDCalculatorDBFieldReal::getNumberOfSteps() const
{
  return _t.getSize(_time_steps.size());
}

int MEDCalculatorDBFieldReal::getNumberOfFetchedSteps() const
{
  int ret=0;
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  for(std::vector<std::size_t>::const_iterator it=ids.begin();it!=ids.end();it++)
    if(_time_steps[*it]->isFetched())
      ret++;
  return ret;
}

int MEDCalculatorDBFieldReal::getNumberOfComponents() const
{
  return _c.getSize(_c_labels.size());
}

/*!
 * WARNING the caller has the ownership of all of instances in returned vector.
 */
std::vector<MEDCouplingFieldDouble *> MEDCalculatorDBFieldReal::getFields() const
{
  fetchData();
  std::vector<std::size_t> ids=_t.getIds(_time_steps.size());
  std::vector<MEDCouplingFieldDouble *> ret(ids.size());
  int i=0;
  for(std::vector<std::size_t>::const_iterator it=ids.begin();it!=ids.end();it++,i++)
    ret[i]=_time_steps[*it]->getFieldWithoutQuestion(_c_labels.size(),_c);
  return ret;
}

std::string MEDCalculatorDBFieldReal::getInfoOnComponent(int i) const
{
  if(i>=(int)_c_labels.size())
    throw INTERP_KERNEL::Exception("MEDCalculatorDBFieldReal::getInfoOnComponent : specified id >= number of component !");
  return _c_labels[i];
}

void MEDCalculatorDBFieldReal::setInfoOnComponent(int i, const char *info)
{
  if(i>=(int)_c_labels.size())
    throw INTERP_KERNEL::Exception("MEDCalculatorDBFieldReal::setInfoOnComponent : specified id >= number of component !");
  _c_labels[i]=info;
}

MEDCalculatorDBFieldCst::MEDCalculatorDBFieldCst(double val):_val(val)
{
}

MEDCalculatorDBField *MEDCalculatorDBFieldCst::operator+(const MEDCalculatorDBField& other) const
{
  const MEDCalculatorDBField *other2=&other;
  const MEDCalculatorDBFieldCst *otherc=dynamic_cast<const MEDCalculatorDBFieldCst *>(other2);
  if(otherc)
    {
      MCAuto<MEDCalculatorDBFieldCst> ret=new MEDCalculatorDBFieldCst(*this);
      ret->_val=_val+otherc->_val;
      ret->incrRef();
      return ret;
    }
  else
    {
      const MEDCalculatorDBFieldReal *otherr=dynamic_cast<const MEDCalculatorDBFieldReal *>(other2);
      if(otherr)
        {
          MCAuto<MEDCalculatorDBFieldReal> thisr=otherr->buildCstFieldFromThis(_val);
          MEDCalculatorDBField *ret=(*thisr)+other;
          return ret;
        }
      else
        throw INTERP_KERNEL::Exception("FieldCst::operator+ : unrecognized type of parameter received !");
    }
}

MEDCalculatorDBField *MEDCalculatorDBFieldCst::operator-(const MEDCalculatorDBField& other) const
{
  const MEDCalculatorDBField *other2=&other;
  const MEDCalculatorDBFieldCst *otherc=dynamic_cast<const MEDCalculatorDBFieldCst *>(other2);
  if(otherc)
    {
      MCAuto<MEDCalculatorDBFieldCst> ret=new MEDCalculatorDBFieldCst(*this);
      ret->_val=_val-otherc->_val;
      ret->incrRef();
      return ret;
    }
  else
    {
      const MEDCalculatorDBFieldReal *otherr=dynamic_cast<const MEDCalculatorDBFieldReal *>(other2);
      if(otherr)
        {
          MCAuto<MEDCalculatorDBFieldReal> thisr=otherr->buildCstFieldFromThis(_val);
          MEDCalculatorDBField *ret=(*thisr)-other;
          return ret;
        }
      else
        throw INTERP_KERNEL::Exception("FieldCst::operator- : unrecognized type of parameter received !");
    }
}

MEDCalculatorDBField *MEDCalculatorDBFieldCst::operator*(const MEDCalculatorDBField& other) const
{
  const MEDCalculatorDBField *other2=&other;
  const MEDCalculatorDBFieldCst *otherc=dynamic_cast<const MEDCalculatorDBFieldCst *>(other2);
  if(otherc)
    {
      MCAuto<MEDCalculatorDBFieldCst> ret=new MEDCalculatorDBFieldCst(*this);
      ret->_val=_val*otherc->_val;
      ret->incrRef();
      return ret;
    }
  else
    {
      const MEDCalculatorDBFieldReal *otherr=dynamic_cast<const MEDCalculatorDBFieldReal *>(other2);
      if(otherr)
        {
          MCAuto<MEDCalculatorDBFieldReal> thisr=otherr->buildCstFieldFromThis(_val);
          MEDCalculatorDBField *ret=(*thisr)*other;
          return ret;
        }
      else
        throw INTERP_KERNEL::Exception("FieldCst::operator* : unrecognized type of parameter received !");
    }
}

MEDCalculatorDBField *MEDCalculatorDBFieldCst::operator/(const MEDCalculatorDBField& other) const
{
  const MEDCalculatorDBField *other2=&other;
  const MEDCalculatorDBFieldCst *otherc=dynamic_cast<const MEDCalculatorDBFieldCst *>(other2);
  if(otherc)
    {
      MCAuto<MEDCalculatorDBFieldCst> ret=new MEDCalculatorDBFieldCst(*this);
      ret->_val=_val/otherc->_val;
      ret->incrRef();
      return ret;
    }
  else
    {
      const MEDCalculatorDBFieldReal *otherr=dynamic_cast<const MEDCalculatorDBFieldReal *>(other2);
      if(otherr)
        {
          MCAuto<MEDCalculatorDBFieldReal> thisr=otherr->buildCstFieldFromThis(_val);
          MEDCalculatorDBField *ret=(*thisr)/other;
          return ret;
        }
      else
        throw INTERP_KERNEL::Exception("FieldCst::operator/ : unrecognized type of parameter received !");
    }
}

bool MEDCalculatorDBFieldCst::isEqual(const MEDCalculatorDBField& other, double precM, double precF) const
{
  const MEDCalculatorDBField *other2=&other;
  const MEDCalculatorDBFieldCst *otherc=dynamic_cast<const MEDCalculatorDBFieldCst *>(other2);
  if(otherc)
    return fabs(otherc->_val-_val)<precF;
  else
    {
      const MEDCalculatorDBFieldReal *otherr=dynamic_cast<const MEDCalculatorDBFieldReal *>(other2);
      if(otherr)
        {
          MCAuto<MEDCalculatorDBFieldReal> thisr=otherr->buildCstFieldFromThis(_val);
          bool ret=thisr->isEqual(other,precM,precF);
          return ret;
        }
      else
        throw INTERP_KERNEL::Exception("FieldCst::isEqual : unrecognized type of parameter received !");
    }
}
