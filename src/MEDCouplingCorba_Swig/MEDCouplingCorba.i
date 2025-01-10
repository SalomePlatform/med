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

%module MEDCouplingCorba

%{
#include "MEDCouplingFieldDoubleServant.hxx"  
#include "MEDCouplingFieldTemplateServant.hxx"
#include "MEDCouplingMultiFieldsServant.hxx"
#include "MEDCouplingFieldOverTimeServant.hxx"
#include "MEDCouplingUMeshServant.hxx"
#include "MEDCoupling1SGTUMeshServant.hxx"
#include "MEDCoupling1DGTUMeshServant.hxx"
#include "MEDCouplingExtrudedMeshServant.hxx"
#include "MEDCouplingCMeshServant.hxx"
#include "MEDCouplingIMeshServant.hxx"
#include "MEDCouplingCurveLinearMeshServant.hxx"
#include "DataArrayDoubleServant.hxx"
#include "DataArrayIntServant.hxx"
#include "MEDCouplingCorbaTypemaps.i"

using namespace MEDCoupling;
%}

%include "MEDCouplingCommon.i"

namespace MEDCoupling
{
  class MEDCouplingFieldDoubleServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingFieldDouble *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingFieldDoubleServant>(cppPointerOfMesh);
         }
         
         static PyObject *_this2(const MEDCouplingFieldDouble *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingFieldDoubleServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingFieldTemplateServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingFieldTemplate *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingFieldTemplateServant>(cppPointerOfMesh);
         }
         
         static PyObject *_this2(const MEDCouplingFieldTemplate *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingFieldTemplateServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingMultiFieldsServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingMultiFields *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingMultiFieldsServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCouplingMultiFields *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingMultiFieldsServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingFieldOverTimeServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingFieldOverTime *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingFieldOverTimeServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCouplingFieldOverTime *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingFieldOverTimeServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           if(!cppPointerOfMesh)
             throw INTERP_KERNEL::Exception("MEDCouplingMeshServant::_this : input pointer is NULL !");
           if(dynamic_cast<const MEDCouplingUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCouplingUMeshServant>(dynamic_cast<const MEDCouplingUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCoupling1SGTUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCoupling1SGTUMeshServant>(dynamic_cast<const MEDCoupling1SGTUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCoupling1DGTUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCoupling1DGTUMeshServant>(dynamic_cast<const MEDCoupling1DGTUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingMappedExtrudedMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCouplingExtrudedMeshServant>(dynamic_cast<const MEDCouplingMappedExtrudedMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingCMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCouplingCMeshServant>(dynamic_cast<const MEDCouplingCMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingIMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCouplingIMeshServant>(dynamic_cast<const MEDCouplingIMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingCurveLinearMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCouplingCurveLinearMeshServant>(dynamic_cast<const MEDCouplingCurveLinearMesh *>(cppPointerOfMesh));
           throw INTERP_KERNEL::Exception("MEDCouplingMeshServant::_this : unrecognized type (or not managed type) of Mesh in input !");
         }

         static PyObject *_this2(const MEDCouplingMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           if(!cppPointerOfMesh)
             throw INTERP_KERNEL::Exception("MEDCouplingMeshServant::_this2 : input pointer is NULL !");
           if(dynamic_cast<const MEDCouplingUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCouplingUMeshServant>(dynamic_cast<const MEDCouplingUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCoupling1SGTUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCoupling1SGTUMeshServant>(dynamic_cast<const MEDCoupling1SGTUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCoupling1DGTUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCoupling1DGTUMeshServant>(dynamic_cast<const MEDCoupling1DGTUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingMappedExtrudedMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCouplingExtrudedMeshServant>(dynamic_cast<const MEDCouplingMappedExtrudedMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingCMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCouplingCMeshServant>(dynamic_cast<const MEDCouplingCMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingIMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCouplingIMeshServant>(dynamic_cast<const MEDCouplingIMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingCurveLinearMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCouplingCurveLinearMeshServant>(dynamic_cast<const MEDCouplingCurveLinearMesh *>(cppPointerOfMesh));
           throw INTERP_KERNEL::Exception("MEDCouplingMeshServant::_this2 : unrecognized type (or not managed type) of Mesh in input !");
         }
       }
  };

  class MEDCouplingUMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingUMeshServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCouplingUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingUMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCoupling1SGTUMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCoupling1SGTUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCoupling1SGTUMeshServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCoupling1SGTUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCoupling1SGTUMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCoupling1DGTUMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCoupling1DGTUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCoupling1DGTUMeshServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCoupling1DGTUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCoupling1DGTUMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingExtrudedMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingMappedExtrudedMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingExtrudedMeshServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCouplingMappedExtrudedMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingExtrudedMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingCMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingCMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingCMeshServant>(cppPointerOfMesh);
         }
         
         static PyObject *_this2(const MEDCouplingCMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingCMeshServant>(cppPointerOfMesh);
         }
       }
  };
  
  class MEDCouplingIMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingIMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingIMeshServant>(cppPointerOfMesh);
         }
         
         static PyObject *_this2(const MEDCouplingIMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingIMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingCurveLinearMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingCurveLinearMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingCurveLinearMeshServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCouplingCurveLinearMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingCurveLinearMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class DataArrayDoubleServant
  {
  public:
    %extend
       {
         static PyObject *_this(const DataArrayDouble *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<DataArrayDoubleServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const DataArrayDouble *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<DataArrayDoubleServant>(cppPointerOfMesh);
         }
       }
  };

  class DataArrayIntServant
  {
  public:
    %extend
       {
         static PyObject *_this(const DataArrayInt *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<DataArrayIntServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const DataArrayInt *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<DataArrayIntServant>(cppPointerOfMesh);
         }
       }
  };
}

%pythoncode %{
def MEDCouplingDataArrayDoubleIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDouble____iadd___(self, self, *args)
def MEDCouplingDataArrayDoubleIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDouble____isub___(self, self, *args)
def MEDCouplingDataArrayDoubleImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDouble____imul___(self, self, *args)
def MEDCouplingDataArrayDoubleIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDouble____idiv___(self, self, *args)
def MEDCouplingDataArrayDoubleIpow(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDouble____ipow___(self, self, *args)
def MEDCouplingFieldDoubleIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.MEDCouplingFieldDouble____iadd___(self, self, *args)
def MEDCouplingFieldDoubleIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.MEDCouplingFieldDouble____isub___(self, self, *args)
def MEDCouplingFieldDoubleImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.MEDCouplingFieldDouble____imul___(self, self, *args)
def MEDCouplingFieldDoubleIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.MEDCouplingFieldDouble____idiv___(self, self, *args)
def MEDCouplingFieldDoubleIpow(self,*args):
    import _MEDCoupling
    return _MEDCoupling.MEDCouplingFieldDouble____ipow___(self, self, *args)
def MEDCouplingDataArrayInt32Iadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____iadd___(self, self, *args)
def MEDCouplingDataArrayInt32Isub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____isub___(self, self, *args)
def MEDCouplingDataArrayInt32Imul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____imul___(self, self, *args)
def MEDCouplingDataArrayInt32Idiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____idiv___(self, self, *args)
def MEDCouplingDataArrayInt32Imod(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____imod___(self, self, *args)
def MEDCouplingDataArrayInt32Ipow(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32____ipow___(self, self, *args)
def MEDCouplingDataArrayInt64Iadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____iadd___(self, self, *args)
def MEDCouplingDataArrayInt64Isub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____isub___(self, self, *args)
def MEDCouplingDataArrayInt64Imul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____imul___(self, self, *args)
def MEDCouplingDataArrayInt64Idiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____idiv___(self, self, *args)
def MEDCouplingDataArrayInt64Imod(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____imod___(self, self, *args)
def MEDCouplingDataArrayInt64Ipow(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64____ipow___(self, self, *args)
def MEDCouplingDataArrayFloatIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayFloat____iadd___(self, self, *args)
def MEDCouplingDataArrayFloatIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayFloat____isub___(self, self, *args)
def MEDCouplingDataArrayFloatImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayFloat____imul___(self, self, *args)
def MEDCouplingDataArrayFloatIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayFloat____idiv___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDoubleTuple____iadd___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDoubleTuple____isub___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDoubleTuple____imul___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayDoubleTuple____idiv___(self, self, *args)
def MEDCouplingDataArrayInt32TupleIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32Tuple____iadd___(self, self, *args)
def MEDCouplingDataArrayInt32TupleIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32Tuple____isub___(self, self, *args)
def MEDCouplingDataArrayInt32TupleImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32Tuple____imul___(self, self, *args)
def MEDCouplingDataArrayInt32TupleIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32Tuple____idiv___(self, self, *args)
def MEDCouplingDataArrayInt32TupleImod(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt32Tuple____imod___(self, self, *args)
def MEDCouplingDataArrayInt64TupleIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64Tuple____iadd___(self, self, *args)
def MEDCouplingDataArrayInt64TupleIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64Tuple____isub___(self, self, *args)
def MEDCouplingDataArrayInt64TupleImul(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64Tuple____imul___(self, self, *args)
def MEDCouplingDataArrayInt64TupleIdiv(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64Tuple____idiv___(self, self, *args)
def MEDCouplingDataArrayInt64TupleImod(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DataArrayInt64Tuple____imod___(self, self, *args)
def MEDCouplingDenseMatrixIadd(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DenseMatrix____iadd___(self, self, *args)
def MEDCouplingDenseMatrixIsub(self,*args):
    import _MEDCoupling
    return _MEDCoupling.DenseMatrix____isub___(self, self, *args)
%}

%include "MEDCouplingFinalize.i"
