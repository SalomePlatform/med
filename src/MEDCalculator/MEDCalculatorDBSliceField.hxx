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

#ifndef __MEDCALCULATORDBSLICEFIELD_HXX__
#define __MEDCALCULATORDBSLICEFIELD_HXX__

#include "MedCalculatorDefines.hxx"
#include "MCType.hxx"
#include "MEDCouplingRefCountObject.hxx"

#include "InterpKernelException.hxx"

#include <string>

namespace MEDCoupling
{
  class MEDCouplingFieldDouble;
  class MEDCouplingMesh;
  class DataArrayInt;
  class MEDCalculatorDBRangeSelection;
  
  class MEDCALCULATOR_EXPORT MEDCalculatorDBSliceField : public RefCountObject
  {
  public:
    MEDCalculatorDBSliceField(int iter, int order);
    MEDCalculatorDBSliceField(MEDCouplingFieldDouble *f);
    std::size_t getHeapMemorySizeWithoutChildren() const;
    std::vector<const BigMemoryObject *> getDirectChildrenWithNull() const;
    bool isFetched() const { return _field!=0; }
    void getDtIt(int& it, int& order) const { it=_iteration;  order=_order; }
    void setField(MEDCouplingFieldDouble *f) const;
    void setName(const char *name);
    void setDescription(const char *descr);
    void write(const char *fName, const std::string& n, const std::string& d) const;
    const MEDCouplingMesh *getMesh(TypeOfField type, const std::string& fname, const std::string& mname, const std::string& fieldName) const;
    MEDCouplingFieldDouble *getField(TypeOfField type, const std::string& fname, const std::string& mname, const std::string& fieldName) const;
    MEDCouplingFieldDouble *getFieldWithoutQuestion(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const;
    MEDCouplingFieldDouble *getFieldAttribute() const { return _field; }
    MEDCouplingFieldDouble *buildCstFromThis(double val, int nbOfComp, const MEDCouplingFieldDouble *m) const;
    //
    void assign(const MEDCalculatorDBSliceField* other, int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                int sizeCOther, const MEDCalculatorDBRangeSelection& otherC);
    MEDCalculatorDBSliceField *add(const MEDCalculatorDBSliceField* other, const DataArrayIdType *cc, const DataArrayIdType *nc,
                                int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const;
    MEDCalculatorDBSliceField *substract(const MEDCalculatorDBSliceField* other, const DataArrayIdType *cc, const DataArrayIdType *nc,
                                      int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                      int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const;
    MEDCalculatorDBSliceField *multiply(const MEDCalculatorDBSliceField* other, const DataArrayIdType *cc, const DataArrayIdType *nc,
                                     int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                     int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const;
    MEDCalculatorDBSliceField *divide(const MEDCalculatorDBSliceField* other, const DataArrayIdType *cc, const DataArrayIdType *nc,
                                   int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                   int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const;
    MEDCalculatorDBSliceField *dot(const MEDCalculatorDBSliceField* other,
                                int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const;
    MEDCalculatorDBSliceField *crossProduct(const MEDCalculatorDBSliceField* other,
                                         int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                                         int sizeCOther, const MEDCalculatorDBRangeSelection& otherC) const;
    MEDCalculatorDBSliceField *doublyContractedProduct(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const;
    MEDCalculatorDBSliceField *determinant(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const;
    MEDCalculatorDBSliceField *eigenValues(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const;
    MEDCalculatorDBSliceField *eigenVectors(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const;
    MEDCalculatorDBSliceField *inverse(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const;
    MEDCalculatorDBSliceField *trace(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const;
    MEDCalculatorDBSliceField *deviator(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const;
    MEDCalculatorDBSliceField *magnitude(int sizeCThis, const MEDCalculatorDBRangeSelection& thisC) const;
    void applyFunc(const char *func, int sizeCThis, const MEDCalculatorDBRangeSelection& thisC);
    bool isEqual(const MEDCalculatorDBSliceField* other, const DataArrayIdType *cc, const DataArrayIdType *nc,
                 int sizeCThis, const MEDCalculatorDBRangeSelection& thisC,
                 int sizeCOther, const MEDCalculatorDBRangeSelection& otherC, double prec) const;
  private:
    ~MEDCalculatorDBSliceField();
  private:
    int _iteration;
    int _order;
    mutable MEDCouplingFieldDouble *_field;
    MEDCouplingFieldDouble *_work;
  };
}

#endif
