// Copyright (C) 2011-2023  CEA, EDF
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

// Authors : Guillaume Boulant (EDF) - 01/06/2011

#include "MEDCalculator_i.hxx"
#include "SALOME_KernelServices.hxx"
#include "Basics_Utils.hxx"
#include <string>

#include "MEDCouplingFieldDouble.hxx"
using namespace MEDCoupling;

MEDCalculator_i * MEDCalculator_i::_instance = NULL;

MEDCalculator_i * MEDCalculator_i::getInstance() {
  if ( _instance == NULL ) {
    _instance = new MEDCalculator_i();
  }
  return _instance;
}

MEDCalculator_i::MEDCalculator_i()
{
  LOG("Creating a MEDCalculator_i instance");
  _medDataManager = MEDDataManager_i::getInstance();
}

MEDCalculator_i::~MEDCalculator_i()
{
  LOG("Deleting MEDCalculator_i instance");
}

/*!
 * This function realizes the addition of the MEDCoupling field
 * objects identified by the FieldHandler objects passed in arguments.
 *
 * It returns a FieldHandler that identifies the resulting MEDCoupling
 * field object. The resulting MEDCoupling field object is physically
 * in the SALOME MED container and is automatically registered in the
 * MEDDataManager.
 */
MEDCALC::FieldHandler * MEDCalculator_i::add(const MEDCALC::FieldHandler & f1_hdl,
             const MEDCALC::FieldHandler & f2_hdl)
{
  // We first check that both operands share the same mesh id. Note
  // that it's not strictly required because the MEDCoupling operation
  // would raise an exception if the fields does not share the same
  // mesh support.
  if ( f1_hdl.meshid != f2_hdl.meshid ) {
    std::string message =
      std::string("ERROR: Mesh ids are different for the field operands ") +
      std::string(f1_hdl.fieldname) + std::string(" and ") + std::string(f2_hdl.fieldname);
    throw KERNEL::createSalomeException(message.c_str());
  }

  MEDCouplingFieldDouble* f1 = _medDataManager->getFieldDouble(&f1_hdl);
  MEDCouplingFieldDouble* f2 = _medDataManager->getFieldDouble(&f2_hdl);

  MEDCouplingFieldDouble* field_result;
  // This operation could raise an INTERP_KERNEL exception, for
  // example, if the fields are not defined on the same support.
  try {
    field_result = (*f1) + (*f2);
  }
  catch (INTERP_KERNEL::Exception &ex) {
    throw KERNEL::createSalomeException(ex.what());
  }

  std::string name = std::string(f1->getName()) + "+" + std::string(f2->getName());
  field_result->setName(name.c_str());

  //
  // The field must be registered in the dataManager structure so
  // that it could be reused in a future operation (in particular for
  // a manipulation that combine several operation as "r = f1+f2+f3",
  // the operator will try to make the addition f1+f2 and then
  // resultof(f1+f2) + f3. This last operation will fail if the field
  // resultof(f1+f2) is not in the dataManager structure).
  //
  // Note that we choose arbitrary the first field operande to get the
  // meshid (both have the same meshid).
  //
  long meshHandlerId = f1_hdl.meshid;
  MEDCALC::FieldHandler * fieldResultHandler = _medDataManager->addField(field_result,
                       meshHandlerId);

  // >>>>>>>>>
  // _GBO_ We should test here if the iteration and order of the input
  // files are identical for both operands. A convention has to be
  // defined here. By default, we let the iteration and order be
  // determined by the resulting MEDCouplingFieldDouble instance (see
  // function addField of the data manager).
  // <<<<<<<<<

  return fieldResultHandler;
}

/*!
 * This function realizes the substraction of the MEDCoupling field
 * objects identified by the FieldHandler objects passed in arguments.
 *
 * It returns a FieldHandler that identifies the resulting MEDCoupling
 * field object. The resulting MEDCoupling field object is physically
 * in the SALOME MED container and is automatically registered in the
 * MEDDataManager.
 */
MEDCALC::FieldHandler * MEDCalculator_i::sub(const MEDCALC::FieldHandler & f1_hdl,
             const MEDCALC::FieldHandler & f2_hdl)
{
  if ( f1_hdl.meshid != f2_hdl.meshid ) {
    std::string message =
      std::string("ERROR: Mesh ids are different for the field operands ") +
      std::string(f1_hdl.fieldname) + std::string(" and ") + std::string(f2_hdl.fieldname);
    throw KERNEL::createSalomeException(message.c_str());
  }

  MEDCouplingFieldDouble* f1 = _medDataManager->getFieldDouble(&f1_hdl);
  MEDCouplingFieldDouble* f2 = _medDataManager->getFieldDouble(&f2_hdl);

  MEDCouplingFieldDouble* field_result;
  try {
    field_result = (*f1) - (*f2);
  }
  catch (INTERP_KERNEL::Exception &ex) {
    throw KERNEL::createSalomeException(ex.what());
  }

  std::string name = std::string(f1->getName()) + "-" + std::string(f2->getName());
  field_result->setName(name.c_str());

  long meshHandlerId = f1_hdl.meshid;
  MEDCALC::FieldHandler * fieldResultHandler = _medDataManager->addField(field_result,
                       meshHandlerId);
  return fieldResultHandler;
}

