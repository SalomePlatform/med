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

#include "MEDCalculatorDBSliceField.hxx"
#include "MEDCalculatorDBRangeSelection.hxx"

#include "MEDLoader.hxx"

#include "MEDCouplingFieldDouble.hxx"
#include "MCAuto.hxx"

using namespace MEDCoupling;

MEDCalculatorDBSliceField::MEDCalculatorDBSliceField(int iter, int order):_iteration(iter),_order(order),_field(0),_work(0)
{
}

MEDCalculatorDBSliceField::MEDCalculatorDBSliceField(MEDCouplingFieldDouble *f):_field(f),_work(0)
{
}

std::size_t MEDCalculatorDBSliceField::getHeapMemorySizeWithoutChildren() const
{
  return 0;
}

std::vector<const BigMemoryObject *> MEDCalculatorDBSliceField::getDirectChildrenWithNull() const
{
  return std::vector<const BigMemoryObject *>();
}

void MEDCalculatorDBSliceField::setField(MEDCouplingFieldDouble *f) const
{
  if(_field!=f)
    {
      if(_field)
        _field->decrRef();
      _field=f;
    }
}

void MEDCalculatorDBSliceField::setName(const char *name)
{
  _field->setName(name);
}

void MEDCalculatorDBSliceField::setDescription(const char *descr)
{
  _field->setDescription(descr);
}

void MEDCalculatorDBSliceField::write(const char *fName, const std::string& n, const std::string& d) const
{
  std::string kn=_field->getName();
  std::string kd=_field->getDescription();
  MEDCouplingFieldDouble *myF=const_cast<MEDCouplingFieldDouble *>(_field);
  myF->setName(n.c_str());
  myF->setDescription(d.c_str());
  WriteFieldUsingAlreadyWrittenMesh(fName,_field);
  myF->setName(kn.c_str());
  myF->setDescription(kd.c_str());
}

const MEDCouplingMesh *MEDCalculatorDBSliceField::getMesh(TypeOfField type, const std::string& fname, const std::string& mname, const std::string& fieldName) const
{
  MEDCouplingFieldDouble *f=getField(type,fname,mname,fieldName);
  return f->getMesh();
}

MEDCalculatorDBSliceField::~MEDCalculatorDBSliceField()
{
  if(_field)
    _field->decrRef();
  if(_work)
    _work->decrRef();
}

MEDCouplingFieldDouble *MEDCalculatorDBSliceField::getField(TypeOfField type, const std::string& fname, const std::string& mname, const std::string& fieldName) const
{
  if(!_field)
    {
      MCAuto<MEDCouplingField> tmpp(ReadField(type,fname.c_str(),mname.c_str(),0,fieldName.c_str(),_iteration,_order));
      MCAuto<MEDCouplingFieldDouble> tmp(DynamicCast<MEDCouplingField,MEDCouplingFieldDouble>(tmpp));
      _field=tmp.retn();
    }
  return _field;
}

/*!
 * _field is expected to be already loaded !
 */
MEDCouplingFieldDouble *MEDCalculatorDBSliceField::getFieldWithoutQuestion(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  return _field->keepSelectedComponents(tIds);
}

MEDCouplingFieldDouble *MEDCalculatorDBSliceField::buildCstFromThis(double val, int nbOfComp, const MEDCouplingFieldDouble *f) const
{
  MEDCouplingFieldDouble *ret=MEDCouplingFieldDouble::New(f->getTypeOfField(),ONE_TIME);
  ret->setMesh(f->getMesh());
  ret->applyFunc(nbOfComp,val);
  ret->copyTinyAttrFrom(f);
  return ret;
}

