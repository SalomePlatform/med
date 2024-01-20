// Copyright (C) 2007-2024  CEA, EDF
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

#ifndef __MEDCALCULATORDBFIELD_HXX__
#define __MEDCALCULATORDBFIELD_HXX__

#include "MedCalculatorDefines.hxx"
#include "MEDCalculatorDBSliceField.hxx"
#include "MEDCalculatorDBRangeSelection.hxx"

#include "MEDCouplingRefCountObject.hxx"
#include "MCAuto.hxx"

#include "InterpKernelException.hxx"

#include <string>
#include <vector>

namespace MEDCoupling
{
  class MEDCalculatorBrowserField;
  class MEDCalculatorDBFieldReal;
  class MEDCalculatorDBFieldCst;


  class MEDCALCULATOR_EXPORT MEDCalculatorDBField : public RefCountObject
  {
  public:
    static MEDCalculatorDBFieldReal *New(const MEDCalculatorBrowserField& ls);
    static MEDCalculatorDBFieldCst *New(double val);
    std::size_t getHeapMemorySizeWithoutChildren() const;
    std::vector<const BigMemoryObject *> getDirectChildrenWithNull() const;
    virtual MEDCalculatorDBField *operator+(const MEDCalculatorDBField& other) const = 0;
    MEDCalculatorDBField *operator+(double val) const;
    virtual MEDCalculatorDBField *operator-(const MEDCalculatorDBField& other) const = 0;
    MEDCalculatorDBField *operator-(double val) const;
    virtual MEDCalculatorDBField *operator*(const MEDCalculatorDBField& other) const = 0;
    MEDCalculatorDBField *operator*(double val) const;
    virtual MEDCalculatorDBField *operator/(const MEDCalculatorDBField& other) const = 0;
    MEDCalculatorDBField *operator/(double val) const;
    virtual bool isEqual(const MEDCalculatorDBField& other, double precM, double precF) const = 0;
  };

  class MEDCALCULATOR_EXPORT MEDCalculatorDBFieldReal : public MEDCalculatorDBField
  {
  public:
    MEDCalculatorDBFieldReal(const MEDCalculatorBrowserField& ls);
    ~MEDCalculatorDBFieldReal();
    void setName(const char *name);
    void setDescription(const char *descr);
    void write(const char *fName, bool writeFromScratch) const;
    void display() const;
    std::string simpleRepr() const;
    MEDCalculatorDBFieldReal *operator()(const MEDCalculatorDBRangeSelection& t, const MEDCalculatorDBRangeSelection& p, const MEDCalculatorDBRangeSelection& c);
    const MEDCalculatorDBFieldReal& operator=(const MEDCalculatorDBFieldReal& other);
    const MEDCalculatorDBFieldReal& operator=(double val);
    MEDCalculatorDBField *operator+(const MEDCalculatorDBField& other) const;
    MEDCalculatorDBField *add(const MEDCalculatorDBFieldReal& other) const;
    MEDCalculatorDBField *operator-(const MEDCalculatorDBField& other) const;
    MEDCalculatorDBField *substract(const MEDCalculatorDBFieldReal& other) const;
    MEDCalculatorDBField *operator*(const MEDCalculatorDBField& other) const;
    MEDCalculatorDBField *multiply(const MEDCalculatorDBFieldReal& other) const;
    MEDCalculatorDBField *operator/(const MEDCalculatorDBField& other) const;
    MEDCalculatorDBField *divide(const MEDCalculatorDBFieldReal& other) const;
    MEDCalculatorDBField *operator^(const MEDCalculatorDBFieldReal& other) const;
    MEDCalculatorDBField *dot(const MEDCalculatorDBFieldReal& other) const;
    MEDCalculatorDBField *crossProduct(const MEDCalculatorDBFieldReal& other) const;
    MEDCalculatorDBField *doublyContractedProduct() const;
    MEDCalculatorDBField *determinant() const;
    MEDCalculatorDBField *eigenValues() const;
    MEDCalculatorDBField *eigenVectors() const;
    MEDCalculatorDBField *inverse() const;
    MEDCalculatorDBField *trace() const;
    MEDCalculatorDBField *deviator() const;
    MEDCalculatorDBField *magnitude() const;
    void applyFunc(const char *func);
    bool isEqual(const MEDCalculatorDBField& other, double precM, double precF) const;
    bool isEqualSameType(const MEDCalculatorDBFieldReal& other, double precM, double precF) const;
    MEDCalculatorDBFieldReal *buildCstFieldFromThis(double val) const;
    void checkConsistencyLight(const MEDCalculatorDBFieldReal& other) const;
    void fetchData() const;
    TypeOfField getType() const { return _type; }
    int getNumberOfSteps() const;
    int getNumberOfFetchedSteps() const;
    int getNumberOfComponents() const;
    std::vector<MEDCouplingFieldDouble *> getFields() const;
    std::string getInfoOnComponent(int i) const;
    void setInfoOnComponent(int i, const char *info);
  private:
    MEDCalculatorDBFieldReal(TypeOfField type);
  private:
    std::string _name;
    std::string _description;
    //
    std::string _file_name;
    std::string _mesh_name;
    std::string _field_name;
    TypeOfField _type;
    MEDCalculatorDBRangeSelection _t;
    MEDCalculatorDBRangeSelection _p;
    std::vector<std::string> _c_labels;
    MEDCalculatorDBRangeSelection _c;
    std::vector< MCAuto<MEDCalculatorDBSliceField> > _time_steps;
  };

  class MEDCALCULATOR_EXPORT MEDCalculatorDBFieldCst : public MEDCalculatorDBField
  {
  public:
    MEDCalculatorDBFieldCst(double val);
    MEDCalculatorDBField *operator+(const MEDCalculatorDBField& other) const;
    MEDCalculatorDBField *operator-(const MEDCalculatorDBField& other) const;
    MEDCalculatorDBField *operator*(const MEDCalculatorDBField& other) const;
    MEDCalculatorDBField *operator/(const MEDCalculatorDBField& other) const;
    bool isEqual(const MEDCalculatorDBField& other, double precM, double precF) const;
    MEDCalculatorDBFieldReal *convertIntoReal(const MEDCalculatorDBFieldReal& ref) const;
    double getValue() const { return _val; }
  private:
    double _val;
  };
}

#endif