/*!
 * This function realizes the multiplication of the MEDCoupling field
 * objects identified by the FieldHandler objects passed in arguments.
 *
 * It returns a FieldHandler that identifies the resulting MEDCoupling
 * field object. The resulting MEDCoupling field object is physically
 * in the SALOME MED container and is automatically registered in the
 * MEDDataManager.
 */
MEDCALC::FieldHandler * MEDCalculator_i::mul(const MEDCALC::FieldHandler & f1_hdl,
             const MEDCALC::FieldHandler & f2_hdl)
{
  if ( f1_hdl.meshid != f2_hdl.meshid ) {
    std::string message =
      std::string("ERROR: Mesh ids are different for the field operands ") +
      std::string(f1_hdl.fieldname) + std::string(" and ") + std::string(f2_hdl.fieldname);
    throw KERNEL::createSalomeException(message.c_str());
  }

  MEDCouplingFieldDouble* f1 = _medDataManager->getFieldDouble(&f1_hdl);
  MEDCouplingFieldDouble* f2 = _medDataManager->getFieldDouble(&f2_hdl);

  MEDCouplingFieldDouble* field_result;
  try {
    field_result = (*f1) * (*f2);
  }
  catch (INTERP_KERNEL::Exception &ex) {
    throw KERNEL::createSalomeException(ex.what());
  }
  std::string name = std::string(f1->getName()) + "*" + std::string(f2->getName());
  field_result->setName(name.c_str());

  long meshHandlerId = f1_hdl.meshid;
  MEDCALC::FieldHandler * fieldResultHandler = _medDataManager->addField(field_result,
                       meshHandlerId);
  return fieldResultHandler;
}

/*!
 * This function realizes the division of the MEDCoupling field
 * objects identified by the FieldHandler objects passed in arguments.
 *
 * It returns a FieldHandler that identifies the resulting MEDCoupling
 * field object. The resulting MEDCoupling field object is physically
 * in the SALOME MED container and is automatically registered in the
 * MEDDataManager.
 */
MEDCALC::FieldHandler * MEDCalculator_i::div(const MEDCALC::FieldHandler & f1_hdl,
             const MEDCALC::FieldHandler & f2_hdl)
{
  if ( f1_hdl.meshid != f2_hdl.meshid ) {
    std::string message =
      std::string("ERROR: Mesh ids are different for the field operands ") +
      std::string(f1_hdl.fieldname) + std::string(" and ") + std::string(f2_hdl.fieldname);
    throw KERNEL::createSalomeException(message.c_str());
  }

  MEDCouplingFieldDouble* f1 = _medDataManager->getFieldDouble(&f1_hdl);
  MEDCouplingFieldDouble* f2 = _medDataManager->getFieldDouble(&f2_hdl);

  MEDCouplingFieldDouble* field_result;
  try {
    field_result = (*f1) / (*f2);
  }
  catch (INTERP_KERNEL::Exception &ex) {
    throw KERNEL::createSalomeException(ex.what());
  }

  std::string name = std::string(f1->getName()) + "/" + std::string(f2->getName());
  field_result->setName(name.c_str());

  long meshHandlerId = f1_hdl.meshid;
  MEDCALC::FieldHandler * fieldResultHandler = _medDataManager->addField(field_result,
                       meshHandlerId);
  return fieldResultHandler;
}

/*!
 * This function realizes the power of the MEDCoupling field
 * object identified by the FieldHandler object passed in argument.
 *
 * It returns a FieldHandler that identifies the resulting MEDCoupling
 * field object. The resulting MEDCoupling field object is physically
 * in the SALOME MED container and is automatically registered in the
 * MEDDataManager.
 */
MEDCALC::FieldHandler * MEDCalculator_i::pow(const MEDCALC::FieldHandler & f_hdl, CORBA::Long power)
{
  MEDCouplingFieldDouble* field = _medDataManager->getFieldDouble(&f_hdl);

  // We use the applyFunc function to execute this operation. The
  // applyFunc function modifies the original field (that is not what
  // we want). So we have first to make a deep copy.
  MEDCouplingFieldDouble* field_result;
  try {
    field_result = field->clone(true);
    std::string functionToApply = "u^"+ToString(power);
    field_result->applyFunc(functionToApply.c_str());
  }
  catch (INTERP_KERNEL::Exception &ex) {
    throw KERNEL::createSalomeException(ex.what());
  }

  // Set the name (the default is the same as the original field)
  std::string name(field_result->getName());
  name.append("^");
  name.append(ToString(power));
  field_result->setName(name.c_str());

  long meshHandlerId = f_hdl.meshid;
  MEDCALC::FieldHandler * fieldResultHandler = _medDataManager->addField(field_result,
                       meshHandlerId);
  return fieldResultHandler;
}