void MEDCalculatorDBSliceField::assign(const MEDCalculatorDBSliceField* other, int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                    int sizeCOther, const MEDCalculatorDBRangeSelection& otherC)
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  std::vector<std::size_t> oIds=otherC.getIds(sizeCOther);
  MCAuto<MEDCouplingFieldDouble> f1=other->_field->keepSelectedComponents(oIds);
  _field->setSelectedComponents(f1,tIds);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::add(const MEDCalculatorDBSliceField* other, const DataArrayIdType *cc, const DataArrayIdType *nc,
                                                    int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                                    int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const
{
  if(cc!=0 || nc!=0)
    throw INTERP_KERNEL::Exception("Slice::add : not implemented yet node/cell permutation !");
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  std::vector<std::size_t> oIds=otherC.getIds(sizeCOther);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MCAuto<MEDCouplingFieldDouble> f2=other->_field->keepSelectedComponents(oIds);
  f2->setMesh(f1->getMesh());
  MEDCouplingFieldDouble *f3=(*f1)+(*f2);
  return new MEDCalculatorDBSliceField(f3);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::substract(const MEDCalculatorDBSliceField* other, const DataArrayIdType *cc, const DataArrayIdType *nc,
                                                          int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                                          int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const
{
  if(cc!=0 || nc!=0)
    throw INTERP_KERNEL::Exception("Slice::substract : not implemented yet node/cell permutation !");
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  std::vector<std::size_t> oIds=otherC.getIds(sizeCOther);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MCAuto<MEDCouplingFieldDouble> f2=other->_field->keepSelectedComponents(oIds);
  f2->setMesh(f1->getMesh());
  MEDCouplingFieldDouble *f3=(*f1)-(*f2);
  return new MEDCalculatorDBSliceField(f3);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::multiply(const MEDCalculatorDBSliceField* other, const DataArrayIdType *cc, const DataArrayIdType *nc,
                                                         int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                                         int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const
{
  if(cc!=0 || nc!=0)
    throw INTERP_KERNEL::Exception("Slice::multiply : not implemented yet node/cell permutation !");
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  std::vector<std::size_t> oIds=otherC.getIds(sizeCOther);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MCAuto<MEDCouplingFieldDouble> f2=other->_field->keepSelectedComponents(oIds);
  f2->setMesh(f1->getMesh());
  MEDCouplingFieldDouble *f3=(*f1)*(*f2);
  return new MEDCalculatorDBSliceField(f3);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::divide(const MEDCalculatorDBSliceField* other, const DataArrayIdType *cc, const DataArrayIdType *nc,
                                                       int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                                       int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const
{
  if(cc!=0 || nc!=0)
    throw INTERP_KERNEL::Exception("Slice::divide : not implemented yet node/cell permutation !");
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  std::vector<std::size_t> oIds=otherC.getIds(sizeCOther);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MCAuto<MEDCouplingFieldDouble> f2=other->_field->keepSelectedComponents(oIds);
  f2->setMesh(f1->getMesh());
  MEDCouplingFieldDouble *f3=(*f1)/(*f2);
  return new MEDCalculatorDBSliceField(f3);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::dot(const MEDCalculatorDBSliceField* other, int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                                    int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  std::vector<std::size_t> oIds=otherC.getIds(sizeCOther);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MCAuto<MEDCouplingFieldDouble> f2=other->_field->keepSelectedComponents(oIds);
  f2->setMesh(f1->getMesh());
  MEDCouplingFieldDouble *f3=f1->dot(*f2);
  return new MEDCalculatorDBSliceField(f3);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::crossProduct(const MEDCalculatorDBSliceField* other, int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                                             int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  std::vector<std::size_t> oIds=otherC.getIds(sizeCOther);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MCAuto<MEDCouplingFieldDouble> f2=other->_field->keepSelectedComponents(oIds);
  f2->setMesh(f1->getMesh());
  MEDCouplingFieldDouble *f3=f1->crossProduct(*f2);
  return new MEDCalculatorDBSliceField(f3);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::doublyContractedProduct(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MEDCouplingFieldDouble *f2=f1->doublyContractedProduct();
  return new MEDCalculatorDBSliceField(f2);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::determinant(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MEDCouplingFieldDouble *f2=f1->determinant();
  return new MEDCalculatorDBSliceField(f2);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::eigenValues(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MEDCouplingFieldDouble *f2=f1->eigenValues();
  return new MEDCalculatorDBSliceField(f2);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::eigenVectors(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MEDCouplingFieldDouble *f2=f1->eigenVectors();
  return new MEDCalculatorDBSliceField(f2);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::inverse(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MEDCouplingFieldDouble *f2=f1->inverse();
  return new MEDCalculatorDBSliceField(f2);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::trace(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MEDCouplingFieldDouble *f2=f1->trace();
  return new MEDCalculatorDBSliceField(f2);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::deviator(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MEDCouplingFieldDouble *f2=f1->deviator();
  return new MEDCalculatorDBSliceField(f2);
}

MEDCalculatorDBSliceField *MEDCalculatorDBSliceField::magnitude(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MEDCouplingFieldDouble *f2=f1->magnitude();
  return new MEDCalculatorDBSliceField(f2);
}

void MEDCalculatorDBSliceField::applyFunc(const char *func, int sizeCThis, const MEDCalculatorDBRangeSelection& thisC)
{
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  f1->applyFunc(func);
  _field->setSelectedComponents(f1,tIds);
}

bool MEDCalculatorDBSliceField::isEqual(const MEDCalculatorDBSliceField* other, const DataArrayIdType *cc, const DataArrayIdType *nc,
                                     int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                     int sizeCOther, const MEDCalculatorDBRangeSelection& otherC, double prec) const
{
  if(cc!=0 || nc!=0)
    throw INTERP_KERNEL::Exception("Slice::isEqual : not implemented yet node/cell permutation !");
  std::vector<std::size_t> tIds=thisC.getIds(sizeCThis);
  std::vector<std::size_t> oIds=otherC.getIds(sizeCOther);
  MCAuto<MEDCouplingFieldDouble> f1=_field->keepSelectedComponents(tIds);
  MCAuto<MEDCouplingFieldDouble> f2=other->_field->keepSelectedComponents(oIds);
  f2->setMesh(f1->getMesh());
  return f1->isEqualWithoutConsideringStr(f2,0,prec);
}