/*!
 * This function creates a new field as the linear transformation of
 * the field object identified by the FieldHandler in argument. The
 * transformation is y= factor*x + offset.
 *
 * It returns a FieldHandler that identifies the resulting MEDCoupling
 * field object. The resulting MEDCoupling field object is physically
 * in the SALOME MED container and is automatically registered in the
 * MEDDataManager.
 */
MEDCALC::FieldHandler * MEDCalculator_i::lin(const MEDCALC::FieldHandler & f_hdl, double factor, double offset)
{
  MEDCouplingFieldDouble* field = _medDataManager->getFieldDouble(&f_hdl);

  // We use the applyFunc function to execute this operation. The
  // applyFunc function modifies the original field (that is not what
  // we want). So we have first to make a deep copy.
  MEDCouplingFieldDouble* field_result;
  try {
    field_result = field->clone(true);
    std::string functionToApply = "u*"+ToString(factor)+"+"+ToString(offset);
    field_result->applyFunc(functionToApply.c_str());
  }
  catch (INTERP_KERNEL::Exception &ex) {
    throw KERNEL::createSalomeException(ex.what());
  }

  // Set the name (the default is the same as the original field)
  std::string name = std::string("lin(");
  name.append(field->getName());
  name.append(",factor=");
  name.append(ToString(factor));
  name.append(",offset=");
  name.append(ToString(offset));
  name.append(")");
  field_result->setName(name.c_str());

  long meshHandlerId = f_hdl.meshid;
  MEDCALC::FieldHandler * fieldResultHandler = _medDataManager->addField(field_result,
                       meshHandlerId);
  return fieldResultHandler;

}

/*!
 * This function creates a new field as the duplicate of the field
 * object identified by the FieldHandler in argument.
 *
 * It returns a FieldHandler that identifies the resulting MEDCoupling
 * field object. The resulting MEDCoupling field object is physically
 * in the SALOME MED container and is automatically registered in the
 * MEDDataManager.
 */
MEDCALC::FieldHandler * MEDCalculator_i::dup(const MEDCALC::FieldHandler & f_hdl)
{
  MEDCouplingFieldDouble* field = _medDataManager->getFieldDouble(&f_hdl);

  // We just make a deep copy of the input field
  MEDCouplingFieldDouble* field_result;
  try {
    field_result = field->clone(true);
  }
  catch (INTERP_KERNEL::Exception &ex) {
    throw KERNEL::createSalomeException(ex.what());
  }

  // Set the name (the default is the same as the original field)
  std::string name = std::string("dup(");
  name.append(field->getName());
  name.append(")");
  field_result->setName(name.c_str());

  long meshHandlerId = f_hdl.meshid;
  MEDCALC::FieldHandler * fieldResultHandler = _medDataManager->addField(field_result,
                       meshHandlerId);
  return fieldResultHandler;
}

/*!
 * This creates a new field by applying a function on the specified
 * field. The function is defined by a string of characters
 * (function), that specifies the literal equation to apply, and an
 * integer (nbcomponents), that specifies the number of components to
 * consider in the resulting field. This is to mimic the interface of
 * MEDCouplingFieldDouble::applyFunc.
 * Note that if nbcomponents is not in the range 1..N where N is the
 * number of components of the specified field, then the function is
 * apply on the whole field (as if specified N=numberOfComponents).
 */
MEDCALC::FieldHandler * MEDCalculator_i::fct(const MEDCALC::FieldHandler & f_hdl,
             const char * function, CORBA::Long nbResComponents)
{
  MEDCouplingFieldDouble* field = _medDataManager->getFieldDouble(&f_hdl);

  // We first make a deep copy of the input field
  MEDCouplingFieldDouble* field_result;
  try {
    field_result = field->clone(true);
    if ( (nbResComponents == MEDCALC::NBCOMP_DEFAULT ) ||
	 (nbResComponents < 1) || (nbResComponents > (int)field_result->getNumberOfComponents()) ) {
      field_result->applyFunc(function);
    }
    else {
      field_result->applyFunc(nbResComponents,function);
    }
  }
  catch (INTERP_KERNEL::Exception &ex) {
    throw KERNEL::createSalomeException(ex.what());
  }

  std::string name = std::string("fct("); name.append(field->getName());
  name.append(",\""); name.append(function);
  name.append(",\""); name.append(ToString(nbResComponents)); name.append("\")");
  field_result->setName(name.c_str());

  long meshHandlerId = f_hdl.meshid;
  MEDCALC::FieldHandler * fieldResultHandler = _medDataManager->addField(field_result,
                       meshHandlerId);
  return fieldResultHandler;
}
